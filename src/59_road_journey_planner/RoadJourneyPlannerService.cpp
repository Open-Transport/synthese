
/** RoadJourneyPlannerService class implementation.
	@file RoadJourneyPlannerService.cpp

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
*/

#include "RoadJourneyPlannerService.hpp"

#include "AlgorithmLogger.hpp"
#include "CoordinatesSystem.hpp"
#include "Edge.h"
#include "GraphTypes.h"
#include "MainRoadChunk.hpp"
#include "NamedPlace.h"
#include "Place.h"
#include "PlacesListService.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ReverseRoadChunk.hpp"
#include "Road.h"
#include "RoadChunk.h"
#include "RoadJourneyPlanner.h"
#include "RoadJourneyPlannerResult.h"
#include "RoadJourneyPlannerModule.hpp"
#include "RoadPlace.h"
#include "Service.h"
#include "ServicePointer.h"
#include "Vertex.h"
#include "Webpage.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiLineString.h>
#include <geos/io/WKTWriter.h>

using namespace std;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace algorithm;
	using namespace cms;
	using namespace geography;
	using namespace graph;
	using namespace pt_website;
	using namespace road;
	using namespace server;
	using namespace road_journey_planner;
	using namespace util;

	template<> const string FactorableTemplate<Function, RoadJourneyPlannerService>::FACTORY_KEY("RoadJourneyPlannerService");

	namespace road_journey_planner
	{
		const std::string RoadJourneyPlannerService::PARAMETER_DEPARTURE_PLACE_TEXT("dpt");
		const std::string RoadJourneyPlannerService::PARAMETER_ARRIVAL_PLACE_TEXT("apt");
		const std::string RoadJourneyPlannerService::PARAMETER_CAR_TRIP("ct");
		const std::string RoadJourneyPlannerService::PARAMETER_SRID("srid");

		const std::string RoadJourneyPlannerService::PARAMETER_PAGE("page");
		const std::string RoadJourneyPlannerService::PARAMETER_BOARD_PAGE("board_page");
		const std::string RoadJourneyPlannerService::PARAMETER_STEP_PAGE("step_page");
		const std::string RoadJourneyPlannerService::PARAMETER_ERROR_PAGE("error_page");

		const std::string RoadJourneyPlannerService::DATA_WKT("wkt");

		const std::string RoadJourneyPlannerService::DATA_BOARD("board");
		const std::string RoadJourneyPlannerService::DATA_DEPARTURE_NAME("departure_name");
		const std::string RoadJourneyPlannerService::DATA_ARRIVAL_NAME("arrival_name");
		const std::string RoadJourneyPlannerService::DATA_DEPARTURE_TIME("departure_time");
		const std::string RoadJourneyPlannerService::DATA_ARRIVAL_TIME("arrival_time");
		const std::string RoadJourneyPlannerService::DATA_DURATION("duration");
		const std::string RoadJourneyPlannerService::DATA_DISTANCE("distance");

		const std::string RoadJourneyPlannerService::DATA_STEPS("steps");
		const std::string RoadJourneyPlannerService::DATA_RANK("rank");
		const std::string RoadJourneyPlannerService::DATA_ROAD_NAME("road_name");
		const std::string RoadJourneyPlannerService::DATA_STEP_DISTANCE("distance");
		const std::string RoadJourneyPlannerService::DATA_TOTAL_DISTANCE("total_distance");
		const std::string RoadJourneyPlannerService::DATA_DEPARTURE_STEP_TIME("departure_time");
		const std::string RoadJourneyPlannerService::DATA_ARRIVAL_STEP_TIME("arrival_time");
		const std::string RoadJourneyPlannerService::DATA_STEP_DURATION("duration");

		const std::string RoadJourneyPlannerService::DATA_ERROR_MESSAGE("error_message");

		const std::string RoadJourneyPlannerService::DATA_BOARD_MAP("boardMap");
		const std::string RoadJourneyPlannerService::DATA_STEP_MAP("stepMap");



		RoadJourneyPlannerService::RoadJourneyPlannerService()
		{
		}



		ParametersMap RoadJourneyPlannerService::_getParametersMap() const
		{
			ParametersMap map;

			// Departure place
			if(_departure_place.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_departure_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PLACE_TEXT,
						dynamic_cast<NamedPlace*>(_departure_place.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_departure_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PLACE_TEXT,
						dynamic_cast<City*>(_departure_place.placeResult.value.get())->getName()
					);
				}
			}
			else
			{
				map.insert(PARAMETER_DEPARTURE_PLACE_TEXT, _originPlaceText);
				map.insert(PARAMETER_ARRIVAL_PLACE_TEXT, _destinationPlaceText);
			}

			map.insert(PARAMETER_CAR_TRIP, _carTrip ? 1 : 0);

			return map;
		}



		void RoadJourneyPlannerService::_setFromParametersMap(const ParametersMap& map)
		{
			// Set coordinate system if provided else 4326 (WGS84)
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, 4326)
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			if(map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT) && map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT))
			{
				PlacesListService placesListService;
				placesListService.setNumber(1);
				placesListService.setMinScore(0.6);
				placesListService.setCitiesWithAtLeastAStop(false);
				placesListService.setCoordinatesSystem(_coordinatesSystem);

				// Departure
				placesListService.setText(map.get<string>(PARAMETER_DEPARTURE_PLACE_TEXT));
				_departure_place.placeResult = placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				);

				// Arrival
				placesListService.setText(map.get<string>(PARAMETER_ARRIVAL_PLACE_TEXT));
				_arrival_place.placeResult = placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				);
			}

			_carTrip = map.getDefault<bool>(PARAMETER_CAR_TRIP, false);
			if(_carTrip)
			{
				_accessParameters = AccessParameters(USER_CAR, false, false, 300000, boost::posix_time::hours(5), 1000, 1000);
			}
			else
			{
				_accessParameters = AccessParameters(USER_PEDESTRIAN, false, false, 300000, boost::posix_time::hours(5), 1.111, 1000);
			}

			try
			{
				boost::optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE));
				if(id)
				{
					_page = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such main page : " + e.getMessage());
			}

			try
			{
				boost::optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_BOARD_PAGE));
				if(id)
				{
					_boardPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such board page : " + e.getMessage());
			}

			try
			{
				boost::optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_STEP_PAGE));
				if(id)
				{
					_stepPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map page : " + e.getMessage());
			}

			try
			{
				boost::optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_ERROR_PAGE));
				if(id)
				{
					_errorPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such error page : " + e.getMessage());
			}
		}



		util::ParametersMap RoadJourneyPlannerService::run(
			ostream& stream,
			const Request& request
		) const {
			ParametersMap result;
			const GeometryFactory& gf(CoordinatesSystem::GetDefaultGeometryFactory());

			// Checks if there is something to plan
			if(!_departure_place.placeResult.value || !_arrival_place.placeResult.value)
			{
				result.merge(getTemplateParameters());
				result.insert(DATA_ERROR_MESSAGE, string("Departure or arrival place not found"));
				_errorPage->display(stream, request, result);
				return ParametersMap();
			}

			boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
			boost::posix_time::ptime endDate(now);
			endDate += boost::posix_time::hours(24);

			AlgorithmLogger logger;
			Place* departure = _departure_place.placeResult.value.get();
			Place* arrival = _arrival_place.placeResult.value.get();

			RoadJourneyPlanner r(
				departure,
				arrival,
				now,
				endDate,
				now,
				endDate,
				1,
				_accessParameters,
				DEPARTURE_FIRST,
				logger
			);
			RoadJourneyPlannerResult resultJourney(r.run());

			if(resultJourney.getJourneys().size())
			{
				Journey journey = *resultJourney.getJourneys().begin();
				boost::shared_ptr<ParametersMap> board(new ParametersMap);
				size_t rank(0);
				int total_distance(0);

				if(dynamic_cast<const NamedPlace*>(departure))
				{
					board->insert(DATA_DEPARTURE_NAME, dynamic_cast<const NamedPlace*>(departure)->getFullName());
				}
				else
				{
					board->insert(DATA_DEPARTURE_NAME, dynamic_cast<const City*>(departure)->getName());
				}

				if(dynamic_cast<const NamedPlace*>(arrival))
				{
					board->insert(DATA_ARRIVAL_NAME, dynamic_cast<const NamedPlace*>(arrival)->getFullName());
				}
				else
				{
					board->insert(DATA_ARRIVAL_NAME, dynamic_cast<const City*>(arrival)->getName());
				}

				board->insert(DATA_DEPARTURE_TIME, journey.getServiceUses().begin()->getDepartureDateTime());
				board->insert(DATA_ARRIVAL_TIME, journey.getServiceUses().rbegin()->getArrivalDateTime());
				board->insert(DATA_DURATION, journey.getServiceUses().rbegin()->getArrivalDateTime() - journey.getServiceUses().begin()->getDepartureDateTime());

				int curDistance(0);
				RoadPlace* curRoadPlace;
				boost::posix_time::ptime departureTime;
				boost::posix_time::ptime arrivalTime;
				vector<Geometry*> geometries;
				vector<Geometry*> curGeom;


				BOOST_FOREACH(Journey::ServiceUses::value_type& service, journey.getServiceUses())
				{
					const Road* road;
					if(dynamic_cast<const Road*>(service.getService()->getPath()))
					{
						road = static_cast<const Road*>(service.getService()->getPath());
					}
					else
					{
						continue;
					}

					geos::geom::CoordinateSequence *coords(gf.getCoordinateSequenceFactory()->create(0,2));
					const graph::Edge *e = service.getDepartureEdge();
					while(true)
					{
						geos::geom::Coordinate c;
						const Road *curRoad = dynamic_cast<const Road*>(e->getParentPath());
						if(!curRoad->isReversed()) {
							c.x = e->getFromVertex()->getGeometry()->getX();
							c.y = e->getFromVertex()->getGeometry()->getY();
							coords->add(c,0);
						}
						bool addViaPoints = true;
						if(e == service.getArrivalEdge() && !curRoad->isReversed())
							addViaPoints = false;
						if(e == service.getDepartureEdge() && curRoad->isReversed())
							addViaPoints = false;
						if(addViaPoints)
						{
							LineString* geometry;
							const RoadChunk* r(static_cast<const RoadChunk*>(e));

							if(r->isReversed())
								geometry = static_cast<LineString*>(static_cast<const ReverseRoadChunk*>(r)->getMainRoadChunk()->getGeometry()->reverse());
							else
								geometry = e->getGeometry().get();

							if(geometry)
							{
								for(size_t i(0); i<geometry->getCoordinatesRO()->getSize(); ++i)
								{
									coords->add(geometry->getCoordinatesRO()->getAt(i), false);
								}
							}
						}
						if(curRoad->isReversed())
						{
							c.x = e->getFromVertex()->getGeometry()->getX();
							c.y = e->getFromVertex()->getGeometry()->getY();
							coords->add(c, false);
						}
						if(e == service.getArrivalEdge())
							break;

						e = e->getFollowingArrivalForFineSteppingOnly();
					}
					LineString* serviceLineString = gf.createLineString(coords);

					if(!curDistance)
					{
						curDistance = static_cast<int>(service.getDistance());
						departureTime = service.getDepartureDateTime();
						arrivalTime = service.getArrivalDateTime();
						curRoadPlace = road->getRoadPlace();
						curGeom.push_back(serviceLineString->clone());
						rank++;
					}
					else if(curRoadPlace->getName() == road->getRoadPlace()->getName())
					{
						curDistance += static_cast<int>(service.getDistance());
						arrivalTime = service.getArrivalDateTime();
						curGeom.push_back(serviceLineString->clone());
					}
					else
					{
						boost::shared_ptr<ParametersMap> step(new ParametersMap);
						step->insert(DATA_RANK, rank);
						step->insert(DATA_ROAD_NAME, curRoadPlace->getName());
						step->insert(DATA_STEP_DISTANCE, curDistance);
						step->insert(DATA_TOTAL_DISTANCE, total_distance);
						step->insert(DATA_DEPARTURE_STEP_TIME, departureTime);
						step->insert(DATA_ARRIVAL_STEP_TIME, arrivalTime);
						step->insert(DATA_STEP_DURATION, arrivalTime - departureTime);

						MultiLineString* multiLineString = _coordinatesSystem->getGeometryFactory().createMultiLineString(curGeom);
						geometries.push_back(multiLineString->clone());

						board->insert(DATA_STEP_MAP, step);

						curDistance = static_cast<int>(service.getDistance());
						departureTime = service.getDepartureDateTime();
						arrivalTime = service.getArrivalDateTime();
						curRoadPlace = road->getRoadPlace();
						curGeom.clear();
						curGeom.push_back(serviceLineString->clone());
						rank++;
					}

					total_distance += service.getDistance();
				}

				MultiLineString* multiLineString = _coordinatesSystem->getGeometryFactory().createMultiLineString(curGeom);
				geometries.push_back(multiLineString->clone());

				boost::shared_ptr<ParametersMap> step(new ParametersMap);
				step->insert(DATA_RANK, rank);
				step->insert(DATA_ROAD_NAME, curRoadPlace->getName());
				step->insert(DATA_STEP_DISTANCE, curDistance);
				step->insert(DATA_TOTAL_DISTANCE, total_distance);
				step->insert(DATA_DEPARTURE_STEP_TIME, departureTime);
				step->insert(DATA_ARRIVAL_STEP_TIME, arrivalTime);

				board->insert(DATA_STEP_MAP, step);
				board->insert(DATA_DISTANCE, total_distance);

				result.insert(DATA_BOARD_MAP, board);

				GeometryCollection* geometryCollection(_coordinatesSystem->getGeometryFactory().createGeometryCollection(geometries));
				result.insert(DATA_WKT, geometryCollection->toText());
			}
			else
			{
				if(dynamic_cast<const NamedPlace*>(departure))
				{
					result.insert(DATA_DEPARTURE_NAME, dynamic_cast<const NamedPlace*>(departure)->getFullName());
				}
				else
				{
					result.insert(DATA_DEPARTURE_NAME, dynamic_cast<const City*>(departure)->getName());
				}

				if(dynamic_cast<const NamedPlace*>(arrival))
				{
					result.insert(DATA_ARRIVAL_NAME, dynamic_cast<const NamedPlace*>(arrival)->getFullName());
				}
				else
				{
					result.insert(DATA_ARRIVAL_NAME, dynamic_cast<const City*>(arrival)->getName());
				}

				result.merge(getTemplateParameters());
				result.insert(DATA_ERROR_MESSAGE, string("No results"));
				_errorPage->display(stream, request, result);
				return ParametersMap();
			}

			if(_page.get())
			{
				if(_boardPage.get() && result.hasSubMaps(DATA_BOARD_MAP))
				{
					stringstream boardStream;
					const boost::shared_ptr<ParametersMap> boardMap = *result.getSubMaps(DATA_BOARD_MAP).begin();

					if(_stepPage.get() && boardMap->hasSubMaps(DATA_STEP_MAP))
					{
						stringstream stepStream;
						BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& step, boardMap->getSubMaps(DATA_STEP_MAP))
						{
							step->merge(getTemplateParameters());
							_stepPage->display(stepStream, request, *step);
						}
						boardMap->merge(getTemplateParameters());
						boardMap->insert(DATA_STEPS, stepStream.str());
					}
					_boardPage->display(boardStream, request, *boardMap);
					result.insert(DATA_BOARD, boardStream.str());
				}

				result.merge(getTemplateParameters());
				_page->display(stream, request, result);
			}

			return ParametersMap();
		}



		bool RoadJourneyPlannerService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string RoadJourneyPlannerService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/plain";
		}
	}
}
