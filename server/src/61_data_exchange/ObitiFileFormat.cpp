
/** ObitiFileFormat class implementation.
	@file ObitiFileFormat.cpp

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

#include "ObitiFileFormat.hpp"

#include "Import.hpp"
#include "Importer.hpp"
#include "ImpExModule.h"
#include "IConv.hpp"
#include "DataSource.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "RollingStock.hpp"
#include "CalendarLink.hpp"
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
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace server;
	using namespace geography;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,ObitiFileFormat>::FACTORY_KEY("Obiti");
	}

	namespace data_exchange
	{
		const std::string ObitiFileFormat::Importer_::FILE_ARRET("arrets");
		const std::string ObitiFileFormat::Importer_::FILE_ITINERAIRES("itineraires");
		const std::string ObitiFileFormat::Importer_::FILE_LIGNE("ligne");

		const std::string ObitiFileFormat::Importer_::PATH_HORAIRES("horaires");

		const std::string ObitiFileFormat::Importer_::SEP(";");

		const std::string ObitiFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID("rolling_stock_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string ObitiFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOPS_DATA_SOURCE_ID("stops_data_source_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOPS_FROM_DATA_SOURCE("stops_from_data_source");

		const std::string ObitiFileFormat::Importer_::PARAMETER_PERIOD_CALENDAR_FIELD("period_calendar_field");
		const std::string ObitiFileFormat::Importer_::PARAMETER_DAYS_CALENDAR_FIELD("days_calendar_field");
		const std::string ObitiFileFormat::Importer_::PARAMETER_NUMBER_OF_OTHER_PARAMETERS("nb_other_parameters");
		const std::string ObitiFileFormat::Importer_::PARAMETER_BACKWARD_IN_SAME_FILE("backward_in_same_file");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<ObitiFileFormat>::Files MultipleFileTypesImporter<ObitiFileFormat>::FILES(
			ObitiFileFormat::Importer_::FILE_LIGNE.c_str(),
			ObitiFileFormat::Importer_::FILE_ARRET.c_str(),
			ObitiFileFormat::Importer_::FILE_ITINERAIRES.c_str(),
			ObitiFileFormat::Importer_::PATH_HORAIRES.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool ObitiFileFormat::Importer_::_checkPathsMap() const
		{
			/*
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_LIGNE));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it= _pathsMap.find(FILE_ARRET);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ITINERAIRES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			*/
			return true;
		}



		ObitiFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<ObitiFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_interactive(true),
			_lines(*_import.get<DataSource>(), env),
			_stopAreas(*_import.get<DataSource>(), env),
			_stopPoints(*_import.get<DataSource>(), env)
		{}



		bool ObitiFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			ifstream inFile;
			string line;

			DataSource& dataSource(*_import.get<DataSource>());

			if(key != PATH_HORAIRES)
			{
				inFile.open(filePath.string().c_str());
				if(!inFile)
				{
					throw Exception("Could no open the file " + filePath.string());
				}

				if(!getline(inFile, line))
				{
					return false;
				}
				_loadFieldsMap(line);
				_logDebug(
					"Loading file "+ filePath.string() +" as "+ key
				);
			}
			else
			{
				_logDebug(
					"Loading path "+ filePath.string() +" as "+ key
				);
			}


			// 1 : lines
			if(key == FILE_LIGNE)
			{
				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					LineObitiElement lineObiti;

					string idLigne = _getValue("IDLigne");
					lineObiti.name = _getValue("Nom");
					lineObiti.shortname = _getValue("Code Commerciale");
					lineObiti.backward = _getValue("Sens") == "Retour" ? true : false;
					_linesMap.insert(make_pair(idLigne, lineObiti));
				}
			}

			// 2 : Stop Areas
			if(key == FILE_ARRET)
			{
				PTFileFormat::ImportableStopAreas linkedStopAreas;
				PTFileFormat::ImportableStopAreas nonLinkedStopAreas;
				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					string id(_getValue("IDArret"));
					string name(_getValue("Appellation Commerciale1"));
					string cityName(_getValue("Commune"));

					// City
					City* city(NULL);
					CityTableSync::SearchResult cities(
						CityTableSync::Search(_env, cityName, optional<string>(), optional<string>(), 0, 1)
					);
					if(cities.empty())
					{
						_logWarning(
							"City "+ cityName +" not found"
						);
					}
					else
					{
						city = cities.begin()->get();
					}

					PTFileFormat::ImportableStopArea isa;
					isa.operatorCode = id;
					isa.name = name;
					isa.linkedStopAreas = _stopAreas.get(id);
					isa.cityName = cityName;

					if(isa.linkedStopAreas.empty())
					{
						nonLinkedStopAreas.push_back(isa);
					}

					_createOrUpdateStopAreas(
						_stopAreas,
						id,
						name,
						city,
						false,
						_stopAreaDefaultTransferDuration,
						dataSource
					);
				}
			}

			// 3 : Physical Stops
			if(key == FILE_ITINERAIRES)
			{
				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					// Line
					string idLigne(_getValue("idLigne"));
					LinesMap::const_iterator it(_linesMap.find(idLigne));
					if(it == _linesMap.end())
					{
						_logWarning(
							"Obiti line ID "+ idLigne +" not found"
						);
						return false;
					}
					LineObitiElement lineObiti(it->second);

					string stopAreaId(_getValue("idarret"));
					string id(_getValue("nom_commune") + " " + _getValue("nom_arret") + " " + lineObiti.shortname + " " + (lineObiti.backward ? "R" : "A"));
					string name(_getValue("app_comm_arret"));

					// Stop area
					const StopArea* stopArea(NULL);
					if(_stopAreas.contains(stopAreaId))
					{
						stopArea = *_stopAreas.get(stopAreaId).begin();
					}
					else
					{
						_logWarning(
							"Inconsistent stop area id "+ stopAreaId +" in the stop point "+ id
						);
						continue;
					}

					// Point
					boost::shared_ptr<geos::geom::Point> point;

					PTFileFormat::ImportableStopPoint isp;
					isp.name = name;
					isp.cityName = _getValue("nom_commune");
					isp.linkedStopPoints = _stopPoints.get(id);
					isp.stopArea = stopArea;
					isp.coords = point;

					if(isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.insert(
							make_pair(id, isp)
						);
					}

					// Creation or update
					_createOrUpdateStop(
						_stopPoints,
						id,
						name,
						NULL,
						stopArea,
						point.get(),
						dataSource
					);
				}
			}

			// 4 : schedules path
			if(key == PATH_HORAIRES)
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
					_logError(
						"RollingStock not defined"
					);
					return false;
				}

				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);
				boost::shared_ptr<ImportableTableSync::ObjectBySource<StopPointTableSync> > stopPointsFromOtherImport;
				if(_stopsFromDataSource && _stopsDataSource)
					stopPointsFromOtherImport.reset(new ImportableTableSync::ObjectBySource<StopPointTableSync>(*_stopsDataSource, _env));

				int numSens = 1;
				if(_backwardInSameFile)
					numSens = 2;

				BOOST_FOREACH(const string& file, schedulesFiles)
				{
					ifstream inFile;
					string fileWithPath = filePath.string() + file;
					inFile.open(fileWithPath.c_str());
					if(!inFile)
					{
						_logError(
							"Could no open the file "+ fileWithPath
						);
						return false;
					}

					for(int i=1; i<=numSens; i++)
					{
						bool backward=false;
						if((numSens == 1 && file[file.size()-5] == 'R') || (numSens == 2 && i == 2))
							backward = true;

						streampos posSchedulesTable = inFile.tellg();
						if(i == 2)
						{
							// move to second Schedules table
							while(_line[0] != "")
							{
								getline(inFile, line);
								_loadLine(line);
							}
							while(_line[0] == "")
							{
								posSchedulesTable = inFile.tellg();
								getline(inFile, line);
								_loadLine(line);
							}
						}

						CommercialLine* commercialLine(
							_getLine(lines, file, dataSource)
						);
						if(!commercialLine)
						{
							_logError(
								"No such line "+ file
							);
							return false;
						}

						// Routes
						string serviceID;
						string daysCalendarName;
						string periodCalendarName;

						ScheduledService::Schedules departureSchedules;
						ScheduledService::Schedules arrivalSchedules;
						time_duration lastTd(minutes(0));

						// Number of services
						_firstLine(inFile, line, posSchedulesTable);

						_logDebug(
							"Number of services : "+ lexical_cast<string>(_line.size())
						);

						for(size_t numService = 1; numService < _line.size(); numService++)
						{
							_firstLine(inFile, line, posSchedulesTable);

							// Get Period Calendar name
							if(!_moveToField(inFile,_periodCalendarField))
							{
								_logError(
									"No such period field "+ _periodCalendarField
								);
								return false;
							}
							periodCalendarName = _line[numService] + " " + commercialLine->getNetwork()->getName();

							// Get Days Calendar name
							if(!_moveToField(inFile,_daysCalendarField))
							{
								_logError(
									"No such period field "+ _daysCalendarField
								);
								return false;
							}
							daysCalendarName = _line[numService] + " " + commercialLine->getNetwork()->getName();

							// Ignore other parameters
							for(size_t i = 0; i < _numberOfOtherParameters; i++)
							{
								if(getline(inFile, line))
									_loadLine(line);
								else
								{
									_logError(
										"Error with the number of other parameters : "+ lexical_cast<string>(_numberOfOtherParameters)
									);
									return false;
								}
							}

							JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
							departureSchedules.clear();
							arrivalSchedules.clear();
							lastTd = minutes(0);

							// Schedule
							while(getline(inFile, line))
							{
								_loadLine(line);
								if(_line[0] == "")
									break;
								string stopPointId(to_lower_copy(trim_copy(_line[0])) + " " + commercialLine->getShortName() + " " + (backward ? "R" : "A"));
								string timeStr(_line[numService]);

								JourneyPattern::StopWithDepartureArrivalAuthorization::StopsSet stopPoints;
								if(_stopPoints.contains(stopPointId))
									stopPoints = _stopPoints.get(stopPointId);
								else if(stopPointsFromOtherImport)
								{
									// TODO Add a generic function
									string stopPointName = to_lower_copy(trim_copy(_line[0]));
									replace_all(stopPointName,"é","e");
									replace_all(stopPointName,"è","e");
									replace_all(stopPointName,"ê","e");
									replace_all(stopPointName,"ë","e");
									replace_all(stopPointName,"â","a");
									replace_all(stopPointName,"à","a");
									replace_all(stopPointName,"ô","o");
									replace_all(stopPointName,"'"," ");
									replace_all(stopPointName,"-"," ");
									replace_all(stopPointName,"*","");

									string stopPointId(stopPointName + " " + (backward ? "R" : "A"));
									string stopPointId2(stopPointName);
									string stopPointId3(stopPointName + " " + (backward ? "A" : "R"));
									if(stopPointsFromOtherImport->contains(stopPointId))
									{
										stopPoints = stopPointsFromOtherImport->get(stopPointId);
										_logDebug(
											"Stop id "+ stopPointId +" found in the service "+ lexical_cast<string>(numService)
										);
									}
									else if(stopPointsFromOtherImport->contains(stopPointId2))
									{
										stopPoints = stopPointsFromOtherImport->get(stopPointId2);
										_logDebug(
											"Stop id "+ stopPointId2 +" found in the service "+ lexical_cast<string>(numService)
										);
									}
									else if(stopPointsFromOtherImport->contains(stopPointId3))
									{
										stopPoints = stopPointsFromOtherImport->get(stopPointId3);
										_logDebug(
											"Stop id "+ stopPointId3 +" found in the service "+ lexical_cast<string>(numService)
										);
									}
									else
									{
										_logError(
											"Stop id "+ stopPointId +"/"+ stopPointId2 +" not found in the service "+ lexical_cast<string>(numService)
										);
										return false;
									}
								}
								else
								{
									_logError(
										"Inconsistent stop id "+ stopPointId +" in the service "+ lexical_cast<string>(numService)
									);
									return false;
								}

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
									departureSchedules.push_back(td - seconds(td.seconds()));
									arrivalSchedules.push_back(td.seconds() ? td + seconds(60 - td.seconds()) : td);
									lastTd = td;

									JourneyPattern::StopWithDepartureArrivalAuthorization stop(
										stopPoints,
										optional<double>(),
										true,
										true,
										true
									);
									stops.push_back(stop);
								}
							}

							if(stops.size() > 0)
							{
								_logDebug(
									"Service number "+ lexical_cast<string>(numService)
								);

								CalendarTemplate* periodCalendar(NULL);
								CalendarTemplateTableSync::SearchResult calendars(
									CalendarTemplateTableSync::Search(
										_env,
										periodCalendarName,
										optional<RegistryKeyType>(),
										false,
										true,
										0,
										1,
										UP_LINKS_LOAD_LEVEL,
										optional<RegistryKeyType>())
								);
								if(calendars.empty())
								{
									_logWarning(
										"Calendar <pre>\""+ periodCalendarName +"\"</pre> not found"
									);
								}
								else
								{
									periodCalendar = calendars.begin()->get();
								}


								CalendarTemplate* daysCalendar(NULL);
								CalendarTemplateTableSync::SearchResult calendars2(
									CalendarTemplateTableSync::Search(
										_env,
										daysCalendarName,
										optional<RegistryKeyType>(),
										false,
										true,
										0,
										1,
										UP_LINKS_LOAD_LEVEL,
										optional<RegistryKeyType>()
								)	);
								if(calendars2.empty())
								{
									_logWarning(
										"Calendar <pre>\""+ daysCalendarName +"\"</pre> not found"
									);
								}
								else
								{
									daysCalendar = calendars2.begin()->get();
								}

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
										stops,
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
										departureSchedules,
										arrivalSchedules,
										serviceID,
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
											0
									)	);
									if(!serviceCalendarLinks.empty())
									{
										BOOST_FOREACH(const boost::shared_ptr<CalendarLink>& scl, serviceCalendarLinks)
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
										"Failure at service creation ("+ serviceID +")"
									);
									return false;
								}
							}
						}
					}
				}
			}
			return true;
		}



		db::DBTransaction ObitiFileFormat::Importer_::_save() const
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



		void ObitiFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
		{
			vector<string> cols;
			split(
				cols,
				line[line.size() - 1] == '\r' ? line.substr(0, line.size() - 1) : line,
				is_any_of(SEP)
			);
			_fieldsMap.clear();
			size_t rank(0);
			BOOST_FOREACH(const string& col, cols)
			{
				_fieldsMap[col] = rank;
				++rank;
			}
		}



		std::string ObitiFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void ObitiFileFormat::Importer_::_loadLine( const std::string& line ) const
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



		void ObitiFileFormat::Importer_::_firstLine(ifstream& inFile, std::string& line, streampos pos) const
		{
			inFile.clear();
			inFile.seekg(pos);
			getline(inFile, line);
			_loadLine(line);
		}



		bool ObitiFileFormat::Importer_::_moveToField(ifstream& inFile, const std::string& field) const
		{
			if(_line[0] == field)
				return true;
			else
			{
				std::string line;
				bool notTheEnd;
				do {
					notTheEnd = (bool) getline(inFile, line);
					if(notTheEnd) _loadLine(line);
				} while(_line[0] != field && notTheEnd);
				if((_line[0] != field) || !notTheEnd)
					return false;
				else return true;
			}
		}



		util::ParametersMap ObitiFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());

			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			if(_rollingStock.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID, _rollingStock->getKey());
			}
			if(_stopsDataSource.get())
			{
				map.insert(PARAMETER_STOPS_DATA_SOURCE_ID, _stopsDataSource->getKey());
			}
			map.insert(PARAMETER_DAYS_CALENDAR_FIELD, _daysCalendarField);
			map.insert(PARAMETER_PERIOD_CALENDAR_FIELD, _periodCalendarField);
			map.insert(PARAMETER_NUMBER_OF_OTHER_PARAMETERS, _numberOfOtherParameters);
			map.insert(PARAMETER_BACKWARD_IN_SAME_FILE, _backwardInSameFile);
			map.insert(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks));
			map.insert(PARAMETER_STOPS_FROM_DATA_SOURCE, _stopsFromDataSource);
			return map;
		}



		void ObitiFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_daysCalendarField = map.getDefault<string>(PARAMETER_DAYS_CALENDAR_FIELD);
			_periodCalendarField = map.getDefault<string>(PARAMETER_PERIOD_CALENDAR_FIELD);
			_numberOfOtherParameters = map.getDefault<size_t>(PARAMETER_NUMBER_OF_OTHER_PARAMETERS,0);
			_backwardInSameFile = map.getDefault<bool>(PARAMETER_BACKWARD_IN_SAME_FILE,0);
			_stopsFromDataSource = map.getDefault<bool>(PARAMETER_STOPS_FROM_DATA_SOURCE,0);

			if(_stopsFromDataSource)
				if(map.getDefault<RegistryKeyType>(PARAMETER_STOPS_DATA_SOURCE_ID, 0))
				{
					_stopsDataSource = DataSourceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_STOPS_DATA_SOURCE_ID), _env);
				}

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID, 0))
			{
				_rollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID), _env);
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



		std::string ObitiFileFormat::Importer_::_serializePTUseRuleBlockMasks( const PTUseRuleBlockMasks& object )
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
