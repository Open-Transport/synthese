
/** HeuresOperationFileFormat class implementation.
	@file HeuresOperationFileFormat.cpp

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

#include "HeuresOperationFileFormat.hpp"

#include "DataSource.h"
#include "Import.hpp"
#include "PTOperationFileFormat.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
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
#include "PTFileFormat.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
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
#include "Depot.hpp"
#include "DeadRun.hpp"
#include "DeadRunTableSync.hpp"
#include "VehicleService.hpp"
#include "VehicleServiceTableSync.hpp"
#include "DriverServiceTableSync.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
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
	using namespace pt_operation;
	using namespace road;
	using namespace admin;
	using namespace geography;
	using namespace db;
	using namespace graph;
	using namespace calendar;
	using namespace server;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HeuresOperationFileFormat>::FACTORY_KEY("Heures (exploitation)");
	}

	namespace pt_operation
	{
		const string HeuresOperationFileFormat::Importer_::FILE_TRONCONS("troncons");
		const string HeuresOperationFileFormat::Importer_::FILE_POINTSARRETS("pointsarrets");
		const string HeuresOperationFileFormat::Importer_::FILE_ITINERAI("itinerai");
		const string HeuresOperationFileFormat::Importer_::FILE_SERVICES("services");

		const string HeuresOperationFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const string HeuresOperationFileFormat::Importer_::PARAMETER_NETWORK_ID("network_id");
		const string HeuresOperationFileFormat::Importer_::PARAMETER_PT_DATASOURCE_ID("pt_datasource_id");
		const string HeuresOperationFileFormat::Importer_::PARAMETER_STOPS_DATASOURCE_ID = "stops_datasource_id";
		const string HeuresOperationFileFormat::Importer_::PARAMETER_END_DATE("end_date");
		const string HeuresOperationFileFormat::Importer_::PARAMETER_START_DATE("start_date");
		const string HeuresOperationFileFormat::Importer_::PARAMETER_CLEAN_OLD_DATA("clean_old_data");
		const string HeuresOperationFileFormat::Importer_::PARAMETER_DAY7_CALENDAR_ID("day7_calendar_id");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HeuresOperationFileFormat>::Files MultipleFileTypesImporter<HeuresOperationFileFormat>::FILES(
			HeuresOperationFileFormat::Importer_::FILE_POINTSARRETS.c_str(),
			HeuresOperationFileFormat::Importer_::FILE_ITINERAI.c_str(),
			HeuresOperationFileFormat::Importer_::FILE_TRONCONS.c_str(),
			HeuresOperationFileFormat::Importer_::FILE_SERVICES.c_str(),
		"");
	}

	namespace pt_operation
	{
		bool HeuresOperationFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it;
			it = _pathsMap.find(FILE_TRONCONS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_POINTSARRETS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ITINERAI);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_SERVICES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		DBTransaction HeuresOperationFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(Registry<Depot>::value_type depot, _env.getRegistry<Depot>())
			{
				DepotTableSync::Save(depot.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DeadRun>::value_type deadRun, _env.getRegistry<DeadRun>())
			{
				DeadRunTableSync::Save(deadRun.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<VehicleService>::value_type vehicleService, _env.getRegistry<VehicleService>())
			{
				VehicleServiceTableSync::Save(vehicleService.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DriverService>::value_type driverService, _env.getRegistry<DriverService>())
			{
				DriverServiceTableSync::Save(driverService.second.get(), transaction);
			}
			return transaction;
		}



		bool HeuresOperationFileFormat::Importer_::_parse(
			const path& filePath,
			const std::string& key,
			boost::optional<const server::Request&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				_logError("Could no open the file " + filePath.file_string());
				throw Exception("Could no open the file " + filePath.file_string());
			}

			if(!_startDate || !_endDate)
			{
				_logError("Start date and end date must be defined");
				throw RequestException("Start date and end date must be defined");
			}

			DataSource& dataSource(*_import.get<DataSource>());

			// Depots
			if(key == FILE_POINTSARRETS)
			{
				string line;
				ImportableTableSync::ObjectBySource<DepotTableSync> depots(dataSource, _env);

				while(getline(inFile, line))
				{
					// Avoid depots
					if(boost::algorithm::trim_copy(line.substr(55, 3)) != "DEP")
					{
						continue;
					}

					// Extraction of values
					string name(
						IConv(dataSource.get<Charset>(), "UTF-8").convert(
							boost::algorithm::trim_copy(line.substr(5, 50))
					)	);
					string id(boost::algorithm::trim_copy(line.substr(0, 4)));

					// Search for existing depot
					set<Depot*> loadedDepots(depots.get(id));
					if(!loadedDepots.empty())
					{
						stringstream logStream;
						logStream << "Link between depots " << id << " (" << name << ")" << " and ";
						BOOST_FOREACH(Depot* dp, loadedDepots)
						{
							logStream << dp->getKey() << " (" << dp->getName() << ") ";
						}
						_logLoad(logStream.str());
					}
					else
					{
						boost::shared_ptr<Depot> depot(new Depot(DepotTableSync::getId()));

						Importable::DataSourceLinks links;
						links.insert(make_pair(&dataSource, id));
						depot->setDataSourceLinksWithoutRegistration(links);
						_env.getEditableRegistry<Depot>().add(depot);
						_depots.add(*depot);
						loadedDepots.insert(depot.get());

						_logCreation(
							"Creation of the depot with key "+ id +" ("+ name +")"
						);
					}

					// Update of properties
					BOOST_FOREACH(Depot* dp, loadedDepots)
					{
						dp->setName(name);
					}
				}
				inFile.close();
			}
			else if(key == FILE_ITINERAI) // 1 : Routes
			{
				if(!_network.get())
				{
					_logError(
						"The transport network was not specified."
					);
					return false;
				}

				// Load of the depots and stops
				ImportableTableSync::ObjectBySource<StopPointTableSync> stops(*_stopsDataSource, _env);
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*_ptDataSource, _env);

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
					if(routeType == 0 || routeType == 1)
					{
						// Declarations
						bool ignoreRoute(false);

						// Commercial line number
						int commercialLineNumber(lexical_cast<int>(trim_copy(line.substr(0, 4))));

						cline = PTFileFormat::GetLine(
							lines,
							lexical_cast<string>(commercialLineNumber),
							*_ptDataSource,
							_env,
							_logger
						);

						// Check of the line
						if(!cline)
						{
							_logWarning(
								"Line "+ lexical_cast<string>(commercialLineNumber) +" not found in route "+ routeNumber
							);
							ignoreRoute = true;
						}

						// Stops
						JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
						MetricOffset distance(0);
						for(size_t i(10); i+1<line.size(); i += 10)
						{
							if(line.size() < i+9)
							{
								_logWarning(
									"Inconsistent line size "+ line
								);
								ignoreRoute = true;
								break;
							}

							// Stop search
							string stopNumber(trim_copy(line.substr(i,4)));

							// Fake stop : girouette
							if(lexical_cast<int>(stopNumber) > 9000)
							{
								*servedStops.rbegin()->_metricOffset += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));
								continue;
							}

							bool regul(lexical_cast<bool>(line.substr(i+4,1)));
							distance += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));

							if(!stops.contains(stopNumber))
							{
								_logWarning(
									"Stop "+ stopNumber +" not found"
								);
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
						set<JourneyPattern*> routeSet(
							PTFileFormat::GetRoutes(
								*cline,
								servedStops,
								*_ptDataSource,
								_logger
						)	);
						if(routeSet.empty())
						{
							_logWarning(
								"Route "+ routeNumber +" not found"
							);
							continue;
						}
						if(routeSet.size() > 1)
						{
							_logWarning(
								"Route "+ routeNumber +" is present twice or more in line "+ cline->getShortName()
							);
						}

						_routes.insert(
							make_pair(
								make_pair(technicalLineNumber, routeNumber),
								*routeSet.begin()
						)	);
					}
					else
					{
						DeadRunRoute route;

						// Origin
						{
							// Stop search
							string stopNumber(trim_copy(line.substr(10,4)));
							if(_depots.contains(stopNumber))
							{
								route.depotToStop = true;
								route.depot = *_depots.get(stopNumber).begin();
							}
							else if(stops.contains(stopNumber))
							{
								route.depotToStop = false;
								route.stop = *stops.get(stopNumber).begin();
							}
							else
							{
								_logWarning("Stop "+ stopNumber +" was not found.");
								continue;
							}
						}

						// Destination
						{
							// Stop search
							string stopNumber(trim_copy(line.substr(20,4)));
							if(route.depotToStop && stops.contains(stopNumber))
							{
								route.stop = *stops.get(stopNumber).begin();
							}
							else if(!route.depotToStop && _depots.contains(stopNumber))
							{
								route.depot = *_depots.get(stopNumber).begin();
							}
							else
							{
								_logWarning("Stop "+ stopNumber +" was not found.");
								continue;
							}

							// Length
							route.length = lexical_cast<MetricOffset>(trim_copy(line.substr(25,5)));
						}

						// Registration
						_deadRunRoutes.insert(
							make_pair(
								make_pair(technicalLineNumber, routeNumber),
								route
						)	);
				}	}
			}
			else if(key == FILE_TRONCONS)
			{
				string line;
				typedef map<
					pair<JourneyPattern*, string>, // string is service number
					ScheduleMapElement
				> SchedulesMap;
				SchedulesMap services;

				ImportableTableSync::ObjectBySource<DeadRunTableSync> deadRuns(dataSource, _env);
				ImportableTableSync::ObjectBySource<VehicleServiceTableSync> vehicleServices(dataSource, _env);

				// Cleaning all vehicle services
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<VehicleServiceTableSync>::Map::value_type& vsset, vehicleServices.getMap())
				{
					BOOST_FOREACH(VehicleService* vs, vsset.second)
					{
						vs->clear();
					}
				}

				// Reading of the file
				while(getline(inFile, line))
				{
					// Vehicle service
					string fullCode(trim_copy(line.substr(23,6)));
					vector<string> fullCodeVec;
					split(fullCodeVec, fullCode, is_any_of("/"));
					string vehicleServiceCode(
						lexical_cast<string>(
							lexical_cast<int>(fullCodeVec[0])*100+lexical_cast<int>(fullCodeVec[1])
					)	);

					VehicleService* vehicleService(
						PTOperationFileFormat::CreateOrUpdateVehicleService(
							vehicleServices,
							vehicleServiceCode,
							dataSource,
							_env,
							_logger
					)	);

					Troncons::mapped_type troncon(new DriverService::Chunk(vehicleService));

					// Line number
					int lineNumber(lexical_cast<int>(trim_copy(line.substr(0,3))));
					pair<int, int> lineKey(
						make_pair(
							lineNumber,
							lexical_cast<int>(trim_copy(line.substr(3,3)))
					)	);

					// Service codes
					for(size_t i(29); i+1<line.size(); ++i)
					{
						string routeNumber(trim_copy(line.substr(i,2)));
						DriverService::Chunk::Element tronconElement;

						// Dead run
						DeadRunRoutes::iterator it(_deadRunRoutes.find(make_pair(lineNumber, routeNumber)));
						if(it != _deadRunRoutes.end())
						{
							DeadRun* deadRun(NULL);

							const DeadRunRoute& route(it->second);
							time_duration departureSchedule(
								lexical_cast<int>(line.substr(i+15, 2)),
								lexical_cast<int>(line.substr(i+17, 2)),
								0
							);
							time_duration arrivalSchedule(
								lexical_cast<int>(line.substr(i+19, 2)),
								lexical_cast<int>(line.substr(i+21, 2)),
								0
							);

							// Search for existing dead run
							BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DeadRunTableSync>::Map::value_type& deadRunSet, deadRuns.getMap())
							{
								BOOST_FOREACH(DeadRun* curDeadRun, deadRunSet.second)
								{
									if(curDeadRun->isUndefined())
									{
										continue;
									}
									if(	curDeadRun->getTransportNetwork() == _network.get() &&
										curDeadRun->getFromDepotToStop() == route.depotToStop &&
										curDeadRun->getDepot() == route.depot &&
										curDeadRun->getStop() == route.stop &&
										curDeadRun->getDepartureSchedule(false, 0) == departureSchedule &&
										curDeadRun->getArrivalSchedule(false, 1) == arrivalSchedule
									){
										deadRun = curDeadRun;
										break;
									}
								}
								if(deadRun)
								{
									break;
								}
							}

							// Existing dead run
							if(deadRun)
							{
								_logLoad("Use of existing dead run "+ lexical_cast<string>(deadRun->getKey()));
							}
							else
							{
								deadRun = new DeadRun(DeadRunTableSync::getId());

								// Transport network
								deadRun->setTransportNetwork(const_cast<TransportNetwork*>(_network.get()));

								// Source links
								Importable::DataSourceLinks links;
								links.insert(make_pair(&dataSource, string()));
								deadRun->setDataSourceLinksWithoutRegistration(links);

								// Route
								deadRun->setRoute(
									*route.depot,
									*route.stop,
									route.length,
									route.depotToStop
								);

								// Schedules
								SchedulesBasedService::Schedules arrivalSchedules;
								arrivalSchedules.push_back(departureSchedule);
								arrivalSchedules.push_back(arrivalSchedule);
								SchedulesBasedService::Schedules departureSchedules;
								departureSchedules.push_back(departureSchedule);
								departureSchedules.push_back(arrivalSchedule);
								deadRun->setSchedules(arrivalSchedules, departureSchedules, false);

								// Registration
								_env.getEditableRegistry<DeadRun>().add(boost::shared_ptr<DeadRun>(deadRun));
								deadRuns.add(*deadRun);

								_logCreation("Creation of the dead run with key "+ lexical_cast<string>(deadRun->getKey()));
							}

							vehicleService->insert(*deadRun);
							_services[line.substr(0,6)].push_back(deadRun);

							tronconElement.service = deadRun;
							tronconElement.startRank = 0;
							tronconElement.endRank = 1;
							troncon->elements.push_back(tronconElement);

							for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
						}
						else // ScheduledService
						{
							RoutesMap::iterator it(_routes.find(make_pair(lineNumber, routeNumber)));
							if(it == _routes.end())
							{
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
									_logWarning(
										"Inconsistent route in service file "+ serviceNumber +"/"+ lexical_cast<string>(lineNumber) +"/"+ routeNumber
									);
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
									itS->second.departure.push_back(time_duration(not_a_date_time));
									itS->second.arrival.push_back(time_duration(not_a_date_time));
								}

								// Register the vehicle service
								itS->second.vehicleServices.push_back(vehicleService);
							}

							tronconElement.service = NULL;
							tronconElement.startRank = 0;

							// Read the available schedules
							size_t rank(0);
							bool alreadyNonNull(false);
							bool alreadyNull(false);
							for(i+=11; i<line.size() && line[i]!=';'; i+=8, ++rank)
							{
								string arrivalSchedule(line.substr(i, 4));
								string departureSchedule(line.substr(i+4, 4));

								if(rank >= itS->second.departure.size())
								{
									_logWarning(
										"Inconsistent stops number in troncons file "+ serviceNumber +"/"+ lexical_cast<string>(lineNumber) +"/"+ routeNumber
									);
									continue;
								}

								if(departureSchedule != "9999")
								{
									itS->second.departure[rank] = time_duration(
										lexical_cast<int>(departureSchedule.substr(0,2)),
										lexical_cast<int>(departureSchedule.substr(2,2)),
										0
									);

									if(!alreadyNonNull)
									{
										alreadyNonNull = true;
										tronconElement.startRank = route->getLineStop(rank, true)->getRankInPath();
									}
								}
								else
								{
									if(!alreadyNull && alreadyNonNull)
									{
										alreadyNull = true;
										tronconElement.endRank = route->getLineStop(rank - 1, true)->getRankInPath();
									}
								}
								if(arrivalSchedule != "9999")
								{
									itS->second.arrival[rank] = time_duration(
										lexical_cast<int>(arrivalSchedule.substr(0,2)),
										lexical_cast<int>(arrivalSchedule.substr(2,2)),
										0
									);
								}
							}
							if(!alreadyNull)
							{
								tronconElement.endRank = route->getLineStop(rank - 1, true)->getRankInPath();
							}
							troncon->elements.push_back(tronconElement);
							itS->second.driverServices.push_back(
								make_pair(
									troncon,
									troncon->elements.size() - 1
							)	);
						}
					}
					_troncons.insert(make_pair(line.substr(0,6), troncon));
				}

				// Storage of the ScheduledServices
				BOOST_FOREACH(SchedulesMap::value_type& it, services)
				{
					JourneyPattern* route(it.first.first);
					boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
								*route->sharedServicesMutex
					);
					ScheduledService* curService(NULL);
					BOOST_FOREACH(Service* service, route->getServices())
					{
						if(!dynamic_cast<ScheduledService*>(service))
						{
							continue;
						}
						if(	service->getServiceNumber() == it.first.second &&
							static_cast<ScheduledService*>(service)->comparePlannedSchedules(it.second.departure, it.second.arrival)
						){
							curService = static_cast<ScheduledService*>(service);
							break;
						}
					}

					if(curService == NULL)
					{
						_logWarning(
							"Inconsistent service in service file "+ it.first.second +"/"+ it.first.first->getCommercialLine()->getShortName() +"/"+ it.first.first->getName()
						);
						continue;
					}

					BOOST_FOREACH(VehicleService* vs, it.second.vehicleServices)
					{
						vs->insert(*curService);
					}

					BOOST_FOREACH(const ScheduleMapElement::DriverServices::value_type& es, it.second.driverServices)
					{
						es.first->elements[es.second].service = static_cast<SchedulesBasedService*>(curService);
					}
				}
			} // 3 : Services
			else if (key == FILE_SERVICES)
			{
				if(!_startDate || !_endDate)
				{
					_logError(
						"Start date or end date not defined"
					);
					return false;
				}

				// Cleaning all vehicle services
				ImportableTableSync::ObjectBySource<DriverServiceTableSync> driverServices(dataSource, _env);
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DriverServiceTableSync>::Map::value_type& dsset, driverServices.getMap())
				{
					BOOST_FOREACH(DriverService* ds, dsset.second)
					{
						ds->clear();
					}
				}

				string line;
				Calendar day7;
				if(_day7CalendarTemplate.get())
				{
					day7 = _day7CalendarTemplate->getResult(Calendar(*_startDate, *_endDate));
				}

				while(getline(inFile, line))
				{
					// Driver service
					string driverServiceCode(trim_copy(line.substr(0,6)));

					// Read of calendar
					vector<bool> days(7, false);
					for(size_t i(0); i<7; ++i)
					{
						days[i] = (line[i==0 ? 12 : i+5] == '1');
					}
					Calendar cal;
					for(gregorian::date d(*_startDate); d<=*_endDate; d += gregorian::days(1))
					{
						if(	((day7.isActive(d) || d.day_of_week() == 0) && days[0]) ||
							(!day7.isActive(d) && d.day_of_week() != 0 && days[d.day_of_week()])
						){
							cal.setActive(d);
						}
					}

					// Driver service
					set<DriverService*> loadedDriverServices(driverServices.get(driverServiceCode));
					if(!loadedDriverServices.empty())
					{
						stringstream logStream;
						logStream << "Link between driver services " << driverServiceCode << " and ";
						BOOST_FOREACH(DriverService* ds, loadedDriverServices)
						{
							logStream << ds->getKey();
						}
						_logLoad(logStream.str());
					}
					else
					{
						boost::shared_ptr<DriverService> ds(new DriverService(DriverServiceTableSync::getId()));

						Importable::DataSourceLinks links;
						links.insert(make_pair(&dataSource, driverServiceCode));
						ds->setDataSourceLinksWithoutRegistration(links);
						_env.getEditableRegistry<DriverService>().add(ds);
						driverServices.add(*ds);
						loadedDriverServices.insert(ds.get());

						_logCreation("Creation of the driver service with key "+ driverServiceCode);
					}
					DriverService* driverService(
						*loadedDriverServices.begin()
					);
					*driverService |= cal;
					DriverService::Chunks services;

					// Services list
					for(size_t i(13); i+6<line.size(); i+=29)
					{
						const vector<DeadRun*>& deadRuns(
							_services[line.substr(i,6)]
						);

						BOOST_FOREACH(DeadRun* deadRun, deadRuns)
						{
							*deadRun |= cal;
						}

						// Driver services
						services.push_back(*_troncons[line.substr(i,6)]);
					}

					driverService->setChunks(services);
				}
			}
			inFile.close();

			return true;
		}



		void HeuresOperationFileFormat::Importer_::displayAdmin(
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
			stream << t.cell("Date début", t.getForm().getCalendarInput(PARAMETER_START_DATE, _startDate ? *_startDate : date(not_a_date_time)));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAMETER_END_DATE, _endDate ? *_endDate : date(not_a_date_time)));
			stream << t.cell("Réseau", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Source de données offre de transport public", t.getForm().getTextInput(PARAMETER_PT_DATASOURCE_ID, _ptDataSource.get() ? lexical_cast<string>(_ptDataSource->getKey()) : string()));
			stream << t.cell("Source de données arrêts", t.getForm().getTextInput(PARAMETER_STOPS_DATASOURCE_ID, _stopsDataSource.get() ? lexical_cast<string>(_stopsDataSource->getKey()) : string()));
			stream << t.cell("Calendrier des jours fériés",
				t.getForm().getSelectInput(
					PARAMETER_DAY7_CALENDAR_ID,
					CalendarTemplateTableSync::GetCalendarTemplatesList("(aucun)"),
					optional<RegistryKeyType>(_day7CalendarTemplate.get() ? _day7CalendarTemplate->getKey() : RegistryKeyType(0))
			)	);
			stream << t.close();
		}



		util::ParametersMap HeuresOperationFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_ptDataSource.get())
			{
				map.insert(PARAMETER_PT_DATASOURCE_ID, _ptDataSource->getKey());
			}
			if(_stopsDataSource.get())
			{
				map.insert(PARAMETER_STOPS_DATASOURCE_ID, _stopsDataSource->getKey());
			}
			if(_startDate)
			{
				map.insert(PARAMETER_START_DATE, *_startDate);
			}
			if(_endDate)
			{
				map.insert(PARAMETER_END_DATE, *_endDate);
			}
			if(_day7CalendarTemplate.get())
			{
				map.insert(PARAMETER_DAY7_CALENDAR_ID, _day7CalendarTemplate->getKey());
			}
			return map;
		}



		void HeuresOperationFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = from_string(map.get<string>(PARAMETER_END_DATE));
			}

			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);
			if(map.getOptional<RegistryKeyType>(PARAMETER_NETWORK_ID))
			{
				try
				{
					_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
				}
				catch(ObjectNotFoundException<TransportNetwork>&)
				{
					throw RequestException("No such transport network");
				}
			}
			if(!_network.get())
			{
				throw RequestException("A network must be defined");
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

			// Public transport data source
			if(map.getDefault<RegistryKeyType>(PARAMETER_PT_DATASOURCE_ID, 0))
			{
				try
				{
					_ptDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PT_DATASOURCE_ID), _env);
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					throw RequestException("No such data source for PT data");
				}
			}
			if(!_ptDataSource.get())
			{
				throw RequestException("A PT data source must be defined");
			}

			// Stops data source
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
			if(!_stopsDataSource.get())
			{
				throw RequestException("A stops data source must be defined");
			}
		}



		HeuresOperationFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			const impex::ImportLogger& logger
		):	impex::Importer(env, import, logger),
			impex::MultipleFileTypesImporter<HeuresOperationFileFormat>(env, import, logger),
			_depots(*import.get<DataSource>(), env)
		{}
}	}
