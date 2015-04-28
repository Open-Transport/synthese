
//////////////////////////////////////////////////////////////////////////
/// HanoverTheoricalFileFormat class implementation.
/// @file HanoverTheoricalFileFormat.cpp
/// @author Thomas Puigt
/// @date 2015
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

#include "HanoverTheoricalFileFormat.hpp"

#include "CityTableSync.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "DataSourceTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "Import.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ParametersMap.h"
#include "Request.h"
#include "RequestException.h"
#include "ScheduledServiceTableSync.h"
#include "ServerModule.h"
#include "StopPointTableSync.hpp"

#include <geos/geom/Geometry.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>

#include <boost/filesystem.hpp>

using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace impex;
	using namespace geography;
	using namespace calendar;
	using namespace pt;
	using namespace server;
	using namespace security;
	using namespace util;

	template<>
	const string FactorableTemplate<FileFormat, data_exchange::HanoverTheoricalFileFormat>::FACTORY_KEY = "HanoverTheorical";

	namespace data_exchange
	{
		const string HanoverTheoricalFileFormat::Importer_::PARAMETER_DB_CONN_STRING("conn_string");
		const string HanoverTheoricalFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY = "default_city";
		const string HanoverTheoricalFileFormat::Importer_::PARAMETER_NETWORK_ID = "ni";
		const string HanoverTheoricalFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION = "sa_td";



		HanoverTheoricalFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			DatabaseReadImporter<HanoverTheoricalFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_stopAreas(*import.get<DataSource>(), env),
			_stopPoints(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env)
		{}



		ParametersMap HanoverTheoricalFileFormat::Importer_::getParametersMap() const
		{
			ParametersMap map;
			if (_network.get() != NULL)
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
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



		void HanoverTheoricalFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			_dbConnString = map.getOptional<string>(PARAMETER_DB_CONN_STRING);

			try
			{
				_network = TransportNetworkTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}
			catch (ObjectNotFoundException<TransportNetwork>&)
			{
			}

			// Default City
			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}

			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
		}



		void HanoverTheoricalFileFormat::Importer_::_selectAndLoadRun(
			RunsMap& runs,
			int runId,
			Route& route,
			const calendar::Calendar& calendar,
			const HanoverTheoricalSchedules& schedules,
			const std::string& service_number
		) const {

			// Jump over courses with incomplete chainages
			if(schedules.size() > route.links.size() + 1)
			{
				_logWarningDetail(
					"RUN",
					lexical_cast<string>(runId),
					service_number,0,
					lexical_cast<string>(schedules.size()),
					lexical_cast<string>(route.links.size() + 1),
					string(),
					"Bad schedules number compared to Route stops number "
				);
				return;
			}
			else
			{
				// OK let's store the service
				Run& run(
					_runs.insert(
						make_pair(
							runId,
							Run()
					)	).first->second
				);
				run.id = runId;
				run.schedules = schedules;
				run.route = &route;
				run.calendar = calendar;
				run.service_number = service_number;

				// Trace
				_logLoadDetail(
					"RUN",lexical_cast<string>(runId),service_number,0,string(),string(), string(),"OK"
				);
			}
		}

		void HanoverTheoricalFileFormat::Importer_::_selectAndLoadLink(
			LinksMap& links,
			int id,
			const Link::HanoverTheoricalLineStops& stops,
			boost::shared_ptr<geos::geom::LineString> lineString
		) const {
			if(stops.size() < 2)
			{
				_logWarningDetail(
					"LINK",lexical_cast<string>(id),string(),stops.size(),string(),string(), string(),"This link has less than 2 stops"
				);
				return;
			}
			Link& link(
				links.insert(
					make_pair(
						id,
						Link()
				)	).first->second
			);

			link.id = id;
			link.stops = stops;
			link.lineString = lineString;
			_logLoadDetail(
				"LINK",lexical_cast<string>(id),string(),0,string(),string(),lexical_cast<string>(stops[1].offsetFromPreviousStop),"OK"
			);
		}



		void HanoverTheoricalFileFormat::Importer_::_selectAndLoadRoute(
			RoutesMap& routes,
			const Route::Links& links,
			pt::CommercialLine* line,
			const std::string& name,
			bool direction,
			int id
		) const {
			if(links.size() == 0)
			{
				_logWarningDetail(
					"ROUTE",lexical_cast<string>(id),line->getShortName(),0,string(),string(), string(),"This route has no link"
				);
				return;
			}
			else
			{
				Route& route(
					routes.insert(
						make_pair(
							id,
							Route()
					)	).first->second
				);

				route.id = id;
				route.name = name;
				route.links = links;
				route.line = line;
				route.direction = direction;
				_logLoadDetail(
					"ROUTE",lexical_cast<string>(id),line->getShortName(),0,name,lexical_cast<string>(direction),lexical_cast<string>(links.size()),"OK"
				);
			}
		}




		bool HanoverTheoricalFileFormat::Importer_::_read(
		) const {
			DataSource& dataSource(*_import.get<DataSource>());
			boost::shared_ptr<DB> db;
			if(_dbConnString)
			{
				db = DBModule::GetDBForStandaloneUse(*_dbConnString);
			}
			else
			{
				db = DBModule::GetDBSPtr();
			}


			//////////////////////////////////////////////////////////////////////////
			// Pre-loading objects from HanoverTheorical
			LinksMap _links;

			// Stop areas
			{
				string query(
					"SELECT * FROM "+ _database +".v_rcs_stop_group"
				);
				DBResultSPtr result(db->execQuery(query));
				while(result->next())
				{
					// Fields load
					string name(result->get<string>("sgr_name"));
					string ref(result->get<string>("sgr_short_name"));

					_createOrUpdateStopAreas(
						_stopAreas,
						ref,
						name,
						_defaultCity.get(),
						false,
						_stopAreaDefaultTransferDuration,
						dataSource
					);
				}
			}

			// Stop Areas and Physical Stops
			{
				string query(
					"SELECT * FROM "+ _database +".v_rcs_stop"
				);
				DBResultSPtr result(db->execQuery(query));
				while(result->next())
				{
					// Fields load
					string areaRef(result->get<string>("sgr_short_name"));
					string name(result->get<string>("sto_name"));
					string ref(result->get<string>("sto_graph_key"));

					const StopArea* stopArea(NULL);
					if(_stopAreas.contains(areaRef))
					{
						stopArea = *_stopAreas.get(areaRef).begin();
					}
					else if(_stopPoints.contains(ref))
					{
						stopArea = (*_stopPoints.get(ref).begin())->getConnectionPlace();
					}
					else
					{
						_logWarning(
							"inconsistent STOP GROUP reference "+ areaRef +" in stop "+ ref
						);
						continue;
					}


					// Point
					boost::shared_ptr<geos::geom::Point> geometry;
					geometry = dataSource.getActualCoordinateSystem().createPoint(
						lexical_cast<double>(result->get<string>("pnt_longitude")) / static_cast<double>(3600000),
						lexical_cast<double>(result->get<string>("pnt_latitude")) / static_cast<double>(3600000)
					);
					if(geometry->isEmpty())
					{
						geometry.reset();
					}

					_createOrUpdateStop(
						_stopPoints,
						ref,
						name,
						optional<const RuleUser::Rules&>(),
						stopArea,
						geometry.get(),
						dataSource
					);
				}
			}

			// Commercial Lines
			{
				string query(
					"SELECT * FROM "+ _database +".v_rcs_line"
				);
				DBResultSPtr result(db->execQuery(query));
				while(result->next())
				{
					// Fields load
					string name(result->get<string>("lin_linename"));
					string shortName(result->get<string>("lin_number"));
					int colorR(result->get<int>("in_color_r"));
					int colorG(result->get<int>("lin_color_g"));
					int colorB(result->get<int>("lin_color_b"));
					const RGBColor color(colorR,colorG, colorB);

					_createOrUpdateLine(
						_lines,
						shortName,
						name,
						shortName,
						optional<RGBColor>(color),
						*_network,
						dataSource
					);
				}
			}

			// Links and Points
			{
				string query(
					"SELECT * FROM " + _database + ".v_rcs_link" + " ORDER BY lnk_id ASC, ptl_order ASC"
				);
				DBResultSPtr result(db->execQuery(query));
				int lastLinkId(0);
				Link::HanoverTheoricalLineStops linkStops;
				graph::MetricOffset offsetSum(0);
				geos::geom::CoordinateSequence* sequence(NULL);
				while(result->next())
				{
					int id(result->get<int>("lnk_id"));
					if(lastLinkId > 0 && id > lastLinkId)
					{
						// Register last built HanoverTheorical Link
						_selectAndLoadLink(
							_links,
							lastLinkId,
							linkStops,
							boost::shared_ptr<geos::geom::LineString>(
								dataSource.getActualCoordinateSystem().getGeometryFactory().createLineString(sequence))
						);
					}
					if(lastLinkId != id)
					{
						offsetSum = 0;
						linkStops.clear();
						sequence = dataSource.getActualCoordinateSystem().getGeometryFactory().getCoordinateSequenceFactory()->create(0, 2);
					}
					lastLinkId = id;
					string stopref(result->get<string>("sto_graph_key"));
					int pntId(result->get<int>("pnt_id"));
					MetricOffset lg(result->get<MetricOffset>("ptl_length"));
					boost::shared_ptr<geos::geom::Point> geometry;
					geometry = dataSource.getActualCoordinateSystem().createPoint(
						lexical_cast<double>(result->get<string>("pnt_longitude")) / static_cast<double>(3600000),
						lexical_cast<double>(result->get<string>("pnt_latitude")) / static_cast<double>(3600000)
					);
					offsetSum += lg;

					const StopPoint* stop(NULL);
					if(_stopPoints.contains(stopref))
					{
						stop = *_stopPoints.get(stopref).begin();
					}
					if(stop)
					{
						HanoverTheoricalLineStop& vs(*linkStops.insert(linkStops.end(),HanoverTheoricalLineStop()));
						vs.id = pntId;
						vs.syntheseStop = const_cast<StopPoint*>(stop);
						vs.geometry = geometry;
						vs.offsetFromPreviousStop = offsetSum;
						sequence->add(geos::geom::Coordinate(
							result->get<double>("pnt_longitude") / static_cast<double>(3600000),
							result->get<double>("pnt_latitude") / static_cast<double>(3600000)
						)	);
					}
					else
					{
						// Putting waypoints coordinates in the sequence
						sequence->add(geos::geom::Coordinate(
							result->get<double>("pnt_longitude"),
							result->get<double>("pnt_latitude")
						)	);
					}
				}
				// Register last link
				_selectAndLoadLink(
					_links,
					lastLinkId,
					linkStops,
					boost::shared_ptr<geos::geom::LineString>(
						dataSource.getActualCoordinateSystem().getGeometryFactory().createLineString(sequence))
				);
			}

			// Routes
			{
				string query(
					string("SELECT * FROM ") + _database + ".v_rcs_route " + "ORDER BY rou_id ASC, rol_order ASC"
				);
				DBResultSPtr result(db->execQuery(query));
				int lastRouteId(0);
				bool direction(false);
				string name;
				string dest;
				pt::CommercialLine* commercialLine(NULL);
				Route::Links links;
				while(result->next())
				{
					// Fields load
					int id(result->get<int>("rou_id"));

					// The route id has changed : transform last collected data into a route if selected
					if( commercialLine &&
						lastRouteId != id &&
						lastRouteId > 0
					){
						_selectAndLoadRoute(
							_routes,
							links,
							commercialLine,
							name,
							direction,
							lastRouteId
						);
					}

					// Entering new route
					if(id != lastRouteId)
					{
						string commercialLineRef(result->get<string>("lin_number"));

						// Check of the commercial line
						if(!_lines.contains(commercialLineRef))
						{
							_logWarning(
								"Inconsistent line id "+ lexical_cast<string>(commercialLineRef) +" in ROUTE "+ lexical_cast<string>(id)
							);
							continue;
						}
						commercialLine = *_lines.get(commercialLineRef).begin();

						// Beginning load of the next ref
						lastRouteId = id;
						links.clear();
					}

					// Fields load
					dest = result->get<string>("rou_destination_name");
					direction = (result->get<int>("rou_direction") == 6 ? false : true);
					int linkId(result->get<int>("lnk_id"));

					// Check of link associated
					LinksMap::iterator itLink(_links.find(linkId));
					if(itLink == _links.end())
					{
						_logWarningDetail(
							"ROUTE",lexical_cast<string>(id),lexical_cast<string>(linkId),0,string(),string(), string(),"Bad link reference"
						);
						continue;
					}

					Link& lnk(
						*links.insert(
							links.end(),
							Link()
					)	);
					lnk.id = linkId;
					lnk.stops = itLink->second.stops;

					// Build the route name
					string frontName;
					string backName;
					if(links.size() == 1)
					{
						frontName = links.front().stops.front().getStopName();
						backName = links.front().stops.back().getStopName();
					}
					else
					{
						frontName = links.front().stops.front().getStopName();
						backName = links.back().stops.back().getStopName();
					}
					name = frontName + "-" + backName;
				}
				// Load the last route
				if(	commercialLine
				){
					_selectAndLoadRoute(
						_routes,
						links,
						commercialLine,
						name,
						direction,
						lastRouteId
					);
				}
			}

			// Calendars
			{
				string query(
					string("SELECT * FROM ") + _database + ".v_rcs_calendar" + " ORDER BY HTY_Id"
				);
				DBResultSPtr result(db->execQuery(query));
				int lastHtyId(0);
				Calendar c;

				while(result->next())
				{
					int id(result->get<int>("hty_id"));
					if(id != lastHtyId &&
					   lastHtyId > 0)
					{
						_calendars.insert(make_pair(lastHtyId, c));
					}
					if(id != lastHtyId)
					{
						lastHtyId = id;
						c.clear();
					}
					date calDate(from_string(result->get<string>("cal_day")));
					c.setActive(calDate);
				}
				// Load last calendar
				_calendars.insert(make_pair(lastHtyId, c));
			}

			// Services
			{
				string query(
					string("SELECT * FROM ") + _database +".v_rcs_passing_time " + "WHERE run_number_ext NOT LIKE 'HLP%' " + 
						"ORDER BY rou_id ASC, run_id ASC, pti_rank ASC"
				);
				DBResultSPtr result(db->execQuery(query));
				int lastRunId(0);
				const Route* route(NULL);
				Calendar calendar;
				HanoverTheoricalSchedules schedules;
				string service_number;
				int htyId(0);

				while(result->next())
				{
					int id(result->get<int>("run_id"));
					if(route &&
					   lastRunId != id &&
					   lastRunId > 0)
					{
						_selectAndLoadRun(
							_runs,
							lastRunId,
							*const_cast<Route*>(route),
							calendar,
							schedules,
							service_number
						);
					}
					// Entering new run : check if the route exists
					if(id != lastRunId)
					{
						schedules.clear();
						lastRunId = id;
						int routeId(result->get<int>("rou_id"));
						service_number = result->get<string>("run_number_ext");

						// Check of the journey
						RoutesMap::const_iterator itRoute(
							_routes.find(
								routeId
						)	);
						if(itRoute == _routes.end())
						{
							route = NULL;
							_logWarningDetail(
								"RUN",lexical_cast<string>(id),service_number,0, string(), string(), lexical_cast<string>(routeId),"Bad route field in run"
							);
						}
						else
						{
							route = &itRoute->second;
						}
					}
					if(!route)
					{
						continue;
					}
					// Getting the schedule
					HanoverTheoricalSchedule& schedule(
						*schedules.insert(
							schedules.end(),
							HanoverTheoricalSchedule()
					)	);
					schedule.dept = duration_from_string(result->get<string>("pti_scheduled"));

					// Getting calendar
					htyId = result->get<int>("hty_id");
					Calendars::const_iterator it(_calendars.find(htyId));
					if(it == _calendars.end())
					{
						_logWarning(
							"Inconsistent HTY id "+ lexical_cast<string>(htyId) +" in the run "+ lexical_cast<string>(id)
						);
						continue;
					}
					calendar = it->second;
				}
				// Load last run
				if(route)
				{
					_selectAndLoadRun(
						_runs,
						lastRunId,
						*const_cast<Route*>(route),
						calendar,
						schedules,
						service_number
					);
				}
			}

			// Registering data in ENV
			{
				BOOST_FOREACH(const RunsMap::value_type& run, _runs)
				{
					JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
					std::set<StopPoint*> sps;
					size_t rank(0);
					MetricOffset nextOffset(0);
					BOOST_FOREACH(const Link& link, run.second.route->links)
					{
						bool isDeparture = (rank+1 < run.second.route->links.size() + 1);
						bool isArrival = rank > 0;
						sps.insert(link.stops.front().syntheseStop);
						JourneyPattern::StopWithDepartureArrivalAuthorization stop(
							sps,
							nextOffset,
							isDeparture,
							isArrival,
							1 /* with times */,
							link.lineString
						);
						stops.push_back(stop);
						// make the stop points set begin by the stop point associated to the next link point
						sps.clear();

						// Only for last link, get the second stop point of the link too
						if(rank +1 == run.second.route->links.size())
						{
							isDeparture = (rank+1 < run.second.route->links.size() + 1);
							isArrival = rank > 0;
							sps.insert(link.stops.back().syntheseStop);
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								sps,
								link.stops[1].offsetFromPreviousStop,
								isDeparture,
								isArrival,
								1 /* with times = 1 and no line string given as last arg */
							);
							stops.push_back(stop);
							sps.clear();
						}
						nextOffset = link.stops[1].offsetFromPreviousStop;
						++rank;
					}

					JourneyPattern* journeyPattern(
						_createOrUpdateRoute(
							*run.second.route->line,
							lexical_cast<string>(run.second.route->id),
							run.second.route->name,
							optional<const string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							run.second.route->direction,
							NULL,
							stops,
							dataSource,
							true,
							true,
							true,
							true
					)	);

					// Service
					ScheduledService::Schedules departures;
					ScheduledService::Schedules arrivals;
					time_duration nextArrivalTime = run.second.schedules.front().dept;
					BOOST_FOREACH(const HanoverTheoricalSchedule& schedule, run.second.schedules)
					{
						arrivals.push_back(nextArrivalTime);
						departures.push_back(schedule.dept);
						nextArrivalTime = schedule.dept;
					}

					ScheduledService* service(
						_createOrUpdateService(
							*journeyPattern,
							departures,
							arrivals,
							run.second.service_number,
							dataSource,
							optional<const string&>(),
							optional<const RuleUser::Rules&>(),
							optional<const JourneyPattern::StopsWithDepartureArrivalAuthorization&>(stops),
							run.second.service_number
					)	);
					if(service)
					{
						*service |= run.second.calendar;
					}
				}
			} // end of registering data in ENV

			return true;
		}



		void HanoverTheoricalFileFormat::Importer_::_logLoadDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logLoad(content.str());
		}



		void HanoverTheoricalFileFormat::Importer_::_logWarningDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logWarning(content.str());
		}



		void HanoverTheoricalFileFormat::Importer_::_logDebugDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logDebug(content.str());
		}



		void HanoverTheoricalFileFormat::Importer_::_logTraceDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logTrace(content.str());
		}



		string HanoverTheoricalFileFormat::Importer_::HanoverTheoricalLineStop::getStopName() const
		{
			return syntheseStop->getName();
		}



		DBTransaction HanoverTheoricalFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

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

			// Created journey patterns
			BOOST_FOREACH(const JourneyPattern::Registry::value_type& journeyPattern, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(journeyPattern.second.get(), transaction);
			}

			// Created line stops
			BOOST_FOREACH(const LineStop::Registry::value_type& lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}

			// Created services
			BOOST_FOREACH(const ScheduledService::Registry::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}

			return transaction;
		}
	}
}

