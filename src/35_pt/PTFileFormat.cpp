
/** PTFileFormat class implementation.
	@file PTFileFormat.cpp

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

#include "PTFileFormat.hpp"
#include "LineStop.h"
#include "JourneyPatternTableSync.hpp"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "LineStopTableSync.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
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
#include "StopPointMoveAction.hpp"

#include <geos/operation/distance/DistanceOp.h>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace impex;
	using namespace graph;
	using namespace admin;
	using namespace html;

	namespace pt
	{
		boost::shared_ptr<JourneyPattern> PTFileFormat::CreateJourneyPattern(
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
			CommercialLine& line,
			const impex::DataSource& source,
			Env& env,
			std::ostream& logStream
		){
			shared_ptr<JourneyPattern> route(new JourneyPattern);
			route->setCommercialLine(&line);
			route->setCodeBySource(source, string());
			route->setKey(JourneyPatternTableSync::getId());
			env.getEditableRegistry<JourneyPattern>().add(route);
			line.addPath(route.get());

			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& stop, stops)
			{
				shared_ptr<LineStop> ls(new LineStop);
				ls->setLine(route.get());
				ls->setPhysicalStop(*stop._stop.begin());
				ls->setRankInPath(rank);
				ls->setIsArrival(rank > 0 && stop._arrival);
				ls->setIsDeparture(rank+1 < stops.size() && stop._departure);
				ls->setMetricOffset(stop._metricOffset ? *stop._metricOffset : 0);
				ls->setKey(LineStopTableSync::getId());
				ls->setScheduleInput(stop._withTimes);
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
			std::ostream& logStream
		){
			TransportNetwork* network;
			if(networks.contains(id))
			{
				set<TransportNetwork*> loadedNetworks(networks.get(id));
				if(loadedNetworks.size() > 1)
				{
					logStream << "WARN : more than one network with key " << id << "<br />";
				}
				network = *loadedNetworks.begin();
				logStream << "LOAD : use of existing network " << network->getKey() << " (" << network->getName() << ")<br />";
			}
			else
			{
				network = new TransportNetwork(
					TransportNetworkTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				network->setDataSourceLinks(links);
				env.getEditableRegistry<TransportNetwork>().add(shared_ptr<TransportNetwork>(network));
				networks.add(*network);
				logStream << "CREA : Creation of the network with key " << id << " (" << name <<  ")<br />";
			}
			network->setName(name);
			return network;
		}



		std::set<StopArea*> PTFileFormat::GetStopAreas(
			const impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			boost::optional<const std::string&> name,
			std::ostream& logStream,
			bool errorIfNotFound /*= true */
		){
			if(stopAreas.contains(id))
			{
				set<StopArea*> loadedStopAreas(stopAreas.get(id));
				
				logStream
					<< "LOAD : link between stop areas " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << " and ";
				BOOST_FOREACH(StopArea* sp, loadedStopAreas)
				{
					logStream << sp->getKey() << " (" << sp->getFullName() << ") ";
				}
				logStream << "<br />";

				return loadedStopAreas;
			}

			if(errorIfNotFound)
			{
				logStream << "ERR  : stop area not found " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << "<br />";
			}
			return set<StopArea*>();
		}



		set<StopArea*> PTFileFormat::CreateOrUpdateStopAreas(
			impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			const std::string& name,
			const geography::City& city,
			boost::posix_time::time_duration defaultTransferDuration,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			// Load if possible
			set<StopArea*> result(GetStopAreas(stopAreas, id, name, logStream, false));

			// Create if necessary
			if(result.empty())
			{
				StopArea* stopArea(
					new StopArea(
						StopAreaTableSync::getId(),
						true,
						defaultTransferDuration
				)	);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				stopArea->setDataSourceLinks(links);
				env.getEditableRegistry<StopArea>().add(shared_ptr<StopArea>(stopArea));
				stopAreas.add(*stopArea);
				result.insert(stopArea);
			}

			// Update
			BOOST_FOREACH(StopArea* stopArea, result)
			{
				stopArea->setCity(&city);
				stopArea->setName(name);
			}

			return result;
		}



		std::set<StopPoint*> PTFileFormat::GetStopPoints(
			const impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
			const std::string& id,
			boost::optional<const std::string&> name,
			std::ostream& logStream,
			bool errorIfNotFound
		){
			if(stopPoints.contains(id))
			{
				set<StopPoint*> loadedStopPoints(stopPoints.get(id));

				logStream
					<< "LOAD : link between stops " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << " and ";
				BOOST_FOREACH(StopPoint* sp, loadedStopPoints)
				{
					logStream << sp->getKey() << " (" << sp->getConnectionPlace()->getFullName() << ") ";
				}
				logStream << "<br />";

				return loadedStopPoints;
			}
			if(errorIfNotFound)
			{
				logStream << "ERR  : stop not found " << id;
				if(name)
				{
					logStream << " (" << *name << ")";
				}
				logStream << "<br />";
			}
			return set<StopPoint*>();
		}



		set<StopPoint*> PTFileFormat::CreateOrUpdateStopPoints(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
			const std::string& id,
			const std::string& name,
			const StopArea& stopArea,
			const StopPoint::Geometry* geometry,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			// Load if possible
			set<StopPoint*> result(GetStopPoints(stopPoints, id, name, logStream, false));

			// Creation if necessary
			if(result.empty())
			{
				StopPoint* stopPoint(new StopPoint(StopPointTableSync::getId()));
				stopPoint->setHub(&stopArea);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				stopPoint->setDataSourceLinks(links);
				env.getEditableRegistry<StopPoint>().add(shared_ptr<StopPoint>(stopPoint));
				stopPoints.add(*stopPoint);
				result.insert(stopPoint);

				logStream << "CREA : Creation of the physical stop with key " << id << " (" << name <<  ")<br />";
			}

			// Update
			BOOST_FOREACH(StopPoint* stopPoint, result)
			{
				stopPoint->setName(name);
				if(geometry)
				{
					stopPoint->setGeometry(
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*geometry)
					);
				}
			}
			return result;
		}



		CommercialLine* PTFileFormat::CreateOrUpdateLine(
			impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
			const std::string& id,
			const std::string& name,
			const std::string& shortName,
			boost::optional<util::RGBColor> color,
			const TransportNetwork& network,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			CommercialLine* line;
			if(lines.contains(id))
			{
				set<CommercialLine*> loadedLines(lines.get(id));
				if(loadedLines.size() > 1)
				{
					logStream << "WARN : more than one line with key " << id << "<br />";
				}
				line = *loadedLines.begin();

				logStream << "LOAD : use of existing commercial line" << line->getKey() << " (" << line->getName() << ")<br />";
			}
			else
			{
				line = new CommercialLine(CommercialLineTableSync::getId());

				logStream << "CREA : Creation of the commercial line with key " << id << " (" << name <<  ")<br />";

				line->setNetwork(&network);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				line->setDataSourceLinks(links);
				env.getEditableRegistry<CommercialLine>().add(shared_ptr<CommercialLine>(line));
				lines.add(*line);
			}

			line->setName(name);
			line->setShortName(shortName);

			return line;
		}



		JourneyPattern* PTFileFormat::CreateOrUpdateRoute(
			pt::CommercialLine& line,
			boost::optional<const std::string&> id,
			boost::optional<const std::string&> name,
			boost::optional<const std::string&> destination,
			bool direction,
			pt::RollingStock* rollingStock,
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			// Attempting to find an existing route by value comparison
			JourneyPattern* result(NULL);
			BOOST_FOREACH(Path* route, line.getPaths())
			{
				JourneyPattern* jp(static_cast<JourneyPattern*>(route));

				if(!jp->hasLinkWithSource(source))
				{
					continue;
				}

				if(	(!rollingStock || jp->getRollingStock() == rollingStock) &&
					(!id || jp->getCodeBySource(source) == *id) &&
					*jp == servedStops
				){
					logStream << "LOAD : Use of route " << jp->getKey() << " (" << jp->getName() << ") for " << (id ? *id : string("unknown")) << ")<br />";
					result = jp;
					break;
				}
			}

			// Create a new route if necessary
			if(!result)
			{
				logStream << "CREA : Creation of route " << (name ? *name : string()) << " for " << (id ? *id : string("unknown")) << "<br />";
				result = new JourneyPattern(
					JourneyPatternTableSync::getId()
				);
				result->setCommercialLine(&line);
				Importable::DataSourceLinks links;
				if(id)
				{
					links.insert(make_pair(&source, *id));
				}
				else
				{
					links.insert(make_pair(&source, string()));
				}
				result->setDataSourceLinks(links);
				if(rollingStock)
				{
					result->setRollingStock(rollingStock);
				}
				result->setWayBack(direction);
				env.getEditableRegistry<JourneyPattern>().add(shared_ptr<JourneyPattern>(result));

				size_t rank(0);
				BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
				{
					shared_ptr<LineStop> ls(new LineStop);
					ls->setLine(result);
					ls->setPhysicalStop(*stop._stop.begin());
					ls->setRankInPath(rank);
					ls->setIsArrival(rank > 0 && stop._arrival);
					ls->setIsDeparture(rank+1 < servedStops.size() && stop._departure);
					ls->setMetricOffset(0);
					ls->setScheduleInput(stop._withTimes);
					ls->setKey(LineStopTableSync::getId());
					result->addEdge(*ls);
					env.getEditableRegistry<LineStop>().add(ls);
					++rank;
				}
			}

			// Update
			Path::Edges::const_iterator it(result->getEdges().begin());
			BOOST_FOREACH(const JourneyPattern::StopWithDepartureArrivalAuthorization stop, servedStops)
			{
				if(stop._metricOffset)
				{
					const_cast<Edge*>(*it)->setMetricOffset(*stop._metricOffset);
				}
				++it;
			}
			if(name)
			{
				result->setName(*name);
			}
			if(destination)
			{
				result->setDirection(*destination);
			}

			return result;
		}



		ScheduledService* PTFileFormat::CreateOrUpdateService(
			JourneyPattern& route,
			const ScheduledService::Schedules& departureSchedules,
			const ScheduledService::Schedules& arrivalSchedules,
			const std::string& number,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			// Search for a corresponding service
			ScheduledService* result(NULL);
			BOOST_FOREACH(Service* tservice, route.getServices())
			{
				ScheduledService* curService(dynamic_cast<ScheduledService*>(tservice));

				if(!curService) continue;

				if(	curService->getServiceNumber() == number &&
					curService->getDepartureSchedules(false) == departureSchedules &&
					curService->getArrivalSchedules(false) == arrivalSchedules
				){
					result = curService;
					break;
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
				result->setSchedules(departureSchedules, arrivalSchedules);
				result->setPathId(route.getKey());
				route.addService(result, false);
				env.getEditableRegistry<ScheduledService>().add(shared_ptr<ScheduledService>(result));
				
				logStream << "CREA : Creation of service " << result->getServiceNumber() << " for " << number << " (" << departureSchedules[0] << ") on route " << route.getKey() << " (" << route.getName() << ")<br />";
			}
			else
			{
				logStream << "LOAD : Use of service " << result->getKey() << " (" << result->getServiceNumber() << ") for " << number << " (" << departureSchedules[0] << ") on route " << route.getKey() << " (" << route.getName() << ")<br />";
			}
			return result;
		}



		void PTFileFormat::DisplayStopAreaImportScreen(
			const ImportableStopAreas& objects,
			const admin::AdminRequest& request,
			bool createCityIfNecessary,
			bool createPhysicalStop,
			boost::shared_ptr<const geography::City> defaultCity,
			util::Env& env,
			const impex::DataSource& source,
			std::ostream& stream
		){
			if(objects.empty())
			{
				return;
			}

			// Variables
			bool linked(!objects.begin()->linkedStopAreas.empty());
			AdminFunctionRequest<PTPlaceAdmin> openRequest(request);
			
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
		}



		void PTFileFormat::DisplayStopPointImportScreen(
			const ImportableStopPoints& objects,
			const admin::AdminRequest& request,
			util::Env& env,
			const impex::DataSource& source,
			std::ostream& stream
		){
			if(objects.empty())
			{
				return;
			}

			// Variables
			bool linked(!objects.begin()->linkedStopPoints.empty());
			AdminFunctionRequest<PTPlaceAdmin> openRequest(request);
			AdminFunctionRequest<StopPointAdmin> openStopPointRequest(request);
			
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

				shared_ptr<geos::geom::Point> projectedPoint;
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
							AdminActionFunctionRequest<StopPointMoveAction, DataSourceAdmin> moveRequest(request);
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

					if(object.stopArea)
					{
						AdminActionFunctionRequest<StopPointAddAction, DataSourceAdmin> addRequest(request);
						Importable::DataSourceLinks links;
						links.insert(make_pair(&source, object.operatorCode));
						addRequest.getAction()->setDataSourceLinks(links);
						if(object.coords)
						{
							addRequest.getAction()->setPoint(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*object.coords));
						}
						addRequest.getAction()->setPlace(env.getEditableSPtr(const_cast<StopArea*>(object.stopArea)));
						addRequest.getAction()->setName(object.name);
						stream << HTMLModule::getLinkButton(addRequest.getURL(), "Ajouter");
					}
					else
					{
						stream << "Lien auto impossible";
					}
				}
			}
			stream << t.close();
		}
}	}
