
/** HeuresFileFormat class implementation.
	@file HeuresFileFormat.cpp

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

#include "HeuresFileFormat.hpp"

#include "DataSource.h"
#include "NonConcurrencyRuleTableSync.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "TreeFolder.hpp"
#include "TreeFolderTableSync.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "DBTransaction.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "Calendar.h"
#include "ImportFunction.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "ImportableTableSync.hpp"
#include "PTFileFormat.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "IConv.hpp"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "TransportNetworkTableSync.h"
#include "RollingStockTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "RequestException.h"
#include "CalendarTemplateTableSync.h"
#include "DestinationTableSync.hpp"
#include "DataSourceTableSync.h"
#include "ZipWriter.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace data_exchange;
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace road;
	using namespace admin;
	using namespace geography;
	using namespace db;
	using namespace graph;
	using namespace calendar;
	using namespace server;
	using namespace html;
	using namespace tree;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HeuresFileFormat>::FACTORY_KEY("Heures");
	}

	namespace data_exchange
	{
		const std::string HeuresFileFormat::Importer_::FILE_POINTSARRETS("pointsarrets");
		const std::string HeuresFileFormat::Importer_::FILE_ITINERAI("itinerai");
		const std::string HeuresFileFormat::Importer_::FILE_TRONCONS("troncons");
		const std::string HeuresFileFormat::Importer_::FILE_SERVICES("services");

		const std::string HeuresFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const std::string HeuresFileFormat::Importer_::PARAMETER_NETWORK_ID("network_id");
		const std::string HeuresFileFormat::Importer_::PARAMETER_DAY7_CALENDAR_ID("day7_calendar_id");
		const std::string HeuresFileFormat::Importer_::PARAMETER_STOPS_DATASOURCE_ID("stops_datasource_id");

		const std::string HeuresFileFormat::Exporter_::PARAMETER_DATASOURCE_ID = "datasource_id";
		const std::string HeuresFileFormat::Exporter_::PARAMETER_NETWORK_ID = "network_id";
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HeuresFileFormat>::Files MultipleFileTypesImporter<HeuresFileFormat>::FILES(
			HeuresFileFormat::Importer_::FILE_POINTSARRETS.c_str(),
			HeuresFileFormat::Importer_::FILE_ITINERAI.c_str(),
			HeuresFileFormat::Importer_::FILE_TRONCONS.c_str(),
			HeuresFileFormat::Importer_::FILE_SERVICES.c_str(),
		"");
	}

	namespace data_exchange
	{
		bool HeuresFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ITINERAI));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_TRONCONS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_SERVICES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_POINTSARRETS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		DBTransaction HeuresFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);
			BOOST_FOREACH(Registry<CommercialLine>::value_type line, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type route, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(route.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				if(service.second->empty())
				{
					DBModule::GetDB()->deleteStmt(service.second->getKey(), transaction);
				}
				else
				{
					ScheduledServiceTableSync::Save(service.second.get(), transaction);
				}
			}
			return transaction;
		}



		bool HeuresFileFormat::Importer_::_parse(
			const path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const server::Request&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}

			if(_calendar.empty())
			{
				throw RequestException("Base calendar must be non empty");
			}

			if(key == FILE_POINTSARRETS)
			{
				string line;

				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;
				const DataSource& stopsDataSource(_stopsDataSource.get() ? *_stopsDataSource : _dataSource);
				ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(stopsDataSource, _env);
				ImportableTableSync::ObjectBySource<DestinationTableSync> destinations(stopsDataSource, _env);

				while(getline(inFile, line))
				{
					if(!_dataSource.getCharset().empty())
					{
						line = IConv(stopsDataSource.getCharset(), "UTF-8").convert(line);
					}

					string id(boost::algorithm::trim_copy(line.substr(0, 4)));
					if(lexical_cast<int>(id) > 9000)
					{
						string destinationCode(line.substr(5,4));
						set<Destination*> destinationSet(destinations.get(destinationCode));
						if(destinationSet.empty())
						{
							stream << "WARN : The destination " << destinationCode << " was not found in the database<br/>";
						}
						else
						{
							_destinations[lexical_cast<int>(id)] = *destinationSet.begin();
						}
						continue;
					}

					// Avoid depots
					if(boost::algorithm::trim_copy(line.substr(55, 3)) == "DEP")
					{
						continue;
					}

					string name(boost::algorithm::trim_copy(line.substr(5, 50)));

					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = id;
					isp.name = name;
					isp.linkedStopPoints = stopPoints.get(id);

					if(isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.push_back(isp);
					}
					else if(_displayLinkedStops)
					{
						linkedStopPoints.push_back(isp);
					}
				}
				inFile.close();

				if(request)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						nonLinkedStopPoints,
						*request,
						_env,
						stopsDataSource,
						stream
					);
					if(_displayLinkedStops)
					{
						PTFileFormat::DisplayStopPointImportScreen(
							linkedStopPoints,
							*request,
							_env,
							stopsDataSource,
							stream
						);
					}
				}
				if(!nonLinkedStopPoints.empty())
				{
					stream << "ERR  : At least a stop could not be linked.<br/>";
					return false;
				}
			}
			if(key == FILE_ITINERAI) // 1 : Routes
			{
				if(!_network.get())
				{
					stream << "ERR  : The transport network was not specified.<br/>";
					return false;
				}

				// Bus
				RollingStockTableSync::SearchResult rollingstock(RollingStockTableSync::Search(_env, string("Bus")));
				if(rollingstock.empty())
				{
					stream << "ERR  : The bus transport mode is not registered in the table 49.<br />";
					return false;
				}
				RollingStock* bus(rollingstock.front().get());

				// Load of the stops
				const DataSource& stopsDataSource(_stopsDataSource.get() ? *_stopsDataSource : _dataSource);
				ImportableTableSync::ObjectBySource<StopPointTableSync> stops(stopsDataSource, _env);
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);

				// Parsing the file
				string line;
				CommercialLine* cline(NULL);
				while(getline(inFile, line))
				{
					// Length control
					if(line.length() < 30)
					{
						continue;
					}

					// Route number
					string routeNumber(trim_copy(line.substr(7,2)));
					int technicalLineNumber(lexical_cast<int>(trim_copy(line.substr(4, 3))));

					// Route type
					int routeType(lexical_cast<int>(line.substr(9,1)));
					if(routeType != 0 && routeType != 1)
					{
						_technicalRoutes.insert(
							make_pair(technicalLineNumber, routeNumber)
						);
						continue;
					}

					// Commercial line number
					int commercialLineNumber(lexical_cast<int>(trim_copy(line.substr(0, 4))));

					cline = PTFileFormat::CreateOrUpdateLine(
						lines,
						lexical_cast<string>(commercialLineNumber),
						string(),
						lexical_cast<string>(commercialLineNumber),
						optional<RGBColor>(),
						*_network,
						_dataSource,
						_env,
						stream
					);

					// Stops
					JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
					MetricOffset distance(0);
					bool ignoreRoute(false);
					Destination* destination(NULL);
					for(size_t i(10); i+1<line.size(); i += 10)
					{
						if(line.size() < i+9)
						{
							stream << "WARN : inconsistent line size " << line << "<br />";
							ignoreRoute = true;
							break;
						}

						// Stop search
						string stopNumber(trim_copy(line.substr(i,4)));

						// Fake stop : girouette
						if(lexical_cast<int>(stopNumber) > 9000)
						{
							*servedStops.rbegin()->_metricOffset += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));
							DestinationsMap::const_iterator it(_destinations.find(lexical_cast<int>(stopNumber)));
							if(it != _destinations.end())
							{
								destination = it->second;
							}
							else
							{
								stream << "WARN : destination " << stopNumber << " was not registered.<br />";
							}
							continue;
						}

						bool regul(lexical_cast<bool>(line.substr(i+4,1)));
						distance += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));

						if(!stops.contains(stopNumber))
						{
							stream << "WARN : stop " << stopNumber << " not found<br />";
							ignoreRoute = true;
						}

						servedStops.push_back(
							JourneyPattern::StopWithDepartureArrivalAuthorization(
								stops.get(stopNumber),
								distance,
								true,
								true,
								regul
						)	);
					}

					if(ignoreRoute)
					{
						continue;
					}

					// Route identification
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							*cline,
							optional<const string&>(),
							optional<const string&>(),
							optional<const string&>(),
							destination,
							optional<const RuleUser::Rules&>(),
							routeType == 1,
							bus,
							servedStops,
							_dataSource,
							_env,
							stream,
							true,
							true
					)	);

					_routes.insert(
						make_pair(
							make_pair(technicalLineNumber, routeNumber),
							route
					)	);
				}
			} // 2 : Nodes
			else if(key == FILE_TRONCONS)
			{
				string line;
				typedef map<
					pair<JourneyPattern*, string>, // string is service number
					pair<
						pair<ScheduledService::Schedules, ScheduledService::Schedules>, // departure / arrival schedules
						vector<pair<int, int> > // technical line, elementary service number
					>
				> SchedulesMap;
				SchedulesMap services;

				// Reading of the file
				while(getline(inFile, line))
				{
					int lineNumber(lexical_cast<int>(trim_copy(line.substr(0,3))));
					pair<int, int> lineKey(
						make_pair(
							lineNumber,
							lexical_cast<int>(trim_copy(line.substr(3,3)))
					)	);

					for(size_t i(29); i+1<line.size(); ++i)
					{
						string routeNumber(trim_copy(line.substr(i,2)));
						RoutesMap::iterator it(_routes.find(make_pair(lineNumber, routeNumber)));
						if(it == _routes.end())
						{
							if(_technicalRoutes.find(make_pair(lineNumber, routeNumber)) == _technicalRoutes.end())
							{
								stream << "WARN : route not found in service file " << lineNumber << "/" << routeNumber << "<br />";
							}
							for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
							continue;
						}
						JourneyPattern* route(it->second);

						string serviceNumber(trim_copy(line.substr(i+8,3)));
						SchedulesMap::iterator itS(services.find(make_pair(route, serviceNumber)));
						if(itS != services.end())
						{
							if(itS->first.first != route)
							{
								stream << "WARN : inconsistent route in service file " << serviceNumber << "/" << lineNumber << "/" << routeNumber << "<br />";
								for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
								continue;
							}
						}
						else
						{
							itS = services.insert(
								make_pair(
									make_pair(route, serviceNumber),
									SchedulesMap::mapped_type()
							)	).first;
							size_t schedulesNumber(route->getScheduledStopsNumber());
							for(size_t s(0); s<schedulesNumber; ++s)
							{
								itS->second.first.first.push_back(time_duration(not_a_date_time));
								itS->second.first.second.push_back(time_duration(not_a_date_time));
							}
						}

						// Register the line key
						itS->second.second.push_back(lineKey);

						// Read the available schedules
						size_t rank(0);
						for(i+=11; i<line.size() && line[i]!=';'; i+=8, ++rank)
						{
							string arrivalSchedule(line.substr(i, 4));
							string departureSchedule(line.substr(i+4, 4));

							if(departureSchedule != "9999")
							{
								itS->second.first.first[rank] = time_duration(
									lexical_cast<int>(departureSchedule.substr(0,2)),
									lexical_cast<int>(departureSchedule.substr(2,2)),
									0
								);
							}
							if(arrivalSchedule != "9999")
							{
								itS->second.first.second[rank] = time_duration(
									lexical_cast<int>(arrivalSchedule.substr(0,2)),
									lexical_cast<int>(arrivalSchedule.substr(2,2)),
									0
								);
							}
						}
				}	}

				// Storage as ScheduledService
				BOOST_FOREACH(const SchedulesMap::value_type& it, services)
				{
					JourneyPattern* route(it.first.first);

					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							it.second.first.first,
							it.second.first.second,
							it.first.second,
							_dataSource,
							_env,
							stream
					)	);

					if(service == NULL)
					{
						continue;
					}

					BOOST_FOREACH(const SchedulesMap::mapped_type::second_type::value_type& itKey, it.second.second)
					{
						_services[itKey].push_back(service);
					}
				}
			} // 3 : Services
			else if (key == FILE_SERVICES)
			{
				if(_calendar.empty())
				{
					stream << "ERR  : Start date or end date not defined<br />";
					return false;
				}

				string line;
				Calendar day7;
				if(_day7CalendarTemplate.get())
				{
					day7 = _day7CalendarTemplate->getResult(_calendar);
				}

				while(getline(inFile, line))
				{
					// Read of calendar
					vector<bool> days(7, false);
					for(size_t i(0); i<7; ++i)
					{
						days[i] = (line[i==0 ? 12 : i+5] == '1');
					}
					Calendar cal;
					BOOST_FOREACH(const date& d, _calendar.getActiveDates())
					{
						if(	((day7.isActive(d) || d.day_of_week() == 0) && days[0]) ||
							(!day7.isActive(d) && d.day_of_week() != 0 && days[d.day_of_week()])
						){
							cal.setActive(d);
						}
					}

					// Services list
					for(size_t i(13); i+6<line.size(); i+=29)
					{
						int lineNumber(lexical_cast<int>(trim_copy(line.substr(i,3))));
						int serviceNumber(lexical_cast<int>(trim_copy(line.substr(i+3,3))));

						ServicesMap::iterator itS(_services.find(
								make_pair(lineNumber, serviceNumber)
						)	);
						if(itS == _services.end())
						{
							stream << "WARN : inconsistent service number " << lineNumber << "/" << serviceNumber << " in " << line << "<br />";
							continue;
						}

						BOOST_FOREACH(ScheduledService* service, itS->second)
						{
							*service |= cal;
						}
					}
				}
			}
			inFile.close();

			return true;
		}



		void HeuresFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const Request& request
		) const {

			stream << "<h1>Horaires</h1>";
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Effacer données anciennes", t.getForm().getOuiNonRadioInput(PARAMETER_CLEAN_OLD_DATA, false));
			stream << t.title("Données");
			stream << t.cell("Arrêts", t.getForm().getTextInput(_getFileParameterName(FILE_POINTSARRETS), _pathsMap[FILE_POINTSARRETS].file_string()));
			stream << t.cell("Itineraires", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAI), _pathsMap[FILE_ITINERAI].file_string()));
			stream << t.cell("Troncons", t.getForm().getTextInput(_getFileParameterName(FILE_TRONCONS), _pathsMap[FILE_TRONCONS].file_string()));
			stream << t.cell("Services", t.getForm().getTextInput(_getFileParameterName(FILE_SERVICES), _pathsMap[FILE_SERVICES].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Date début", t.getForm().getCalendarInput(PARAMETER_START_DATE, _calendar.empty() ? date(not_a_date_time) : _calendar.getFirstActiveDate()));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAMETER_END_DATE, _calendar.empty() ? date(not_a_date_time) : _calendar.getLastActiveDate()));
			stream << t.cell("Réseau", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Source de données arrêts (si différente)", t.getForm().getTextInput(PARAMETER_STOPS_DATASOURCE_ID, _stopsDataSource.get() ? lexical_cast<string>(_stopsDataSource->getKey()) : string()));
			stream << t.cell("Calendrier des jours fériés",
				t.getForm().getSelectInput(
					PARAMETER_DAY7_CALENDAR_ID,
					CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
					optional<RegistryKeyType>(_day7CalendarTemplate.get() ? _day7CalendarTemplate->getKey() : RegistryKeyType(0))
			)	);
			stream << t.close();
		}


		util::ParametersMap HeuresFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_day7CalendarTemplate.get())
			{
				map.insert(PARAMETER_DAY7_CALENDAR_ID, _day7CalendarTemplate->getKey());
			}
			if(_stopsDataSource.get())
			{
				map.insert(PARAMETER_STOPS_DATASOURCE_ID, _stopsDataSource->getKey());
			}
			return map;
		}



		void HeuresFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			// Network or folder
			if(map.getOptional<RegistryKeyType>(PARAMETER_NETWORK_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID));

				_network = TransportNetworkTableSync::GetEditable(id, _env);
			}
			if(map.getDefault<RegistryKeyType>(PARAMETER_DAY7_CALENDAR_ID, 0))
			{
				try
				{
					_day7CalendarTemplate = CalendarTemplateTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DAY7_CALENDAR_ID), _env);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar");
				}
			}
			if(map.getDefault<RegistryKeyType>(PARAMETER_STOPS_DATASOURCE_ID, 0))
			{
				try
				{
					_stopsDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOPS_DATASOURCE_ID), _env);
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					throw RequestException("No such data source for stops");
				}

			}
		}



		void HeuresFileFormat::Exporter_::build(
			std::ostream& os
		) const	{

			//////////////////////////////////////////////////////////////////////////
			// Load

			// Lines
			CommercialLineTableSync::Search(
				_env,
				_network.get() ? _network->getKey() : _folder->getKey()
			);
			
			// Journey patterns
			BOOST_FOREACH(Registry<CommercialLine>::value_type itline, _env.getRegistry<CommercialLine>())
			{
				JourneyPatternTableSync::Search(
					_env,
					itline.second->getKey()
				);

				NonConcurrencyRuleTableSync::Search(
					_env,
					itline.second->getKey(),
					itline.second->getKey(),
					false
				);
			}

			// Stops and services
			BOOST_FOREACH(Registry<JourneyPattern>::value_type itjp, _env.getRegistry<JourneyPattern>())
			{
				const JourneyPattern& line(*itjp.second);
				LineStopTableSync::Search(
					_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
				ScheduledServiceTableSync::Search(
					_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					optional<string>(),
					false,
					0,
					optional<size_t>(),
					true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
			}

			//////////////////////////////////////////////////////////////////////////
			// Output

			// Pointsarrets
			stringstream pointsarretsStream;
			bool oneStopWithoutCode(false);
			BOOST_FOREACH(Registry<StopPoint>::value_type itstop, _env.getRegistry<StopPoint>())
			{
				// Local variables
				const StopPoint& stop(*itstop.second);

				// Data source code
				vector<string> codes(stop.getCodesBySource(*_dataSource));
				if(codes.empty())
				{
					oneStopWithoutCode = true;
					os << "ERR  : Stop " << stop.getConnectionPlace()->getFullName() << "/" << stop.getName() << " has no code for the datasource" << endl;
					continue;
				}

				// Output
				IConv iconv("UTF-8", "CP1252");
				BOOST_FOREACH(const string& codeBySource, stop.getCodesBySource(*_dataSource))
				{
					pointsarretsStream << setw(4) << setfill(' ') << codeBySource;
					pointsarretsStream << "0";
					_writeTextAndSpaces(pointsarretsStream, iconv.convert(stop.getName().empty() ? stop.getConnectionPlace()->getName() : stop.getName()), 50);
					_writeTextAndSpaces(pointsarretsStream, iconv.convert(stop.getName().empty() ? stop.getConnectionPlace()->getName() : stop.getName()), 5);
					_writeTextAndSpaces(pointsarretsStream, string(), 4);
					_writeTextAndSpaces(pointsarretsStream, iconv.convert(stop.getConnectionPlace()->getCity()->getName()), 30);
					_writeTextAndSpaces(pointsarretsStream, string(), 15);
					pointsarretsStream << endl;
				}
			}

			// Break if one stop without code
			if(oneStopWithoutCode)
			{
				os << "STOP : Import broken, at least a stop without code for the datasource" << endl;
				return;
			}


			// Itinerai
			stringstream itineraiStream;
			BOOST_FOREACH(Registry<JourneyPattern>::value_type itjp, _env.getRegistry<JourneyPattern>())
			{
				bool hasArea(false);
				BOOST_FOREACH(Edge* edge, itjp.second->getEdges())
				{
					if(!dynamic_cast<DesignatedLinePhysicalStop*>(edge))
					{
						hasArea = true;
						continue;
					}
				}
				if(hasArea)
				{
					continue;
				}

				const JourneyPattern& jp(*itjp.second);
				_writeTextAndSpaces(itineraiStream, jp.getCommercialLine()->getCodeBySources(), 4);
				_writeTextAndSpaces(itineraiStream, jp.getCommercialLine()->getCodeBySources(), 3, false);
				_writeTextAndSpaces(itineraiStream, jp.getName(), 2, false);
				itineraiStream << jp.getWayBack();
				Path::Edges::const_iterator itLastEdge(jp.getEdges().end());
				for(Path::Edges::const_iterator itEdge(jp.getEdges().begin()); itEdge != jp.getEdges().end(); ++itEdge)
				{
					if(!dynamic_cast<DesignatedLinePhysicalStop*>(*itEdge))
					{
						continue;
					}

					const DesignatedLinePhysicalStop& ls(*static_cast<DesignatedLinePhysicalStop*>(*itEdge));
					const StopPoint& stop(*ls.getPhysicalStop());
					_writeTextAndSpaces(itineraiStream, *stop.getCodesBySource(*_dataSource).begin(), 4, false);
					itineraiStream << ls.getScheduleInput();
					if(itLastEdge == jp.getEdges().end())
					{
						itineraiStream << "    0";
					}
					else
					{
						itineraiStream << setw(5) << setfill(' ') << ((*itEdge)->getMetricOffset() - (*itLastEdge)->getMetricOffset());
					}
					itLastEdge = itEdge;
				}
				itineraiStream << endl;
			}

			// Troncons
			stringstream tronconsStream;

			BOOST_FOREACH(Registry<ScheduledService>::value_type itss, _env.getRegistry<ScheduledService>())
			{
				bool hasArea(false);
				BOOST_FOREACH(Edge* edge, itss.second->getPath()->getEdges())
				{
					if(!dynamic_cast<DesignatedLinePhysicalStop*>(edge))
					{
						hasArea = true;
						continue;
					}
				}
				if(hasArea)
				{
					continue;
				}

				const ScheduledService& ss(*itss.second);

				_writeTextAndSpaces(tronconsStream, static_cast<const JourneyPattern*>(ss.getPath())->getCommercialLine()->getCodeBySources(), 3, false);
				_writeTextAndSpaces(tronconsStream, ss.getServiceNumber(), 3, false);
				tronconsStream << "2";
				_writeTextAndSpaces(tronconsStream, *static_cast<StopPoint*>(ss.getPath()->getEdge(0)->getFromVertex())->getCodesBySource(*_dataSource).begin(), 4, false);
				_writeHour(tronconsStream, ss.getDepartureSchedule(false, 0));
				_writeTextAndSpaces(tronconsStream, *static_cast<StopPoint*>(ss.getPath()->getLastEdge()->getFromVertex())->getCodesBySource(*_dataSource).begin(), 4, false);
				_writeHour(tronconsStream, ss.getLastArrivalSchedule(false));
				tronconsStream << "000000";
				_writeTextAndSpaces(tronconsStream, static_cast<const JourneyPattern*>(ss.getPath())->getName(), 2, false);
				_writeTextAndSpaces(tronconsStream, static_cast<const JourneyPattern*>(ss.getPath())->getCommercialLine()->getCodeBySources(), 4);
				_writeTextAndSpaces(tronconsStream, ss.getServiceNumber(), 5, false, '0');

				// Schedules
				size_t schedulesNumber(ss.getDepartureSchedules(true, false).size());
				for(size_t i(0); i<schedulesNumber; ++i)
				{
					time_duration departureSchedule(ss.getDepartureSchedule(false, i));
					time_duration arrivalSchedule(ss.getArrivalSchedule(false, i));
					_writeHour(
						tronconsStream,
						(i== 0 || arrivalSchedule > departureSchedule) ? departureSchedule : arrivalSchedule
					);
					_writeHour(
						tronconsStream,
						(i+1 == schedulesNumber) ? arrivalSchedule : departureSchedule
					);
				}
				tronconsStream << ";" << endl;
			}

			// Services
			stringstream servicesStream;

			BOOST_FOREACH(Registry<ScheduledService>::value_type itss, _env.getRegistry<ScheduledService>())
			{
				bool hasArea(false);
				BOOST_FOREACH(Edge* edge, itss.second->getPath()->getEdges())
				{
					if(!dynamic_cast<DesignatedLinePhysicalStop*>(edge))
					{
						hasArea = true;
						continue;
					}
				}
				if(hasArea)
				{
					continue;
				}

				const ScheduledService& ss(*itss.second);

				_writeTextAndSpaces(servicesStream, ss.getServiceNumber(), 6, false);
				servicesStream << "1111111";
				_writeTextAndSpaces(servicesStream, static_cast<const JourneyPattern*>(ss.getPath())->getCommercialLine()->getCodeBySources(), 3, false);
				_writeTextAndSpaces(servicesStream, ss.getServiceNumber(), 3, false);
				servicesStream << "2";
				servicesStream << "0000000000000000000000";
				servicesStream << endl;
			}

			ZipWriter * zip = new ZipWriter(os);

			zip->Write("pointsarrets.tmp", pointsarretsStream);
			zip->Write("services.tmp", servicesStream);
			zip->Write("itinerai.tmp", itineraiStream);
			zip->Write("troncons.tmp", tronconsStream);

			zip->WriteDirectory();

			os << flush;
		}



		void HeuresFileFormat::Exporter_::setFromParametersMap(
			const util::ParametersMap& map
		){
			// Network
			try
			{
				RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));

				if(decodeTableId(id) == TransportNetworkTableSync::TABLE.ID)
				{
					_network = TransportNetworkTableSync::GetEditable(id, _env);
				}
				else if(decodeTableId(id) == TreeFolder::CLASS_NUMBER)
				{
					_folder = TreeFolderTableSync::GetEditable(id, _env);
				}
			}
			catch (...)
			{
				throw Exception("Transport network " + lexical_cast<string>(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID)) + " not found");
			}

			// Data source
			try
			{
				_dataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID), _env);
			}
			catch (...)
			{
				throw Exception("Data source " + lexical_cast<string>(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID)) + " not found");
			}
		}



		util::ParametersMap HeuresFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap map;
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_dataSource.get())
			{
				map.insert(PARAMETER_DATASOURCE_ID, _dataSource->getKey());
			}
			return map;
		}



		void HeuresFileFormat::Exporter_::_writeHour( std::ostream& os, const boost::posix_time::time_duration& duration )
		{
			os << setw(2) << setfill('0') << duration.hours();
			os << setw(2) << setfill('0') << duration.minutes();
		}



		void HeuresFileFormat::Exporter_::_writeTextAndSpaces(
			std::ostream& os,
			const std::string& value,
			size_t width,
			bool spacesAtRight,
			char spaceChar
		){
			if(!spacesAtRight)
			{
				for(size_t i(value.size()); i<width; ++i)
				{
					os << spaceChar;
				}
			}
			os << (value.size() > width ? value.substr(0, width) : value);
			if(spacesAtRight)
			{
				for(size_t i(value.size()); i<width; ++i)
				{
					os << spaceChar;
				}
			}
		}
}	}
