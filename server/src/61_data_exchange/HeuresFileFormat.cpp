
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

#include "CalendarTemplateElementTableSync.h"
#include "CityTableSync.h"
#include "DataSourceTableSync.h"
#include "DeadRunTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "DRTArea.hpp"
#include "Import.hpp"
#include "NonConcurrencyRuleTableSync.h"
#include "OperationUnitTableSync.hpp"
#include "PTModule.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "TreeFolderTableSync.hpp"
#include "VehicleServiceTableSync.hpp"
#include "DBTransaction.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "Calendar.h"
#include "IConv.hpp"
#include "DBModule.h"
#include "TransportNetworkTableSync.h"
#include "RollingStockTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "RequestException.h"
#include "CalendarTemplateTableSync.h"
#include "DestinationTableSync.hpp"
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
	using namespace pt_operation;
	using namespace road;
	using namespace admin;
	using namespace geography;
	using namespace db;
	using namespace graph;
	using namespace calendar;
	using namespace server;
	using namespace html;
	using namespace tree;
	using namespace vehicle;

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
		const std::string HeuresFileFormat::Importer_::PARAMETER_OPERATION_UNIT_ID = "operation_unit_id";

		const std::string HeuresFileFormat::Exporter_::PARAMETER_DATASOURCE_ID = "datasource_id";
		const std::string HeuresFileFormat::Exporter_::PARAMETER_NETWORK_ID = "network_id";
		const std::string HeuresFileFormat::Exporter_::PARAMETER_GENERATE_ROUTE_CODE = "generate_route_code";
		const std::string HeuresFileFormat::Exporter_::PARAMETER_SERVICE_NUMBER_POSITION = "service_number_position";
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
		HeuresFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	impex::Importer(env, import, minLogLevel, logPath, outputStream, pm),
			impex::MultipleFileTypesImporter<HeuresFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTOperationFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_depots(*import.get<DataSource>(), env)
		{}



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

			const DataSource& dataSource(*_import.get<DataSource>());
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				if(service.second->empty() && service.second->hasLinkWithSource(dataSource))
				{
					DBModule::GetDB()->deleteStmt(service.second->getKey(), transaction);
				}
				else
				{
					ScheduledServiceTableSync::Save(service.second.get(), transaction);
				}
			}
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



		bool HeuresFileFormat::Importer_::_parse(
			const path& filePath,
			const std::string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				_logError(
					"Could no open the file " + filePath.string()
				);
				throw Exception("Could no open the file " + filePath.string());
			}

			if(_calendar.empty())
			{
				throw RequestException("Base calendar must be non empty");
			}

			DataSource& dataSource(*_import.get<DataSource>());

			if(key == FILE_POINTSARRETS)
			{
				// Declarations
				string line;
				const DataSource& stopsDataSource(_stopsDataSource.get() ? *_stopsDataSource : dataSource);
				ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(stopsDataSource, _env);
				ImportableTableSync::ObjectBySource<DestinationTableSync> destinations(stopsDataSource, _env);
				ImportableTableSync::ObjectBySource<DepotTableSync> depots(dataSource, _env);

				while(getline(inFile, line))
				{
					// ID
					string id(boost::algorithm::trim_copy(line.substr(0, 4)));

					// Direction signs
					if(lexical_cast<int>(id) > 9000)
					{
						string destinationCode(line.substr(5,4));
						set<Destination*> destinationSet(destinations.get(destinationCode));
						if(destinationSet.empty())
						{
							_logWarning(
								"The destination "+ destinationCode +" was not found in the database"
							);
						}
						else
						{
							_destinations[lexical_cast<int>(id)] = *destinationSet.begin();
						}
						continue;
					}

					// Extraction of values
					string name(
						boost::algorithm::trim_copy(line.substr(5, 50))
					);
					if(!dataSource.get<Charset>().empty())
					{
						name = IConv(stopsDataSource.get<Charset>(), "UTF-8").convert(name);
					}
					
					// Case depots
					if(boost::algorithm::trim_copy(line.substr(55, 3)) == "DEP")
					{
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
					else
					{ // Case stops
						PTFileFormat::ImportableStopPoint isp;
						isp.name = name;
						isp.linkedStopPoints = stopPoints.get(id);

						if(isp.linkedStopPoints.empty())
						{
							_nonLinkedStopPoints.insert(
								make_pair(id, isp)
							);
						}
						else
						{
							_linkedStopPoints.insert(
								make_pair(id, isp)
							);
						}
					}
				}
				inFile.close();

				return true;
			}
			if(key == FILE_ITINERAI) // 1 : Routes
			{
				bool atLeastAnIgnoredRoute(false);
				if(!_network.get())
				{
					_logError("The transport network was not specified.");
					return false;
				}

				// Bus
				RollingStockTableSync::SearchResult rollingstock(RollingStockTableSync::Search(_env, string("Bus")));
				if(rollingstock.empty())
				{
					_logError("The bus transport mode is not registered in the table 49.");
					return false;
				}
				RollingStock* bus(rollingstock.front().get());

				// Load of the stops
				const DataSource& stopsDataSource(_stopsDataSource.get() ? *_stopsDataSource : dataSource);
				ImportableTableSync::ObjectBySource<StopPointTableSync> stops(stopsDataSource, _env);
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);

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
						// Commercial line number
						int commercialLineNumber(lexical_cast<int>(trim_copy(line.substr(0, 4))));

						std::string shortName(lexical_cast<string>(commercialLineNumber));
						std::string longName;
						cline = _createOrUpdateLine(
							lines,
							lexical_cast<string>(commercialLineNumber),
							longName,
							shortName,
							optional<RGBColor>(),
							*_network,
							dataSource
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
								_logError(
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
								DestinationsMap::const_iterator it(_destinations.find(lexical_cast<int>(stopNumber)));
								if(it != _destinations.end())
								{
									destination = it->second;
								}
								else
								{
									_logWarning(
										"Destination "+ stopNumber +" was not registered."
									);
								}
								continue;
							}

							bool regul(lexical_cast<bool>(line.substr(i+4,1)));
							distance += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));

							if(!stops.contains(stopNumber))
							{
								_logError("Stop "+ stopNumber +" not found");
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

							// Register the line in the stop
							ImportableStopPoints::iterator itStop(
								_linkedStopPoints.find(stopNumber)
							);
							if(itStop != _linkedStopPoints.end())
							{
								itStop->second.lineCodes.insert(
									lexical_cast<string>(commercialLineNumber)
								);
							}
							else
							{
								itStop = _nonLinkedStopPoints.find(stopNumber);
								if(itStop != _nonLinkedStopPoints.end())
								{
									itStop->second.lineCodes.insert(
										lexical_cast<string>(commercialLineNumber)
									);
								}
								else
								{
									_logWarning("The stop "+ stopNumber +" is not present in the stops file.");
								}
							}
						}

						if(ignoreRoute)
						{
							atLeastAnIgnoredRoute = true;
							continue;
						}

						// Route identification
						JourneyPattern* route(
							_createOrUpdateRoute(
								*cline,
								optional<const string&>(),
								optional<const string&>(),
								optional<const string&>(),
								destination,
								optional<const RuleUser::Rules&>(),
								routeType == 1,
								bus,
								servedStops,
								dataSource,
								true,
								true
						)	);

						_routes.insert(
							make_pair(
								make_pair(technicalLineNumber, routeNumber),
								route
						)	);
					}
					else
					{	// Dead runs
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
								_logWarning("Route " + routeNumber + ": origin " + stopNumber + " is an unknown stop/depot.");
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
								_logWarning("Route " + routeNumber + ": destination " + stopNumber + " is an unknown " + (route.depotToStop ? "stop." : "depot."));
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

				_exportStopPoints(
					_nonLinkedStopPoints
				);
				if(_displayLinkedStops)
				{
					_exportStopPoints(
						_linkedStopPoints
					);
				}

				if(atLeastAnIgnoredRoute)
				{
					return false;
				}

			} // 2 : Nodes
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
				size_t fileLineNumber(1);
				while(getline(inFile, line))
				{
					// Vehicle service
					string fullCode(trim_copy(line.substr(23,6)));
					vector<string> fullCodeVec;
					split(fullCodeVec, fullCode, is_any_of("/"));
					string vehicleServiceCode = "";
					if (fullCodeVec.size() > 1)
					{
						vehicleServiceCode = lexical_cast<string>(
							lexical_cast<int>(fullCodeVec[0])*100+lexical_cast<int>(fullCodeVec[1])
						);
					}
					else
					{
						vehicleServiceCode = lexical_cast<string>(
							lexical_cast<int>(fullCodeVec[0])
						);
					}

					VehicleService* vehicleService(
						_createOrUpdateVehicleService(
							vehicleServices,
							vehicleServiceCode,
							_operationUnit
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
						// Route number
						string routeNumber(trim_copy(line.substr(i,2)));
						DriverService::Chunk::Element tronconElement;

						// Case Dead run
						DeadRunRoutes::iterator it(_deadRunRoutes.find(make_pair(lineNumber, routeNumber)));
						if(it != _deadRunRoutes.end())
						{
							DeadRun* deadRun(NULL);

							const DeadRunRoute& route(it->second);
							time_duration departureSchedule(
								lexical_cast<int>(line.substr(i+10, 2)),
								lexical_cast<int>(line.substr(i+12, 2)),
								0
							);
							time_duration arrivalSchedule(
								lexical_cast<int>(line.substr(i+14, 2)),
								lexical_cast<int>(line.substr(i+16, 2)),
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
								deadRun->setDataSchedules(arrivalSchedules, departureSchedules);

								// Registration
								_env.getEditableRegistry<DeadRun>().add(boost::shared_ptr<DeadRun>(deadRun));
								deadRuns.add(*deadRun);

								_logCreation("Creation of the dead run with key "+ lexical_cast<string>(deadRun->getKey()));
							}

							// Operation unit
							deadRun->setOperationUnit(_operationUnit);

							vehicleService->insert(*deadRun);
							_services[lineKey].push_back(deadRun);

							tronconElement.service = deadRun;
							tronconElement.startRank = 0;
							tronconElement.endRank = 1;
							troncon->elements.push_back(tronconElement);

							for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
						}
						else
						{
							// Search in commercial routes
							RoutesMap::iterator it(_routes.find(make_pair(lineNumber, routeNumber)));
							if(it == _routes.end())
							{
								_logWarning(
									"Route not found in service file "+ lexical_cast<string>(lineNumber) +"/"+ lexical_cast<string>(routeNumber) +
									" (troncons line " + lexical_cast<string>(fileLineNumber) + ")" +
									" in vehicle service " + vehicleServiceCode
								);
								for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
								continue;
							}
							JourneyPattern* route(it->second);

							string serviceNumber(trim_copy(line.substr(i+6,3)));
							SchedulesMap::iterator itS(services.find(make_pair(route, serviceNumber)));
							if(itS != services.end())
							{
								if(itS->first.first != route)
								{
									_logWarning(
										"Inconsistent route in troncons file at l" + lexical_cast<string>(fileLineNumber) + "c" + lexical_cast<string>(i) +
										": service number=" + serviceNumber + ", line number=" + lexical_cast<string>(lineNumber) +
										", route number="+ lexical_cast<string>(routeNumber) + ", vehicle service=" + vehicleServiceCode
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
							}

							// Register the vehicle service
							itS->second.vehicleServices.push_back(vehicleService);

							tronconElement.service = NULL;
							tronconElement.startRank = 0;

							// Register the line key
							itS->second.technicalLink.push_back(lineKey);

							// Read the available schedules
							size_t rank(0);
							bool alreadyNonNull(false);
							bool alreadyNull(false);
							for(i+=6; i<line.size() && line[i]!=';'; i+=8, ++rank)
							{
								string arrivalSchedule(line.substr(i, 4));
								string departureSchedule(line.substr(i+4, 4));

								if(rank >= itS->second.departure.size())
								{
									_logWarning(
										"Inconsistent stops number in troncons file at l" + lexical_cast<string>(fileLineNumber) + "c" + lexical_cast<string>(i) +
										": service=" + serviceNumber + ", line=" + lexical_cast<string>(lineNumber) + ", route="+ lexical_cast<string>(routeNumber) +
										", vehicle service " + vehicleServiceCode +
										" => expected " + lexical_cast<string>(itS->second.departure.size()) + " schedules"
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
										tronconElement.startRank = route->getLineStop(rank, true)->get<RankInPath>();
									}
								}
								else
								{
									if(!alreadyNull && alreadyNonNull)
									{
										alreadyNull = true;
										tronconElement.endRank = route->getLineStop(rank - 1, true)->get<RankInPath>();
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
							if(!alreadyNull &&
								route->getLineStop(rank - 1, true))
							{
								tronconElement.endRank = route->getLineStop(rank - 1, true)->get<RankInPath>();
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
					
					fileLineNumber++;
				}

				// Storage as ScheduledService
				BOOST_FOREACH(const SchedulesMap::value_type& it, services)
				{
					JourneyPattern* route(it.first.first);

					ScheduledService* service(
						_createOrUpdateService(
							*route,
							it.second.departure,
							it.second.arrival,
							it.first.second,
							dataSource
					)	);

					if(service == NULL)
					{
						_logWarning(
							"Inconsistent service in service file "+ it.first.second +"/"+ it.first.first->getCommercialLine()->getShortName() +"/"+ it.first.first->getName()
						);
						continue;
					}

					BOOST_FOREACH(const ScheduleMapElement::TechnicalLink::value_type& itKey, it.second.technicalLink)
					{
						_services[itKey].push_back(service);
					}

					BOOST_FOREACH(VehicleService* vs, it.second.vehicleServices)
					{
						vs->insert(*service);
					}

					BOOST_FOREACH(const ScheduleMapElement::DriverServices::value_type& es, it.second.driverServices)
					{
						es.first->elements[es.second].service = static_cast<SchedulesBasedService*>(service);
					}
				}
			} // 3 : Services
			else if (key == FILE_SERVICES)
			{
				if(_calendar.empty())
				{
					_logError("Start date or end date not defined");
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
					day7 = _day7CalendarTemplate->getResult(_calendar);
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
					BOOST_FOREACH(const date& d, _calendar.getActiveDates())
					{
						if(	((day7.isActive(d) || d.day_of_week() == 0) && days[0]) ||
							(!day7.isActive(d) && d.day_of_week() != 0 && days[d.day_of_week()])
						){
							cal.setActive(d);
						}
					}

					
					// Driver service
					DriverService* driverService(
						_createOrUpdateDriverService(
							driverServices,
							driverServiceCode,
							_operationUnit
					)	);

					*driverService |= cal;
					DriverService::Chunks services;

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
							_logWarning(
								"Inconsistent service number "+ lexical_cast<string>(lineNumber) +"/"+ lexical_cast<string>(serviceNumber) +" in "+ line
							);
							continue;
						}

						BOOST_FOREACH(SchedulesBasedService* service, itS->second)
						{
							*service |= cal;
						}

						// Driver services
						string code(line.substr(i,6));
						services.push_back(*_troncons[code]);

						*_troncons[code]->vehicleService |= cal;
					}

					driverService->setChunks(services);
				}
			}
			inFile.close();

			return true;
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
					RegistryKeyType calendarId(map.get<RegistryKeyType>(PARAMETER_DAY7_CALENDAR_ID));
					_day7CalendarTemplate = CalendarTemplateTableSync::Get(calendarId, _env);
					CalendarTemplateElementTableSync::Search(_env, calendarId);
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

			// Operation unit
			RegistryKeyType unitId(map.getDefault<RegistryKeyType>(PARAMETER_OPERATION_UNIT_ID, 0));
			if(unitId) try
			{
				_operationUnit = *OperationUnitTableSync::GetEditable(unitId, _env);
			}
			catch(ObjectNotFoundException<OperationUnit&>)
			{
				throw RequestException("No such operation unit");
			}
		}



		void HeuresFileFormat::Exporter_::build(
			std::ostream& os
		) const	{

			//////////////////////////////////////////////////////////////////////////
			// Load

			// Lines
			vector<const CommercialLine*> lines;
			if(_network)
			{
				_addLines(lines, *_network);
			}
			if(_folder)
			{
				_addLines(lines, *_folder);
			}
			
			// Stop points
			set<const StopPoint*> stops;
			BOOST_FOREACH(const CommercialLine* line, lines)
			{
				BOOST_FOREACH(const Path* jp, line->getPaths())
				{
					BOOST_FOREACH(const Edge* edge, static_cast<const JourneyPattern*>(jp)->getEdges())
					{
						stops.insert(static_cast<const StopPoint*>(edge->getFromVertex()));
					}
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// Output

			// Pointsarrets
			stringstream pointsarretsStream;
			bool oneStopWithoutCode(false);
			BOOST_FOREACH(const StopPoint* itstop, stops)
			{
				// Local variables
				const StopPoint& stop(*itstop);

				// Data source code
				vector<string> codes(stop.getCodesBySource(*_dataSource));
				if(codes.empty())
				{
					oneStopWithoutCode = true;
					os << "ERR: Stop "<< stop.getConnectionPlace()->getFullName() <<"/"<< stop.getName() <<" has no code for the datasource";
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
					
					// Lon / Lat
					if(stop.getGeometry())
					{
						boost::shared_ptr<StopPoint::Geometry> geometry(
							CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*stop.getGeometry()
						)	);
						double lon(geometry->getX());
						double lat(geometry->getY());

						pointsarretsStream << setw(2) << setfill('0') << floor(lon);
						pointsarretsStream << ",";
						string lonFloatStr(lexical_cast<string>(int(100000*(lon-floor(lon)))).substr(0,6));
						pointsarretsStream << lonFloatStr;
						for(size_t i(lonFloatStr.length()); i<6; ++i)
						{
							pointsarretsStream << "0";
						}
						pointsarretsStream << " ";
						pointsarretsStream << setw(2) << setfill('0') << floor(lat);
						pointsarretsStream << ",";
						string latFloatStr(lexical_cast<string>(int(100000*(lat-floor(lat)))).substr(0,6));
						pointsarretsStream << latFloatStr;
						for(size_t i(latFloatStr.length()); i<6; ++i)
						{
							pointsarretsStream << "0";
						}
					}
					else
					{
						pointsarretsStream << "00,000000 00,000000";
					}

					pointsarretsStream << endl;
				}
			}

			// Break if one stop without code
			if(oneStopWithoutCode)
			{
				os << "ERR : export broken, at least a stop without code for the datasource";
				return;
			}


			// Itinerai
			// Troncons
			// Services
			stringstream servicesStream;
			stringstream tronconsStream;
			stringstream itineraiStream;
			BOOST_FOREACH(const CommercialLine* line, lines)
			{
				size_t routeCode(0);
				BOOST_FOREACH(const Path* itjp, line->getPaths())
				{
					const JourneyPattern& jp(static_cast<const JourneyPattern&>(*itjp));
					_writeTextAndSpaces(itineraiStream, line->getCodeBySources(), 4);
					_writeTextAndSpaces(itineraiStream, line->getCodeBySources(), 3, false);
					if(_generateRouteCode)
					{
						++routeCode;
						_writeTextAndSpaces(itineraiStream, lexical_cast<string>(routeCode), 2, false);
					}
					else
					{
						_writeTextAndSpaces(itineraiStream, jp.getName(), 2, false);
					}
					itineraiStream << jp.getWayBack();
					Path::Edges allEdges(jp.getEdges());
					Path::Edges::const_iterator itLastEdge(allEdges.end());
					for(Path::Edges::const_iterator itEdge(allEdges.begin()); itEdge != allEdges.end(); ++itEdge)
					{
						const LinePhysicalStop& ls(*static_cast<LinePhysicalStop*>(*itEdge));
						const StopPoint& stop(*ls.getPhysicalStop());
						_writeTextAndSpaces(itineraiStream, *stop.getCodesBySource(*_dataSource).begin(), 4, false);
						itineraiStream << ls.getScheduleInput();
						if(itLastEdge == allEdges.end())
						{
							itineraiStream << "    0";
						}
						else
						{
							double distance((*itEdge)->getMetricOffset() - (*itLastEdge)->getMetricOffset());
							if(distance < 0)
							{
								distance = 0;
							}
							itineraiStream << setw(5) << setfill(' ') << distance;
						}
						itLastEdge = itEdge;
					}
					itineraiStream << endl;


					// Troncons
					BOOST_FOREACH(const Service* itss, jp.getAllServices())
					{
						if(!dynamic_cast<const ScheduledService*>(itss))
						{
							continue;
						}

						const ScheduledService& ss(static_cast<const ScheduledService&>(*itss));

						_writeTextAndSpaces(tronconsStream, line->getCodeBySources(), 3, false);
						_writeTextAndSpaces(
							tronconsStream,
							(	ss.getServiceNumber().size() > _serviceNumberPosition ?
								ss.getServiceNumber().substr(_serviceNumberPosition, 3) :
								string()
							),
							3,
							false
						);
						tronconsStream << "2";
						Path::Edges allEdges(ss.getPath()->getEdges());
						_writeTextAndSpaces(tronconsStream, *static_cast<StopPoint*>((*allEdges.begin())->getFromVertex())->getCodesBySource(*_dataSource).begin(), 4, false);
						_writeHour(tronconsStream, ss.getDepartureSchedule(false, 0));
						_writeTextAndSpaces(tronconsStream, *static_cast<StopPoint*>((*allEdges.rbegin())->getFromVertex())->getCodesBySource(*_dataSource).begin(), 4, false);
						_writeHour(tronconsStream, ss.getLastArrivalSchedule(false));
						tronconsStream << "000000";
						if(_generateRouteCode)
						{
							_writeTextAndSpaces(tronconsStream, lexical_cast<string>(routeCode), 2, false);
						}
						else
						{
							_writeTextAndSpaces(tronconsStream, jp.getName(), 2, false);
						}
						_writeTextAndSpaces(tronconsStream, line->getCodeBySources(), 4);
						_writeTextAndSpaces(tronconsStream, ss.getServiceNumber(), 5, false, '0');

						// Schedules
						size_t schedulesNumber(ss.getDepartureSchedules(true, false).size());
						time_duration lastSchedule(not_a_date_time);
						JourneyPattern::LineStops::const_iterator itEdge(static_cast<const JourneyPattern*>(ss.getPath())->getLineStops().begin());
						for(size_t i(0); i<schedulesNumber; ++i, ++itEdge)
						{
							time_duration departureSchedule(ss.getDepartureSchedule(false, i));
							time_duration arrivalSchedule(ss.getArrivalSchedule(false, i));

							size_t writings(
								(*itEdge)->getGeneratedLineStops().size()
							);

							// Arrival time
							time_duration firstSchedule(
								(i== 0 || arrivalSchedule > departureSchedule) ? departureSchedule : arrivalSchedule
							);
							if(!lastSchedule.is_not_a_date_time() && firstSchedule < lastSchedule)
							{
								firstSchedule = lastSchedule;
							}
							lastSchedule = firstSchedule;

							// Departure time
							time_duration secondSchedule(
								(i+1 == schedulesNumber) ? arrivalSchedule : departureSchedule
							);
							if(!lastSchedule.is_not_a_date_time() && secondSchedule < lastSchedule)
							{
								secondSchedule = lastSchedule;
							}
							lastSchedule = secondSchedule;

							// A schedule per generated edge
							for(size_t ii(0); ii<writings; ++ii)
							{
								_writeHour(tronconsStream, (ii>0 ? secondSchedule : firstSchedule));
								_writeHour(tronconsStream, secondSchedule);
							}
							
							i = i + writings - 1;
						}
						tronconsStream << ";" << endl;
		

						// Services
						_writeTextAndSpaces(servicesStream, ss.getServiceNumber(), 6, false);
						servicesStream << "1111111";
						_writeTextAndSpaces(servicesStream, line->getCodeBySources(), 3, false);
						_writeTextAndSpaces(
							servicesStream,
							(	ss.getServiceNumber().size() > _serviceNumberPosition ?
								ss.getServiceNumber().substr(_serviceNumberPosition, 3) :
								string()
							),
							3,
							false
						);
						servicesStream << "2";
						servicesStream << "0000000000000000000000";
						servicesStream << endl;
			}	}	}

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
					_network = Env::GetOfficialEnv().get<TransportNetwork>(id);
				}
				else if(decodeTableId(id) == TreeFolder::CLASS_NUMBER)
				{
					_folder = Env::GetOfficialEnv().get<TreeFolder>(id);
				}
			}
			catch (...)
			{
				throw Exception("Transport network " + lexical_cast<string>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) + " not found");
			}

			// Data source
			try
			{
				_dataSource = Env::GetOfficialEnv().get<DataSource>(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID));
			}
			catch (...)
			{
				throw Exception("Data source " + lexical_cast<string>(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID)) + " not found");
			}

			// Generate route code
			_generateRouteCode = map.getDefault<bool>(PARAMETER_GENERATE_ROUTE_CODE, false);

			// Service number position
			_serviceNumberPosition = map.getDefault<size_t>(PARAMETER_SERVICE_NUMBER_POSITION, 0);
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
			if(_generateRouteCode)
			{
				map.insert(PARAMETER_GENERATE_ROUTE_CODE, _generateRouteCode);
			}
			if(_serviceNumberPosition > 0)
			{
				map.insert(PARAMETER_SERVICE_NUMBER_POSITION, _serviceNumberPosition);
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



		HeuresFileFormat::Exporter_::Exporter_(
			const Export& export_
		):	OneFileExporter<HeuresFileFormat>(export_),
			_generateRouteCode(false),
			_serviceNumberPosition(0)
		{

		}



		void HeuresFileFormat::Exporter_::_addLines( std::vector<const pt::CommercialLine*>& lines, const TreeFolderUpNode& node )
		{
			vector<CommercialLine*> toadd(node.getChildren<CommercialLine>());
			lines.insert(lines.end(), toadd.begin(), toadd.end());
			BOOST_FOREACH(TreeFolder* folder, node.getSubFolders())
			{
				_addLines(lines, *folder);
			}
		}
}	}
