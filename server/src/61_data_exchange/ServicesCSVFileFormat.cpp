
//////////////////////////////////////////////////////////////////////////
///	ServicesCSVFileFormat class implementation.
///	@file ServicesCSVFileFormat.cpp
///	@author Gael Sauvanet
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ServicesCSVFileFormat.hpp"

#include "Import.hpp"
#include "Importer.hpp"
#include "ImpExModule.h"
#include "IConv.hpp"
#include "DataSource.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "TransportNetwork.h"
#include "CalendarTemplateTableSync.h"
#include "CityTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "PTUseRuleTableSync.h"
#include "RollingStockTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CalendarLinkTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "Fare.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/filesystem/operations.hpp>
#include <geos/geom/Geometry.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace data_exchange;
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace vehicle;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,ServicesCSVFileFormat>::FACTORY_KEY("ServicesCSV");
	}

	namespace data_exchange
	{
		const std::string ServicesCSVFileFormat::Importer_::PATH_SERVICES("services");

		const std::string ServicesCSVFileFormat::Importer_::SEP(";");

		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID("rolling_stock_id");
		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");
		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");

		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_NUMBER_OF_LINES_TO_IGNORE("nb_lines_to_ignore");

		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_FIELD_SERVICE_NUMBER("field_service_number");
		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_FIELD_STOP_CODE("field_stop_code");
		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_FIELD_STOP_NAME("field_stop_name");
		const std::string ServicesCSVFileFormat::Importer_::PARAMETER_FIELD_TIME("field_time");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<ServicesCSVFileFormat>::Files MultipleFileTypesImporter<ServicesCSVFileFormat>::FILES(
			ServicesCSVFileFormat::Importer_::PATH_SERVICES.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool ServicesCSVFileFormat::Importer_::_checkPathsMap() const
		{
			return true;
		}



		ServicesCSVFileFormat::Importer_::Importer_(
			util::Env& env,
			const Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<ServicesCSVFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_interactive(true),
			_lines(*import.get<DataSource>(), env),
			_stopAreas(*import.get<DataSource>(), env),
			_stopPoints(*import.get<DataSource>(), env)
		{}



		bool ServicesCSVFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			ifstream inFile;
			string line;

			DataSource& dataSource(*_import.get<DataSource>());

			if(key == PATH_SERVICES)
			{
				boost::filesystem::path path(filePath.string().c_str());
				std::set<std::string> schedulesFiles;

				if(boost::filesystem::is_directory(path)) {
					for(boost::filesystem::directory_iterator it(path), end; it != end; ++it) {
						if(boost::filesystem::is_regular_file(it->status())) {
							schedulesFiles.insert(it->path().filename().string());
						}
					}
				}
				else
				{
					_logError("Invalid path!");
					return false;
				}

				// RollingStock
				RollingStock* rollingStock;
				if(_rollingStock.get())
				{
					rollingStock = _rollingStock.get();
				}
				else
				{
					_logError("RollingStock not defined");
					return false;
				}

				boost::shared_ptr<const City> cityForStopAreaAutoGeneration;
				if(_defaultCity)
				{
					cityForStopAreaAutoGeneration = _defaultCity;
				}
				else
				{
					_logError("No default city");
					return false;
				}

				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);
				boost::shared_ptr<ImportableTableSync::ObjectBySource<CalendarTemplateTableSync> > calendarTemplates;
				calendarTemplates.reset(new ImportableTableSync::ObjectBySource<CalendarTemplateTableSync>(dataSource, _env));

				BOOST_FOREACH(const string& file, schedulesFiles)
				{
					ifstream inFile;
					string fileWithPath = filePath.string() + file;
					_logDebug(
						"Loading file "+ fileWithPath
					);
					inFile.open(fileWithPath.c_str());
					if(!inFile)
					{
						_logError(
							"Could no open the file "+ fileWithPath
						);
						return false;
					}
					// Ignore header lines
					for(int i = 0; i < _numberOfLinesToIgnore; i++)
					{
						if(getline(inFile, line))
							_loadLine(line);
						else
						{
							_logError(
								"Error with the number of lines to ignore : "+ lexical_cast<string>(_numberOfLinesToIgnore)
							);
							return false;
						}
					}

					string daysCalendarName;
					string periodCalendarName;
					string lineNameStr;
					string backwardStr;

					//TODO selection of attributs must be generic
					string fileStr = _replaceAllSubStrings(file, string(" _"), string("-"));
					vector<string> fields;
					split(fields, fileStr, is_any_of(string("_")));
					if(fields.size() < 4)
						return false;
					lineNameStr = fields[0];
					backwardStr = fields[1];
					periodCalendarName = fields[2];
					daysCalendarName = fields[3].substr(0,fields[3].size()-4);
					_logDebug(
						periodCalendarName +" ; "+ daysCalendarName +" ; "+ lineNameStr +" ;"+ backwardStr
					);

					bool backward = false;
					if(backwardStr == "RETOUR")
					{
						backward = true;
					}

					CommercialLine* commercialLine(
						_getLine(lines, lineNameStr, dataSource)
					);
					if(!commercialLine)
					{
						_logError(
							"No such line "+ lineNameStr
						);
						return false;
					}

					if(!_serviceNumberField)
					{
						_logError(
							"Service number field not defined"
						);
						return false;
					}
					else if(!_timeField)
					{
						_logError(
							"Time field not defined"
						);
						return false;
					}
					else if(!_stopNameField && !_stopCodeField)
					{
						_logError(
							"Stop name field or stop code field must be defined"
						);
						return false;
					}

					CalendarTemplate* periodCalendar(NULL);
					if(calendarTemplates->contains(periodCalendarName))
					{
						periodCalendar = *calendarTemplates->get(periodCalendarName).begin();
					}
					else
					{
						_logWarning(
							"Calendar <pre>\""+ periodCalendarName +"\"</pre> not found"
						);
					}

					CalendarTemplate* daysCalendar(NULL);
					if(calendarTemplates->contains(daysCalendarName))
					{
						daysCalendar = *calendarTemplates->get(daysCalendarName).begin();
					}
					else
					{
						_logWarning(
							"Calendar <pre>\""+ daysCalendarName +"\"</pre> not found"
						);
					}

					ServiceDetail serviceDetail;
					ServiceDetailVector serviceDetailVector;
					time_duration lastTd(minutes(0));
					serviceDetail.serviceNumber = "-1";

					while(getline(inFile, line))
					{
						_loadLine(line);
						string serviceNumberStr;
						string stopNameStr;
						string stopCodeStr;
						string timeStr;

						// Test if this line is correct
						if(_line.size() > *_serviceNumberField)
							serviceNumberStr = _getValue(*_serviceNumberField);
						else
							continue;
						if(_stopCodeField && _line.size() > *_stopCodeField)
							stopCodeStr = _getValue(*_stopCodeField);
						if(_stopNameField && _line.size() > *_stopNameField)
							stopNameStr = _getValue(*_stopNameField);

						if((_stopCodeField && _line.size() <= *_stopCodeField) && (_stopNameField && _line.size() <= *_stopNameField))
							continue;

						if(_line.size() > *_timeField)
							timeStr = _getValue(*_timeField);
						else
							continue;

						if(serviceNumberStr != serviceDetail.serviceNumber)
						{
							if(serviceDetail.serviceNumber != "-1")
							{
								// creation of the service if we have more than one
								// stop in it.
								if(serviceDetail.stops.size() > 1)
								{
									serviceDetailVector.push_back(serviceDetail);
								}
								serviceDetail.arrivalSchedules.clear();
								serviceDetail.departureSchedules.clear();
								serviceDetail.stops.clear();
								lastTd = minutes(0);
							}
							serviceDetail.serviceNumber = serviceNumberStr;
						}

						// Stop
						set<StopPoint*> stopPoints = _createOrUpdateStopWithStopAreaAutocreation(
							_stopPoints,
							_stopCodeField ? stopCodeStr : string(),
							_stopNameField ? stopNameStr : string(),
							optional<const StopPoint::Geometry*>(),
							*cityForStopAreaAutoGeneration.get(),
							optional<time_duration>(),
							dataSource,
							boost::optional<const graph::RuleUser::Rules&>()
						);

						if(stopPoints.empty())
						{
							_logError(
								"Physical stop not found "+ stopCodeStr +" - "+ stopNameStr
							);
							return false;
						}
						else
						{
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								stopPoints,
								optional<MetricOffset>(),
								true,
								true,
								true
							);
							serviceDetail.stops.push_back(stop);
						}

						// Time
						if(timeStr.size() > 4)
						{
							time_duration td(
								lexical_cast<int>(timeStr.substr(0,2)),
								lexical_cast<int>(timeStr.substr(3,2)),
								0
							);
							if(td < lastTd)
							{
								td += hours(24);
							}
							serviceDetail.departureSchedules.push_back(td - seconds(td.seconds()));
							serviceDetail.arrivalSchedules.push_back(td.seconds() ? td + seconds(60 - td.seconds()) : td);
							lastTd = td;
						}
					}

					BOOST_FOREACH(const ServiceDetail& serviceDetail, serviceDetailVector)
					{
						// Route
						JourneyPattern* route(
							_createOrUpdateRoute(
								*commercialLine,
								commercialLine->getShortName(),
								commercialLine->getShortName(),
								optional<const string&>(),
								optional<Destination*>(),
								optional<const RuleUser::Rules&>(),
								backward,
								rollingStock,
								serviceDetail.stops,
								dataSource,
								true,
								true,
								true,
								true
						)	);
						if(route == NULL)
						{
							_logError(
								"Failure at route creation ("+ commercialLine->getShortName() +")"
							);
							return false;
						}

						// Service
						ScheduledService* service(
							_createOrUpdateService(
								*route,
								serviceDetail.departureSchedules,
								serviceDetail.arrivalSchedules,
								serviceDetail.serviceNumber,
								dataSource
						)	);

						// Calendars
						if(service)
						{
							boost::shared_ptr<CalendarLink> serviceCalendarLink;

							// Search for existing CalendarLink
							CalendarLinkTableSync::SearchResult serviceCalendarLinks(
								CalendarLinkTableSync::Search(
									_env,
									service->getKey(),
									0)
							);
							if(!serviceCalendarLinks.empty())
							{
								BOOST_FOREACH(boost::shared_ptr<CalendarLink> scl, serviceCalendarLinks)
								{
									if((scl->getCalendarTemplate2() == periodCalendar) && (scl->getCalendarTemplate() == daysCalendar))
									{
										serviceCalendarLink = scl;
									}
								}
							}

							if(!serviceCalendarLink)
							{
								serviceCalendarLink = boost::shared_ptr<CalendarLink>(new CalendarLink(CalendarLinkTableSync::getId()));

								if(periodCalendar)
								{
									serviceCalendarLink->setCalendarTemplate2(periodCalendar);
								}
								else
								{
									_logWarning(
										"Calendar <pre>\""+ periodCalendarName +"\"</pre> not found"
									);
								}
								if(daysCalendar)
								{
									serviceCalendarLink->setCalendarTemplate(daysCalendar);
								}
								else
								{
									_logWarning(
										"Calendar <pre>\""+ daysCalendarName +"\"</pre> not found"
									);
								}

								serviceCalendarLink->setCalendar(service);

								service->addCalendarLink(*serviceCalendarLink,true);

								_env.getEditableRegistry<CalendarLink>().add(boost::shared_ptr<CalendarLink>(serviceCalendarLink));
							}

						}
						else
						{
							_logError(
								"Failure at service creation ("+ serviceDetail.serviceNumber +")"
							);
							return false;
						}
					}
				}
			}

			return true;
		}



		db::DBTransaction ServicesCSVFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(cstop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CalendarLink>::value_type link, _env.getRegistry<CalendarLink>())
			{
				CalendarLinkTableSync::Save(link.second.get(), transaction);
			}

			return transaction;
		}



		std::string ServicesCSVFileFormat::Importer_::_getValue( const std::size_t field ) const
		{
			return trim_copy(_line[field]);
		}



		void ServicesCSVFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();
			if(!line.empty())
			{
				string utfline(
					line[line.size() - 1] == '\r' ?
					line.substr(0, line.size() - 1) :
					line
				);
				utfline = IConv(_import.get<DataSource>()->get<Charset>(), "UTF-8").convert(line);
				split(_line, utfline, is_any_of(SEP));
			}
		}



		std::string ServicesCSVFileFormat::Importer_::_replaceAllSubStrings(
			const std::string source,
			const std::string& replaceWhat,
			const std::string& replaceWithWhat
		) const {
			string result = source;
			while(1)
			{
				const int pos = result.find(replaceWhat);
				if(pos == -1)
				{
					break;
				}
				result.replace(pos, replaceWhat.size(), replaceWithWhat);
			}
			return result;
		}



		util::ParametersMap ServicesCSVFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());

			if(_rollingStock.get())
				map.insert(PARAMETER_ROLLING_STOCK_ID, _rollingStock->getKey());
			if(_defaultCity.get())
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			if(_serviceNumberField)
				map.insert(PARAMETER_FIELD_SERVICE_NUMBER, *_serviceNumberField);
			if(_stopCodeField)
				map.insert(PARAMETER_FIELD_STOP_CODE, *_stopCodeField);
			if(_stopNameField)
				map.insert(PARAMETER_FIELD_STOP_NAME, *_stopNameField);
			if(_timeField)
				map.insert(PARAMETER_FIELD_TIME, *_timeField);
			map.insert(PARAMETER_NUMBER_OF_LINES_TO_IGNORE, _numberOfLinesToIgnore);
			map.insert(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks));
			return map;
		}



		void ServicesCSVFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			_numberOfLinesToIgnore = map.getDefault<int>(PARAMETER_NUMBER_OF_LINES_TO_IGNORE,0);
			_serviceNumberField = map.getOptional<size_t>(PARAMETER_FIELD_SERVICE_NUMBER);
			_stopCodeField = map.getOptional<size_t>(PARAMETER_FIELD_STOP_CODE);
			_stopNameField = map.getOptional<size_t>(PARAMETER_FIELD_STOP_NAME);
			_timeField = map.getOptional<size_t>(PARAMETER_FIELD_TIME);

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID, 0))
			{
				_rollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID), _env);
			}
			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}

			string ptUseRuleBlockMasksStr(map.getDefault<string>(PARAMETER_USE_RULE_BLOCK_ID_MASK));
			if(!ptUseRuleBlockMasksStr.empty())
			{
				vector<string> rules;
				split(rules, ptUseRuleBlockMasksStr, is_any_of(","));
				BOOST_FOREACH(const string& rule, rules)
				{
					vector<string> parts;
					split(parts, rule, is_any_of("="));

					if(parts.size() < 2)
					{
						continue;
					}

					try
					{
						boost::shared_ptr<const PTUseRule> ptUseRule(
							PTUseRuleTableSync::Get(
								lexical_cast<RegistryKeyType>(parts[1]),
								_env
						)	);
						_ptUseRuleBlockMasks.insert(make_pair(parts[0], ptUseRule.get()));
					}
					catch (...)
					{
					}
				}
			}

		}



		std::string ServicesCSVFileFormat::Importer_::_serializePTUseRuleBlockMasks( const PTUseRuleBlockMasks& object )
		{
			bool first(true);
			stringstream serializedPTUseRuleBlockMasks;
			BOOST_FOREACH(const PTUseRuleBlockMasks::value_type& rule, object)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					serializedPTUseRuleBlockMasks << ",";
				}
				serializedPTUseRuleBlockMasks << rule.first << "=" << rule.second->getKey();
			}
			return serializedPTUseRuleBlockMasks.str();
		}
	}
}
