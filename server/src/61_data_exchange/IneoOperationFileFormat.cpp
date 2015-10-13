
/** IneoOperationFileFormat class implementation.
	@file IneoOperationFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "IneoOperationFileFormat.hpp"

#include "DataSource.h"
#include "DBModule.h"
#include "DriverAllocationTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "IConv.hpp"
#include "ImpExModule.h"
#include "Import.hpp"
#include "Importer.hpp"
#include "Profile.h"
#include "PTModule.h"
#include "UserTableSync.h"
#include "VehicleServiceTableSync.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	using namespace calendar;
	using namespace data_exchange;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace pt_operation;
	using namespace security;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, IneoOperationFileFormat>::FACTORY_KEY("IneoOperation");
	}

	namespace data_exchange
	{
		const string IneoOperationFileFormat::Importer_::FILE_AFA = "afa";
		const string IneoOperationFileFormat::Importer_::FILE_SAB = "sab";
		const string IneoOperationFileFormat::Importer_::SEP = ";";

		const string IneoOperationFileFormat::Importer_::PARAMETER_PT_DATASOURCE_ID = "pt_datasource_id";
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<IneoOperationFileFormat>::Files MultipleFileTypesImporter<IneoOperationFileFormat>::FILES(
			IneoOperationFileFormat::Importer_::FILE_SAB.c_str(),
			IneoOperationFileFormat::Importer_::FILE_AFA.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool IneoOperationFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_SAB));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		IneoOperationFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<IneoOperationFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTOperationFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_startDate(not_a_date_time),
			_endDate(not_a_date_time),
			_activities(*import.get<DataSource>(), _env),
			_driverAllocationTemplates(*import.get<DataSource>(), _env)
		{}



		void IneoOperationFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			// Datasource of the services (scheduled services and dead runs)
			try
			{
				_ptDatasource = DataSourceTableSync::Get(
					map.get<RegistryKeyType>(PARAMETER_PT_DATASOURCE_ID),
					_env
				);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw Exception("Data source not found");
			}

			// Determination of start and end dates from the content of SAB
			// Calendar dates
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_SAB));
			if(it == _pathsMap.end())
			{
				throw Exception("SAB file path not specified");
			}

			ifstream inFile;
			inFile.open(it->second.string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the calendar file.");
			}
			_clearFieldsMap();
			while(_readLine(inFile))
			{
				if(_section != "SASB")
				{
					continue;
				}

				string dateStr(_getValue("DATE"));
				date calDate(
					lexical_cast<int>(dateStr.substr(6,4)),
					lexical_cast<int>(dateStr.substr(3,2)),
					lexical_cast<int>(dateStr.substr(0,2))
				);
				if(_startDate.is_not_a_date() || calDate < _startDate)
				{
					_startDate = calDate;
				}
				if(_endDate.is_not_a_date() || calDate > _endDate)
				{
					_endDate = calDate;
				}
			}
		}



		bool IneoOperationFileFormat::Importer_::beforeParsing()
		{
			if(_startDate.is_not_a_date() || _endDate.is_not_a_date())
			{
				return false;
			}

			DataSource& dataSource(*_import.get<DataSource>());

			// Driver allocations
			date undefinedDate(not_a_date_time);
			ImportableTableSync::ObjectBySource<DriverAllocationTableSync> driverAllocations(dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTableSync>::Map::value_type& itDASet, driverAllocations.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTableSync>::Map::mapped_type::value_type& itDA, itDASet.second)
				{
					if(	_startDate <= itDA->get<Date>() &&
						_endDate >= itDA->get<Date>()
					){
						itDA->set<Date>(undefinedDate);
					}
			}	}

			// Driver allocation templates
			DriverService::Vector::Type emptyDATChunks;
			ImportableTableSync::ObjectBySource<DriverAllocationTemplateTableSync> driverAllocationTemplates(dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTemplateTableSync>::Map::value_type& itDASet, driverAllocationTemplates.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverAllocationTemplateTableSync>::Map::mapped_type::value_type& itDA, itDASet.second)
				{
					if(	_startDate <= itDA->get<Date>() &&
						_endDate >= itDA->get<Date>()
					){
						itDA->set<DriverService::Vector>(emptyDATChunks);
					}
			}	}

			// Driver services
			ImportableTableSync::ObjectBySource<DriverServiceTableSync> driverServices(dataSource, _env);
			DriverService::Chunks emptyChunks;
			Calendar importDate(_startDate, _endDate);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverServiceTableSync>::Map::value_type& itDSSet, driverServices.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverServiceTableSync>::Map::mapped_type::value_type& itDS, itDSSet.second)
				{
					if(!itDS->hasAtLeastOneCommonDateWith(importDate))
					{
						continue;
					}
					itDS->setChunks(emptyChunks);
			}	}

			return true;
		}



		bool IneoOperationFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				_logError("Could no open the file " + filePath.string());
				throw Exception("Could no open the file " + filePath.string());
			}
			_clearFieldsMap();
			date now(day_clock::local_day());

			DataSource& dataSource(*_import.get<DataSource>());

			if(key == FILE_SAB)
			{
				ImportableTableSync::ObjectBySource<VehicleServiceTableSync> vehicleServices(*_ptDatasource, _env);
				ImportableTableSync::ObjectBySource<DriverServiceTableSync> driverServices(dataSource, _env);
				string lastKey;
				DriverService* ds(NULL);
				DriverAllocationTemplate* da(NULL);
				do
				{
					// File read
					_readLine(inFile);
					if(_section == "SASB" || _section == "#")
					{
						string tpstra(_getValue("TpsTra"));
						string ampli(_getValue("Ampli"));
						string onBoardTicketing(_getValue("VaB"));

						// Load
						string key(_getValue("SA"));
						string vsKey(_getValue("SB"));
						string dateStr(_getValue("DATE"));
						string fullKey(key+"/"+dateStr);

						// Date
						date vsDate;
						vector<string> parts;
						split(parts, dateStr, is_any_of("/.-"));
						vsDate = date(
							(parts[2].size() == 2 ? 2000 : 0) + lexical_cast<int>(parts[2]),
							lexical_cast<int>(parts[1]),
							lexical_cast<int>(parts[0])
						);

						if(fullKey != lastKey)
						{
							ds = _loadOrCreateObject<DriverServiceTableSync>(
								driverServices,
								fullKey,
								dataSource,
								"driver service"
							);
							lastKey = fullKey;
							DriverService::Chunks emptyChunks;
							ds->setChunks(emptyChunks);
							ds->setActive(vsDate);

							// Name
							ds->setName(key);

							// Allocation template
							da = _loadOrCreateObject<DriverAllocationTemplateTableSync>(
								_driverAllocationTemplates,
								fullKey,
								dataSource,
								"driver allocation template"
							);

							// Driver service link
							DriverService::Vector::Type driverServices;
							driverServices.push_back(ds);
							da->set<DriverService::Vector>(driverServices);

							// Date
							da->set<Date>(vsDate);

							// Amount
							string amountStr(_getValue("Frs"));
							double amount(
								amountStr.empty() || amountStr == "vide" ?
								0 :
								lexical_cast<double>(amountStr)
							);
							da->set<Amount>(amount);

							// Max Boni amount
							string boniAmountStr(_getValue("ATTmaxfrs"));
							da->set<MaxBoniAmount>(
								boniAmountStr.empty() || boniAmountStr == "vide" ?
								0 :
								lexical_cast<double>(boniAmountStr)
							);

							// Boni time
							string bonifStr(_getValue("ATTmaxtps"));
							if(bonifStr.empty() || bonifStr == "vide")
							{
								da->set<MaxBoniTime>(minutes(0));
							}
							else
							{
								vector<string> parts;
								split(parts, bonifStr, is_any_of("h"));
								da->set<MaxBoniTime>(hours(lexical_cast<long>(parts[0])) + minutes(lexical_cast<long>(parts[1])));
							}

							// Work range
							string workRangeStr(_getValue("Ampli"));
							if(workRangeStr.empty() || workRangeStr == "vide")
							{
								da->set<WorkRange>(time_duration(not_a_date_time));
							}
							else
							{
								vector<string> parts;
								split(parts, workRangeStr, is_any_of("h"));
								da->set<WorkRange>(hours(lexical_cast<long>(parts[0])) + minutes(lexical_cast<long>(parts[1])));
							}

							// Work duration
							string workDurationStr(_getValue("TpsTra"));
							if(workDurationStr.empty() || workDurationStr == "vide")
							{
								da->set<WorkDuration>(time_duration(not_a_date_time));
							}
							else
							{
								vector<string> parts;
								split(parts, workDurationStr, is_any_of("h"));
								da->set<WorkDuration>(hours(lexical_cast<long>(parts[0])) + minutes(lexical_cast<long>(parts[1])));
							}

							// Default value for ticket sales
							da->set<WithTicketSales>(false);
						}

						// On board ticketing
						if(da && !onBoardTicketing.empty())
						{
							da->set<WithTicketSales>(true);
						}

						// Journey
						bool afterMidnight(_getValue("DAPM")=="O");
						string hdebstr(_getValue("HDeb"));
						vector<string> hdebParts;
						split(hdebParts, hdebstr, is_any_of("h:"));
						time_duration hdeb(
							hours(lexical_cast<long>(hdebParts[0])) +
							minutes(lexical_cast<long>(hdebParts[1]))
						);
						string hfinstr(_getValue("HFin"));
						vector<string> hfinParts;
						split(hfinParts, hfinstr, is_any_of("h:"));
						time_duration hfin(
							hours(lexical_cast<long>(hfinParts[0])) +
							minutes(lexical_cast<long>(hfinParts[1]))
						);
						if(hfin < hdeb)
						{
							hfin += hours(24);
						}
						if(afterMidnight)
						{
							hdeb += hours(24);
							hfin += hours(24);
						}

						// Service start and end
						string hdebpStr(_getValue("HDebP"));
						vector<string> hdebpParts;
						split(hdebpParts, hdebpStr, is_any_of("h:"));
						time_duration hdebp(
							hours(lexical_cast<long>(hdebpParts[0])) +
							minutes(lexical_cast<long>(hdebpParts[1]))
						);
						string hfinpStr(_getValue("HFinP"));
						vector<string> hfinpParts;
						split(hfinpParts, hfinpStr, is_any_of("h:"));
						time_duration hfinp(
							hours(lexical_cast<long>(hfinpParts[0])) +
							minutes(lexical_cast<long>(hfinpParts[1]))
						);
						if(hfinp < hdebp)
						{
							hfinp += hours(24);
						}
						if(afterMidnight)
						{
							hdebp += hours(24);
							hfinp += hours(24);
						}

						// Vehicle service
						VehicleService* vs(NULL);
						if(!vsKey.empty())
						{
							set<VehicleService*> lvs;
							lvs = vehicleServices.getBeginWith(vsKey);
							if(lvs.empty())
							{
								_logWarning(
									"Vehicle service "+ vsKey +" not foud in driver service "+ key +"."
								);
								continue;
							}

							// Select the vehicle service which runs at the specified date
							BOOST_FOREACH(VehicleService* item, lvs)
							{
								if(item->isActive(vsDate))
								{
									vs = item;
									break;
								}
							}
						}

						// Activity
						string activityStr(_getValue("Act"));
						set<DriverActivity*> activities;
						if(!activityStr.empty() && activityStr != "vide")
						{
							// Search for an activity
							activities = _activities.get(activityStr);
						}

						DriverService::Chunks chunks(ds->getChunks());
						chunks.push_back(
							DriverService::Chunk(
								ds,
								vs,
								activities.empty() ? NULL : *activities.begin(),
								vsDate,
								hdeb,
								hfin,
								hdebp,
								hfinp
						)	);
						ds->setChunks(chunks);
					}

				} while(!_section.empty());
			}
			else if(key == FILE_AFA)
			{
				ImportableTableSync::ObjectBySource<UserTableSync> users(dataSource, _env);
				ImportableTableSync::ObjectBySource<DriverAllocationTableSync> driverAllocations(dataSource, _env);
				do
				{
					// File read
					_readLine(inFile);
					if(_section == "AFF" || _section == "#")
					{
						// Primary key
						string userKey(_getValue("MAT"));
						string dateStr(_getValue("DATE"));
						if(userKey.empty() || dateStr.empty())
						{
							continue;
						}
						string fullKey(userKey +"/"+ dateStr);

						// Object creation
						DriverAllocation* da = _loadOrCreateObject<DriverAllocationTableSync>(
							driverAllocations,
							fullKey,
							dataSource,
							"allocation"
						);

						// Date
						date vsDate;
						vector<string> parts;
						split(parts, dateStr, is_any_of("/.-"));
						vsDate = date(
							(parts[2].size() == 2 ? 2000 : 0) + lexical_cast<int>(parts[2]),
							lexical_cast<int>(parts[1]),
							lexical_cast<int>(parts[0])
						);
						da->set<Date>(vsDate);

						// User
						User* user(
							_loadOrCreateObject<UserTableSync>(
								users,
								userKey,
								dataSource,
								"conducteur"
						)	);
						da->set<Driver>(*user);

						// Search for an activity
						string key(_getValue("SA"));
						set<DriverActivity*> activities(
							_activities.get(key)
						);
						if(activities.empty())
						{
							// Driver service
							string templateFullKey(
								key + "/" + dateStr
							);
							set<DriverAllocationTemplate*> templates(
								_driverAllocationTemplates.get(templateFullKey)
							);
							if(templates.empty())
							{
								_logWarning(
									"Undefined allocation template "+ templateFullKey +"."
								);
								continue;
							}
							da->set<DriverAllocationTemplate>(
								**templates.begin()
							);
						}
						else
						{
							// Standalone activity
							da->set<DriverActivity>(**activities.begin());
						}

						// Bonif amount
						string bonifAmountStr(_getValue("bonifATTfrs"));
						da->set<BoniAmount>(
							bonifAmountStr.empty() || bonifAmountStr == "vide" ?
							0 :
							lexical_cast<double>(_getValue("bonifATTfrs"))
						);

						// Bonif time
						string bonifStr(_getValue("bonifATTtps"));
						time_duration bonifTime(minutes(0));
						if(!bonifStr.empty() && bonifStr != "vide")
						{
							vector<string> parts;
							split(parts, bonifStr, is_any_of("h"));
							bonifTime = hours(lexical_cast<long>(parts[0])) + minutes(lexical_cast<long>(parts[1]));
						}
						da->set<BoniTime>(bonifTime);
					}

				} while(!_section.empty());
			}
			return true;
		}



		bool IneoOperationFileFormat::Importer_::afterParsing()
		{
			DataSource& dataSource(*_import.get<DataSource>());

			// Driver services without services
			BOOST_FOREACH(
				const Registry<DriverService>::value_type& itDriverService,
				_env.getRegistry<DriverService>()
			){
				if(	!itDriverService.second->hasLinkWithSource(dataSource) ||
					!itDriverService.second->getChunks().empty()
				){
					continue;
				}

				_driverServicesToRemove.insert(itDriverService.second);
			}

			// Empty driver allocation template
			BOOST_FOREACH(
				const Registry<DriverAllocationTemplate>::value_type& itDriverAllocationTemplate,
				_env.getRegistry<DriverAllocationTemplate>()
			){
				if(	!itDriverAllocationTemplate.second->hasLinkWithSource(dataSource) ||
					!itDriverAllocationTemplate.second->get<DriverService::Vector>().empty()
				){
					continue;
				}

				_driverAllocationTemplatesToRemove.insert(itDriverAllocationTemplate.second);
			}

			// Deleted driver allocations
			BOOST_FOREACH(
				const Registry<DriverAllocation>::value_type& itDriverAllocation,
				_env.getRegistry<DriverAllocation>()
			){
				if(	!itDriverAllocation.second->hasLinkWithSource(dataSource) ||
					!itDriverAllocation.second->get<Date>().is_not_a_date()
				){
					continue;
				}

				_driverAllocationsToRemove.insert(itDriverAllocation.second);
			}
	
			// Driver services
			BOOST_FOREACH(
				const boost::shared_ptr<const DriverService>& driverService,
				_driverServicesToRemove
			){
				_env.getEditableRegistry<DriverService>().remove(
					driverService->getKey()
				);
			}

			// Driver allocation
			BOOST_FOREACH(
				const boost::shared_ptr<const DriverAllocation>& driverAllocation,
				_driverAllocationsToRemove
			){
				_env.getEditableRegistry<DriverAllocation>().remove(
					driverAllocation->getKey()
				);
			}

			// Driver allocation template
			BOOST_FOREACH(
				const boost::shared_ptr<const DriverAllocationTemplate>& driverAllocationTemplate,
				_driverAllocationTemplatesToRemove
			){
				_env.getEditableRegistry<DriverAllocationTemplate>().remove(
					driverAllocationTemplate->getKey()
				);
			}

			return true;
		}



		db::DBTransaction IneoOperationFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Driver allocations removals
			BOOST_FOREACH(
				const boost::shared_ptr<const DriverAllocation>& driverAllocation,
				_driverAllocationsToRemove
			){
				DBModule::GetDB()->deleteStmt(driverAllocation->getKey(), transaction);
			}

			// Driver allocation templates removals
			BOOST_FOREACH(
				const boost::shared_ptr<const DriverAllocationTemplate>& driverAllocation,
				_driverAllocationTemplatesToRemove
			){
				DBModule::GetDB()->deleteStmt(driverAllocation->getKey(), transaction);
			}

			// Driver services removals
			BOOST_FOREACH(
				const boost::shared_ptr<const DriverService>& driverService,
				_driverServicesToRemove
			){
				DBModule::GetDB()->deleteStmt(driverService->getKey(), transaction);
			}

			// New driver services
			BOOST_FOREACH(const Registry<DriverService>::value_type& dservice, _env.getRegistry<DriverService>())
			{
				DriverServiceTableSync::Save(dservice.second.get(), transaction);
			}

			// New driver allocation templates
			BOOST_FOREACH(
				const Registry<DriverAllocationTemplate>::value_type& driverAllocationTemplate,
				_env.getRegistry<DriverAllocationTemplate>()
			){
				DriverAllocationTemplateTableSync::Save(driverAllocationTemplate.second.get(), transaction);
			}

			// New drivers
			BOOST_FOREACH(const Registry<User>::value_type& driver, _env.getRegistry<User>())
			{
				UserTableSync::Save(driver.second.get(), transaction);
			}

			// New driver allocations
			BOOST_FOREACH(
				const Registry<DriverAllocation>::value_type& driverAllocation,
				_env.getRegistry<DriverAllocation>()
			){
				DriverAllocationTableSync::Save(driverAllocation.second.get(), transaction);
			}

			return transaction;
		}



		void IneoOperationFileFormat::Importer_::_clearFieldsMap() const
		{
			_fieldsMap.clear();
		}



		string IneoOperationFileFormat::Importer_::_getValue( const string& field ) const
		{
			return _line[field];
		}



		bool IneoOperationFileFormat::Importer_::_readLine(ifstream& file) const
		{
			string line;
			if(!getline(file, line))
			{
				_section.clear();
				return false;
			}
			_loadLine(line);
			return true;
		}



		void IneoOperationFileFormat::Importer_::_loadLine( const string& line ) const
		{
			_line.clear();

			string trim_line(trim_copy(line));
			if (trim_line[0] == ';')
				trim_line = trim_line.substr(1);

			size_t separator(trim_line.find_first_of(":"));
			if(separator == string::npos || separator == 0 || separator == trim_line.size()-1)
			{
				_section = "#";
				return;
			}

			_section = trim_line.substr(0, separator);
			if(_section == "F")
			{
				string content(trim_line.substr(separator+1));
				vector<string> parts;
				split(parts, content, is_any_of(":"));
				string code(trim_copy(parts[0]));
				vector<string> fields;
				split(fields, parts[1], is_any_of(SEP));
				BOOST_FOREACH(const string& field, fields)
				{
					_fieldsMap[code].push_back(trim_copy(field));
				}
			}
			else
			{
				FieldMaps::const_iterator itFieldsMap(_fieldsMap.find(_section));
				if(itFieldsMap == _fieldsMap.end())
				{
					_section = "#";
					return;
				}
				vector<string> fields;
				string utfLine(IConv(_import.get<DataSource>()->get<Charset>(), "UTF-8").convert(trim_line.substr(separator+1)));
				split(fields, utfLine, is_any_of(SEP));
				const vector<string>& cols(itFieldsMap->second);
				for(size_t i=0; i<fields.size() && i<cols.size(); ++i)
				{
					_line[cols[i]] = trim_copy(fields[i]);
				}
			}
		}



		util::ParametersMap IneoOperationFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;

			// Journey pattern line overload field
			if(_ptDatasource.get())
			{
				map.insert(PARAMETER_PT_DATASOURCE_ID, _ptDatasource->getKey());
			}

			return map;
		}
}	}
