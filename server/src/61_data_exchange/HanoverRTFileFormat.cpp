
//////////////////////////////////////////////////////////////////////////
/// HanoverRTFileFormat class implementation.
/// @file HanoverRTFileFormat.cpp
/// @author Camille Hue
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

#include "HanoverRTFileFormat.hpp"

#include "ContinuousServiceTableSync.h"
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
	const string FactorableTemplate<FileFormat, data_exchange::HanoverRTFileFormat>::FACTORY_KEY = "HanoverRealTime";

	namespace data_exchange
	{
		const string HanoverRTFileFormat::Importer_::PARAMETER_DB_CONN_STRING("conn_string");
		const string HanoverRTFileFormat::Importer_::PARAMETER_HYSTERESIS = "hysteresis";



		HanoverRTFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			DatabaseReadImporter<HanoverRTFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			HanoverFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_stopAreas(*import.get<DataSource>(), env),
			_stopPoints(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env)
		{}



		ParametersMap HanoverRTFileFormat::Importer_::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void HanoverRTFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			_dbConnString = map.getOptional<string>(PARAMETER_DB_CONN_STRING);

			// Hysteresis
			_hysteresis = seconds(
				map.getDefault<long>(PARAMETER_HYSTERESIS, 0)
			);
		}




		bool HanoverRTFileFormat::Importer_::_read(
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
			// Pre-loading objects from Hanover
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

					if (!_stopAreas.contains(ref))
					{
						stringstream content;
						content << "Unrecognized stop area " << ref << "(" << name << ")";
						_logError(content.str());
						return false;
					}
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

					if(!_stopPoints.contains(ref))
					{
						stringstream content;
						content << "Unrecognized stop point " << ref << "(" << name << "), should be in stop area " << areaRef;
						_logError(content.str());
						return false;
					}
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

					if(!_lines.contains(shortName))
					{
						stringstream content;
						content << "Unrecognized commercial line " << shortName << "(" << name << ")";
						_logError(content.str());
						return false;
					}

					CommercialLine* commercialLine = *_lines.get(shortName).begin();
					JourneyPatternTableSync::Search(_env, commercialLine->getKey());
					ScheduledServiceTableSync::Search(_env, optional<RegistryKeyType>(), commercialLine->getKey());
					ContinuousServiceTableSync::Search(_env, optional<RegistryKeyType>(), commercialLine->getKey());
					BOOST_FOREACH(const Path* route, commercialLine->getPaths())
					{
						LineStopTableSync::Search(_env, route->getKey());
					}
				}
			}

			// Links and Points
			{
				string query(
					"SELECT * FROM " + _database + ".v_rcs_link" + " ORDER BY lnk_id ASC, ptl_order ASC"
				);
				DBResultSPtr result(db->execQuery(query));
				int lastLinkId(0);
				Link::HanoverLineStops linkStops;
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
						HanoverLineStop& vs(*linkStops.insert(linkStops.end(),HanoverLineStop()));
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
						"AND run_id in (SELECT DISTINCT run_id FROM " + _database + "v_rcs_passing_time WHERE pti_real_estimated IS NOT NULL) " +
						"ORDER BY rou_id ASC, run_id ASC, pti_rank ASC"
				);
				DBResultSPtr result(db->execQuery(query));
				int lastRunId(0);
				const Route* route(NULL);
				Calendar calendar;
				HanoverSchedules schedules;
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
					HanoverSchedule& schedule(
						*schedules.insert(
							schedules.end(),
							HanoverSchedule()
					)	);
					schedule.dept = duration_from_string(result->get<string>("pti_scheduled"));
					if (result->get<string>("pti_real_estimated").empty())
					{
						schedule.deptRT = schedule.dept;
					}
					else
					{
						schedule.deptRT = duration_from_string(result->get<string>("pti_real_estimated"));
					}

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

			// Update RT schedules
			{
				BOOST_FOREACH(const RunsMap::value_type& run, _runs)
				{
					JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
					std::set<StopPoint*> sps;
					size_t rank(0);
					MetricOffset nextOffset(0);
					BOOST_FOREACH(const Link& link, run.second.route->links)
					{
						bool isDeparture = (rank < run.second.route->links.size());
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
						if(rank + 1 == run.second.route->links.size())
						{
							isDeparture = false;//last stop is never departure
							isArrival = true;//last stop is always arrival
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

					string routeId(lexical_cast<string>(run.second.route->id));
					JourneyPattern* journeyPattern(
						_createOrUpdateRoute(
							*run.second.route->line,
							routeId,
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
					
					// Theorical Service (find or create it)
					ScheduledService::Schedules departures;
					ScheduledService::Schedules arrivals;
					ScheduledService::Schedules RTdepartures;
					ScheduledService::Schedules RTarrivals;
					time_duration nextArrivalTime = run.second.schedules.front().dept;
					BOOST_FOREACH(const HanoverSchedule& schedule, run.second.schedules)
					{
						time_duration delta(schedule.dept - schedule.deptRT);
						if(nextArrivalTime.seconds())
						{
							arrivals.push_back(nextArrivalTime + seconds(60 - nextArrivalTime.seconds()));
							RTarrivals.push_back(nextArrivalTime + seconds(60 - nextArrivalTime.seconds()) - delta);
						}
						else
						{
							arrivals.push_back(nextArrivalTime);
							RTarrivals.push_back(nextArrivalTime - delta);
						}
						if(schedule.dept.seconds())
						{
							departures.push_back(schedule.dept - seconds(schedule.dept.seconds()));
							RTdepartures.push_back(schedule.dept - seconds(schedule.dept.seconds()) - delta);
						}
						else
						{
							departures.push_back(schedule.dept);
							RTdepartures.push_back(schedule.dept - delta);
						}
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
					
					// Update it with Real Time info
					bool updated(false);
					time_duration maxDelta(seconds(0));
					time_duration minDelta(seconds(0));
					for(size_t i(0); i<departures.size(); ++i)
					{
						time_duration delta(departures[i] - RTdepartures[i]);
						if(delta<minDelta)
						{
							minDelta = delta;
						}
						if(delta>maxDelta)
						{
							maxDelta = delta;
						}
						if(delta < -_hysteresis || delta > _hysteresis)
						{
							updated = true;
							break;
						}
						delta = arrivals[i] - RTarrivals[i];
						if(delta<minDelta)
						{
							minDelta = delta;
						}
						if(delta>maxDelta)
						{
							maxDelta = delta;
						}
						if(delta < -_hysteresis || delta > _hysteresis)
						{
							updated = true;
							break;
						}
					}
					if(!updated && !service->hasRealTimeData())
					{
						// No update is needed but we do it anyway so that RT schedule will be set
						service->setRealTimeSchedules(departures, arrivals);
					}
					else if(updated)
					{
						// if course is ended, don't update it because it may have been cleaned by RTDataCleaner
						// and we do not want update the course for tomorrow
						boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
						if(!(now.time_of_day() > service->GetTimeOfDay(RTdepartures[RTdepartures.size()-1]) + minutes(1)))
						{
							service->setRealTimeSchedules(RTdepartures, RTarrivals);
							_logDebugDetail(
								"REAL-TIME SCHEDULES UPDATE",
								run.second.service_number,
								run.second.route->line->getShortName()+"/"+routeId,
								service->getKey(),
								service->getServiceNumber(),
								string(),
								lexical_cast<string>(minDelta)+"s/"+lexical_cast<string>(maxDelta)+"s",
								string()
							);
						}
					}
				}
			} // end of registering data in ENV

			return true;
		}



		DBTransaction HanoverRTFileFormat::Importer_::_save() const
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

