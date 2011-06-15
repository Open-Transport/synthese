
/** IneoFileFormat class implementation.
	@file IneoFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "IneoFileFormat.hpp"
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "PTFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopPointAdmin.hpp"
#include "StopAreaAddAction.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "ImpExModule.h"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "IneoFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,IneoFileFormat>::FACTORY_KEY("Ineo");
	}

	namespace pt
	{
		const std::string IneoFileFormat::Importer_::FILE_PNT("pnt");
		const std::string IneoFileFormat::Importer_::FILE_DIS("dis");
		const std::string IneoFileFormat::Importer_::FILE_DST("dst");
		const std::string IneoFileFormat::Importer_::FILE_LIG("lig");
		const std::string IneoFileFormat::Importer_::FILE_CJV("cjv");
		const std::string IneoFileFormat::Importer_::FILE_HOR("hor");
		const std::string IneoFileFormat::Importer_::FILE_CAL("cal");
		const std::string IneoFileFormat::Importer_::SEP(";");

		const std::string IneoFileFormat::Importer_::PARAMETER_NETWORK_ID("net");
		const std::string IneoFileFormat::Importer_::PARAMETER_AUTO_IMPORT_STOPS("isa");
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string IneoFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<IneoFileFormat>::Files MultipleFileTypesImporter<IneoFileFormat>::FILES(
			IneoFileFormat::Importer_::FILE_PNT.c_str(),
			IneoFileFormat::Importer_::FILE_DIS.c_str(),
			IneoFileFormat::Importer_::FILE_DST.c_str(),
			IneoFileFormat::Importer_::FILE_LIG.c_str(),
			IneoFileFormat::Importer_::FILE_CAL.c_str(),
			IneoFileFormat::Importer_::FILE_CJV.c_str(),
			IneoFileFormat::Importer_::FILE_HOR.c_str(),
		"");
	}


	namespace pt
	{
		bool IneoFileFormat::Importer_::_controlPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_PNT));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_DIS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_DST);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_LIG);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CJV);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_HOR);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CAL);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		IneoFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<IneoFileFormat>(env, dataSource),
			Importer(env, dataSource),
			_autoImportStops(false),
			_interactive(false),
			_displayLinkedStops(false),
			_stopPoints(_dataSource, _env),
			_lines(_dataSource, _env),
			_destinations(_dataSource, _env)
		{}



		bool IneoFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			_clearFieldsMap();

			// 1 : Routes
			// Stops
			if(key == FILE_PNT)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(_dataSource, _env);
				map<string, StopArea*> stopAreasByName;

				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				while(_readLine(inFile))
				{
					if(_section != "P")
					{
						continue;
					}

					string id(_getValue("MNLP"));
					string name(_getValue("LIBP"));

					// Point
					shared_ptr<geos::geom::Point> point(
						_dataSource.getCoordinatesSystem()->createPoint(
							lexical_cast<double>(_getValue("X")),
							lexical_cast<double>(_getValue("Y"))
					)	);
					if(point->isEmpty())
					{
						point.reset();
					}

					if(request && !_autoImportStops)
					{
						PTFileFormat::ImportableStopPoint isp;
						isp.operatorCode = id;
						isp.name = name;
						isp.linkedStopPoints = _stopPoints.get(id);
						isp.coords = point;

						if(isp.linkedStopPoints.empty())
						{
							nonLinkedStopPoints.push_back(isp);
						}
						else if(_displayLinkedStops)
						{
							linkedStopPoints.push_back(isp);
						}
					}
					else
					{
						if(_stopPoints.contains(id))
						{
							BOOST_FOREACH(StopPoint* stopPoint, _stopPoints.get(id))
							{
								stopPoint->setName(name);
								if(point.get())
								{
									stopPoint->setGeometry(point);
								}
								const_cast<StopArea*>(stopPoint->getConnectionPlace())->setName(name);
							}
						}
						else
						{
							if(_autoImportStops)
							{
								StopArea* stopArea(NULL);
								 // Search in the last created stop areas map
								map<string, StopArea*>::const_iterator it(stopAreasByName.find(name));
								if(it != stopAreasByName.end())
								{
									stopArea = it->second;
								}
								// Search in the database
								if(!stopArea)
								{
									StopAreaTableSync::SearchResult stopAreas(
										StopAreaTableSync::Search(
											_env,
											_defaultCity->getKey(),
											logic::indeterminate,
											optional<string>(),
											name
									)	);
									if(!stopAreas.empty())
									{
										stopArea = stopAreas.begin()->get();
									}
								}
								// Creation of the stop area
								if(!stopArea)
								{
									stopArea = new StopArea(
										StopAreaTableSync::getId(),
										true,
										_stopAreaDefaultTransferDuration
									);
									stopArea->setCity(_defaultCity.get());
									stopArea->setName(name);
									_env.getEditableRegistry<StopArea>().add(shared_ptr<StopArea>(stopArea));
									stopAreasByName.insert(make_pair(name, stopArea));
								}
								PTFileFormat::CreateOrUpdateStopPoints(
									_stopPoints,
									id,
									name,
									stopArea,
									point.get(),
									_defaultCity.get(),
									_stopAreaDefaultTransferDuration,
									_dataSource,
									_env,
									stream
								);
							}
							else
							{
								return false;
							}
						}
					}
				}

				if(!nonLinkedStopPoints.empty() && !_autoImportStops)
				{
					if(request)
					{
						PTFileFormat::DisplayStopPointImportScreen(
							nonLinkedStopPoints,
							*request,
							_env,
							_dataSource,
							stream
							);
						if(_displayLinkedStops)
						{
							PTFileFormat::DisplayStopPointImportScreen(
								linkedStopPoints,
								*request,
								_env,
								_dataSource,
								stream
							);
						}
					}
					return false;
				}
			}
			// 2 : Distances
			else if(key == FILE_DIS)
			{
				while(_readLine(inFile))
				{
					if(_section == "D")
					{
						_distances.insert(
							make_pair(
								make_pair(
									_getValue("MNEO"),
									_getValue("MNED")
								),
								lexical_cast<MetricOffset>(_getValue("DIST"))
						)	);
					}
				}
			}
			// 3 : Destinations
			else if(key == FILE_DST)
			{
				ImportableTableSync::ObjectBySource<DestinationTableSync> destinations(_dataSource, _env);
				while(_readLine(inFile))
				{
					if(_section == "DST")
					{
						PTFileFormat::CreateOrUpdateDestination(
							_destinations,
							_getValue("NDSTG"),
							_getValue("DSTBL"),
							_getValue("DSTTS"),
							_dataSource,
							_env,
							stream
						);
					}
				}
			}
			// 4 : Lines
			else if(key == FILE_LIG)
			{
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);
				CommercialLine* line(NULL);
				JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
				string lineId;
				string jpName;
				bool jpWayback(false);
				string jpKey;
				string lastStopCode;
				MetricOffset dst(0);
				while(true)
				{
					_readLine(inFile);

					if((_section == "L" || _section =="CH" || _section.empty()) && !stops.empty())
					{
						JourneyPattern* route(
							PTFileFormat::CreateOrUpdateRoute(
								*line,
								optional<const string&>(),
								jpName,
								optional<const string&>(), // destination
								optional<Destination*>(),
								jpWayback,
								NULL,
								stops,
								_dataSource,
								_env,
								stream
						)	);
						stops.clear();
						_journeyPatterns[make_pair(lineId,jpKey)] = route;
					}
					if(_section.empty())
					{
						break;
					}
					if(_section == "L")
					{
						lineId = _getValue("MNLG");
						line = PTFileFormat::CreateOrUpdateLine(
							lines,
							lineId,
							_getValue("LIBLG"),
							_getValue("NLGIV"),
							optional<RGBColor>(),
							*_network,
							_dataSource,
							_env,
							stream
						);
					}
					else if(_section == "CH")
					{
						jpName = _getValue("LIBCH");
						jpWayback = (_getValue("SENS") == "A");
						jpKey = _getValue("NCH");
						stops.clear();
						lastStopCode.clear();
						dst = 0;
					}
					else if(_section == "PC")
					{
						string stopCode(_getValue("MNL"));
						if(!lastStopCode.empty())
						{
							std::map<std::pair<std::string, std::string>, graph::MetricOffset>::const_iterator it(_distances.find(
									make_pair(lastStopCode, stopCode)
							)	);
							if(it != _distances.end())
							{
								dst += it->second;
							}
							else
							{
								stream << "WARN : distance between " << lastStopCode << " and " << stopCode << " not found.<br />";
							}
						}
						JourneyPattern::StopWithDepartureArrivalAuthorization stop(
							_stopPoints.get(stopCode),
							dst,
							true,
							true,
							_getValue("TYPC") == "R"
						);
						stops.push_back(stop);
						lastStopCode = stopCode;
					}
				}
			}
			else if(key == FILE_CAL)
			{
				while(_readLine(inFile))
				{
					if(_section == "CAL")
					{
						string dateStr(_getValue("DATE"));
						_dates[make_pair(lexical_cast<int>(_getValue("PH")),lexical_cast<int>(_getValue("TYP_JOUR")))].push_back(
							date(
								lexical_cast<int>(dateStr.substr(6,4)),
								lexical_cast<int>(dateStr.substr(3,2)),
								lexical_cast<int>(dateStr.substr(0,2))
						)	);
					}
				}
			}
			else if(key == FILE_CJV)
			{
				int day(0);
				while(_readLine(inFile))
				{
					if(_section == "JC")
					{
						day = lexical_cast<int>(_getValue("CODE_JC"));
					}
					else if(_section == "CJDV")
					{
						vector<string> cjdvs;
						for(int i=0; i<10; ++i)
						{
							cjdvs.push_back(lexical_cast<string>(i));
						}
						cjdvs.push_back("A");
						cjdvs.push_back("B");
						cjdvs.push_back("C");
						cjdvs.push_back("D");
						cjdvs.push_back("E");
						cjdvs.push_back("F");
						BOOST_FOREACH(const string& cjdv, cjdvs)
						{
							if(_getValue("C"+cjdv) == "O")
							{
								_calendars[cjdv].push_back(day);
							}
						}
					}
				}
			}
			else if(key == FILE_HOR)
			{
				bool active(true);
				JourneyPattern* route(NULL);
				ScheduledService::Schedules schedules;
				int ph(0);
				vector<date> dates;
				time_duration lastTd(minutes(0));
				while(true)
				{
					_readLine(inFile);

					if((_section == "C" || _section.empty()) && !schedules.empty())
					{
						ScheduledService* service(
							PTFileFormat::CreateOrUpdateService(
								*route,
								schedules,
								schedules,
								string(),
								_dataSource,
								_env,
								stream
						)	);
						if(service)
						{
							BOOST_FOREACH(const date& dat, dates)
							{
								service->setActive(dat);
							}
						}
					}
					if(_section.empty())
					{
						break;
					}

					if(_section == "PH")
					{
						ph = lexical_cast<int>(_getValue("NPH"));
					}
					else if(_section == "C")
					{
						if(_getValue("TCOU") != "0")
						{
							active = false;
							continue;
						}
						active = true;
						string lineNum(lexical_cast<string>(lexical_cast<int>(_getValue("CIDX").substr(5,2))));
						string jpNum(_getValue("ORD"));
						route = _journeyPatterns[make_pair(lineNum,jpNum)];

						schedules.clear();
						dates.clear();
						lastTd = minutes(0);

						BOOST_FOREACH(int day, _calendars[_getValue("CJDV")])
						{
							BOOST_FOREACH(const date& dat, _dates[make_pair(ph, day)])
							{
								dates.push_back(dat);
							}
						}
					}
					else if(active && _section == "H")
					{
						string timeStr(_getValue("HOR"));
						time_duration td(
							lexical_cast<int>(timeStr.substr(0,2)),
							lexical_cast<int>(timeStr.substr(2,2)),
							lexical_cast<int>(timeStr.substr(4,2))
						);
						if(td < lastTd)
						{
							td += hours(24);
						}
						schedules.push_back(td);
						lastTd = td;
					}
					if(_section.empty())
					{
						break;
					}
				}
			}
			return true;
		}



		void IneoFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Fichiers");
			stream << t.cell("Fichier PNT (arrêts)", t.getForm().getTextInput(_getFileParameterName(FILE_PNT), _pathsMap[FILE_PNT].file_string()));
			stream << t.cell("Fichier DIS (distances)", t.getForm().getTextInput(_getFileParameterName(FILE_DIS), _pathsMap[FILE_DIS].file_string()));
			stream << t.cell("Fichier DST (destinations)", t.getForm().getTextInput(_getFileParameterName(FILE_DST), _pathsMap[FILE_DST].file_string()));
			stream << t.cell("Fichier LIG (lignes)", t.getForm().getTextInput(_getFileParameterName(FILE_LIG), _pathsMap[FILE_LIG].file_string()));
			stream << t.cell("Fichier CJV (dates)", t.getForm().getTextInput(_getFileParameterName(FILE_CJV), _pathsMap[FILE_CJV].file_string()));
			stream << t.cell("Fichier CAL (calendriers)", t.getForm().getTextInput(_getFileParameterName(FILE_CAL), _pathsMap[FILE_CAL].file_string()));
			stream << t.cell("Fichier HOR (horaires)", t.getForm().getTextInput(_getFileParameterName(FILE_HOR), _pathsMap[FILE_HOR].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Réseau (ID)", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Import automatique arrêts", t.getForm().getOuiNonRadioInput(PARAMETER_AUTO_IMPORT_STOPS, _autoImportStops));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.close();
		}



		db::DBTransaction IneoFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Saving of each created or altered objects
			if(_autoImportStops)
			{
				BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(cstop.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
				{
					StopPointTableSync::Save(stop.second.get(), transaction);
				}
			}
			BOOST_FOREACH(const Registry<Destination>::value_type& destination, _env.getRegistry<Destination>())
			{
				DestinationTableSync::Save(destination.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Junction>::value_type& junction, _env.getRegistry<Junction>())
			{
				JunctionTableSync::Save(junction.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<TransportNetwork>::value_type network, _env.getRegistry<TransportNetwork>())
			{
				TransportNetworkTableSync::Save(network.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DesignatedLinePhysicalStop>::value_type lineStop, _env.getRegistry<DesignatedLinePhysicalStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			return transaction;
		}



		void IneoFileFormat::Importer_::_clearFieldsMap() const
		{
			_fieldsMap.clear();
		}



		std::string IneoFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return _line[field];
		}


		bool IneoFileFormat::Importer_::_readLine(ifstream& file) const
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



		void IneoFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();

			vector<string> parts;
			string trim_line(trim_copy(line));
			if (trim_line[0] == ';')
				trim_line = trim_line.substr(1);
			split(parts, trim_line, is_any_of(":"));
			if(parts.size() < 2 || parts[0].empty() || parts[1].empty())
			{
				_section = "#";
				return;
			}
			_section = parts[0];
			if(_section == "F")
			{
				string code(trim_copy(parts[1]));
				vector<string> fields;
				split(fields, parts[2], is_any_of(SEP));
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
				string utfLine(ImpExModule::ConvertChar(parts[1], _dataSource.getCharset(), "UTF-8"));
				split(fields, utfLine, is_any_of(SEP));
				const vector<string>& cols(itFieldsMap->second);
				for(size_t i=0; i<fields.size(); ++i)
				{
					_line[cols[i]] = trim_copy(fields[i]);
				}
			}
		}



		server::ParametersMap IneoFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_AUTO_IMPORT_STOPS, _autoImportStops);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			return map;
		}



		void IneoFileFormat::Importer_::_setFromParametersMap( const server::ParametersMap& map )
		{
			if(map.isDefined(PARAMETER_NETWORK_ID)) try
			{
				_network = TransportNetworkTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}
			catch (ObjectNotFoundException<TransportNetwork>&)
			{
			}

			_autoImportStops = map.getDefault<bool>(PARAMETER_AUTO_IMPORT_STOPS, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}
		}
}	}
