
/** PTFileFormat class implementation.
	@file PTFileFormat.cpp

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

#include "PTFileFormat.hpp"

#include "DesignatedLinePhysicalStop.hpp"
#include "ImportLogger.hpp"
#include "JourneyPatternTableSync.hpp"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "LineStopTableSync.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "LineStopTableSync.h"
#include "PTPlaceAdmin.h"
#include "StopAreaAddAction.h"
#include "AdminActionFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "StopPointAdmin.hpp"
#include "StopPointAddAction.hpp"
#include "AdminFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLTable.h"
#include "City.h"
#include "StopPointUpdateAction.hpp"
#include "DestinationTableSync.hpp"
#include "LineStopTableSync.h"
#include "RollingStockTableSync.hpp"

#include <geos/operation/distance/DistanceOp.h>

using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace pt;
	using namespace util;
	using namespace vehicle;
	using namespace impex;
	using namespace graph;
	using namespace admin;
	using namespace html;

	namespace data_exchange
	{
		boost::shared_ptr<JourneyPattern> PTFileFormat::CreateJourneyPattern(
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
			CommercialLine& line,
			const impex::DataSource& source,
			Env& env,
			const impex::ImportLogger& importLogger
		){
			boost::shared_ptr<JourneyPattern> route(new JourneyPattern);
			route->setCommercialLine(&line);
			route->addCodeBySource(source, string());
			route->setKey(JourneyPatternTableSync::getId());
			env.getEditableRegistry<JourneyPattern>().add(route);
			line.addPath(route.get());

			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& stop, stops)
			{
				boost::shared_ptr<DesignatedLinePhysicalStop> ls(
					new DesignatedLinePhysicalStop(
						LineStopTableSync::getId(),
						route.get(),
						rank,
						rank+1 < stops.size() && stop._departure,
						rank > 0 && stop._arrival,
						stop._metricOffset ? *stop._metricOffset : 0,
						*stop._stop.begin(),
						stop._withTimes
				)	);
				route->addEdge(*ls);
				env.getEditableRegistry<LineStop>().add(ls);

				++rank;
			}

			return route;
		}



		TransportNetwork* PTFileFormat::CreateOrUpdateNetwork(
			impex::ImportableTableSync::ObjectBySource<TransportNetworkTableSync>& networks,
			const std::string& id,
			const std::string& name,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger
		){
			TransportNetwork* network;
			if(networks.contains(id))
			{
				set<TransportNetwork*> loadedNetworks(networks.get(id));
				if(loadedNetworks.size() > 1)
				{
					importLogger.log(
						ImportLogger::WARN, 
						"More than one network with key "+ id
					);
				}
				network = *loadedNetworks.begin();
				importLogger.log(
					ImportLogger::LOAD,
					"Use of existing network "+ lexical_cast<string>(network->getKey()) +" ("+ network->getName() +")"
				);
			}
			else
			{
				network = new TransportNetwork(
					TransportNetworkTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				network->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<TransportNetwork>().add(boost::shared_ptr<TransportNetwork>(network));
				networks.add(*network);
				importLogger.log(
					ImportLogger::CREA,
					"Creation of the network with key "+ id +" ("+ name + ")"
				);
			}
			network->setName(name);
			return network;
		}



		std::set<StopArea*> PTFileFormat::GetStopAreas(
			const impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			boost::optional<const std::string&> name,
			const impex::ImportLogger& importLogger,
			bool errorIfNotFound /*= true */
		){
			if(stopAreas.contains(id))
			{
				set<StopArea*> loadedStopAreas(stopAreas.get(id));

				stringstream logStream;
				logStream
					<< "Link between stop areas " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << " and ";
				BOOST_FOREACH(StopArea* sp, loadedStopAreas)
				{
					logStream << sp->getKey() << " (" << sp->getFullName() << ") ";
				}
				importLogger.log(ImportLogger::LOAD, logStream.str());

				return loadedStopAreas;
			}

			if(errorIfNotFound)
			{
				stringstream logStream;
				logStream << "Stop area not found " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				importLogger.log(ImportLogger::ERROR, logStream.str());
			}
			return set<StopArea*>();
		}





		StopArea* PTFileFormat::CreateStopArea(
			impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			const std::string& name,
			geography::City& city,
			boost::posix_time::time_duration defaultTransferDuration,
			bool mainStopArea,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger
		){
			StopArea* stopArea(
				new StopArea(
					StopAreaTableSync::getId(),
					true,
					defaultTransferDuration
			)	);
			Importable::DataSourceLinks links;
			links.insert(make_pair(&source, id));
			stopArea->setDataSourceLinksWithoutRegistration(links);
			stopArea->setCity(&city);
			if(mainStopArea)
			{
				city.addIncludedPlace(*stopArea);
			}
			env.getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(stopArea));
			stopAreas.add(*stopArea);

			importLogger.logCreation(
				"Creation of the stop area with key "+ id +" ("+ city.getName() +" "+ name + ")"
			);

			return stopArea;
		}



		set<StopArea*> PTFileFormat::CreateOrUpdateStopAreas(
			impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			const std::string& name,
			const geography::City* city,
			bool updateCityIfExists,
			boost::posix_time::time_duration defaultTransferDuration,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger
		){
			// Load if possible
			set<StopArea*> result(GetStopAreas(stopAreas, id, name, importLogger, false));

			// Create if necessary
			if(result.empty())
			{
				// Abort if undefined city
				if(!city)
				{
					importLogger.log(
						ImportLogger::WARN,
						"The stop area "+ name +" cannot be created because of undefined city."
					);
					return result;
				}

				result.insert(
					CreateStopArea(
						stopAreas,
						id,
						name,
						*const_cast<geography::City*>(city), // Possible because of false in main parameter
						defaultTransferDuration,
						false,
						source,
						env,
						importLogger
				)	);
			}

			// Update
			BOOST_FOREACH(StopArea* stopArea, result)
			{
				if(updateCityIfExists)
				{
					stopArea->setCity(city);
				}
				stopArea->setName(name);
			}

			return result;
		}



		std::set<StopPoint*> PTFileFormat::GetStopPoints(
			const impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
			const std::string& id,
			boost::optional<const std::string&> name,
			const impex::ImportLogger& importLogger,
			bool errorIfNotFound
		){
			if(stopPoints.contains(id))
			{
				set<StopPoint*> loadedStopPoints(stopPoints.get(id));

				stringstream logStream;
				logStream << "Link between stops " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << " and ";
				BOOST_FOREACH(StopPoint* sp, loadedStopPoints)
				{
					logStream << sp->getKey() << " (" << sp->getConnectionPlace()->getFullName() << ") ";
				}
				importLogger.log(ImportLogger::LOAD, logStream.str());

				return loadedStopPoints;
			}
			if(errorIfNotFound)
			{
				stringstream logStream;
				logStream << "Stop not found " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				importLogger.log(ImportLogger::ERROR, logStream.str());
			}
			return set<StopPoint*>();
		}



		StopPoint* PTFileFormat::CreateStop(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stops,
			const std::string& code,
			boost::optional<const std::string&> name,
			const StopArea& stopArea,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger
		){
			// Object creation
			StopPoint* stop(
				new StopPoint(
					StopPointTableSync::getId(),
					string(),
					&stopArea,
					boost::shared_ptr<Point>(),
					false
			)	);
			Importable::DataSourceLinks links;
			links.insert(make_pair(&source, code));
			stop->setDataSourceLinksWithoutRegistration(links);
			env.getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stop));
			stops.add(*stop);

			// Properties
			if(name)
			{
				stop->setName(*name);
			}

			// Log
			stringstream logStream;
			logStream << "Creation of the physical stop with key " << code;
			if(name)
			{
				logStream << " (" << *name <<  ")";
			}
			importLogger.log(ImportLogger::CREA, logStream.str());

			// Return
			return stop;
		}



		set<StopPoint*> PTFileFormat::CreateOrUpdateStop(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stops,
			const std::string& code,
			boost::optional<const std::string&> name,
			boost::optional<const graph::RuleUser::Rules&> rules,
			boost::optional<const StopArea*> stopArea,
			boost::optional<const StopPoint::Geometry*> geometry,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger,
			bool doNotUpdate
		){
			// Load if possible
			bool creation(false);
			set<StopPoint*> result(GetStopPoints(stops, code, name, importLogger, false));

			// Creation if necessary
			if(result.empty())
			{
				if(!stopArea || !*stopArea)
				{
					return result;
				}

				result.insert(
					CreateStop(
						stops,
						code,
						name,
						**stopArea,
						source,
						env,
						importLogger
				)	);
				creation = true;
			}
			else
			{
				stringstream logStream;
				logStream << "Link with existing stop " << (*result.begin())->getName() << " for stop " << code;
				if(name)
				{
					logStream << " (" << *name <<  ")";
				}
				importLogger.log(ImportLogger::LOAD, logStream.str());
			}

			// Update
			if(!creation && doNotUpdate)
			{
				return result;
			}

			BOOST_FOREACH(StopPoint* stop, result)
			{
				if(name)
				{
					stop->setName(*name);
				}

				if(geometry && *geometry)
				{
					stop->setGeometry(
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(**geometry)
					);
				}

				if(rules)
				{
					stop->setRules(*rules);
				}

				if(stopArea)
				{
					stop->setHub(*stopArea);
				}
			}
			return result;
		}



		set<StopPoint*> PTFileFormat::CreateOrUpdateStopWithStopAreaAutocreation(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stops,
			const std::string& code,
			const std::string& name,
			boost::optional<const StopPoint::Geometry*> geometry,
			const geography::City& cityForStopAreaAutoGeneration,
			boost::optional<boost::posix_time::time_duration> defaultTransferDuration,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger,
			boost::optional<const graph::RuleUser::Rules&> rules
		){
			// Load if possible
			set<StopPoint*> result(GetStopPoints(stops, code, name, importLogger, false));

			// Creation if necessary
			if(result.empty())
			{
				// Search for an existing stop area
				StopArea* curStop(NULL);
				StopAreaTableSync::SearchResult stopAreas(
					StopAreaTableSync::Search(
						env,
						cityForStopAreaAutoGeneration.getKey(),
						logic::indeterminate,
						optional<string>(),
						name
				)	);
				if(stopAreas.empty())
				{
					BOOST_FOREACH(Registry<StopArea>::value_type stopArea, env.getRegistry<StopArea>())
					{
						if((stopArea.second->getName() == name) && (cityForStopAreaAutoGeneration.getKey() == stopArea.second->getCity()->getKey()))
						{
							curStop = stopArea.second.get();
							break;
						}
					}

					if(!curStop)
					{
						curStop = new StopArea(StopAreaTableSync::getId(), true);
						Importable::DataSourceLinks links;
						links.insert(make_pair(&source, string()));
						curStop->setDataSourceLinksWithoutRegistration(links);
						if(defaultTransferDuration)
						{
							curStop->setDefaultTransferDelay(*defaultTransferDuration);
						}
						curStop->setName(name);
						curStop->setCity(&cityForStopAreaAutoGeneration);
						env.getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(curStop));
						importLogger.log(
							ImportLogger::CREA,
							"Auto generation of the commercial stop for stop "+ code +" ("+ name +")"
						);
					}
					else
					{
						importLogger.log(
							ImportLogger::LOAD,
							"Link with existing commercial stop "+ curStop->getFullName() +" for stop "+ code +" ("+ name +")"
						);
					}
				}
				else
				{
					curStop = stopAreas.begin()->get();
					importLogger.log(
						ImportLogger::LOAD,
						"Link with existing commercial stop "+ curStop->getFullName() +" for stop "+ code +" ("+ name +")"
					);
				}

				// Stop creation
				result.insert(
					CreateStop(
						stops,
						code,
						name,
						*curStop,
						source,
						env,
						importLogger
				)	);
			}
			else
			{
				importLogger.log(
					ImportLogger::LOAD,
					"Link with existing stop "+ (*result.begin())->getName() +" for stop "+ code +" ("+ name +")"
				);
			}

			// Update
			BOOST_FOREACH(StopPoint* stop, result)
			{
				if(stop->getName() != name)
				{
					importLogger.log(
						ImportLogger::INFO,
						"Stop "+ code +" ("+ stop->getName() +") renamed to "+ name
					);
					stop->setName(name);
				}
				if(geometry && *geometry)
				{
					stop->setGeometry(
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(**geometry)
					);
				}

				if(rules)
				{
					stop->setRules(*rules);
				}
			}
			return result;
		}



		CommercialLine* PTFileFormat::CreateOrUpdateLine(
			impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
			const std::string& id,
			optional<const std::string&> name,
			optional<const std::string&> shortName,
			boost::optional<util::RGBColor> color,
			TransportNetwork& defaultNetwork,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger,
			bool restrictInDefaultNetwork
		){
			CommercialLine* line(
				GetLine(
					lines,
					id,
					source,
					env,
					importLogger,
					restrictInDefaultNetwork ? optional<TransportNetwork&>(defaultNetwork) : optional<TransportNetwork&>()
			)	);
			if(!line)
			{
				line = new CommercialLine(CommercialLineTableSync::getId());

				stringstream logStream;
				logStream << "Creation of the commercial line with key " << id;
				if(name)
				{
					logStream << " (" << *name <<  ")";
				}
				importLogger.log(ImportLogger::CREA, logStream.str());

				line->setParent(defaultNetwork);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				line->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<CommercialLine>().add(boost::shared_ptr<CommercialLine>(line));
				lines.add(*line);
			}

			if(name)
			{
				line->setName(*name);
			}
			if(shortName)
			{
				line->setShortName(*shortName);
			}
			if(color)
			{
				line->setColor(color);
			}

			return line;
		}



		JourneyPattern* PTFileFormat::CreateOrUpdateRoute(
			pt::CommercialLine& line,
			boost::optional<const std::string&> id,
			boost::optional<const std::string&> name,
			boost::optional<const std::string&> destination,
			boost::optional<Destination*> destinationObj,
			boost::optional<const RuleUser::Rules&> rules,
			boost::optional<bool> wayBack,
			vehicle::RollingStock* rollingStock,
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger,
			bool removeOldCodes,
			bool updateMetricOffsetOnUpdate,
			bool attemptToCopyExistingGeometries,
			bool allowDifferentStopPointsInSameStopArea
		){
			// Declaration
			bool creation(false);

			// Attempting to find an existing route by value comparison
			JourneyPattern* result(NULL);
			BOOST_FOREACH(Path* route, line.getPaths())
			{
				// Avoid junctions
				if(!dynamic_cast<JourneyPattern*>(route))
				{
					continue;
				}

				JourneyPattern* jp(static_cast<JourneyPattern*>(route));

				if(!jp->hasLinkWithSource(source))
				{
					continue;
				}

				if(	(!rollingStock || jp->getRollingStock() == rollingStock) &&
					(!id || jp->hasCodeBySource(source, *id)) &&
					(!rules || jp->getRules() == *rules) &&
					(	(allowDifferentStopPointsInSameStopArea && jp->compareStopAreas(servedStops)) ||
						(!allowDifferentStopPointsInSameStopArea && *jp == servedStops)
					)
				){
					if(!result)
					{
						importLogger.log(
							ImportLogger::LOAD,
							"Use of route "+ lexical_cast<string>(jp->getKey()) +" ("+ jp->getName() +") for "+ (id ? *id : string("unknown")) +")"
						);
						result = jp;
						if(!id)
						{
							break;
						}
					}
					else
					{
						if(removeOldCodes && id)
						{
							jp->removeSourceLink(source, *id);
							jp->addCodeBySource(source, string());
							importLogger.log(
								ImportLogger::INFO,
								"Code "+ *id +" was removed from route "+ lexical_cast<string>(jp->getKey())
							);
						}
						else
						{
							importLogger.log(
								ImportLogger::WARN,
								"Route "+ *id +") is defined twice or more."
							);
						}
					}
				}
			}

			// Create a new route if necessary
			if(!result)
			{
				creation = true;
				importLogger.log(
					ImportLogger::CREA,
					"Creation of route "+ (name ? *name : string()) +" for "+ (id ? *id : string("unknown"))
				);
				result = new JourneyPattern(
					JourneyPatternTableSync::getId()
				);

				// Line link
				result->setCommercialLine(&line);
				line.addPath(result);

				// Source links
				Importable::DataSourceLinks links;
				if(id)
				{
					links.insert(make_pair(&source, *id));
				}
				else
				{
					links.insert(make_pair(&source, string()));
				}
				result->setDataSourceLinksWithoutRegistration(links);

				// Storage in the environment
				env.getEditableRegistry<JourneyPattern>().add(boost::shared_ptr<JourneyPattern>(result));

				// Served stops
				size_t rank(0);
				BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
				{
					boost::shared_ptr<DesignatedLinePhysicalStop> ls(
						new DesignatedLinePhysicalStop(
							LineStopTableSync::getId(),
							result,
							rank,
							rank+1 < servedStops.size() && stop._departure,
							rank > 0 && stop._arrival,
							0,
							*stop._stop.begin(),
							stop._withTimes ? *stop._withTimes : true
					)	);
					result->addEdge(*ls);
					env.getEditableRegistry<LineStop>().add(ls);
					++rank;
				}

				// Geometries
				if(attemptToCopyExistingGeometries)
				{
					for(Path::Edges::iterator itEdge(result->getEdges().begin()); itEdge != result->getEdges().end() && itEdge+1 != result->getEdges().end(); ++itEdge)
					{
						// Don't update already defined geometry
						if((*itEdge)->getGeometry().get())
						{
							continue;
						}

						Env env2;
						boost::shared_ptr<DesignatedLinePhysicalStop> templateObject(
							LineStopTableSync::SearchSimilarLineStop(
								static_cast<const StopPoint&>(*(*itEdge)->getFromVertex()),
								static_cast<const StopPoint&>(*(*(itEdge+1))->getFromVertex()),
								env2
						)	);
						if(templateObject.get())
						{
							(*itEdge)->setGeometry(templateObject->getGeometry());
						}
				}	}
			}


			//////////////////////////////////////////////////////////////////////////
			// Updates

			// Metric offsets
			if(creation || updateMetricOffsetOnUpdate)
			{
				Path::Edges::const_iterator it(result->getEdges().begin());
				BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
				{
					if(stop._metricOffset)
					{
						const_cast<Edge*>(*it)->setMetricOffset(*stop._metricOffset);
					}
					++it;
				}
			}

			// Geometries
			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
			{
				if(stop._geometry.get())
				{
					const_cast<Edge*>(result->getEdge(rank))->setGeometry(stop._geometry);
				}
				++rank;
			}

			// Name
			if(name)
			{
				result->setName(*name);
			}

			result->setNetwork(line.getNetwork());
			result->setTimetableName(line.getShortName());

			// Destination text
			if(destination)
			{
				result->setDirection(*destination);
			}

			// Destination sign id
			if(destinationObj)
			{
				result->setDirectionObj(*destinationObj);
			}

			// Transport mode
			if(rollingStock)
			{
				result->setRollingStock(rollingStock);
			}

			// Use rules
			if(rules)
			{
				result->setRules(*rules);
			}

			// Wayback
			if(wayBack)
			{
				result->setWayBack(*wayBack);
			}

			return result;
		}



		ScheduledService* PTFileFormat::CreateOrUpdateService(
			JourneyPattern& route,
			const SchedulesBasedService::Schedules& departureSchedules,
			const SchedulesBasedService::Schedules& arrivalSchedules,
			const std::string& number,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger,
			boost::optional<const std::string&> team,
			boost::optional<const graph::RuleUser::Rules&> rules,
			boost::optional<const JourneyPattern::StopsWithDepartureArrivalAuthorization&> servedVertices
		){
			// Comparison of the size of schedules and the size of the route
			if(	route.getScheduledStopsNumber() != departureSchedules.size() ||
				route.getScheduledStopsNumber() != arrivalSchedules.size()
			){
				importLogger.log(
					ImportLogger::WARN,
					"Inconsistent schedules size in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
				return NULL;
			}

			// Checks for schedules validity
			BOOST_FOREACH(const time_duration& td, departureSchedules)
			{
				if(td.is_not_a_date_time())
				{
					importLogger.log(
						ImportLogger::WARN,
						"At least an undefined time in departure schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}
			BOOST_FOREACH(const time_duration& ta, arrivalSchedules)
			{
				if(ta.is_not_a_date_time())
				{
					importLogger.log(
						ImportLogger::WARN,
						"At least an undefined time in arrival schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}

			// Search for a corresponding service
			ScheduledService* result(NULL);
			{
				boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
					*route.sharedServicesMutex
				);
				BOOST_FOREACH(Service* tservice, route.getServices())
				{
					ScheduledService* curService(dynamic_cast<ScheduledService*>(tservice));

					if(!curService) continue;

					if(	curService->getServiceNumber() == number &&
						curService->comparePlannedSchedules(departureSchedules, arrivalSchedules) &&
						(!servedVertices || curService->comparePlannedStops(*servedVertices)) &&
						(team ? curService->getTeam() == *team : true) &&
						(rules ? curService->getRules() == *rules : true)
					){
						result = curService;
						break;
					}
				}
			}

			// If not found creation
			if(!result)
			{
				result = new ScheduledService(
					ScheduledServiceTableSync::getId(),
					number,
					&route
				);
				result->setSchedules(departureSchedules, arrivalSchedules, true);
				result->setPath(&route);

				if(team)
				{
					result->setTeam(*team);
				}

				if(rules)
				{
					result->setRules(*rules);
				}

				if(servedVertices)
				{
					SchedulesBasedService::ServedVertices vertices;
					size_t stopRank(0);
					BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& itStop, *servedVertices)
					{
						// Choosing the vertex in the same hub than in the path
						Vertex* vertex(route.getEdge(stopRank)->getFromVertex());
						if(	itStop._stop.find(static_cast<StopPoint*>(vertex)) == itStop._stop.end())
						{
							BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization::StopsSet::value_type& itStopLink, itStop._stop)
							{
								if(vertex->getHub() == itStopLink->getHub())
								{
									vertex = itStopLink;
									break;
								}
							}
						}
						vertices.push_back(vertex);
						++stopRank;
					}
					result->setVertices(vertices);
				}

				route.addService(*result, false);
				env.getEditableRegistry<ScheduledService>().add(boost::shared_ptr<ScheduledService>(result));

				importLogger.log(
					ImportLogger::CREA,
					"Creation of service "+ result->getServiceNumber() +" for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}
			else
			{
				importLogger.log(
					ImportLogger::LOAD,
					"Use of service "+ lexical_cast<string>(result->getKey()) +" ("+ result->getServiceNumber() +") for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}

			return result;
		}



		ContinuousService* PTFileFormat::CreateOrUpdateContinuousService(
			JourneyPattern& route,
			const SchedulesBasedService::Schedules& departureSchedules,
			const SchedulesBasedService::Schedules& arrivalSchedules,
			const std::string& number,
			const boost::posix_time::time_duration& range,
			const boost::posix_time::time_duration& waitingTime,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger
		){
			// Comparison of the size of schedules and the size of the route
			if(	route.getScheduledStopsNumber() != departureSchedules.size() ||
				route.getScheduledStopsNumber() != arrivalSchedules.size()
			){
				importLogger.log(
					ImportLogger::WARN,
					"Inconsistent schedules size in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
				return NULL;
			}

			// Checks for schedules validity
			BOOST_FOREACH(const time_duration& td, departureSchedules)
			{
				if(td.is_not_a_date_time())
				{
					importLogger.log(
						ImportLogger::WARN,
						"At least an undefined time in departure schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}
			BOOST_FOREACH(const time_duration& ta, arrivalSchedules)
			{
				if(ta.is_not_a_date_time())
				{
					importLogger.log(
						ImportLogger::WARN,
						"At least an undefined time in arrival schedules in the service "+ number +" at "+ lexical_cast<string>(departureSchedules[0]) +" on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
					);
					return NULL;
				}
			}

			// Search for a corresponding service
			ContinuousService* result(NULL);
			boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
				*route.sharedServicesMutex
			);
			BOOST_FOREACH(Service* tservice, route.getServices())
			{
				ContinuousService* curService(dynamic_cast<ContinuousService*>(tservice));

				if(!curService) continue;

				if(	curService->getServiceNumber() == number &&
					curService->comparePlannedSchedules(departureSchedules, arrivalSchedules) &&
					curService->getRange() == range &&
					curService->getMaxWaitingTime() == waitingTime
					){
					result = curService;
					break;
				}
			}

			// If not found creation
			if(!result)
			{
				result = new ContinuousService(
					ContinuousServiceTableSync::getId(),
					number,
					&route,
					range,
					waitingTime
				);
				result->setSchedules(departureSchedules, arrivalSchedules, true);
				result->setPath(&route);
				route.addService(*result, false);
				env.getEditableRegistry<ContinuousService>().add(boost::shared_ptr<ContinuousService>(result));

				importLogger.log(
					ImportLogger::CREA,
					"Creation of continuous service "+ result->getServiceNumber() +" for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}
			else
			{
				importLogger.log(
					ImportLogger::LOAD,
					"Use of continuous service "+ lexical_cast<string>(result->getKey()) +" ("+ result->getServiceNumber() +") for "+ number +" ("+ lexical_cast<string>(departureSchedules[0]) +") on route "+ lexical_cast<string>(route.getKey()) +" ("+ route.getName() +")"
				);
			}

			return result;
		}



		void PTFileFormat::DisplayStopAreaImportScreen(
			const ImportableStopAreas& objects,
			const server::Request& request,
			bool createCityIfNecessary,
			bool createPhysicalStop,
			boost::shared_ptr<const geography::City> defaultCity,
			util::Env& env,
			const impex::DataSource& source,
			const impex::ImportLogger& importLogger
		){
			if(objects.empty())
			{
				return;
			}

			// Variables
			bool linked(!objects.begin()->linkedStopAreas.empty());
			AdminFunctionRequest<PTPlaceAdmin> openRequest(request);
			stringstream stream;

			// Title
			stream << "<h1>Zones d'arrêt ";
			if(!linked)
			{
				stream << "non ";
			}
			stream << "liés à SYNTHESE</h1>";

			// Header
			HTMLTable::ColsVector c;
			c.push_back("Code");
			c.push_back("Localité");
			c.push_back("Nom");
			if(linked)
			{
				c.push_back("Zone d'arrêt SYNTHESE");
			}
			if(!linked)
			{
				c.push_back("Actions");
			}

			// Table
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
			stream << t.open();
			stream.precision(0);
			AdminActionFunctionRequest<StopAreaAddAction, DataSourceAdmin> addRequest(request);
			BOOST_FOREACH(const ImportableStopAreas::value_type& object, objects)
			{
				stream << t.row();
				stream << t.col();
				stream << object.operatorCode;

				stream << t.col();
				stream << object.cityName;

				stream << t.col();
				stream << object.name;

				if(linked)
				{
					stream << t.col();
					bool first(true);
					BOOST_FOREACH(StopArea* stopArea, object.linkedStopAreas)
					{
						if(first)
						{
							first = false;
						}
						else
						{
							stream << "<br />";
						}
						openRequest.getPage()->setConnectionPlace(
							env.getSPtr(stopArea)
						);
						stream << HTMLModule::getHTMLLink(
							openRequest.getURL(), stopArea->getFullName()
						);
					}
				}

				if(!linked)
				{
					stream << t.col();
					Importable::DataSourceLinks links;
					links.insert(make_pair(&source, object.operatorCode));
					addRequest.getAction()->setDataSourceLinks(links);
					addRequest.getAction()->setCreateCityIfNecessary(createCityIfNecessary);
					addRequest.getAction()->setName(object.name);
					HTMLForm f(addRequest.getHTMLForm("addStop"+ object.operatorCode));
					stream << f.open();
					stream << f.getTextInput(
						StopAreaAddAction::PARAMETER_CITY_NAME,
						object.cityName.empty() ?
						(defaultCity.get() ? defaultCity->getName() : string()) :
						object.cityName
						);
					stream << f.getSubmitButton("Ajouter");
					stream << f.close();
				}
			}
			stream << t.close();

			importLogger.logRaw(stream.str());
		}



		void PTFileFormat::DisplayStopPointImportScreen(
			const ImportableStopPoints& objects,
			const server::Request& request,
			util::Env& env,
			const impex::DataSource& source,
			const impex::ImportLogger& importLogger
		){
			if(objects.empty())
			{
				return;
			}

			// Variables
			bool linked(!objects.begin()->linkedStopPoints.empty());
			AdminFunctionRequest<PTPlaceAdmin> openRequest(request);
			AdminFunctionRequest<StopPointAdmin> openStopPointRequest(request);
			stringstream stream;

			// Title
			stream << "<h1>Arrêts ";
			if(!linked)
			{
				stream << "non ";
			}
			stream << "liés à SYNTHESE</h1>";

			// Header
			HTMLTable::ColsVector c;
			c.push_back("Code");
			c.push_back("Localité");
			c.push_back("Nom");
			c.push_back("Zone d'arrêt");
			c.push_back("Coords fichier");
			c.push_back("Coords fichier");
			c.push_back("Coords fichier (origine)");
			c.push_back("Coords fichier (origine)");
			if(linked)
			{
				c.push_back("Arrêt SYNTHESE");
				c.push_back("Coords SYNTHESE");
				c.push_back("Coords SYNTHESE");
				c.push_back("Distance");
			}
			c.push_back("Actions");

			// Table
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
			stream << t.open();
			stream.precision(0);
			AdminActionFunctionRequest<StopPointAddAction, DataSourceAdmin> addRequest(request);
			BOOST_FOREACH(const ImportableStopPoints::value_type& object, objects)
			{
				stream << t.row();
				stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
				stream << object.operatorCode;

				stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
				stream << object.cityName;

				stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
				stream << object.name;

				stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
				if(object.stopArea)
				{
					openRequest.getPage()->setConnectionPlace(env.getSPtr(object.stopArea));
					stream << HTMLModule::getHTMLLink(openRequest.getURL(), object.stopArea->getFullName());
				}

				boost::shared_ptr<geos::geom::Point> projectedPoint;
				if(object.coords.get())
				{
					projectedPoint = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*object.coords);

					stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
					stream << fixed << projectedPoint->getX();

					stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
					stream << fixed << projectedPoint->getY();

					stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
					stream << fixed << object.coords->getX();

					stream << t.col(1, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
					stream << fixed << object.coords->getY();
				}
				else
				{
					stream << t.col(4, string(), false, string(), linked ? object.linkedStopPoints.size() : 1);
					stream << "(non localisé)";
				}

				if(linked)
				{
					bool first(true);
					BOOST_FOREACH(StopPoint* stopPoint, object.linkedStopPoints)
					{
						if(first)
						{
							first = false;
						}
						else
						{
							stream << t.row();
						}

						stream << t.col();
						openRequest.getPage()->setConnectionPlace(
							env.getSPtr(stopPoint->getConnectionPlace())
						);
						stream << HTMLModule::getHTMLLink(
							openRequest.getURL(), stopPoint->getConnectionPlace()->getFullName()
						);
						stream << " ";
						openStopPointRequest.getPage()->setStop(
							env.getSPtr(stopPoint)
						);
						stream << HTMLModule::getHTMLLink(
							openStopPointRequest.getURL(), stopPoint->getName()
						);

						stream << t.col();
						if(stopPoint->getGeometry().get())
						{
							stream << std::fixed << stopPoint->getGeometry()->getX();
						}

						stream << t.col();
						if(stopPoint->getGeometry().get())
						{
							stream << std::fixed << stopPoint->getGeometry()->getY();
						}
						else
						{
							stream << "(non localisé)";
						}

						double distance(-1);
						if (stopPoint->getGeometry().get() && object.coords.get())
						{
							distance = geos::operation::distance::DistanceOp::distance(*projectedPoint, *stopPoint->getGeometry());
						}

						stream << t.col();
						if(distance == 0)
						{
							stream << "identiques";
						}
						if(distance > 0)
						{
							stream << distance << " m";
						}

						stream << t.col();
						if(distance > 0)
						{
							AdminActionFunctionRequest<StopPointUpdateAction, DataSourceAdmin> moveRequest(request);
							moveRequest.getAction()->setStop(
								env.getEditableSPtr(stopPoint)
							);
							moveRequest.getAction()->setPoint(
								object.coords
							);
							stream << HTMLModule::getLinkButton(moveRequest.getHTMLForm().getURL(), "Mettre à jour coordonnées");
						}
				}	}

				if(!linked)
				{
					stream << t.col();

					Importable::DataSourceLinks links;
					links.insert(make_pair(&source, object.operatorCode));
					addRequest.getAction()->setDataSourceLinks(links);
					addRequest.getAction()->setName(object.name);
					if(object.coords)
					{
						addRequest.getAction()->setPoint(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*object.coords));
					}
					if(object.stopArea)
					{
						addRequest.getAction()->setPlace(env.getEditableSPtr(const_cast<StopArea*>(object.stopArea)));
						stream << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
					}
					else
					{
						addRequest.getAction()->setCreateCityIfNecessary(true);
						HTMLForm f(addRequest.getHTMLForm("create"+lexical_cast<string>(object.operatorCode)));
						stream << f.open();
						stream << "ID zone arrêt : " << f.getTextInput(StopPointAddAction::PARAMETER_PLACE_ID, string());
						stream << " ou création : commune zone d'arrêt : " << f.getTextInput(StopPointAddAction::PARAMETER_CITY_NAME, string());
						stream << " " << f.getSubmitButton("Créer");
						stream << f.close();
						addRequest.getAction()->setCreateCityIfNecessary(false);
					}
				}
			}
			stream << t.close();

			importLogger.logRaw(stream.str());
		}



		Destination* PTFileFormat::CreateOrUpdateDestination(
			impex::ImportableTableSync::ObjectBySource<DestinationTableSync>& destinations,
			const std::string& id,
			const std::string& displayText,
			const std::string& ttsText,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger
		){
			Destination* destination;
			if(destinations.contains(id))
			{
				set<Destination*> loadedDestination(destinations.get(id));
				if(loadedDestination.size() > 1)
				{
					importLogger.log(
						ImportLogger::WARN,
						"More than one destination with key "+ id
					);
				}
				destination = *loadedDestination.begin();
				importLogger.log(
					ImportLogger::LOAD,
					"Use of existing destination "+ lexical_cast<string>(destination->getKey()) +" ("+ destination->getDisplayedText() +")"
				);
			}
			else
			{
				destination = new Destination(
					DestinationTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				destination->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<Destination>().add(boost::shared_ptr<Destination>(destination));
				destinations.add(*destination);
				importLogger.log(
					ImportLogger::CREA,
					"Creation of the destination with key "+ id +" ("+ displayText +")"
				);
			}
			destination->setDisplayedText(displayText);
			destination->setTTSText(ttsText);
			return destination;
		}



		CommercialLine* PTFileFormat::GetLine(
			impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
			const std::string& id,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& importLogger,
			optional<TransportNetwork&> network
		){
			CommercialLine* line(NULL);
			if(lines.contains(id))
			{
				set<CommercialLine*> loadedLines(lines.get(id));
				// Network
				if(network)
				{
					BOOST_FOREACH(CommercialLine* loadedLine, loadedLines)
					{
						if(loadedLine->getNetwork() == &(*network))
						{
							line = loadedLine;
							break;
						}
					}
				}
				else
				{
					if(loadedLines.size() > 1)
					{
						importLogger.log(
							ImportLogger::WARN,
							"More than one line with key "+ id
						);
					}
					line = *loadedLines.begin();
				}

				if(line)
				{
					if(line->getPaths().empty())
					{
						JourneyPatternTableSync::Search(env, line->getKey());
						ScheduledServiceTableSync::Search(env, optional<RegistryKeyType>(), line->getKey());
						ContinuousServiceTableSync::Search(env, optional<RegistryKeyType>(), line->getKey());
						BOOST_FOREACH(const Path* route, line->getPaths())
						{
							LineStopTableSync::Search(env, route->getKey());
						}
					}

					importLogger.log(
						ImportLogger::LOAD,
						"Use of existing commercial line "+ lexical_cast<string>(line->getKey()) +" ("+ line->getName() +")"
					);
				}
			}
			return line;
		}



		std::set<JourneyPattern*> PTFileFormat::GetRoutes(
			pt::CommercialLine& line,
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
			const impex::DataSource& source,
			const impex::ImportLogger& importLogger
		){
			// Attempting to find an existing route by value comparison
			set<JourneyPattern*> result;
			BOOST_FOREACH(Path* route, line.getPaths())
			{
				// Avoid junctions
				if(!dynamic_cast<JourneyPattern*>(route))
				{
					continue;
				}

				JourneyPattern* jp(static_cast<JourneyPattern*>(route));

				if(!jp->hasLinkWithSource(source))
				{
					continue;
				}

				if(	*jp == servedStops
				){
					result.insert(jp);
					importLogger.log(
						ImportLogger::LOAD,
						"Use of existing route "+ lexical_cast<string>(jp->getKey()) +" ("+ jp->getName() +")"
					);
				}
			}
			return result;
		}



		RollingStock* PTFileFormat::GetTransportMode(
			const impex::ImportableTableSync::ObjectBySource<RollingStockTableSync>& transportModes,
			const std::string& id,
			const impex::ImportLogger& importLogger
		){
			RollingStock* transportMode(NULL);
			if(transportModes.contains(id))
			{
				set<RollingStock*> loadedTransportModes(transportModes.get(id));
				if(loadedTransportModes.size() > 1)
				{
					importLogger.log(
						ImportLogger::WARN,
						"more than one transport mode with key "+ id
					);
				}
				transportMode = *loadedTransportModes.begin();
			}
			return transportMode;
		}
}	}
