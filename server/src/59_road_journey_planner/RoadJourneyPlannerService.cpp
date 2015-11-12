
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

#include "AStarShortestPathCalculator.hpp"
#include "CoordinatesSystem.hpp"
#include "Crossing.h"
#include "GraphTypes.h"
#include "NamedPlace.h"
#include "Place.h"
#include "PlacesListService.hpp"
#include "RequestException.h"
#include "Request.h"
#include "Road.h"
#include "RoadChunk.h"
#include "RoadChunkEdge.hpp"
#include "VertexAccessMap.h"
#include "Webpage.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <geos/algorithm/CGAlgorithms.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiLineString.h>
#include <geos/io/WKTWriter.h>

using namespace std;
using namespace geos::algorithm;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
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
		const std::string RoadJourneyPlannerService::PARAMETER_ACCESSIBILITY("ac");
		const std::string RoadJourneyPlannerService::PARAMETER_SRID("srid");
		const std::string RoadJourneyPlannerService::PARAMETER_DEPARTURE_PLACE_XY("departure_place_XY");
		const std::string RoadJourneyPlannerService::PARAMETER_ARRIVAL_PLACE_XY("arrival_place_XY");
		const std::string RoadJourneyPlannerService::PARAMETER_INVERT_XY("invert_XY");

		const std::string RoadJourneyPlannerService::PARAMETER_PAGE("page");
		const std::string RoadJourneyPlannerService::PARAMETER_BOARD_PAGE("board_page");
		const std::string RoadJourneyPlannerService::PARAMETER_STEP_PAGE("step_page");
		const std::string RoadJourneyPlannerService::PARAMETER_ERROR_PAGE("error_page");

		const std::string RoadJourneyPlannerService::DATA_WKT("wkt");

		const std::string RoadJourneyPlannerService::DATA_BOARD("board");
		const std::string RoadJourneyPlannerService::DATA_DEPARTURE_NAME("departure_name");
		const std::string RoadJourneyPlannerService::DATA_ARRIVAL_NAME("arrival_name");
		const std::string RoadJourneyPlannerService::DATA_ORIGIN_CITY_TEXT("origin_city_text");
		const std::string RoadJourneyPlannerService::DATA_ORIGIN_PLACE_TEXT("origin_place_text");
		const std::string RoadJourneyPlannerService::DATA_DESTINATION_CITY_TEXT("destination_city_text");
		const std::string RoadJourneyPlannerService::DATA_DESTINATION_PLACE_TEXT("destination_place_text");
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

			return map;
		}



		void RoadJourneyPlannerService::_setFromParametersMap(const ParametersMap& map)
		{
			// Set coordinate system if provided else 4326 (WGS84)
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, 4326)
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Accessibility
			UserClassCode userClassCode(map.getDefault<UserClassCode>(PARAMETER_ACCESSIBILITY, USER_PEDESTRIAN));
			if(userClassCode == USER_CAR)
			{
				_accessParameters = AccessParameters(userClassCode, false, false, 1200000, boost::posix_time::hours(24), 13.889);
			}
			else if(userClassCode == USER_BIKE)
			{
				_accessParameters = AccessParameters(userClassCode, false, false, 360000, boost::posix_time::hours(24), 4.167);
			}
			else
			{
				_accessParameters = AccessParameters(userClassCode, false, false, 72000, boost::posix_time::hours(24), 1.111);
			}

			string originPlaceText = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_TEXT);
			string destinationPlaceText = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_TEXT);
			string originPlaceXY = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_XY);
			string destinationPlaceXY = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_XY);
			bool invertXY = map.getDefault<bool>(PARAMETER_INVERT_XY);

			if(!originPlaceText.empty() || !destinationPlaceText.empty())
			{
				PlacesListService placesListService;
				placesListService.setNumber(1);
				placesListService.setMinScore(0.6);
				placesListService.setCitiesWithAtLeastAStop(false);
				placesListService.setCoordinatesSystem(_coordinatesSystem);

				if(!originPlaceText.empty())
				{
					placesListService.setText(originPlaceText);
					_departure_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}

				if(!destinationPlaceText.empty())
				{
					placesListService.setText(destinationPlaceText);
					_arrival_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}
			}

			if(!originPlaceXY.empty() || !destinationPlaceXY.empty())
			{
				if(!originPlaceXY.empty())
				{
					PlacesListService placesListServiceOrigin;
					placesListServiceOrigin.setNumber(1);
					placesListServiceOrigin.setCoordinatesSystem(_coordinatesSystem);
					placesListServiceOrigin.addRequiredUserClass(_accessParameters.getUserClass());
					placesListServiceOrigin.setCoordinatesXY(originPlaceXY, invertXY);
					_departure_place.placeResult = placesListServiceOrigin.getPlaceFromBestResult(placesListServiceOrigin.runWithoutOutput());
				}

				if(!destinationPlaceXY.empty())
				{
					PlacesListService placesListServiceDestination;
					placesListServiceDestination.setNumber(1);
					placesListServiceDestination.setCoordinatesSystem(_coordinatesSystem);
					placesListServiceDestination.addRequiredUserClass(_accessParameters.getUserClass());
					placesListServiceDestination.setCoordinatesXY(destinationPlaceXY, invertXY);
					_arrival_place.placeResult = placesListServiceDestination.getPlaceFromBestResult(placesListServiceDestination.runWithoutOutput());
				}
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

			if(!_departure_place.placeResult.value || !_arrival_place.placeResult.value)
			{
				result.insert(DATA_ERROR_MESSAGE, string("Departure or arrival place not found"));

				if(_errorPage.get())
				{
					result.merge(getTemplateParameters());
					_errorPage->display(stream, request, result);
					return ParametersMap();
				}
				else
					return result;
			}

			boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());

			Place* departure = _departure_place.placeResult.value.get();
			Place* arrival = _arrival_place.placeResult.value.get();

			if(departure->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID, 0).intersercts(arrival->getVertexAccessMap(_accessParameters, RoadModule::GRAPH_ID, 0)))
			{
				result.insert(DATA_ERROR_MESSAGE, string("Same place"));

				if(_errorPage.get())
				{
					result.merge(getTemplateParameters());
					_errorPage->display(stream, request, result);
					return ParametersMap();
				}
				else
					return result;
			}

			algorithm::AStarShortestPathCalculator r(
				departure,
				arrival,
				now,
				_accessParameters
			);

			algorithm::AStarShortestPathCalculator::ResultPath path(r.run());

			if(!path.empty())
			{
				boost::shared_ptr<ParametersMap> board(new ParametersMap);

				if(dynamic_cast<const NamedPlace*>(departure))
				{
					const NamedPlace* place = dynamic_cast<const NamedPlace*>(departure);
					board->insert(DATA_ORIGIN_CITY_TEXT, place->getCity()->getName());
					board->insert(DATA_ORIGIN_PLACE_TEXT, place->getName());
					board->insert(DATA_DEPARTURE_NAME, place->getFullName());
				}
				else
				{
					const City* place = dynamic_cast<const City*>(departure);
					board->insert(DATA_ORIGIN_CITY_TEXT, place->getName());
					board->insert(DATA_ORIGIN_PLACE_TEXT, "");
					board->insert(DATA_DEPARTURE_NAME, place->getName());
				}

				if(dynamic_cast<const NamedPlace*>(arrival))
				{
					const NamedPlace* place = dynamic_cast<const NamedPlace*>(arrival);
					board->insert(DATA_DESTINATION_CITY_TEXT, place->getCity()->getName());
					board->insert(DATA_DESTINATION_PLACE_TEXT, place->getName());
					board->insert(DATA_ARRIVAL_NAME, place->getFullName());
				}
				else
				{
					const City* place = dynamic_cast<const City*>(arrival);
					board->insert(DATA_DESTINATION_CITY_TEXT, place->getName());
					board->insert(DATA_DESTINATION_PLACE_TEXT, "");
					board->insert(DATA_ARRIVAL_NAME, place->getName());
				}

				board->insert(DATA_DEPARTURE_TIME, now);

				int curDistance(0);
				size_t rank(0);
				int total_distance(0);
				bool first(true);
				RoadPlace* curRoadPlace(NULL);
				boost::posix_time::ptime departureTime(now);
				boost::posix_time::ptime arrivalTime;
				vector<Geometry*> geometries;
				vector<Geometry*> curGeom;

				BOOST_FOREACH(const RoadChunkEdge* chunk, path)
				{
					const Road* road(chunk->getRoadChunk()->getRoad());

					boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
					boost::shared_ptr<geos::geom::Geometry> geometryProjected(_coordinatesSystem->convertGeometry(*geometry));

					double distance(0);
					if(geometry)
					{
						CoordinateSequence* coordinates = geometry->getCoordinates();
						distance = CGAlgorithms::length(coordinates);
						delete coordinates;
					}

					double speed(_accessParameters.getApproachSpeed());
					if(_accessParameters.getUserClass() == USER_CAR && chunk->getCarSpeed() > 0)
						speed = chunk->getCarSpeed();

					if(first)
					{
						curDistance = static_cast<int>(distance);
						arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
						curRoadPlace = &*road->getAnyRoadPlace();
						curGeom.push_back(geometryProjected.get()->clone());
						rank++;
						first = false;
					}
					else if(curRoadPlace->getName() == road->getAnyRoadPlace()->getName())
					{
						curDistance += static_cast<int>(distance);
						arrivalTime = arrivalTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
						curGeom.push_back(geometryProjected.get()->clone());
					}
					else
					{
						MultiLineString* multiLineString = _coordinatesSystem->getGeometryFactory().createMultiLineString(curGeom);
						geometries.push_back(multiLineString->clone());

						boost::shared_ptr<ParametersMap> step(new ParametersMap);
						step->insert(DATA_RANK, rank);
						step->insert(DATA_ROAD_NAME, curRoadPlace->getName());
						step->insert(DATA_STEP_DISTANCE, curDistance);
						step->insert(DATA_TOTAL_DISTANCE, total_distance);
						step->insert(DATA_DEPARTURE_STEP_TIME, departureTime);
						step->insert(DATA_ARRIVAL_STEP_TIME, arrivalTime);
						step->insert(DATA_STEP_DURATION, arrivalTime - departureTime);
						step->insert(DATA_WKT, multiLineString->toText());

						delete multiLineString;

						board->insert(DATA_STEP_MAP, step);

						curDistance = static_cast<int>(distance);
						departureTime = arrivalTime;
						arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
						curRoadPlace = &*road->getAnyRoadPlace();

						BOOST_FOREACH(Geometry* geomToDelete, curGeom)
						{
							delete geomToDelete;
						}

						curGeom.clear();
						curGeom.push_back(geometryProjected.get()->clone());
						rank++;
					}

					total_distance += static_cast<int>(distance);
				}

				MultiLineString* multiLineString = _coordinatesSystem->getGeometryFactory().createMultiLineString(curGeom);
				geometries.push_back(multiLineString->clone());

				BOOST_FOREACH(Geometry* geomToDelete, curGeom)
				{
					delete geomToDelete;
				}
				curGeom.clear();

				boost::shared_ptr<ParametersMap> step(new ParametersMap);
				step->insert(DATA_RANK, rank);
				step->insert(DATA_ROAD_NAME, curRoadPlace->getName());
				step->insert(DATA_STEP_DISTANCE, curDistance);
				step->insert(DATA_TOTAL_DISTANCE, total_distance);
				step->insert(DATA_DEPARTURE_STEP_TIME, departureTime);
				step->insert(DATA_ARRIVAL_STEP_TIME, arrivalTime);
				step->insert(DATA_STEP_DURATION, arrivalTime - departureTime);
				step->insert(DATA_WKT, multiLineString->toText());

				delete multiLineString;

				board->insert(DATA_STEP_MAP, step);
				board->insert(DATA_DISTANCE, total_distance);

				board->insert(DATA_ARRIVAL_TIME, arrivalTime);
				board->insert(DATA_DURATION, arrivalTime - now);

				result.insert(DATA_BOARD_MAP, board);

				GeometryCollection* geometryCollection(_coordinatesSystem->getGeometryFactory().createGeometryCollection(geometries));
				BOOST_FOREACH(Geometry* geomToDelete, geometries)
				{
					delete geomToDelete;
				}
				geometries.clear();

				result.insert(DATA_WKT, geometryCollection->toText());
				delete geometryCollection;
			}
			else if(_errorPage.get())
			{
				if(dynamic_cast<const NamedPlace*>(departure))
				{
					const NamedPlace* place = dynamic_cast<const NamedPlace*>(departure);
					result.insert(DATA_ORIGIN_CITY_TEXT, place->getCity()->getName());
					result.insert(DATA_ORIGIN_PLACE_TEXT, place->getName());
					result.insert(DATA_DEPARTURE_NAME, place->getFullName());
				}
				else
				{
					const City* place = dynamic_cast<const City*>(departure);
					result.insert(DATA_ORIGIN_CITY_TEXT, place->getName());
					result.insert(DATA_ORIGIN_PLACE_TEXT, "");
					result.insert(DATA_DEPARTURE_NAME, place->getName());
				}

				if(dynamic_cast<const NamedPlace*>(arrival))
				{
					const NamedPlace* place = dynamic_cast<const NamedPlace*>(arrival);
					result.insert(DATA_DESTINATION_CITY_TEXT, place->getCity()->getName());
					result.insert(DATA_DESTINATION_PLACE_TEXT, place->getName());
					result.insert(DATA_ARRIVAL_NAME, place->getFullName());
				}
				else
				{
					const City* place = dynamic_cast<const City*>(arrival);
					result.insert(DATA_DESTINATION_CITY_TEXT, place->getName());
					result.insert(DATA_DESTINATION_PLACE_TEXT, "");
					result.insert(DATA_ARRIVAL_NAME, place->getName());
				}

				result.merge(getTemplateParameters());
				result.insert(DATA_ERROR_MESSAGE, string("No results"));
				_errorPage->display(stream, request, result);
				return ParametersMap();
			}
			else
			{
				result.insert(DATA_ERROR_MESSAGE, string("No results"));
				return result;
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

			return result;
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
