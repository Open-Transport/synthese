
/** MultimodalJourneyPlannerService class implementation.
	@file MultimodalJourneyPlannerService.cpp

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

#include "MultimodalJourneyPlannerService.hpp"

#include "AccessParameters.h"
#include "AlgorithmLogger.hpp"
#include "AStarShortestPathCalculator.hpp"
#include "CommercialLine.h"
#include "ContinuousService.h"
#include "Crossing.h"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "Junction.hpp"
#include "MultimodalJourneyPlannerResult.h"
#include "NamedPlace.h"
#include "ParametersMap.h"
#include "PlacesListService.hpp"
#include "PTModule.h"
#include "PTRoutePlannerResult.h"
#include "PTTimeSlotRoutePlanner.h"
#include "PublicBikeJourneyPlanner.hpp"
#include "Request.h"
#include "RequestException.h"
#include "RoadChunk.h"
#include "RoadChunkEdge.hpp"
#include "RoadJourneyPlanner.h"
#include "RoadJourneyPlannerResult.h"
#include "RoadPath.hpp"
#include "ScheduledService.h"
#include "Service.h"
#include "Session.h"
#include "TransportNetwork.h"
#include "VertexAccessMap.h"
#include "Log.h"

// OVE!!!
#include "PublicBikingModule.h"
#include "PublicBikeStation.hpp"
#include "PTModule.h"
#include "VAMConverter.hpp"
#include "TimeSlotRoutePlanner.h"
#include "RollingStock.hpp"
// OVE!!!

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <geos/algorithm/CGAlgorithms.h>
#include <geos/geom/MultiLineString.h>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace geography;
	using namespace util;

	template<> const string util::FactorableTemplate<multimodal_journey_planner::MultimodalJourneyPlannerService::_FunctionWithSite,multimodal_journey_planner::MultimodalJourneyPlannerService>::FACTORY_KEY("multimodal_journey_planner");

	namespace multimodal_journey_planner
	{
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_CITY_TEXT = "departure_city";
		const string MultimodalJourneyPlannerService::PARAMETER_ARRIVAL_CITY_TEXT = "arrival_city";
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_PLACE_TEXT = "departure_place";
		const string MultimodalJourneyPlannerService::PARAMETER_ARRIVAL_PLACE_TEXT = "arrival_place";
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_DAY = "departure_date";
		const string MultimodalJourneyPlannerService::PARAMETER_DEPARTURE_TIME = "departure_time";
		const string MultimodalJourneyPlannerService::PARAMETER_MAX_TRANSPORT_CONNECTION_COUNT = "max_transport_connection_count";
		const string MultimodalJourneyPlannerService::PARAMETER_USE_WALK = "use_walk";
		const string MultimodalJourneyPlannerService::PARAMETER_USE_PT = "use_pt";
		const string MultimodalJourneyPlannerService::PARAMETER_USE_PUBLICBIKE = "use_public_bike";
		const string MultimodalJourneyPlannerService::PARAMETER_USE_TAXI = "use_taxi";
		const string MultimodalJourneyPlannerService::PARAMETER_USE_CARPOOLING = "use_car_pooling";
		const string MultimodalJourneyPlannerService::PARAMETER_USE_CARLOCATION = "use_car_location";
		const string MultimodalJourneyPlannerService::PARAMETER_LOGGER_PATH = "logger_path";

		const string MultimodalJourneyPlannerService::PARAMETER_ASTAR_FOR_WALK = "astar_for_walk"; //TODO : remove when algorithm is chosen

		const string MultimodalJourneyPlannerService::DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT = "MultimodalJourneyPlannerResult";
		const string MultimodalJourneyPlannerService::DATA_ERROR_MESSAGE("error_message");


		MultimodalJourneyPlannerService::MultimodalJourneyPlannerService(
		):	_departureDay(boost::gregorian::day_clock::local_day()),
			_departureTime(not_a_date_time),
			_loggerPath()
		{}



		ParametersMap MultimodalJourneyPlannerService::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());

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
						PARAMETER_DEPARTURE_CITY_TEXT,
						dynamic_cast<City*>(_departure_place.placeResult.value.get())->getName()
					);
				}
			}

			// Arrival place
			if(_arrival_place.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_arrival_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PLACE_TEXT,
						dynamic_cast<NamedPlace*>(_arrival_place.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_arrival_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_CITY_TEXT,
						dynamic_cast<City*>(_arrival_place.placeResult.value.get())->getName()
					);
				}
			}

			// Departure day
			if(!_departureDay.is_not_a_date())
			{
				map.insert(PARAMETER_DEPARTURE_DAY, _departureDay);
			}

			// Departure time
			if(!_departureTime.is_not_a_date_time())
			{
				map.insert(PARAMETER_DEPARTURE_TIME, _departureTime);
			}

			// Max transport connection count
			if(_maxTransportConnectionCount)
			{
				map.insert(
					PARAMETER_MAX_TRANSPORT_CONNECTION_COUNT,
					_maxTransportConnectionCount.get()
				);
			}

			// Use walk
			map.insert(PARAMETER_USE_WALK, _useWalk);

			// Use pt
			map.insert(PARAMETER_USE_PT, _useWalk);

			// Use public_bike
			map.insert(PARAMETER_USE_PUBLICBIKE, _usePublicBike);

			// Use taxi
			map.insert(PARAMETER_USE_TAXI, _useTaxi);

			// Use car pooling
			map.insert(PARAMETER_USE_CARPOOLING, _useCarPooling);

			// Use car location
			map.insert(PARAMETER_USE_CARLOCATION, _useCarLocation);

			// Logger path
			if(!_loggerPath.empty())
			{
				map.insert(PARAMETER_LOGGER_PATH, _loggerPath.string());
			}

			return map;
		}



		void MultimodalJourneyPlannerService::_setFromParametersMap(const ParametersMap& map)
		{
			Log::GetInstance().debug("MultimodalJourneyPlannerService::_setFromParametersMap : start");

			_FunctionWithSite::_setFromParametersMap(map);

			// Departure place
			if( // Two fields input
				map.isDefined(PARAMETER_DEPARTURE_CITY_TEXT) &&
				map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT)
			){
				_originCityText = map.getDefault<string>(PARAMETER_DEPARTURE_CITY_TEXT);
				_originPlaceText = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_TEXT);
				if(!_originCityText.empty() && !_originPlaceText.empty())
				{
					_departure_place = road::RoadModule::ExtendedFetchPlace(_originCityText, _originPlaceText);
				}
				else if (!_originPlaceText.empty())
				{
					// One field input
					pt_website::PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

					placesListService.setText(_originPlaceText);
					_departure_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}
				else
				{
					throw server::RequestException("Empty departure place.");
				}
			}
			else if(map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT))
			{ // One field input
				pt_website::PlacesListService placesListService;
				placesListService.setNumber(1);
				placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

				placesListService.setText(map.get<string>(PARAMETER_DEPARTURE_PLACE_TEXT));
				_departure_place.placeResult = placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				);
			}
			else
			{
				throw server::RequestException("Empty departure place.");
			}

			Log::GetInstance().debug("MultimodalJourneyPlannerService::_setFromParametersMap : after matching of departure place");

			// Destination
			if( // Two fields input
				map.isDefined(PARAMETER_ARRIVAL_CITY_TEXT) &&
				map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT)
			){
				_destinationCityText = map.getDefault<string>(PARAMETER_ARRIVAL_CITY_TEXT);
				_destinationPlaceText = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_TEXT);
				if(!_destinationCityText.empty() && !_destinationPlaceText.empty())
				{
					_arrival_place = road::RoadModule::ExtendedFetchPlace(_destinationCityText, _destinationPlaceText);
				}
				else if (!_destinationPlaceText.empty())
				{
					// One field input
					pt_website::PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

					placesListService.setText(_destinationPlaceText);
					_arrival_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}
				else
				{
					throw server::RequestException("Empty arrival place.");
				}
			}
			else if(map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT))
			{ // One field input
				pt_website::PlacesListService placesListService;
				placesListService.setNumber(1);
				placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

				placesListService.setText(map.get<string>(PARAMETER_ARRIVAL_PLACE_TEXT));
				_arrival_place.placeResult = placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				);
			}
			else
			{
				throw server::RequestException("Empty arrival place.");
			}

			Log::GetInstance().debug("MultimodalJourneyPlannerService::_setFromParametersMap : after matching of arrival place");

			// Date parameters
			try
			{
				// by day and time
				if(!map.getDefault<string>(PARAMETER_DEPARTURE_DAY).empty())
				{
					// Day
					_departureDay = from_string(map.get<string>(PARAMETER_DEPARTURE_DAY));

					// Time
					if(!map.getDefault<string>(PARAMETER_DEPARTURE_TIME).empty())
					{
						_departureTime = duration_from_string(map.get<string>(PARAMETER_DEPARTURE_TIME));
					}
				}
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw server::RequestException(e.what());
			}

			// Maximum allowed transport connections
			_maxTransportConnectionCount= map.getOptional<size_t>(PARAMETER_MAX_TRANSPORT_CONNECTION_COUNT);

			// TEMP
			_aStarForWalk = map.getDefault<bool>(PARAMETER_ASTAR_FOR_WALK, false);
			
			_useWalk = map.getDefault<bool>(PARAMETER_USE_WALK, false);
			_usePt = map.getDefault<bool>(PARAMETER_USE_PT, false);
			_usePublicBike = map.getDefault<bool>(PARAMETER_USE_PUBLICBIKE, false);
			_useTaxi = map.getDefault<bool>(PARAMETER_USE_TAXI, false);
			_useCarPooling = map.getDefault<bool>(PARAMETER_USE_CARPOOLING, false);
			_useCarLocation = map.getDefault<bool>(PARAMETER_USE_CARLOCATION, false);

			std::string pathString(map.getDefault<string>(PARAMETER_LOGGER_PATH, ""));
			if (!pathString.empty())
			{
				_loggerPath = boost::filesystem::path(pathString);
			}
			Function::setOutputFormatFromMap(map,string());
		}



		util::ParametersMap MultimodalJourneyPlannerService::run(
			ostream& stream,
			const server::Request& request
		) const	{

			Log::GetInstance().debug("MultimodalJourneyPlannerService::run");

			//////////////////////////////////////////////////////////////////////////
			// Display
			ParametersMap pm;

			if(!_departure_place.placeResult.value || !_arrival_place.placeResult.value)
			{
				pm.insert(DATA_ERROR_MESSAGE, string("Departure or arrival place not found"));

				if(_outputFormat == MimeTypes::XML)
				{
					pm.outputXML(
						stream,
						DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT,
						true,
						"http://schemas.open-transport.org/smile/MultimodalJourneyPlanner.xsd"
					);
				}
				else if(_outputFormat == MimeTypes::JSON)
				{
					pm.outputJSON(stream, DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT);
				}

				return pm;
			}

			Place* departure = _departure_place.placeResult.value.get();
			Place* arrival = _arrival_place.placeResult.value.get();

			graph::AccessParameters pedestrianAccessParameters(graph::USER_PEDESTRIAN, false, false, 72000, boost::posix_time::hours(24), 1.111);

			ptime startDate;
			if (_departureTime.is_not_a_date_time())
			{
				startDate = ptime(boost::posix_time::second_clock::local_time());
			}
			else
			{
				startDate = ptime(_departureDay, _departureTime);
			}
			ptime endDate = startDate + hours(24);
			
			if (_useWalk && _aStarForWalk)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before A* walk");
				// Astar algorithm
				algorithm::AStarShortestPathCalculator r(
					departure,
					arrival,
					startDate,
					pedestrianAccessParameters
				);

				algorithm::AStarShortestPathCalculator::ResultPath path(r.run());

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* walk");
				
				if(!path.empty())
				{
					boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
					submapJourney->insert("departure_date_time", startDate);

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					if(dynamic_cast<const NamedPlace*>(departure))
					{
						submapDeparturePlace->insert("name", dynamic_cast<const NamedPlace*>(departure)->getFullName());
						submapDeparturePlace->insert("type", dynamic_cast<const NamedPlace*>(departure)->getFactoryKey());
						submapDeparturePlace->insert("id", dynamic_cast<const NamedPlace*>(departure)->getKey());
					}
					else
					{
						submapDeparturePlace->insert("name", dynamic_cast<const City*>(departure)->getName());
						string strCityType("City");
						submapDeparturePlace->insert("type", strCityType);
						submapDeparturePlace->insert("id", dynamic_cast<const City*>(departure)->getKey());
					}

					if(departure->getPoint().get() &&
						!departure->getPoint()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(departure->getPoint())
						)	);
						submapDeparturePlace->insert("longitude", wgs84Point->getX());
						submapDeparturePlace->insert("latitude", wgs84Point->getY());
					}

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					if(dynamic_cast<const NamedPlace*>(arrival))
					{
						submapArrivalPlace->insert("name", dynamic_cast<const NamedPlace*>(arrival)->getFullName());
						submapArrivalPlace->insert("type", dynamic_cast<const NamedPlace*>(arrival)->getFactoryKey());
						submapArrivalPlace->insert("id", dynamic_cast<const NamedPlace*>(arrival)->getKey());
					}
					else
					{
						submapArrivalPlace->insert("name", dynamic_cast<const City*>(arrival)->getName());
						string strCityType("City");
						submapArrivalPlace->insert("type", strCityType);
						submapArrivalPlace->insert("id", dynamic_cast<const City*>(arrival)->getKey());
					}

					if(arrival->getPoint().get() &&
						!arrival->getPoint()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(arrival->getPoint())
						)	);
						submapArrivalPlace->insert("longitude", wgs84Point->getX());
						submapArrivalPlace->insert("latitude", wgs84Point->getY());
					}

					submapJourney->insert("departure", submapDeparturePlace);
					submapJourney->insert("arrival", submapArrivalPlace);

					int curDistance(0);
					boost::posix_time::ptime arrivalTime;
					boost::posix_time::ptime departureTime(startDate);
					bool first(true);
					vector<geos::geom::Geometry*> geometries;
					vector<geos::geom::Geometry*> curGeom;
					double speed(pedestrianAccessParameters.getApproachSpeed());
					boost::shared_ptr<ParametersMap> submapLegDeparturePlace(new ParametersMap);
					boost::shared_ptr<ParametersMap> submapLegArrivalPlace(new ParametersMap);
					road::RoadPlace* curRoadPlace(NULL);
					const road::RoadChunkEdge* lastChunk;

					BOOST_FOREACH(const road::RoadChunkEdge* chunk, path)
					{
						const road::Road* road(chunk->getRoadChunk()->getRoad());
						lastChunk = chunk;
						boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
						boost::shared_ptr<geos::geom::Geometry> geometryProjected(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(*geometry));
						double distance(0);
						if(geometry)
						{
							geos::geom::CoordinateSequence* coordinates = geometry->getCoordinates();
							distance = geos::algorithm::CGAlgorithms::length(coordinates);
							delete coordinates;
						}

						if(first)
						{
							curDistance = static_cast<int>(distance);
							arrivalTime = startDate + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curRoadPlace = &*road->getAnyRoadPlace();
							curGeom.push_back(geometryProjected.get()->clone());
							first = false;

							// Departure place
							if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
							{
								submapLegDeparturePlace->insert("name", (string)("Croisement"));
								submapLegDeparturePlace->insert("type", (string)("crossing"));
								submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
							}

							if(chunk->getFromVertex()->getGeometry().get() &&
								!chunk->getFromVertex()->getGeometry()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(chunk->getFromVertex()->getGeometry())
								)	);
								submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
								submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
							}
						}
						else if(curRoadPlace->getName() == road->getAnyRoadPlace()->getName())
						{
							curDistance += static_cast<int>(distance);
							arrivalTime = arrivalTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curGeom.push_back(geometryProjected.get()->clone());
						}
						else
						{
							geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
							geometries.push_back(multiLineString->clone());

							boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
							submapLeg->insert("departure_date_time", departureTime);
							submapLeg->insert("departure", submapLegDeparturePlace);

							// Arrival place
							if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
							{
								submapLegArrivalPlace->insert("name", (string)("Croisement"));
								submapLegArrivalPlace->insert("type", (string)("crossing"));
								submapLegArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
							}

							if(chunk->getFromVertex()->getGeometry().get() &&
								!chunk->getFromVertex()->getGeometry()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(chunk->getFromVertex()->getGeometry())
								)	);
								submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
								submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
							}
							submapLeg->insert("arrival", submapLegArrivalPlace);

							submapLegDeparturePlace.reset(new ParametersMap);
							submapLegArrivalPlace.reset(new ParametersMap);

							submapLeg->insert("arrival_date_time", arrivalTime);
							submapLeg->insert("geometry", multiLineString->toString());
							boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
							submapWalkAttributes->insert("length", curDistance);

							boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
							submapLegRoad->insert("name", curRoadPlace->getName());
							submapLegRoad->insert("id", curRoadPlace->getKey());

							submapWalkAttributes->insert("road", submapLegRoad);

							submapLeg->insert("walk_attributes", submapWalkAttributes);

							submapJourney->insert("leg", submapLeg);

							delete multiLineString;
							curDistance = static_cast<int>(distance);
							departureTime = arrivalTime;
							arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curRoadPlace = &*road->getAnyRoadPlace();

							BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
							{
								delete geomToDelete;
							}

							curGeom.clear();
							curGeom.push_back(geometryProjected.get()->clone());

							// New departure place
							if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
							{
								submapLegDeparturePlace->insert("name", (string)("Croisement"));
								submapLegDeparturePlace->insert("type", (string)("crossing"));
								submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
							}

							if(chunk->getFromVertex()->getGeometry().get() &&
								!chunk->getFromVertex()->getGeometry()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(chunk->getFromVertex()->getGeometry())
								)	);
								submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
								submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
							}
						}
					}

					geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
					geometries.push_back(multiLineString->clone());

					BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
					{
						delete geomToDelete;
					}
					curGeom.clear();
					boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
					submapLeg->insert("departure_date_time", departureTime);
					submapLeg->insert("departure", submapLegDeparturePlace);

					// Arrival place
					if(dynamic_cast<const road::Crossing*>(lastChunk->getFromVertex()->getHub()))
					{
						submapLegArrivalPlace->insert("name", (string)("Croisement"));
						submapLegArrivalPlace->insert("type", (string)("crossing"));
						submapLegArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(lastChunk->getFromVertex()->getHub())->getKey());
					}

					if(lastChunk->getFromVertex()->getGeometry().get() &&
						!lastChunk->getFromVertex()->getGeometry()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(lastChunk->getFromVertex()->getGeometry())
						)	);
						submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
						submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
					}
					submapLeg->insert("arrival", submapLegArrivalPlace);

					submapLeg->insert("arrival_date_time", arrivalTime);
					submapLeg->insert("geometry", multiLineString->toString());
					boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
					submapWalkAttributes->insert("length", curDistance);

					boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
					submapLegRoad->insert("name", curRoadPlace->getName());
					submapLegRoad->insert("id", curRoadPlace->getKey());

					submapWalkAttributes->insert("road", submapLegRoad);

					submapLeg->insert("walk_attributes", submapWalkAttributes);

					submapJourney->insert("leg", submapLeg);

					delete multiLineString;

					submapJourney->insert("arrival_date_time", arrivalTime);
					geos::geom::GeometryCollection* geometryCollection(CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createGeometryCollection(geometries));
					BOOST_FOREACH(geos::geom::Geometry* geomToDelete, geometries)
					{
						delete geomToDelete;
					}
					geometries.clear();
					submapJourney->insert("geometry", geometryCollection->toString());
					delete geometryCollection;
					pm.insert("journey", submapJourney);
					
				}

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* walk data processing");
			}
			else if (_useWalk)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before SYNTHESE walk");

				// Classical synthese algorithm
				// TODO : Note we cannot really use AlgorithmLogger right now since not targetted for pure walk journey search 
				algorithm::AlgorithmLogger logger;
				road_journey_planner::RoadJourneyPlanner rjp(
					departure,
					arrival,
					startDate,
					startDate,
					endDate,
					endDate,
					1,
					pedestrianAccessParameters,
					algorithm::DEPARTURE_FIRST,
					logger,
					true
				);
				road_journey_planner::RoadJourneyPlannerResult results = rjp.run();

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE walk");
				
				if(!results.getJourneys().empty())
				{
					for (road_journey_planner::RoadJourneyPlannerResult::Journeys::const_iterator it(results.getJourneys().begin()); it != results.getJourneys().end(); ++it)
					{
						boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
						_serializeJourney(*it, departure, arrival, submapJourney);
						pm.insert("journey", submapJourney);
					}
				}

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE walk data processing");
			}

			// Public transport (with optional walk)
			if (_usePt)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before pt");

				// Initialization
				graph::AccessParameters approachAccessParameters(
					graph::USER_PEDESTRIAN,			// user class code
					false,							// DRT only
					false,							// without DRT
					_useWalk ? 1000 : 0,			// max approach distance
					boost::posix_time::minutes(23),	// max approach time
					1.111,							// approach speed
					_maxTransportConnectionCount	// max transport connection count (ie : max number of used transport services - 1)
				);

				boost::shared_ptr<algorithm::AlgorithmLogger> logger(new algorithm::AlgorithmLogger(_loggerPath));

				pt_journey_planner::PTTimeSlotRoutePlanner r(
					_departure_place.placeResult.value.get(),
					_arrival_place.placeResult.value.get(),
					startDate,
					endDate,
					startDate,
					endDate,
					3,
					approachAccessParameters,
					algorithm::DEPARTURE_FIRST,
					false,
					*logger
				);
				// Computing
				pt_journey_planner::PTRoutePlannerResult tcResult = r.run();

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after pt");

				if(!tcResult.getJourneys().empty())
				{
					for (pt_journey_planner::PTRoutePlannerResult::Journeys::const_iterator it(tcResult.getJourneys().begin()); it != tcResult.getJourneys().end(); ++it)
					{
						boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
						_serializeJourney(*it, departure, arrival, submapJourney);
						pm.insert("journey", submapJourney);
					}
				}

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after pt data processing");
			}

			// Public bike (with optional walk)
			if (_usePublicBike && _aStarForWalk)
			{
				// Initialization
				graph::AccessParameters approachAccessParameters(
					graph::USER_PEDESTRIAN,			// user class code
					false,							// DRT only
					false,							// without DRT
					_useWalk ? 1000 : 0,			// max approach distance
					boost::posix_time::minutes(23),	// max approach time
					_useWalk ? 1.111 : 0.0,			// approach speed
					_maxTransportConnectionCount	// max transport connection count (ie : max number of used transport services - 1)
				);

				// A* algorithm
				algorithm::AlgorithmLogger logger;
				graph::AccessParameters bikeAccessParameters(graph::USER_BIKE, false, false, 72000, boost::posix_time::hours(24), 4.167);
				public_biking::PublicBikeJourneyPlanner pbjp(
					departure,
					arrival,
					startDate,
					endDate,
					approachAccessParameters,
					bikeAccessParameters,
					logger
				);
				std::vector<public_biking::PublicBikeJourneyPlanner::PublicBikeJourneyPlannerAStarResult> result(pbjp.runAStar());

				BOOST_FOREACH(public_biking::PublicBikeJourneyPlanner::PublicBikeJourneyPlannerAStarResult subResult, result)
				{
					boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
					if(!subResult.get<0>().empty())
					{
						submapJourney->insert("departure_date_time", subResult.get<0>().getFirstDepartureTime());
					}
					else
					{
						submapJourney->insert("departure_date_time", startDate);
					}

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					if(subResult.get<0>().empty() ||
						dynamic_cast<const road::Crossing*>(subResult.get<0>().getOrigin()->getHub()))
					{
						if(dynamic_cast<const NamedPlace*>(departure))
						{
							submapDeparturePlace->insert("name", dynamic_cast<const NamedPlace*>(departure)->getFullName());
							submapDeparturePlace->insert("type", dynamic_cast<const NamedPlace*>(departure)->getFactoryKey());
							submapDeparturePlace->insert("id", dynamic_cast<const NamedPlace*>(departure)->getKey());
						}
						else
						{
							submapDeparturePlace->insert("name", dynamic_cast<const City*>(departure)->getName());
							string strCityType("City");
							submapDeparturePlace->insert("type", strCityType);
							submapDeparturePlace->insert("id", dynamic_cast<const City*>(departure)->getKey());
						}
					}
					else
					{
						submapDeparturePlace->insert("name", dynamic_cast<const NamedPlace*>(subResult.get<0>().getOrigin()->getHub())->getFullName());
						submapDeparturePlace->insert("type", dynamic_cast<const NamedPlace*>(subResult.get<0>().getOrigin()->getHub())->getFactoryKey());
						submapDeparturePlace->insert("id", dynamic_cast<const NamedPlace*>(subResult.get<0>().getOrigin()->getHub())->getKey());
					}

					if(!subResult.get<0>().empty() &&
						subResult.get<0>().getOrigin()->getFromVertex()->getGeometry().get() &&
						!subResult.get<0>().getOrigin()->getFromVertex()->getGeometry()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(subResult.get<0>().getOrigin()->getFromVertex()->getGeometry())
						)	);
						submapDeparturePlace->insert("longitude", wgs84Point->getX());
						submapDeparturePlace->insert("latitude", wgs84Point->getY());
					}

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					if(subResult.get<2>().empty() ||
						dynamic_cast<const road::Crossing*>(subResult.get<2>().getDestination()->getHub()))
					{
						if(dynamic_cast<const NamedPlace*>(arrival))
						{
							submapArrivalPlace->insert("name", dynamic_cast<const NamedPlace*>(arrival)->getFullName());
							submapArrivalPlace->insert("type", dynamic_cast<const NamedPlace*>(arrival)->getFactoryKey());
							submapArrivalPlace->insert("id", dynamic_cast<const NamedPlace*>(arrival)->getKey());
						}
						else
						{
							submapArrivalPlace->insert("name", dynamic_cast<const City*>(arrival)->getName());
							string strCityType("City");
							submapArrivalPlace->insert("type", strCityType);
							submapArrivalPlace->insert("id", dynamic_cast<const City*>(arrival)->getKey());
						}
					}
					else
					{
						submapArrivalPlace->insert("name", dynamic_cast<const NamedPlace*>(subResult.get<2>().getDestination()->getHub())->getFullName());
						submapArrivalPlace->insert("type", dynamic_cast<const NamedPlace*>(subResult.get<2>().getDestination()->getHub())->getFactoryKey());
						submapArrivalPlace->insert("id", dynamic_cast<const NamedPlace*>(subResult.get<2>().getDestination()->getHub())->getKey());
					}

					if(!subResult.get<2>().empty() &&
						subResult.get<2>().getDestination()->getFromVertex()->getGeometry().get() &&
						!subResult.get<2>().getDestination()->getFromVertex()->getGeometry()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(subResult.get<2>().getDestination()->getFromVertex()->getGeometry())
						)	);
						submapArrivalPlace->insert("longitude", wgs84Point->getX());
						submapArrivalPlace->insert("latitude", wgs84Point->getY());
					}

					// Display of approach at the departure
					graph::Journey::ServiceUses::const_iterator its(subResult.get<0>().getServiceUses().begin());
					vector<boost::shared_ptr<geos::geom::Geometry> > geometriesSPtr; // To keep shared_ptr's in scope !
					vector<geos::geom::Geometry*> allGeometries;
					while(true)
					{
						if(its == (subResult.get<0>().getServiceUses().end())) break;
						boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
						submapLeg->insert("departure_date_time", its->getDepartureDateTime());
						// Departure place
						boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
						if(dynamic_cast<const road::Crossing*>(its->getRealTimeDepartureVertex()->getHub()))
						{
							submapDeparturePlace->insert("name", (string)("Croisement"));
							submapDeparturePlace->insert("type", (string)("crossing"));
							submapDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(its->getRealTimeDepartureVertex()->getHub())->getKey());
						}

						if(its->getRealTimeDepartureVertex()->getGeometry().get() &&
							!its->getRealTimeDepartureVertex()->getGeometry()->isEmpty())
						{
							boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*(its->getRealTimeDepartureVertex()->getGeometry())
							)	);
							submapDeparturePlace->insert("longitude", wgs84Point->getX());
							submapDeparturePlace->insert("latitude", wgs84Point->getY());
						}

						submapLeg->insert("departure", submapDeparturePlace);

						const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());

						std::string roadName = road->getAnyRoadPlace()->getName();
						if(roadName.empty()) {
							if(	road->get<RoadTypeField>() == road::ROAD_TYPE_PEDESTRIANPATH ||
								road->get<RoadTypeField>() == road::ROAD_TYPE_PEDESTRIANSTREET
							){
								roadName="Chemin Piéton";
							}
							else if(road->get<RoadTypeField>() == road::ROAD_TYPE_STEPS) {
								roadName="Escaliers";
							}
							else if(road->get<RoadTypeField>() == road::ROAD_TYPE_BRIDGE) {
								roadName="Pont / Passerelle";
							}
							else if(road->get<RoadTypeField>() == road::ROAD_TYPE_TUNNEL) {
								roadName="Tunnel";
							}
							else {
								roadName="Route sans nom";
							}
						}
						double dst = its->getDistance();
						vector<geos::geom::Geometry*> geometries;
						graph::Journey::ServiceUses::const_iterator next = its+1;
						boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
						if(geometry.get())
						{
							boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
								*geometry
							)	);
							geometries.push_back(wgs84LineString.get());
							allGeometries.push_back(wgs84LineString.get());
							geometriesSPtr.push_back(wgs84LineString);
						}
						while (next != subResult.get<0>().getServiceUses().end())
						{
							string nextRoadName(
								dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()->getAnyRoadPlace()->getName()
							);
							if(!roadName.compare(nextRoadName))
							{
								++its;
								dst += its->getDistance();
								boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
								if(geometry.get())
								{
									boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
										*geometry
									)	);
									geometries.push_back(wgs84LineString.get());
									allGeometries.push_back(wgs84LineString.get());
									geometriesSPtr.push_back(wgs84LineString);
								}
								next = its+1;
							}
							else
							{
								break;
							}
						}
						boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
							CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
								geometries
						)	);
						submapLeg->insert("arrival_date_time", its->getArrivalDateTime());
						submapLeg->insert("geometry", multiLineString->toString());
						boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
						submapWalkAttributes->insert("length", dst);

						boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
						submapLegRoad->insert("name", roadName);
						submapLegRoad->insert("id", road->getAnyRoadPlace()->getKey());

						submapWalkAttributes->insert("road", submapLegRoad);

						submapLeg->insert("walk_attributes", submapWalkAttributes);

						// Arrival place
						boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
						if(dynamic_cast<const road::Crossing*>(its->getRealTimeArrivalVertex()->getHub()))
						{
							submapArrivalPlace->insert("name", (string)("Croisement"));
							submapArrivalPlace->insert("type", (string)("crossing"));
							submapArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(its->getRealTimeArrivalVertex()->getHub())->getKey());
						}

						if(its->getRealTimeArrivalVertex()->getGeometry().get() &&
							!its->getRealTimeArrivalVertex()->getGeometry()->isEmpty())
						{
							boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*(its->getRealTimeArrivalVertex()->getGeometry())
							)	);
							submapArrivalPlace->insert("longitude", wgs84Point->getX());
							submapArrivalPlace->insert("latitude", wgs84Point->getY());
						}

						submapLeg->insert("arrival", submapArrivalPlace);

						submapJourney->insert("leg", submapLeg);

						// Next service use
						if(its == (subResult.get<0>().getServiceUses().end()-1)) break;
						++its;
					}
					boost::posix_time::ptime arrivalTime;
					if(!subResult.get<1>().empty())
					{
						int curDistance(0);
						boost::posix_time::ptime departureTime(subResult.get<0>().empty() ?
							startDate : subResult.get<0>().getFirstArrivalTime());
						bool first(true);
						vector<geos::geom::Geometry*> geometries;
						vector<geos::geom::Geometry*> curGeom;
						double speed(bikeAccessParameters.getApproachSpeed());
						boost::shared_ptr<ParametersMap> submapLegDeparturePlace(new ParametersMap);
						boost::shared_ptr<ParametersMap> submapLegArrivalPlace(new ParametersMap);
						road::RoadPlace* curRoadPlace(NULL);
						const road::RoadChunkEdge* lastChunk;

						BOOST_FOREACH(const road::RoadChunkEdge* chunk, subResult.get<1>())
						{
							const road::Road* road(chunk->getRoadChunk()->getRoad());
							lastChunk = chunk;
							boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
							boost::shared_ptr<geos::geom::Geometry> geometryProjected(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(*geometry));
							allGeometries.push_back(geometryProjected.get());
							geometriesSPtr.push_back(geometryProjected);
							double distance(0);
							if(geometry)
							{
								geos::geom::CoordinateSequence* coordinates = geometry->getCoordinates();
								distance = geos::algorithm::CGAlgorithms::length(coordinates);
								delete coordinates;
							}

							if(first)
							{
								curDistance = static_cast<int>(distance);
								arrivalTime = startDate + boost::posix_time::seconds(static_cast<int>(distance / speed));
								curRoadPlace = &*road->getAnyRoadPlace();
								curGeom.push_back(geometryProjected.get()->clone());
								first = false;

								// Departure place
								if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
								{
									submapLegDeparturePlace->insert("name", (string)("Croisement"));
									submapLegDeparturePlace->insert("type", (string)("crossing"));
									submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
								}

								if(chunk->getFromVertex()->getGeometry().get() &&
									!chunk->getFromVertex()->getGeometry()->isEmpty())
								{
									boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
										*(chunk->getFromVertex()->getGeometry())
									)	);
									submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
									submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
								}
							}
							else if(curRoadPlace->getName() == road->getAnyRoadPlace()->getName())
							{
								curDistance += static_cast<int>(distance);
								arrivalTime = arrivalTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
								curGeom.push_back(geometryProjected.get()->clone());
							}
							else
							{
								geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
								geometries.push_back(multiLineString->clone());

								boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
								submapLeg->insert("departure_date_time", departureTime);
								submapLeg->insert("departure", submapLegDeparturePlace);

								// Arrival place
								if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
								{
									submapLegArrivalPlace->insert("name", (string)("Croisement"));
									submapLegArrivalPlace->insert("type", (string)("crossing"));
									submapLegArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
								}

								if(chunk->getFromVertex()->getGeometry().get() &&
									!chunk->getFromVertex()->getGeometry()->isEmpty())
								{
									boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
										*(chunk->getFromVertex()->getGeometry())
									)	);
									submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
									submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
								}
								submapLeg->insert("arrival", submapLegArrivalPlace);

								submapLegDeparturePlace.reset(new ParametersMap);
								submapLegArrivalPlace.reset(new ParametersMap);

								submapLeg->insert("arrival_date_time", arrivalTime);
								submapLeg->insert("geometry", multiLineString->toString());
								boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
								submapWalkAttributes->insert("length", curDistance);

								boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
								submapLegRoad->insert("name", curRoadPlace->getName());
								submapLegRoad->insert("id", curRoadPlace->getKey());

								submapWalkAttributes->insert("road", submapLegRoad);

								submapLeg->insert("bike_attributes", submapWalkAttributes);

								submapJourney->insert("leg", submapLeg);

								delete multiLineString;
								curDistance = static_cast<int>(distance);
								departureTime = arrivalTime;
								arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
								curRoadPlace = &*road->getAnyRoadPlace();

								BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
								{
									delete geomToDelete;
								}

								curGeom.clear();
								curGeom.push_back(geometryProjected.get()->clone());

								// New departure place
								if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
								{
									submapLegDeparturePlace->insert("name", (string)("Croisement"));
									submapLegDeparturePlace->insert("type", (string)("crossing"));
									submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
								}

								if(chunk->getFromVertex()->getGeometry().get() &&
									!chunk->getFromVertex()->getGeometry()->isEmpty())
								{
									boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
										*(chunk->getFromVertex()->getGeometry())
									)	);
									submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
									submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
								}
							}
						}

						geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
						geometries.push_back(multiLineString->clone());

						BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
						{
							delete geomToDelete;
						}
						curGeom.clear();
						boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
						submapLeg->insert("departure_date_time", departureTime);
						submapLeg->insert("departure", submapLegDeparturePlace);

						// Arrival place
						if(dynamic_cast<const road::Crossing*>(lastChunk->getFromVertex()->getHub()))
						{
							submapLegArrivalPlace->insert("name", (string)("Croisement"));
							submapLegArrivalPlace->insert("type", (string)("crossing"));
							submapLegArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(lastChunk->getFromVertex()->getHub())->getKey());
						}

						if(lastChunk->getFromVertex()->getGeometry().get() &&
							!lastChunk->getFromVertex()->getGeometry()->isEmpty())
						{
							boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*(lastChunk->getFromVertex()->getGeometry())
							)	);
							submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
							submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
						}
						submapLeg->insert("arrival", submapLegArrivalPlace);

						submapLeg->insert("arrival_date_time", arrivalTime);
						submapLeg->insert("geometry", multiLineString->toString());
						boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
						submapWalkAttributes->insert("length", curDistance);

						boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
						submapLegRoad->insert("name", curRoadPlace->getName());
						submapLegRoad->insert("id", curRoadPlace->getKey());

						submapWalkAttributes->insert("road", submapLegRoad);

						submapLeg->insert("bike_attributes", submapWalkAttributes);

						submapJourney->insert("leg", submapLeg);

						delete multiLineString;

						BOOST_FOREACH(geos::geom::Geometry* geomToDelete, geometries)
						{
							delete geomToDelete;
						}
						geometries.clear();
					}

					// Display of approach at the arrival
					its = subResult.get<2>().getServiceUses().begin();
					boost::posix_time::time_duration timeShift;
					while(true)
					{
						if(its == (subResult.get<2>().getServiceUses().end())) break;
						boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
						timeShift = arrivalTime - its->getDepartureDateTime();
						submapLeg->insert("departure_date_time", its->getDepartureDateTime() + timeShift);
						// Departure place
						boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
						if(dynamic_cast<const road::Crossing*>(its->getRealTimeDepartureVertex()->getHub()))
						{
							submapDeparturePlace->insert("name", (string)("Croisement"));
							submapDeparturePlace->insert("type", (string)("crossing"));
							submapDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(its->getRealTimeDepartureVertex()->getHub())->getKey());
						}

						if(its->getRealTimeDepartureVertex()->getGeometry().get() &&
							!its->getRealTimeDepartureVertex()->getGeometry()->isEmpty())
						{
							boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*(its->getRealTimeDepartureVertex()->getGeometry())
							)	);
							submapDeparturePlace->insert("longitude", wgs84Point->getX());
							submapDeparturePlace->insert("latitude", wgs84Point->getY());
						}

						submapLeg->insert("departure", submapDeparturePlace);

						const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());

						std::string roadName = road->getAnyRoadPlace()->getName();
						if(roadName.empty()) {
							if(	road->get<RoadTypeField>() == road::ROAD_TYPE_PEDESTRIANPATH ||
								road->get<RoadTypeField>() == road::ROAD_TYPE_PEDESTRIANSTREET
							){
								roadName="Chemin Piéton";
							}
							else if(road->get<RoadTypeField>() == road::ROAD_TYPE_STEPS) {
								roadName="Escaliers";
							}
							else if(road->get<RoadTypeField>() == road::ROAD_TYPE_BRIDGE) {
								roadName="Pont / Passerelle";
							}
							else if(road->get<RoadTypeField>() == road::ROAD_TYPE_TUNNEL) {
								roadName="Tunnel";
							}
							else {
								roadName="Route sans nom";
							}
						}
						double dst = its->getDistance();
						vector<geos::geom::Geometry*> geometries;
						graph::Journey::ServiceUses::const_iterator next = its+1;
						boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
						if(geometry.get())
						{
							boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
								*geometry
							)	);
							geometries.push_back(wgs84LineString.get());
							allGeometries.push_back(wgs84LineString.get());
							geometriesSPtr.push_back(wgs84LineString);
						}
						while (next != subResult.get<2>().getServiceUses().end())
						{
							string nextRoadName(
								dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()->getAnyRoadPlace()->getName()
							);
							if(!roadName.compare(nextRoadName))
							{
								++its;
								dst += its->getDistance();
								boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
								if(geometry.get())
								{
									boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
										*geometry
									)	);
									geometries.push_back(wgs84LineString.get());
									allGeometries.push_back(wgs84LineString.get());
									geometriesSPtr.push_back(wgs84LineString);
								}
								next = its+1;
							}
							else
							{
								break;
							}
						}
						boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
							CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
								geometries
						)	);
						submapLeg->insert("arrival_date_time", its->getArrivalDateTime() + timeShift);
						submapLeg->insert("geometry", multiLineString->toString());
						boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
						submapWalkAttributes->insert("length", dst);

						boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
						submapLegRoad->insert("name", roadName);
						submapLegRoad->insert("id", road->getAnyRoadPlace()->getKey());

						submapWalkAttributes->insert("road", submapLegRoad);

						submapLeg->insert("walk_attributes", submapWalkAttributes);

						// Arrival place
						boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
						if(dynamic_cast<const road::Crossing*>(its->getRealTimeArrivalVertex()->getHub()))
						{
							submapArrivalPlace->insert("name", (string)("Croisement"));
							submapArrivalPlace->insert("type", (string)("crossing"));
							submapArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(its->getRealTimeArrivalVertex()->getHub())->getKey());
						}

						if(its->getRealTimeArrivalVertex()->getGeometry().get() &&
							!its->getRealTimeArrivalVertex()->getGeometry()->isEmpty())
						{
							boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*(its->getRealTimeArrivalVertex()->getGeometry())
							)	);
							submapArrivalPlace->insert("longitude", wgs84Point->getX());
							submapArrivalPlace->insert("latitude", wgs84Point->getY());
						}

						submapLeg->insert("arrival", submapArrivalPlace);

						submapJourney->insert("leg", submapLeg);

						// Next service use
						if(its == (subResult.get<2>().getServiceUses().end()-1)) break;
						++its;
					}

					if(!subResult.get<2>().empty())
					{
						submapJourney->insert("arrival_date_time", subResult.get<2>().getFirstArrivalTime() + timeShift);
					}
					else
					{
						submapJourney->insert("arrival_date_time", arrivalTime);
					}

					submapJourney->insert("departure", submapDeparturePlace);
					submapJourney->insert("arrival", submapArrivalPlace);
					boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
						CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
							allGeometries
					)	);
					submapJourney->insert("geometry", multiLineString->toString());

					pm.insert("journey", submapJourney);
				}
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* bike journey planner data processing");
			}
			else if (_usePublicBike)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before SYNTHESE bike");

				// Initialization
				graph::AccessParameters approachAccessParameters(
					graph::USER_PEDESTRIAN,			// user class code
					false,							// DRT only
					false,							// without DRT
					_useWalk ? 1000 : 0,			// max approach distance
					boost::posix_time::minutes(23),	// max approach time
					_useWalk ? 1.111 : 0.0,			// approach speed
					_maxTransportConnectionCount	// max transport connection count (ie : max number of used transport services - 1)
				);

				// Classical synthese algorithm
				algorithm::AlgorithmLogger logger;
				graph::AccessParameters bikeAccessParameters(graph::USER_BIKE, false, false, 72000, boost::posix_time::hours(24), 4.167);
				public_biking::PublicBikeJourneyPlanner pbjp(
					departure,
					arrival,
					startDate,
					endDate,
					approachAccessParameters,
					bikeAccessParameters,
					logger
				);
				public_biking::PublicBikeJourneyPlannerResult results = pbjp.run();

				if(!results.getJourneys().empty())
				{
					for (public_biking::PublicBikeJourneyPlannerResult::Journeys::const_iterator it(results.getJourneys().begin()); it != results.getJourneys().end(); ++it)
					{
						boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
						_serializeJourney(*it, departure, arrival, submapJourney);
						pm.insert("journey", submapJourney);
					}
				}

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE bike");
			}

			// Public transport + public bike (with optional walk)
			if (_usePublicBike && _usePt)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : public transport + bike, computations of VAMs");

				// Initializations
				graph::AccessParameters pedestrianAccessParameters(
					graph::USER_PEDESTRIAN,			// user class code
					false,							// DRT only
					false,							// without DRT
					_useWalk ? 1000 : 0,			// max approach distance
					boost::posix_time::minutes(23),	// max approach time
					_useWalk ? 1.111 : 0.0,			// approach speed
					_maxTransportConnectionCount	// max transport connection count (ie : max number of used transport services - 1)
				);

				graph::AccessParameters bikeAccessParameters(
					graph::USER_BIKE,				// user class code
					false,							// DRT only
					false,							// without DRT
					4000,							// max approach distance
					boost::posix_time::minutes(16),	// max approach time
					4.167							// approach speed
				);

				// AlgorithmLogger is still broken when performing non PT journey planning
				algorithm::AlgorithmLogger inactiveLogger;

				// Initialize VAMs

				// The final VAMs : they will contain PT stop points with full walk and/or bike approach
				graph::VertexAccessMap departureVam, arrivalVam;

				// Departure VAM containing PT stop points reachable using walk only, before extension
				graph::VertexAccessMap departurePTStopsUsingWalkVam =
					departure->getVertexAccessMap(pedestrianAccessParameters, pt::PTModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0);
				// Arrival VAM containing PT stop points reachable using walk only, before extension
				graph::VertexAccessMap arrivalPTStopsUsingWalkVam =
					arrival->getVertexAccessMap(pedestrianAccessParameters, pt::PTModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0);

				// Departure VAM containing bike stations reachable using walk only, before extension
				graph::VertexAccessMap departureBikeStationsUsingWalkVam =
					departure->getVertexAccessMap(pedestrianAccessParameters, public_biking::PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0);
				// Arrival VAM containing bike stations reachable using walk only, before extension
				graph::VertexAccessMap arrivalBikeStationsUsingWalkVam =
					arrival->getVertexAccessMap(pedestrianAccessParameters, public_biking::PublicBikingModule::GRAPH_ID, road::RoadModule::GRAPH_ID, 0);

				// This VAM will contain bike stations reachable from departure using walk and/or bike
				graph::VertexAccessMap departureBikeStationsUsingBikeVam = departureBikeStationsUsingWalkVam;
				// This VAM will contain bike stations reachable from arrival using walk and/or bike
				graph::VertexAccessMap arrivalBikeStationsUsingBikeVam = arrivalBikeStationsUsingWalkVam;

				/*
				std::cout << "---------------------------" << std::endl;
				std::cout << "BEFORE VAM EXTENSION : " << std::endl;
				std::cout << "departurePTStopsUsingWalkVam has " << departurePTStopsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, departurePTStopsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "arrivalPTStopsUsingWalkVam has " << departurePTStopsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, arrivalPTStopsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "departureBikeStationsUsingWalkVam has " << departureBikeStationsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, departureBikeStationsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "arrivalBikeStationsUsingWalkVam has " << arrivalBikeStationsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, arrivalBikeStationsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				*/

				// If walk is allowed, gather the stop points and the bike stations reachable from departure and arrival
				if(0 < pedestrianAccessParameters.getApproachSpeed())
				{
					// Stop points
					algorithm::VAMConverter extenderToPhysicalStops(
						pedestrianAccessParameters,
						inactiveLogger,
						pt::PTModule::GRAPH_ID,
						road::RoadModule::GRAPH_ID,
						startDate,
						startDate,
						startDate,
						endDate,
						departure,
						arrival
					);

					//std::cout << "fullDeparturePTStopsUsingWalkVam" << std::endl;

					// Gather all reachable stop points from departure using walk only
					graph::VertexAccessMap fullDeparturePTStopsUsingWalkVam = extenderToPhysicalStops.run(
						departurePTStopsUsingWalkVam,
						arrivalPTStopsUsingWalkVam,
						algorithm::DEPARTURE_TO_ARRIVAL
					);

					//std::cout << "fullArrivalPTStopsUsingWalkVam" << std::endl;

					// Gather all reachable stop points from arrival using walk only
					graph::VertexAccessMap fullArrivalPTStopsUsingWalkVam = extenderToPhysicalStops.run(
						arrivalPTStopsUsingWalkVam,
						departurePTStopsUsingWalkVam,
						algorithm::ARRIVAL_TO_DEPARTURE
					);

					// Update VAMs
					departurePTStopsUsingWalkVam = fullDeparturePTStopsUsingWalkVam;
					arrivalPTStopsUsingWalkVam = fullArrivalPTStopsUsingWalkVam;

					// Bike stations
					algorithm::VAMConverter extenderToBikeStations(
						pedestrianAccessParameters,
						inactiveLogger,
						public_biking::PublicBikingModule::GRAPH_ID,
						road::RoadModule::GRAPH_ID,
						startDate,
						startDate,
						startDate,
						endDate,
						departure,
						arrival
					);

					//std::cout << "fullDepartureBikeStationsUsingWalkVam" << std::endl;

					// Gather all reachable bike stations from departure using walk only
					graph::VertexAccessMap fullDepartureBikeStationsUsingWalkVam = extenderToBikeStations.run(
						departureBikeStationsUsingWalkVam,
						arrivalBikeStationsUsingWalkVam,
						algorithm::DEPARTURE_TO_ARRIVAL
					);

					//std::cout << "fullArrivalBikeStationsUsingWalkVam" << std::endl;

					// Gather all reachable bike stations from arrival using walk only
					graph::VertexAccessMap fullArrivalBikeStationsUsingWalkVam = extenderToBikeStations.run(
						arrivalBikeStationsUsingWalkVam,
						departureBikeStationsUsingWalkVam,
						algorithm::ARRIVAL_TO_DEPARTURE
					);

					// Update VAMs
					departureBikeStationsUsingWalkVam = fullDepartureBikeStationsUsingWalkVam;
					arrivalBikeStationsUsingWalkVam = fullArrivalBikeStationsUsingWalkVam;
				}

				/*
				std::cout << "AFTER PEDESTRIAN VAM EXTENSION : " << std::endl;
				std::cout << "departurePTStopsUsingWalkVam has " << departurePTStopsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, departurePTStopsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "arrivalPTStopsUsingWalkVam has " << departurePTStopsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, arrivalPTStopsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "departureBikeStationsUsingWalkVam has " << departureBikeStationsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, departureBikeStationsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "arrivalBikeStationsUsingWalkVam has " << arrivalBikeStationsUsingWalkVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, arrivalBikeStationsUsingWalkVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				*/

				// If bike is allowed, compute the list of bike stations reachable from departure and arrival then gather the stop points reachable from those bike stations
				if(0 < bikeAccessParameters.getApproachSpeed())
				{
					// OVE!!! : This code is wrong, we cannot use a simple VAMConverter because it does not take into account the public bike networks
					// Use it temporarily to get results, then move to proper implementation

					// Bike stations
					algorithm::VAMConverter extenderToBikeStations(
						bikeAccessParameters,
						inactiveLogger,
						public_biking::PublicBikingModule::GRAPH_ID,
						road::RoadModule::GRAPH_ID,
						startDate,
						startDate,
						startDate,
						endDate,
						departure,
						arrival
					);

					// OVE!!! : bike stations VAM contain only bike stations, which do not belong to road::RoadModule::GRAPH_ID and are discarded
					graph::VertexAccessMap crossingsAroundDepartureBikeStationsVam2 =
						_buildCrossingVAMFromBikeStationVAM(departureBikeStationsUsingWalkVam, pedestrianAccessParameters);

					// Gather the bike stations reachable using bike from the bike stations previously reached from departure
					departureBikeStationsUsingBikeVam = extenderToBikeStations.run(
						//departureBikeStationsUsingWalkVam,
						crossingsAroundDepartureBikeStationsVam2,
						arrivalBikeStationsUsingWalkVam,
						algorithm::DEPARTURE_TO_ARRIVAL
					);

					// OVE!!! : bike stations VAM contain only bike stations, which do not belong to road::RoadModule::GRAPH_ID and are discarded
					graph::VertexAccessMap crossingsAroundArrivalBikeStationsVam2 =
						_buildCrossingVAMFromBikeStationVAM(arrivalBikeStationsUsingWalkVam, pedestrianAccessParameters);

					// Gather the bike stations reachable using bike from the bike stations previously reached from arrival
					arrivalBikeStationsUsingBikeVam = extenderToBikeStations.run(
						//arrivalBikeStationsUsingWalkVam,
						crossingsAroundArrivalBikeStationsVam2,
						departureBikeStationsUsingWalkVam,
						algorithm::ARRIVAL_TO_DEPARTURE
					);

					// Stop points
					algorithm::VAMConverter extenderToPhysicalStops(
						pedestrianAccessParameters,
						inactiveLogger,
						pt::PTModule::GRAPH_ID,
						road::RoadModule::GRAPH_ID,
						startDate,
						startDate,
						startDate,
						endDate,
						departure,
						arrival
					);

					// OVE!!! : bike stations VAM contain only bike stations, which do not belong to road::RoadModule::GRAPH_ID and are discarded
					graph::VertexAccessMap crossingsAroundDepartureBikeStationsVam =
						_buildCrossingVAMFromBikeStationVAM(departureBikeStationsUsingBikeVam, pedestrianAccessParameters);

					graph::VertexAccessMap crossingsAroundArrivalBikeStationsVam =
						_buildCrossingVAMFromBikeStationVAM(arrivalBikeStationsUsingBikeVam, pedestrianAccessParameters);

					// Gather all reachable stop points from departure using walk only
					departureVam = extenderToPhysicalStops.run(
						//departureBikeStationsUsingBikeVam,
						crossingsAroundDepartureBikeStationsVam,
						arrivalPTStopsUsingWalkVam,  // OVE!!!
						algorithm::DEPARTURE_TO_ARRIVAL
					);

					// Gather all reachable stop points from arrival using walk only
					arrivalVam = extenderToPhysicalStops.run(
						//arrivalBikeStationsUsingBikeVam,
						crossingsAroundArrivalBikeStationsVam,
						departurePTStopsUsingWalkVam,  // OVE!!!
						algorithm::ARRIVAL_TO_DEPARTURE
					);

					// Merge departurePTStopsUsingWalkVam into departureVam
					BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& pedestrianVamElement, departurePTStopsUsingWalkVam.getMap())
					{
						departureVam.insert(pedestrianVamElement.first, pedestrianVamElement.second);
					}

					// Merge arrivalPTStopsUsingWalkVam into arrivalVam
					BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& pedestrianVamElement, arrivalPTStopsUsingWalkVam.getMap())
					{
						arrivalVam.insert(pedestrianVamElement.first, pedestrianVamElement.second);
					}
				}

				/*
				std::cout << "AFTER BIKE VAM EXTENSION : " << std::endl;
				std::cout << "departureBikeStationsUsingBikeVam has " << departureBikeStationsUsingBikeVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, departureBikeStationsUsingBikeVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "arrivalBikeStationsUsingBikeVam has " << arrivalBikeStationsUsingBikeVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, arrivalBikeStationsUsingBikeVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "departureVam has " << departureVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, departureVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				std::cout << "arrivalVam has " << arrivalVam.getMap().size() << " elements" << std::endl;
				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& vamElement, arrivalVam.getMap())
				{
					std::cout << " * vertex " << vamElement.first->getKey() << " has vertex access = " << vamElement.second.approachTime << "/" << vamElement.second.approachDistance << std::endl;
				}
				*/

				algorithm::TimeSlotRoutePlanner::Result ptResults;
				// OVE!!! : copied from PTTimeSlotRoutePlanner::run()

				// Handle of the case of possible full road approach
				if(departureVam.intersercts(arrivalVam))
				{
					graph::Journey resultJourney(departureVam.getBestIntersection(arrivalVam));
					ptime departureTime(resultJourney.getFirstDepartureTime());
					if(departureTime.time_of_day().seconds())
					{
						resultJourney.shift(seconds(60 - departureTime.time_of_day().seconds()));
					}
					resultJourney.shift(
						startDate - resultJourney.getFirstDepartureTime()
					);
					resultJourney.forceContinuousServiceRange(endDate - startDate);

					ptResults.push_back(resultJourney);
				}

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before public transport + bike journey planning");

				if(ptResults.empty())
				{
					std::cout << "before TimeSlotRoutePlanner :" << std::endl;
					algorithm::TimeSlotRoutePlanner timeSlotRp(
						departureVam,
						arrivalVam,
						startDate,
						endDate,
						startDate,
						endDate,
						pt::PTModule::GRAPH_ID,
						pt::PTModule::GRAPH_ID,
						optional<posix_time::time_duration>(),
						2,
						pedestrianAccessParameters,
						algorithm::DEPARTURE_FIRST,
						70, // 252 km/h TODO take it configurable
						false,
						inactiveLogger
					);

					ptResults = timeSlotRp.run();
					//_logger.closeTimeSlotJourneyPlannerLog();
				}
				else
				{
					algorithm::TimeSlotRoutePlanner timeSlotRp(
						departureVam,
						arrivalVam,
						ptResults.front(),
						pt::PTModule::GRAPH_ID,
						pt::PTModule::GRAPH_ID,
						optional<posix_time::time_duration>(),
						2,
						pedestrianAccessParameters,
						algorithm::DEPARTURE_FIRST,
						100,
						false,
						inactiveLogger
					);
					ptResults = timeSlotRp.run();
					//_logger.closeTimeSlotJourneyPlannerLog();
				}

				pt_journey_planner::PTRoutePlannerResult rpResults(departure, arrival, false, ptResults);
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after public transport + bike");
				// OVE!!!

				if(!rpResults.getJourneys().empty())
				{
					for (pt_journey_planner::PTRoutePlannerResult::Journeys::const_iterator it(rpResults.getJourneys().begin()); it != rpResults.getJourneys().end(); ++it)
					{
						boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
						_serializeJourney(*it, departure, arrival, submapJourney);
						pm.insert("journey", submapJourney);
					}
				}

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after pt+bike data processing");
			}

			if (_useTaxi && _aStarForWalk)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before A* taxi door to door");

				// Astar algorithm
				graph::AccessParameters taxiAccessParameters(graph::USER_CAR, false, false, 1200000, boost::posix_time::hours(24), 13.889);
				algorithm::AStarShortestPathCalculator r(
					departure,
					arrival,
					startDate,
					taxiAccessParameters
				);

				algorithm::AStarShortestPathCalculator::ResultPath path(r.run());

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* taxi door to door");

				if(!path.empty())
				{
					boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
					boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
					submapJourney->insert("departure_date_time", startDate);
					submapLeg->insert("departure_date_time", startDate);

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					if(dynamic_cast<const NamedPlace*>(departure))
					{
						submapDeparturePlace->insert("name", dynamic_cast<const NamedPlace*>(departure)->getFullName());
						submapDeparturePlace->insert("type", dynamic_cast<const NamedPlace*>(departure)->getFactoryKey());
						submapDeparturePlace->insert("id", dynamic_cast<const NamedPlace*>(departure)->getKey());
					}
					else
					{
						submapDeparturePlace->insert("name", dynamic_cast<const City*>(departure)->getName());
						string strCityType("City");
						submapDeparturePlace->insert("type", strCityType);
						submapDeparturePlace->insert("id", dynamic_cast<const City*>(departure)->getKey());
					}

					if(departure->getPoint().get() &&
						!departure->getPoint()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(departure->getPoint())
						)	);
						submapDeparturePlace->insert("longitude", wgs84Point->getX());
						submapDeparturePlace->insert("latitude", wgs84Point->getY());
					}

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					if(dynamic_cast<const NamedPlace*>(arrival))
					{
						submapArrivalPlace->insert("name", dynamic_cast<const NamedPlace*>(arrival)->getFullName());
						submapArrivalPlace->insert("type", dynamic_cast<const NamedPlace*>(arrival)->getFactoryKey());
						submapArrivalPlace->insert("id", dynamic_cast<const NamedPlace*>(arrival)->getKey());
					}
					else
					{
						submapArrivalPlace->insert("name", dynamic_cast<const City*>(arrival)->getName());
						string strCityType("City");
						submapArrivalPlace->insert("type", strCityType);
						submapArrivalPlace->insert("id", dynamic_cast<const City*>(arrival)->getKey());
					}

					if(arrival->getPoint().get() &&
						!arrival->getPoint()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(arrival->getPoint())
						)	);
						submapArrivalPlace->insert("longitude", wgs84Point->getX());
						submapArrivalPlace->insert("latitude", wgs84Point->getY());
					}

					submapJourney->insert("departure", submapDeparturePlace);
					submapJourney->insert("arrival", submapArrivalPlace);
					submapLeg->insert("departure", submapDeparturePlace);
					submapLeg->insert("arrival", submapArrivalPlace);

					int curDistance(0);
					int totalDistance(0);
					boost::posix_time::ptime arrivalTime;
					boost::posix_time::ptime departureTime(startDate);
					bool first(true);
					vector<geos::geom::Geometry*> geometries;
					vector<geos::geom::Geometry*> curGeom;
					double speed(taxiAccessParameters.getApproachSpeed());
					road::RoadPlace* curRoadPlace(NULL);

					BOOST_FOREACH(const road::RoadChunkEdge* chunk, path)
					{
						const road::Road* road(chunk->getRoadChunk()->getRoad());
						boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
						boost::shared_ptr<geos::geom::Geometry> geometryProjected(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(*geometry));
						double distance(0);
						if(geometry)
						{
							geos::geom::CoordinateSequence* coordinates = geometry->getCoordinates();
							distance = geos::algorithm::CGAlgorithms::length(coordinates);
							delete coordinates;
						}

						if(first)
						{
							curDistance = static_cast<int>(distance);
							arrivalTime = startDate + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curRoadPlace = &*road->getAnyRoadPlace();
							curGeom.push_back(geometryProjected.get()->clone());
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
							geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
							geometries.push_back(multiLineString->clone());

							delete multiLineString;
							curDistance = static_cast<int>(distance);
							departureTime = arrivalTime;
							arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curRoadPlace = &*road->getAnyRoadPlace();

							BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
							{
								delete geomToDelete;
							}

							curGeom.clear();
							curGeom.push_back(geometryProjected.get()->clone());
						}
						totalDistance += curDistance;
					}

					geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
					geometries.push_back(multiLineString->clone());

					BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
					{
						delete geomToDelete;
					}
					curGeom.clear();

					delete multiLineString;

					submapJourney->insert("arrival_date_time", arrivalTime);
					submapLeg->insert("arrival_date_time", arrivalTime);
					geos::geom::GeometryCollection* geometryCollection(CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createGeometryCollection(geometries));
					BOOST_FOREACH(geos::geom::Geometry* geomToDelete, geometries)
					{
						delete geomToDelete;
					}
					geometries.clear();
					submapJourney->insert("geometry", geometryCollection->toString());
					submapLeg->insert("geometry", geometryCollection->toString());

					// Road
					boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
					submapRoadDetails->insert("name", (string)("Trajet en taxi"));
					submapRoadDetails->insert("id", 0);

					boost::shared_ptr<ParametersMap> submapLegNetwork(new ParametersMap);
					submapLegNetwork->insert("name", (string)("Taxi"));
					submapLegNetwork->insert("id", 0);

					// Specific leg attributes
					boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
					submapRoadLegAttributes->insert("length", totalDistance);
					submapRoadLegAttributes->insert("road", submapRoadDetails);
					submapRoadLegAttributes->insert("road", submapLegNetwork);

					submapLeg->insert("car_attributes", submapRoadLegAttributes);
					submapJourney->insert("leg", submapLeg);

					delete geometryCollection;
					std::string remark("Trajet Taxi porte à porte, réservation obligatoire au moins 30 minutes avant l'heure de départ au 0844 814 814");
					submapJourney->insert("remark", remark);

					pm.insert("journey", submapJourney);
				}
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* taxi door to door data processing");
			}
			else if (_useTaxi)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before SYNTHESE taxi door to door");

				// Classical synthese algorithm
				// TODO : Note we cannot really use AlgorithmLogger right now since not targetted for pure road journey search
				algorithm::AlgorithmLogger logger;
				graph::AccessParameters taxiAccessParameters(graph::USER_CAR, false, false, 1200000, boost::posix_time::hours(24), 13.889);
				road_journey_planner::RoadJourneyPlanner rjp(
					departure,
					arrival,
					startDate,
					startDate,
					endDate,
					endDate,
					1,
					taxiAccessParameters,
					algorithm::DEPARTURE_FIRST,
					logger,
					true
				);
				road_journey_planner::RoadJourneyPlannerResult results = rjp.run();

				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE taxi door to door");

				if(!results.getJourneys().empty())
				{
					for (road_journey_planner::RoadJourneyPlannerResult::Journeys::const_iterator it(results.getJourneys().begin()); it != results.getJourneys().end(); ++it)
					{
						boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
						boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
						submapJourney->insert("departure_date_time", it->getFirstDepartureTime());
						submapJourney->insert("arrival_date_time", it->getFirstArrivalTime());
						submapLeg->insert("departure_date_time", it->getFirstDepartureTime());
						submapLeg->insert("arrival_date_time", it->getFirstArrivalTime());

						// Departure place
						boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
						if(NULL != dynamic_cast<const road::Crossing*>(it->getOrigin()->getHub()))
						{
							_serializePlace(departure, submapDeparturePlace);
						}
						else if(NULL != dynamic_cast<const NamedPlace*>(it->getOrigin()->getHub()))
						{
							_serializePlace(dynamic_cast<const NamedPlace*>(it->getOrigin()->getHub()), submapDeparturePlace);
						}

						_serializeLatLong(it->getOrigin()->getFromVertex()->getGeometry(), submapDeparturePlace);

						// Arrival place
						boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
						if(NULL != dynamic_cast<const road::Crossing*>(it->getDestination()->getHub()))
						{
							_serializePlace(arrival, submapArrivalPlace);
						}
						else if(NULL != dynamic_cast<const NamedPlace*>(it->getDestination()->getHub()))
						{
							_serializePlace(dynamic_cast<const NamedPlace*>(it->getDestination()->getHub()), submapArrivalPlace);
						}

						_serializeLatLong(it->getDestination()->getFromVertex()->getGeometry(), submapArrivalPlace);

						submapJourney->insert("departure", submapDeparturePlace);
						submapJourney->insert("arrival", submapArrivalPlace);
						submapLeg->insert("departure", submapDeparturePlace);
						submapLeg->insert("arrival", submapArrivalPlace);

						graph::Journey::ServiceUses::const_iterator its(it->getServiceUses().begin());
						vector<boost::shared_ptr<geos::geom::Geometry> > geometriesSPtr; // To keep shared_ptr's in scope !
						vector<geos::geom::Geometry*> allGeometries;
						double total_distance(0.0);
						while(it->getServiceUses().end() != its)
						{
							const graph::Path* path = its->getService()->getPath();
							const road::RoadPath* road(dynamic_cast<const road::RoadPath*>(path));

							if (NULL != road)
							{
								const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());
								std::string roadName = _getRoadName(road);
								double dst = its->getDistance();
								graph::Journey::ServiceUses::const_iterator next = its+1;
								boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
								if(geometry.get())
								{
									boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
										*geometry
									)	);
									allGeometries.push_back(wgs84LineString.get());
									geometriesSPtr.push_back(wgs84LineString);
								}
								// Concatenate road legs on the same road and using the same transportation mean
								while(
									next != it->getServiceUses().end() &&
									dynamic_cast<const road::RoadPath*> (next->getService()->getPath())
								)
								{
									std::string nextRoadName(
										_getRoadName(
											dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()
									)	);
									if(roadName != nextRoadName)
									{
										// Road changed => do not concatenate next leg
										break;
									}
									++its;
									dst += its->getDistance();
									boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
									if(geometry.get())
									{
										boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
											*geometry
										)	);
										allGeometries.push_back(wgs84LineString.get());
										geometriesSPtr.push_back(wgs84LineString);
									}
									next = its+1;
								}
								total_distance += dst;
							}

							// Next service use
							++its;
						}

						boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
							CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
								allGeometries
						)	);
						submapJourney->insert("geometry", multiLineString->toString());
						submapLeg->insert("geometry", multiLineString->toString());

						// Road
						boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
						submapRoadDetails->insert("name", (string)("Trajet en taxi"));
						submapRoadDetails->insert("id", 0);

						boost::shared_ptr<ParametersMap> submapLegNetwork(new ParametersMap);
						submapLegNetwork->insert("name", (string)("Taxi"));
						submapLegNetwork->insert("id", 0);

						// Specific leg attributes
						boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
						submapRoadLegAttributes->insert("length", total_distance);
						submapRoadLegAttributes->insert("road", submapRoadDetails);
						submapRoadLegAttributes->insert("network", submapLegNetwork);

						submapLeg->insert("car_attributes", submapRoadLegAttributes);
						submapJourney->insert("leg", submapLeg);

						std::string remark("Trajet Taxi porte à porte, réservation obligatoire au moins 30 minutes avant l'heure de départ au 0844 814 814");
						submapJourney->insert("remark", remark);
						pm.insert("journey", submapJourney);
					}
				}
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE taxi door to door data processing");
			}

			if (_useCarPooling && _aStarForWalk)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before A* car pooling door to door");

				// Astar algorithm
				graph::AccessParameters carPoolingAccessParameters(graph::USER_CAR, false, false, 1200000, boost::posix_time::hours(24), 13.889);
				algorithm::AStarShortestPathCalculator r(
					departure,
					arrival,
					startDate,
					carPoolingAccessParameters
				);

				algorithm::AStarShortestPathCalculator::ResultPath path(r.run());
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* car pooling door to door");

				if(!path.empty())
				{
					boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
					boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
					submapJourney->insert("departure_date_time", startDate);
					submapLeg->insert("departure_date_time", startDate);

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					if(dynamic_cast<const NamedPlace*>(departure))
					{
						submapDeparturePlace->insert("name", dynamic_cast<const NamedPlace*>(departure)->getFullName());
						submapDeparturePlace->insert("type", dynamic_cast<const NamedPlace*>(departure)->getFactoryKey());
						submapDeparturePlace->insert("id", dynamic_cast<const NamedPlace*>(departure)->getKey());
					}
					else
					{
						submapDeparturePlace->insert("name", dynamic_cast<const City*>(departure)->getName());
						string strCityType("City");
						submapDeparturePlace->insert("type", strCityType);
						submapDeparturePlace->insert("id", dynamic_cast<const City*>(departure)->getKey());
					}

					if(departure->getPoint().get() &&
						!departure->getPoint()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(departure->getPoint())
						)	);
						submapDeparturePlace->insert("longitude", wgs84Point->getX());
						submapDeparturePlace->insert("latitude", wgs84Point->getY());
					}

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					if(dynamic_cast<const NamedPlace*>(arrival))
					{
						submapArrivalPlace->insert("name", dynamic_cast<const NamedPlace*>(arrival)->getFullName());
						submapArrivalPlace->insert("type", dynamic_cast<const NamedPlace*>(arrival)->getFactoryKey());
						submapArrivalPlace->insert("id", dynamic_cast<const NamedPlace*>(arrival)->getKey());
					}
					else
					{
						submapArrivalPlace->insert("name", dynamic_cast<const City*>(arrival)->getName());
						string strCityType("City");
						submapArrivalPlace->insert("type", strCityType);
						submapArrivalPlace->insert("id", dynamic_cast<const City*>(arrival)->getKey());
					}

					if(arrival->getPoint().get() &&
						!arrival->getPoint()->isEmpty())
					{
						boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*(arrival->getPoint())
						)	);
						submapArrivalPlace->insert("longitude", wgs84Point->getX());
						submapArrivalPlace->insert("latitude", wgs84Point->getY());
					}

					submapJourney->insert("departure", submapDeparturePlace);
					submapJourney->insert("arrival", submapArrivalPlace);
					submapLeg->insert("departure", submapDeparturePlace);
					submapLeg->insert("arrival", submapArrivalPlace);

					int curDistance(0);
					int totalDistance(0);
					boost::posix_time::ptime arrivalTime;
					boost::posix_time::ptime departureTime(startDate);
					bool first(true);
					vector<geos::geom::Geometry*> geometries;
					vector<geos::geom::Geometry*> curGeom;
					double speed(carPoolingAccessParameters.getApproachSpeed());
					road::RoadPlace* curRoadPlace(NULL);

					BOOST_FOREACH(const road::RoadChunkEdge* chunk, path)
					{
						const road::Road* road(chunk->getRoadChunk()->getRoad());
						boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
						boost::shared_ptr<geos::geom::Geometry> geometryProjected(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(*geometry));
						double distance(0);
						if(geometry)
						{
							geos::geom::CoordinateSequence* coordinates = geometry->getCoordinates();
							distance = geos::algorithm::CGAlgorithms::length(coordinates);
							delete coordinates;
						}
						if(first)
						{
							curDistance = static_cast<int>(distance);
							arrivalTime = startDate + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curRoadPlace = &*road->getAnyRoadPlace();
							curGeom.push_back(geometryProjected.get()->clone());
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
							geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
							geometries.push_back(multiLineString->clone());
							delete multiLineString;
							curDistance = static_cast<int>(distance);
							departureTime = arrivalTime;
							arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
							curRoadPlace = &*road->getAnyRoadPlace();

							BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
							{
								delete geomToDelete;
							}
							curGeom.clear();
							curGeom.push_back(geometryProjected.get()->clone());
						}
						totalDistance += curDistance;
					}

					geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
					geometries.push_back(multiLineString->clone());

					BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
					{
						delete geomToDelete;
					}
					curGeom.clear();
					delete multiLineString;

					submapJourney->insert("arrival_date_time", arrivalTime);
					submapLeg->insert("arrival_date_time", arrivalTime);
					geos::geom::GeometryCollection* geometryCollection(CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createGeometryCollection(geometries));
					BOOST_FOREACH(geos::geom::Geometry* geomToDelete, geometries)
					{
						delete geomToDelete;
					}
					geometries.clear();
					submapJourney->insert("geometry", geometryCollection->toString());
					submapLeg->insert("geometry", geometryCollection->toString());

					// Road
					boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
					submapRoadDetails->insert("name", (string)("Trajet en covoiturage"));
					submapRoadDetails->insert("id", 0);

					boost::shared_ptr<ParametersMap> submapLegNetwork(new ParametersMap);
					submapLegNetwork->insert("name", (string)("Covoiturage"));
					submapLegNetwork->insert("id", 0);

					// Specific leg attributes
					boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
					submapRoadLegAttributes->insert("length", totalDistance);
					submapRoadLegAttributes->insert("road", submapRoadDetails);
					submapRoadLegAttributes->insert("road", submapLegNetwork);

					submapLeg->insert("car_attributes", submapRoadLegAttributes);
					submapJourney->insert("leg", submapLeg);

					delete geometryCollection;
					std::string remark("Trajet Covoiturage porte à porte, votre trajet est à chercher sur http://www.e-covoiturage.ch");
					submapJourney->insert("remark", remark);

					pm.insert("journey", submapJourney);
				}
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* car pooling door to door data processing");
			}
			else if (_useCarPooling)
			{
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before SYNTHESE car pooling door to door");

				// Classical synthese algorithm
				// TODO : Note we cannot really use AlgorithmLogger right now since not targetted for pure road journey search
				algorithm::AlgorithmLogger logger;
				graph::AccessParameters carPoolingAccessParameters(graph::USER_CAR, false, false, 1200000, boost::posix_time::hours(24), 13.889);
				road_journey_planner::RoadJourneyPlanner rjp(
					departure,
					arrival,
					startDate,
					startDate,
					endDate,
					endDate,
					1,
					carPoolingAccessParameters,
					algorithm::DEPARTURE_FIRST,
					logger,
					true
				);
				road_journey_planner::RoadJourneyPlannerResult results = rjp.run();
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE car pooling door to door");

				if(!results.getJourneys().empty())
				{
					for (road_journey_planner::RoadJourneyPlannerResult::Journeys::const_iterator it(results.getJourneys().begin()); it != results.getJourneys().end(); ++it)
					{
						boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
						boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
						submapJourney->insert("departure_date_time", it->getFirstDepartureTime());
						submapJourney->insert("arrival_date_time", it->getFirstArrivalTime());
						submapLeg->insert("departure_date_time", it->getFirstDepartureTime());
						submapLeg->insert("arrival_date_time", it->getFirstArrivalTime());

						// Departure place
						boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
						if(NULL != dynamic_cast<const road::Crossing*>(it->getOrigin()->getHub()))
						{
							_serializePlace(departure, submapDeparturePlace);
						}
						else if(NULL != dynamic_cast<const NamedPlace*>(it->getOrigin()->getHub()))
						{
							_serializePlace(dynamic_cast<const NamedPlace*>(it->getOrigin()->getHub()), submapDeparturePlace);
						}

						_serializeLatLong(it->getOrigin()->getFromVertex()->getGeometry(), submapDeparturePlace);

						// Arrival place
						boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
						if(NULL != dynamic_cast<const road::Crossing*>(it->getDestination()->getHub()))
						{
							_serializePlace(arrival, submapArrivalPlace);
						}
						else if(NULL != dynamic_cast<const NamedPlace*>(it->getDestination()->getHub()))
						{
							_serializePlace(dynamic_cast<const NamedPlace*>(it->getDestination()->getHub()), submapArrivalPlace);
						}

						_serializeLatLong(it->getDestination()->getFromVertex()->getGeometry(), submapArrivalPlace);

						submapJourney->insert("departure", submapDeparturePlace);
						submapJourney->insert("arrival", submapArrivalPlace);
						submapLeg->insert("departure", submapDeparturePlace);
						submapLeg->insert("arrival", submapArrivalPlace);

						graph::Journey::ServiceUses::const_iterator its(it->getServiceUses().begin());
						vector<boost::shared_ptr<geos::geom::Geometry> > geometriesSPtr; // To keep shared_ptr's in scope !
						vector<geos::geom::Geometry*> allGeometries;
						double total_distance(0.0);
						while(it->getServiceUses().end() != its)
						{
							const graph::Path* path = its->getService()->getPath();
							const road::RoadPath* road(dynamic_cast<const road::RoadPath*>(path));
							if (NULL != road)
							{
								const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());
								std::string roadName = _getRoadName(road);
								double dst = its->getDistance();
								graph::Journey::ServiceUses::const_iterator next = its+1;
								boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
								if(geometry.get())
								{
									boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
										*geometry
									)	);
									allGeometries.push_back(wgs84LineString.get());
									geometriesSPtr.push_back(wgs84LineString);
								}

								// Concatenate road legs on the same road and using the same transportation mean
								while(
									next != it->getServiceUses().end() &&
									dynamic_cast<const road::RoadPath*> (next->getService()->getPath())
								)
								{
									std::string nextRoadName(
										_getRoadName(
											dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()
									)	);
									if(roadName != nextRoadName)
									{
										// Road changed => do not concatenate next leg
										break;
									}
									++its;
									dst += its->getDistance();
									boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
									if(geometry.get())
									{
										boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
											*geometry
										)	);
										allGeometries.push_back(wgs84LineString.get());
										geometriesSPtr.push_back(wgs84LineString);
									}
									next = its+1;
								}
								total_distance += dst;
							}

							// Next service use
							++its;
						}

						boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
							CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
								allGeometries
						)	);
						submapJourney->insert("geometry", multiLineString->toString());
						submapLeg->insert("geometry", multiLineString->toString());

						// Road
						boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
						submapRoadDetails->insert("name", (string)("Trajet en covoiturage"));
						submapRoadDetails->insert("id", 0);

						boost::shared_ptr<ParametersMap> submapLegNetwork(new ParametersMap);
						submapLegNetwork->insert("name", (string)("Covoiturage"));
						submapLegNetwork->insert("id", 0);

						// Specific leg attributes
						boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
						submapRoadLegAttributes->insert("length", total_distance);
						submapRoadLegAttributes->insert("road", submapRoadDetails);
						submapRoadLegAttributes->insert("road", submapLegNetwork);

						submapLeg->insert("car_attributes", submapRoadLegAttributes);
						submapJourney->insert("leg", submapLeg);

						std::string remark("Trajet Covoiturage porte à porte, votre trajet est à chercher sur http://www.e-covoiturage.ch");
						submapJourney->insert("remark", remark);
						pm.insert("journey", submapJourney);
					}
				}
				Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE car pooling door to door data processing");
			}

			// Walk + Enzo location
			if (_useWalk && _useCarLocation && _aStarForWalk)
			{
				// 1st step from departure_place to Enzo Location by walk
				pt_website::PlacesListService placesListService;
				placesListService.setNumber(1);
				placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

				placesListService.setText("Lausanne Rue de Genève 102");
				road::RoadModule::ExtendedFetchPlaceResult intermediatePlace;
				intermediatePlace.placeResult = placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				);

				if(!intermediatePlace.placeResult.value)
				{
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : Enzo location address not found");
				}
				else
				{
					Place* intermediate = intermediatePlace.placeResult.value.get();
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before A* walk to Enzo location");
					// Astar algorithm
					algorithm::AStarShortestPathCalculator r(
						departure,
						intermediate,
						startDate,
						pedestrianAccessParameters
					);

					algorithm::AStarShortestPathCalculator::ResultPath walkPath(r.run());
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* walk to Enzo location");

					if(!walkPath.empty())
					{
						// 2nd step from Enso location to arrival place by car
						graph::AccessParameters enzoAccessParameters(graph::USER_CAR, false, false, 1200000, boost::posix_time::hours(24), 13.889);
						algorithm::AStarShortestPathCalculator r(
							intermediate,
							arrival,
							startDate,
							enzoAccessParameters
						);

						algorithm::AStarShortestPathCalculator::ResultPath carPath(r.run());
						Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after A* from Enzo location to arrival");

						if(!carPath.empty())
						{
							boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
							submapJourney->insert("departure_date_time", startDate);

							// Departure place
							boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
							if(dynamic_cast<const NamedPlace*>(departure))
							{
								submapDeparturePlace->insert("name", dynamic_cast<const NamedPlace*>(departure)->getFullName());
								submapDeparturePlace->insert("type", dynamic_cast<const NamedPlace*>(departure)->getFactoryKey());
								submapDeparturePlace->insert("id", dynamic_cast<const NamedPlace*>(departure)->getKey());
							}
							else
							{
								submapDeparturePlace->insert("name", dynamic_cast<const City*>(departure)->getName());
								string strCityType("City");
								submapDeparturePlace->insert("type", strCityType);
								submapDeparturePlace->insert("id", dynamic_cast<const City*>(departure)->getKey());
							}

							if(departure->getPoint().get() &&
								!departure->getPoint()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(departure->getPoint())
								)	);
								submapDeparturePlace->insert("longitude", wgs84Point->getX());
								submapDeparturePlace->insert("latitude", wgs84Point->getY());
							}

							// Arrival place
							boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
							if(dynamic_cast<const NamedPlace*>(arrival))
							{
								submapArrivalPlace->insert("name", dynamic_cast<const NamedPlace*>(arrival)->getFullName());
								submapArrivalPlace->insert("type", dynamic_cast<const NamedPlace*>(arrival)->getFactoryKey());
								submapArrivalPlace->insert("id", dynamic_cast<const NamedPlace*>(arrival)->getKey());
							}
							else
							{
								submapArrivalPlace->insert("name", dynamic_cast<const City*>(arrival)->getName());
								string strCityType("City");
								submapArrivalPlace->insert("type", strCityType);
								submapArrivalPlace->insert("id", dynamic_cast<const City*>(arrival)->getKey());
							}

							if(arrival->getPoint().get() &&
								!arrival->getPoint()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(arrival->getPoint())
								)	);
								submapArrivalPlace->insert("longitude", wgs84Point->getX());
								submapArrivalPlace->insert("latitude", wgs84Point->getY());
							}

							submapJourney->insert("departure", submapDeparturePlace);
							submapJourney->insert("arrival", submapArrivalPlace);
							int curDistance(0);
							boost::posix_time::ptime arrivalTime;
							boost::posix_time::ptime departureTime(startDate);
							bool first(true);
							vector<geos::geom::Geometry*> geometries;
							vector<geos::geom::Geometry*> curGeom;
							double speed(pedestrianAccessParameters.getApproachSpeed());
							boost::shared_ptr<ParametersMap> submapLegDeparturePlace(new ParametersMap);
							boost::shared_ptr<ParametersMap> submapLegArrivalPlace(new ParametersMap);
							road::RoadPlace* curRoadPlace(NULL);
							const road::RoadChunkEdge* lastChunk;

							BOOST_FOREACH(const road::RoadChunkEdge* chunk, walkPath)
							{
								const road::Road* road(chunk->getRoadChunk()->getRoad());
								lastChunk = chunk;
								boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
								boost::shared_ptr<geos::geom::Geometry> geometryProjected(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(*geometry));
								double distance(0);
								if(geometry)
								{
									geos::geom::CoordinateSequence* coordinates = geometry->getCoordinates();
									distance = geos::algorithm::CGAlgorithms::length(coordinates);
									delete coordinates;
								}

								if(first)
								{
									curDistance = static_cast<int>(distance);
									arrivalTime = startDate + boost::posix_time::seconds(static_cast<int>(distance / speed));
									curRoadPlace = &*road->getAnyRoadPlace();
									curGeom.push_back(geometryProjected.get()->clone());
									first = false;

									// Departure place
									if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
									{
										submapLegDeparturePlace->insert("name", (string)("Croisement"));
										submapLegDeparturePlace->insert("type", (string)("crossing"));
										submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
									}

									if(chunk->getFromVertex()->getGeometry().get() &&
										!chunk->getFromVertex()->getGeometry()->isEmpty())
									{
										boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
											*(chunk->getFromVertex()->getGeometry())
										)	);
										submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
										submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
									}
								}
								else if(curRoadPlace->getName() == road->getAnyRoadPlace()->getName())
								{
									curDistance += static_cast<int>(distance);
									arrivalTime = arrivalTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
									curGeom.push_back(geometryProjected.get()->clone());
								}
								else
								{
									geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
									geometries.push_back(multiLineString->clone());

									boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
									submapLeg->insert("departure_date_time", departureTime);
									submapLeg->insert("departure", submapLegDeparturePlace);

									// Arrival place
									if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
									{
										submapLegArrivalPlace->insert("name", (string)("Croisement"));
										submapLegArrivalPlace->insert("type", (string)("crossing"));
										submapLegArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
									}

									if(chunk->getFromVertex()->getGeometry().get() &&
										!chunk->getFromVertex()->getGeometry()->isEmpty())
									{
										boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
											*(chunk->getFromVertex()->getGeometry())
										)	);
										submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
										submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
									}
									submapLeg->insert("arrival", submapLegArrivalPlace);

									submapLegDeparturePlace.reset(new ParametersMap);
									submapLegArrivalPlace.reset(new ParametersMap);

									submapLeg->insert("arrival_date_time", arrivalTime);
									submapLeg->insert("geometry", multiLineString->toString());
									boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
									submapWalkAttributes->insert("length", curDistance);

									boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
									submapLegRoad->insert("name", curRoadPlace->getName());
									submapLegRoad->insert("id", curRoadPlace->getKey());

									submapWalkAttributes->insert("road", submapLegRoad);

									submapLeg->insert("walk_attributes", submapWalkAttributes);

									submapJourney->insert("leg", submapLeg);
									delete multiLineString;
									curDistance = static_cast<int>(distance);
									departureTime = arrivalTime;
									arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
									curRoadPlace = &*road->getAnyRoadPlace();

									BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
									{
										delete geomToDelete;
									}
									curGeom.clear();
									curGeom.push_back(geometryProjected.get()->clone());

									// New departure place
									if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
									{
										submapLegDeparturePlace->insert("name", (string)("Croisement"));
										submapLegDeparturePlace->insert("type", (string)("crossing"));
										submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
									}

									if(chunk->getFromVertex()->getGeometry().get() &&
										!chunk->getFromVertex()->getGeometry()->isEmpty())
									{
										boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
											*(chunk->getFromVertex()->getGeometry())
										)	);
										submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
										submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
									}
								}
							}

							geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
							geometries.push_back(multiLineString->clone());

							BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
							{
								delete geomToDelete;
							}
							curGeom.clear();
							boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
							submapLeg->insert("departure_date_time", departureTime);
							submapLeg->insert("departure", submapLegDeparturePlace);

							// Enzo location place
							submapLegArrivalPlace->insert("name", (string)("Enzo location"));
							submapLegArrivalPlace->insert("type", (string)("Location de voitures"));
							submapLegArrivalPlace->insert("id", 0);

							if(lastChunk->getFromVertex()->getGeometry().get() &&
								!lastChunk->getFromVertex()->getGeometry()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(lastChunk->getFromVertex()->getGeometry())
								)	);
								submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
								submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
							}
							submapLeg->insert("arrival", submapLegArrivalPlace);
							submapLeg->insert("arrival_date_time", arrivalTime);
							submapLeg->insert("geometry", multiLineString->toString());
							boost::shared_ptr<ParametersMap> submapWalkAttributes(new ParametersMap);
							submapWalkAttributes->insert("length", curDistance);

							boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
							submapLegRoad->insert("name", curRoadPlace->getName());
							submapLegRoad->insert("id", curRoadPlace->getKey());

							submapWalkAttributes->insert("road", submapLegRoad);

							submapLeg->insert("walk_attributes", submapWalkAttributes);

							submapJourney->insert("leg", submapLeg);
							delete multiLineString;

							first = true;
							speed = enzoAccessParameters.getApproachSpeed();
							submapLegDeparturePlace.reset(new ParametersMap);
							submapLegArrivalPlace.reset(new ParametersMap);
							curRoadPlace = NULL;
							departureTime = arrivalTime;
							BOOST_FOREACH(const road::RoadChunkEdge* chunk, carPath)
							{
								const road::Road* road(chunk->getRoadChunk()->getRoad());
								lastChunk = chunk;
								boost::shared_ptr<geos::geom::LineString> geometry = chunk->getRealGeometry();
								boost::shared_ptr<geos::geom::Geometry> geometryProjected(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(*geometry));
								double distance(0);
								if(geometry)
								{
									geos::geom::CoordinateSequence* coordinates = geometry->getCoordinates();
									distance = geos::algorithm::CGAlgorithms::length(coordinates);
									delete coordinates;
								}

								if(first)
								{
									curDistance = static_cast<int>(distance);
									arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
									curRoadPlace = &*road->getAnyRoadPlace();
									curGeom.push_back(geometryProjected.get()->clone());
									first = false;
									// Departure place
									submapLegDeparturePlace->insert("name", (string)("Enzo location"));
									submapLegDeparturePlace->insert("type", (string)("Location de voitures"));
									submapLegDeparturePlace->insert("id", 0);

									if(chunk->getFromVertex()->getGeometry().get() &&
										!chunk->getFromVertex()->getGeometry()->isEmpty())
									{
										boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
											*(chunk->getFromVertex()->getGeometry())
										)	);
										submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
										submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
									}
								}
								else if(curRoadPlace->getName() == road->getAnyRoadPlace()->getName())
								{
									curDistance += static_cast<int>(distance);
									arrivalTime = arrivalTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
									curGeom.push_back(geometryProjected.get()->clone());
								}
								else
								{
									geos::geom::MultiLineString* multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
									geometries.push_back(multiLineString->clone());

									boost::shared_ptr<ParametersMap> submapLeg(new ParametersMap);
									submapLeg->insert("departure_date_time", departureTime);
									submapLeg->insert("departure", submapLegDeparturePlace);

									// Arrival place
									if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
									{
										submapLegArrivalPlace->insert("name", (string)("Croisement"));
										submapLegArrivalPlace->insert("type", (string)("crossing"));
										submapLegArrivalPlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
									}
									if(chunk->getFromVertex()->getGeometry().get() &&
										!chunk->getFromVertex()->getGeometry()->isEmpty())
									{
										boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
											*(chunk->getFromVertex()->getGeometry())
										)	);
										submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
										submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
									}
									submapLeg->insert("arrival", submapLegArrivalPlace);

									submapLegDeparturePlace.reset(new ParametersMap);
									submapLegArrivalPlace.reset(new ParametersMap);

									submapLeg->insert("arrival_date_time", arrivalTime);
									submapLeg->insert("geometry", multiLineString->toString());
									boost::shared_ptr<ParametersMap> submapCarAttributes(new ParametersMap);
									submapCarAttributes->insert("length", curDistance);

									boost::shared_ptr<ParametersMap> submapLegRoad(new ParametersMap);
									submapLegRoad->insert("name", curRoadPlace->getName());
									submapLegRoad->insert("id", curRoadPlace->getKey());

									boost::shared_ptr<ParametersMap> submapLegNetwork(new ParametersMap);
									submapLegNetwork->insert("name", (string)("Enzo Location"));
									submapLegNetwork->insert("id", 0);

									submapCarAttributes->insert("road", submapLegRoad);
									submapCarAttributes->insert("network", submapLegNetwork);

									submapLeg->insert("car_attributes", submapCarAttributes);

									submapJourney->insert("leg", submapLeg);

									delete multiLineString;
									curDistance = static_cast<int>(distance);
									departureTime = arrivalTime;
									arrivalTime = departureTime + boost::posix_time::seconds(static_cast<int>(distance / speed));
									curRoadPlace = &*road->getAnyRoadPlace();

									BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
									{
										delete geomToDelete;
									}
									curGeom.clear();
									curGeom.push_back(geometryProjected.get()->clone());

									// New departure place
									if(dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub()))
									{
										submapLegDeparturePlace->insert("name", (string)("Croisement"));
										submapLegDeparturePlace->insert("type", (string)("crossing"));
										submapLegDeparturePlace->insert("id", dynamic_cast<const road::Crossing*>(chunk->getFromVertex()->getHub())->getKey());
									}

									if(chunk->getFromVertex()->getGeometry().get() &&
										!chunk->getFromVertex()->getGeometry()->isEmpty())
									{
										boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
											*(chunk->getFromVertex()->getGeometry())
										)	);
										submapLegDeparturePlace->insert("longitude", wgs84Point->getX());
										submapLegDeparturePlace->insert("latitude", wgs84Point->getY());
									}
								}
							}

							multiLineString = CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(curGeom);
							geometries.push_back(multiLineString->clone());

							BOOST_FOREACH(geos::geom::Geometry* geomToDelete, curGeom)
							{
								delete geomToDelete;
							}
							curGeom.clear();
							submapLeg.reset(new ParametersMap);
							submapLeg->insert("departure_date_time", departureTime);
							submapLeg->insert("departure", submapLegDeparturePlace);

							// Enzo location place
							if(dynamic_cast<const road::Crossing*>(lastChunk->getFromVertex()->getHub()))
							{
								submapLegArrivalPlace->insert("name", (string)("Enzo location"));
								submapLegArrivalPlace->insert("type", (string)("Location de voiture"));
								submapLegArrivalPlace->insert("id", 0);
							}
							if(lastChunk->getFromVertex()->getGeometry().get() &&
								!lastChunk->getFromVertex()->getGeometry()->isEmpty())
							{
								boost::shared_ptr<geos::geom::Point> wgs84Point(CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
									*(lastChunk->getFromVertex()->getGeometry())
								)	);
								submapLegArrivalPlace->insert("longitude", wgs84Point->getX());
								submapLegArrivalPlace->insert("latitude", wgs84Point->getY());
							}
							submapLeg->insert("arrival", submapLegArrivalPlace);

							submapLeg->insert("arrival_date_time", arrivalTime);
							submapLeg->insert("geometry", multiLineString->toString());
							boost::shared_ptr<ParametersMap> submapCarAttributes(new ParametersMap);
							submapCarAttributes->insert("length", curDistance);

							submapLegRoad.reset(new ParametersMap);
							submapLegRoad->insert("name", curRoadPlace->getName());
							submapLegRoad->insert("id", curRoadPlace->getKey());

							boost::shared_ptr<ParametersMap> submapLegNetwork(new ParametersMap);
							submapLegNetwork->insert("name", (string)("Enzo Location"));
							submapLegNetwork->insert("id", 0);

							submapCarAttributes->insert("road", submapLegRoad);
							submapCarAttributes->insert("network", submapLegNetwork);

							submapLeg->insert("car_attributes", submapCarAttributes);

							submapJourney->insert("leg", submapLeg);
							delete multiLineString;

							submapJourney->insert("arrival_date_time", arrivalTime);
							geos::geom::GeometryCollection* geometryCollection(CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createGeometryCollection(geometries));
							BOOST_FOREACH(geos::geom::Geometry* geomToDelete, geometries)
							{
								delete geomToDelete;
							}
							geometries.clear();
							submapJourney->insert("geometry", geometryCollection->toString());
							delete geometryCollection;

							std::string remark("Trajet avec Enzo location valable uniquement pour un aller/retour puisqu'il faut rendre la voiture à l'agence de location");
							submapJourney->insert("remark", remark);
							pm.insert("journey", submapJourney);
						}
					}
					else
					{
						Log::GetInstance().debug("MultimodalJourneyPlannerService::run : No walk access to Enzo location found");
					}
				}
			}
			// Walk + Enzo location
			else if (_useWalk && _useCarLocation)
			{
				// 1st step from departure_place to Enzo Location by walk
				pt_website::PlacesListService placesListService;
				placesListService.setNumber(1);
				placesListService.setCoordinatesSystem(&CoordinatesSystem::GetInstanceCoordinatesSystem());

				placesListService.setText("Lausanne Rue de Genève 102");
				road::RoadModule::ExtendedFetchPlaceResult intermediatePlace;
				intermediatePlace.placeResult = placesListService.getPlaceFromBestResult(
					placesListService.runWithoutOutput()
				);

				if(!intermediatePlace.placeResult.value)
				{
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : Enzo location address not found");
				}
				else
				{
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before SYNTHESE walk to Enzo location");
					Place* intermediate = intermediatePlace.placeResult.value.get();

					// Classical synthese algorithm
					// TODO : Note we cannot really use AlgorithmLogger right now since not targetted for pure walk journey search
					algorithm::AlgorithmLogger logger;
					road_journey_planner::RoadJourneyPlanner rjp(
						departure,
						intermediate,
						startDate,
						startDate,
						endDate,
						endDate,
						1,
						pedestrianAccessParameters,
						algorithm::DEPARTURE_FIRST,
						logger,
						true
					);
					road_journey_planner::RoadJourneyPlannerResult walkResults = rjp.run();
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE walk to Enzo location");

					if(!walkResults.getJourneys().empty())
					{
						algorithm::AlgorithmLogger logger;
						graph::AccessParameters enzoAccessParameters(graph::USER_CAR, false, false, 1200000, boost::posix_time::hours(24), 13.889);
						road_journey_planner::RoadJourneyPlanner rjp(
							intermediate,
							arrival,
							walkResults.getJourneys().begin()->getLastArrivalTime(),
							walkResults.getJourneys().begin()->getLastArrivalTime(),
							endDate,
							endDate,
							1,
							enzoAccessParameters,
							algorithm::DEPARTURE_FIRST,
							logger,
							true
						);
						road_journey_planner::RoadJourneyPlannerResult carResults = rjp.run();

						if(!carResults.getJourneys().empty())
						{
							for (road_journey_planner::RoadJourneyPlannerResult::Journeys::const_iterator it(walkResults.getJourneys().begin()); it != walkResults.getJourneys().end(); ++it)
							{
								boost::shared_ptr<ParametersMap> submapJourney(new ParametersMap);
								for (road_journey_planner::RoadJourneyPlannerResult::Journeys::const_iterator it2(carResults.getJourneys().begin()); it2 != carResults.getJourneys().end(); ++it2)
								{
									_serializeTwoJourneys(*it, *it2, departure, arrival, submapJourney);
								}
								pm.insert("journey", submapJourney);
							}
						}
					}
					Log::GetInstance().debug("MultimodalJourneyPlannerService::run : after SYNTHESE walk data processing");
				}
			}

			Log::GetInstance().debug("MultimodalJourneyPlannerService::run : before output conversion to " + boost::lexical_cast<std::string>(_outputFormat));

			if(_outputFormat == MimeTypes::XML)
			{
				pm.outputXML(
					stream,
					DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT,
					true,
					"http://schemas.open-transport.org/smile/MultimodalJourneyPlanner.xsd"
				);
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				pm.outputJSON(stream, DATA_MULTIMODAL_JOURNEY_PLANNER_RESULT);
			}

			Log::GetInstance().debug("MultimodalJourneyPlannerService::run : end");

			return pm;
		}



		bool MultimodalJourneyPlannerService::isAuthorized(
			const server::Session* session
		) const {
			return true;
		}



		std::string MultimodalJourneyPlannerService::getOutputMimeType() const
		{
			return _outputFormat;
		}



		graph::VertexAccessMap MultimodalJourneyPlannerService::_buildCrossingVAMFromBikeStationVAM(
			const graph::VertexAccessMap& bikeStationVam,
			const graph::AccessParameters& accessParameters
		) const
		{
			graph::VertexAccessMap crossingVam;

			BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& bikeStationVamElement, bikeStationVam.getMap())
			{
				const public_biking::PublicBikeStation* bikeStation = dynamic_cast<const public_biking::PublicBikeStation*>(bikeStationVamElement.first);
				const graph::VertexAccess& bikeAccess = bikeStationVamElement.second;
				graph::VertexAccessMap bikeStationCrossings;
				if(NULL == bikeStation) continue;

				geography::Place::GraphTypes graphTypes;
				graphTypes.insert(road::RoadModule::GRAPH_ID);
				bikeStation->getVertexAccessMap(bikeStationCrossings, accessParameters, graphTypes);

				BOOST_FOREACH(const graph::VertexAccessMap::VamMap::value_type& crossingVamElement, bikeStationCrossings.getMap())
				{
					const graph::VertexAccess& crossingAccess = crossingVamElement.second;
					graph::VertexAccess fullCrossingAccess(
						bikeAccess.approachTime + crossingAccess.approachTime,
						bikeAccess.approachDistance + crossingAccess.approachDistance,
						bikeAccess.approachJourney
					);
					crossingVam.insert(crossingVamElement.first, fullCrossingAccess);
				}
			}

			return crossingVam;
		}



		void MultimodalJourneyPlannerService::_serializeJourney(
			const synthese::graph::Journey& journey,
			const geography::Place* departure,
			const geography::Place* arrival,
			boost::shared_ptr<util::ParametersMap> parametersMap
		) const
		{
			parametersMap->insert("departure_date_time", journey.getFirstDepartureTime());
			parametersMap->insert("arrival_date_time", journey.getFirstArrivalTime());

			// Departure place
			boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
			if(NULL != dynamic_cast<const road::Crossing*>(journey.getOrigin()->getHub()))
			{
				_serializePlace(departure, submapDeparturePlace);
			}
			else if(NULL != dynamic_cast<const NamedPlace*>(journey.getOrigin()->getHub()))
			{
				_serializePlace(dynamic_cast<const NamedPlace*>(journey.getOrigin()->getHub()), submapDeparturePlace);
			}

			_serializeLatLong(journey.getOrigin()->getFromVertex()->getGeometry(), submapDeparturePlace);

			// Arrival place
			boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
			if(NULL != dynamic_cast<const road::Crossing*>(journey.getDestination()->getHub()))
			{
				_serializePlace(arrival, submapArrivalPlace);
			}
			else if(NULL != dynamic_cast<const NamedPlace*>(journey.getDestination()->getHub()))
			{
				_serializePlace(dynamic_cast<const NamedPlace*>(journey.getDestination()->getHub()), submapArrivalPlace);
			}

			_serializeLatLong(journey.getDestination()->getFromVertex()->getGeometry(), submapArrivalPlace);

			parametersMap->insert("departure", submapDeparturePlace);
			parametersMap->insert("arrival", submapArrivalPlace);

			graph::Journey::ServiceUses::const_iterator its(journey.getServiceUses().begin());
			vector<boost::shared_ptr<geos::geom::Geometry> > geometriesSPtr; // To keep shared_ptr's in scope !
			vector<geos::geom::Geometry*> allGeometries;
			while(journey.getServiceUses().end() != its)
			{
				const graph::Path* path = its->getService()->getPath();
				const road::RoadPath* road(dynamic_cast<const road::RoadPath*>(path));
				const pt::Junction* junction(dynamic_cast<const pt::Junction*>(path));
				const pt::JourneyPattern* ptLine(dynamic_cast<const pt::JourneyPattern*>(path));

				if (NULL != road)
				{
					// Approach leg
					boost::shared_ptr<ParametersMap> submapRoadLeg(new ParametersMap);
					submapRoadLeg->insert("departure_date_time", its->getDepartureDateTime());

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					_serializeHub(its->getRealTimeDepartureVertex()->getHub(), submapDeparturePlace);
					_serializeLatLong(its->getRealTimeDepartureVertex()->getGeometry(), submapDeparturePlace);
					submapRoadLeg->insert("departure", submapDeparturePlace);

					const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());

					std::string roadName = _getRoadName(road);
					std::size_t userClassRank = its->getUserClassRank();
					double dst = its->getDistance();
					vector<geos::geom::Geometry*> geometries;
					graph::Journey::ServiceUses::const_iterator next = its+1;
					boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
					if(geometry.get())
					{
						boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
							*geometry
						)	);
						geometries.push_back(wgs84LineString.get());
						allGeometries.push_back(wgs84LineString.get());
						geometriesSPtr.push_back(wgs84LineString);
					}

					// Concatenate road legs on the same road and using the same transportation mean
					while(
						next != journey.getServiceUses().end() &&
						dynamic_cast<const road::RoadPath*> (next->getService()->getPath())
					)
					{
						std::string nextRoadName(
							_getRoadName(
								dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()
							)
						);
						std::size_t nextUserClassRank = next->getUserClassRank();

						if((roadName != nextRoadName) || (userClassRank != nextUserClassRank))
						{
							// Road changed or transportation mean changed => do not concatenate next leg
							break;
						}

						++its;
						dst += its->getDistance();
						boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
						if(geometry.get())
						{
							boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
								*geometry
							)	);
							geometries.push_back(wgs84LineString.get());
							allGeometries.push_back(wgs84LineString.get());
							geometriesSPtr.push_back(wgs84LineString);
						}
						next = its+1;
					}

					boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
						CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
							geometries
					)	);
					submapRoadLeg->insert("arrival_date_time", its->getArrivalDateTime());
					submapRoadLeg->insert("geometry", multiLineString->toString());

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					_serializeHub(its->getRealTimeArrivalVertex()->getHub(), submapArrivalPlace);
					_serializeLatLong(its->getRealTimeArrivalVertex()->getGeometry(), submapArrivalPlace);
					submapRoadLeg->insert("arrival", submapArrivalPlace);

					// Road
					boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
					submapRoadDetails->insert("name", roadName);
					submapRoadDetails->insert("id", road->getAnyRoadPlace()->getKey());

					// Specific leg attributes
					boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
					submapRoadLegAttributes->insert("length", dst);
					submapRoadLegAttributes->insert("road", submapRoadDetails);

					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_PEDESTRIAN)
					{
						submapRoadLeg->insert("walk_attributes", submapRoadLegAttributes);
					}
					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_BIKE)
					{
						submapRoadLeg->insert("bike_attributes", submapRoadLegAttributes);
					}
					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_CAR)
					{
						submapRoadLeg->insert("car_attributes", submapRoadLegAttributes);
					}

					parametersMap->insert("leg", submapRoadLeg);
				}
				else if (NULL != junction)
				{
					// TODO (junction is a walk_leg between 2 pt_leg)
				}
				else if (NULL != ptLine)
				{
					//pt_leg
					boost::shared_ptr<ParametersMap> submapPtLeg(new ParametersMap);
					submapPtLeg->insert("departure_date_time", its->getDepartureDateTime());
					submapPtLeg->insert("arrival_date_time", its->getArrivalDateTime());

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					_serializeHub(its->getRealTimeDepartureVertex()->getHub(), submapDeparturePlace);
					_serializeLatLong(its->getRealTimeDepartureVertex()->getGeometry(), submapDeparturePlace);
					submapPtLeg->insert("departure", submapDeparturePlace);

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					_serializeHub(its->getRealTimeArrivalVertex()->getHub(), submapArrivalPlace);
					_serializeLatLong(its->getRealTimeArrivalVertex()->getGeometry(), submapArrivalPlace);
					submapPtLeg->insert("arrival", submapArrivalPlace);

					boost::shared_ptr<ParametersMap> submapPtAttributes(new ParametersMap);
					_serializePTService(its->getService(), submapPtAttributes);
					submapPtLeg->insert("pt_attributes", submapPtAttributes);

					// Geometry
					boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
					if(geometry.get())
					{
						boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
							*geometry
						)	);
						allGeometries.push_back(wgs84LineString.get());
						geometriesSPtr.push_back(wgs84LineString);

						submapPtLeg->insert("geometry", wgs84LineString->toString());
					}

					parametersMap->insert("leg", submapPtLeg);
				}

				// Next service use
				++its;
			}

			boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
				CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
					allGeometries
			)	);
			parametersMap->insert("geometry", multiLineString->toString());
		}



		void MultimodalJourneyPlannerService::_serializePlace(
			const geography::Place* place,
			boost::shared_ptr<util::ParametersMap> parametersMap
		) const
		{
			const geography::NamedPlace* namedPlace = dynamic_cast<const geography::NamedPlace*>(place);
			const geography::City* city = dynamic_cast<const geography::City*>(place);

			if(NULL != namedPlace)
			{
				parametersMap->insert("name", namedPlace->getFullName());
				parametersMap->insert("type", namedPlace->getFactoryKey());
				parametersMap->insert("id", namedPlace->getKey());
				parametersMap->insert("city", namedPlace->getCity()->getName());
			}

			else if(NULL != city)
			{
				parametersMap->insert("name", city->getName());
				parametersMap->insert("type", std::string("City"));
				parametersMap->insert("id", city->getKey());
				parametersMap->insert("city", city->getName());
			}

			else
			{
				Log::GetInstance().warn("Cannot serialize place in multimodal_journey_planner results : neither a named place nor a city");
			}
		}



		void MultimodalJourneyPlannerService::_serializeHub(
			const graph::Hub* hub,
			boost::shared_ptr<util::ParametersMap> parametersMap
		) const
		{
			const geography::NamedPlace* namedPlace = dynamic_cast<const geography::NamedPlace*>(hub);
			const road::Crossing* crossing = dynamic_cast<const road::Crossing*>(hub);

			if(NULL != namedPlace)
			{
				parametersMap->insert("name", namedPlace->getFullName());
				parametersMap->insert("type", namedPlace->getFactoryKey());
				parametersMap->insert("id", namedPlace->getKey());
				parametersMap->insert("city", namedPlace->getCity()->getName());
			}

			else if(NULL != crossing)
			{
				parametersMap->insert("name", std::string("Croisement"));
				parametersMap->insert("type", std::string("Crossing"));
				parametersMap->insert("id", crossing->getKey());
				// TODO : Can we identify which city a crossing belongs to ?
				parametersMap->insert("city", std::string(""));
			}

			else
			{
				Log::GetInstance().warn("Cannot serialize hub in multimodal_journey_planner results : neither a named place nor a crossing");
			}
		}



		void MultimodalJourneyPlannerService::_serializeLatLong(
			boost::shared_ptr<geos::geom::Point> point,
			boost::shared_ptr<util::ParametersMap> parametersMap
		) const
		{
			if(point.get() && !point->isEmpty())
			{
				boost::shared_ptr<geos::geom::Point> wgs84Point(
					CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*point)
				);
				parametersMap->insert("longitude", wgs84Point->getX());
				parametersMap->insert("latitude", wgs84Point->getY());
			}
		}



		void MultimodalJourneyPlannerService::_serializePTService(
			const graph::Service* service,
			boost::shared_ptr<util::ParametersMap> parametersMap
		) const
		{
			const pt::JourneyPattern* line(dynamic_cast<const pt::JourneyPattern*>(service->getPath()));

			boost::shared_ptr<ParametersMap> submapService(new ParametersMap);
			submapService->insert("id", service->getKey());
			submapService->insert("number", service->getServiceNumber());
			parametersMap->insert("service", submapService);

			if(NULL == line)
			{
				Log::GetInstance().warn("Cannot serialize pt service in multimodal_journey_planner results : service has no commercial line");
				return;
			}

			submapService->insert("direction", line->getDirection());

			const vehicle::RollingStock* rollingStock(line->getRollingStock());
			submapService->insert("transport_type", (NULL != rollingStock) ? rollingStock->getName() : "Unknown");

			if(NULL != line->getCommercialLine())
			{
				boost::shared_ptr<ParametersMap> submapCommercialLine(new ParametersMap);
				submapCommercialLine->insert("id", line->getCommercialLine()->getKey());
				submapCommercialLine->insert("name", line->getCommercialLine()->getName());
				submapCommercialLine->insert("short_name", line->getCommercialLine()->getShortName());
				submapCommercialLine->insert("image", line->getCommercialLine()->getImage());
				submapCommercialLine->insert("style", line->getCommercialLine()->getStyle());
				submapCommercialLine->insert("color", line->getCommercialLine()->getColor());
				parametersMap->insert("line", submapCommercialLine);

				if(NULL != line->getCommercialLine()->getNetwork())
				{
					boost::shared_ptr<ParametersMap> submapNetwork(new ParametersMap);
					submapNetwork->insert("id", line->getCommercialLine()->getNetwork()->getKey());
					submapNetwork->insert("name", line->getCommercialLine()->getNetwork()->getName());
					submapNetwork->insert("image", line->getCommercialLine()->getNetwork()->get<pt::Image>());
					parametersMap->insert("network", submapNetwork);
				}
			}
		}



		std::string MultimodalJourneyPlannerService::_getRoadName(const road::Road* road) const
		{
			std::string roadName = road->getAnyRoadPlace()->getName();

			if(roadName.empty())
			{
				switch(road->get<RoadTypeField>())
				{
					case road::ROAD_TYPE_PEDESTRIANPATH :
					case road::ROAD_TYPE_PEDESTRIANSTREET :
					{
						roadName = "Chemin Piéton";
						break;
					}

					case road::ROAD_TYPE_STEPS :
					{
						roadName = "Escaliers";
						break;
					}

					case road::ROAD_TYPE_BRIDGE :
					{
						roadName = "Pont / Passerelle";
						break;
					}

					case road::ROAD_TYPE_TUNNEL :
					{
						roadName = "Tunnel";
						break;
					}

					default :
						roadName = "Route sans nom";
				}
			}

			return roadName;
		}

		void MultimodalJourneyPlannerService::_serializeTwoJourneys(
			const synthese::graph::Journey& journey1,
			const synthese::graph::Journey& journey2,
			const geography::Place* departure,
			const geography::Place* arrival,
			boost::shared_ptr<util::ParametersMap> parametersMap
		) const
		{
			parametersMap->insert("departure_date_time", journey1.getFirstDepartureTime());
			parametersMap->insert("arrival_date_time", journey2.getFirstArrivalTime());

			// Departure place
			boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
			if(NULL != dynamic_cast<const road::Crossing*>(journey1.getOrigin()->getHub()))
			{
				_serializePlace(departure, submapDeparturePlace);
			}
			else if(NULL != dynamic_cast<const NamedPlace*>(journey1.getOrigin()->getHub()))
			{
				_serializePlace(dynamic_cast<const NamedPlace*>(journey1.getOrigin()->getHub()), submapDeparturePlace);
			}
			_serializeLatLong(journey1.getOrigin()->getFromVertex()->getGeometry(), submapDeparturePlace);

			// Arrival place
			boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
			if(NULL != dynamic_cast<const road::Crossing*>(journey2.getDestination()->getHub()))
			{
				_serializePlace(arrival, submapArrivalPlace);
			}
			else if(NULL != dynamic_cast<const NamedPlace*>(journey2.getDestination()->getHub()))
			{
				_serializePlace(dynamic_cast<const NamedPlace*>(journey2.getDestination()->getHub()), submapArrivalPlace);
			}
			_serializeLatLong(journey2.getDestination()->getFromVertex()->getGeometry(), submapArrivalPlace);

			parametersMap->insert("departure", submapDeparturePlace);
			parametersMap->insert("arrival", submapArrivalPlace);

			graph::Journey::ServiceUses::const_iterator its(journey1.getServiceUses().begin());
			vector<boost::shared_ptr<geos::geom::Geometry> > geometriesSPtr; // To keep shared_ptr's in scope !
			vector<geos::geom::Geometry*> allGeometries;
			while(journey1.getServiceUses().end() != its)
			{
				const graph::Path* path = its->getService()->getPath();
				const road::RoadPath* road(dynamic_cast<const road::RoadPath*>(path));
				const pt::Junction* junction(dynamic_cast<const pt::Junction*>(path));
				const pt::JourneyPattern* ptLine(dynamic_cast<const pt::JourneyPattern*>(path));

				if (NULL != road)
				{
					// Approach leg
					boost::shared_ptr<ParametersMap> submapRoadLeg(new ParametersMap);
					submapRoadLeg->insert("departure_date_time", its->getDepartureDateTime());

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					_serializeHub(its->getRealTimeDepartureVertex()->getHub(), submapDeparturePlace);
					_serializeLatLong(its->getRealTimeDepartureVertex()->getGeometry(), submapDeparturePlace);
					submapRoadLeg->insert("departure", submapDeparturePlace);

					const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());

					std::string roadName = _getRoadName(road);
					std::size_t userClassRank = its->getUserClassRank();
					double dst = its->getDistance();
					vector<geos::geom::Geometry*> geometries;
					graph::Journey::ServiceUses::const_iterator next = its+1;
					boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
					if(geometry.get())
					{
						boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
							*geometry
						)	);
						geometries.push_back(wgs84LineString.get());
						allGeometries.push_back(wgs84LineString.get());
						geometriesSPtr.push_back(wgs84LineString);
					}

					// Concatenate road legs on the same road and using the same transportation mean
					while(
						next != journey1.getServiceUses().end() &&
						dynamic_cast<const road::RoadPath*> (next->getService()->getPath())
					)
					{
						std::string nextRoadName(
							_getRoadName(
								dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()
						)	);
						std::size_t nextUserClassRank = next->getUserClassRank();

						if((roadName != nextRoadName) || (userClassRank != nextUserClassRank))
						{
							// Road changed or transportation mean changed => do not concatenate next leg
							break;
						}

						++its;
						dst += its->getDistance();
						boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
						if(geometry.get())
						{
							boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
								*geometry
							)	);
							geometries.push_back(wgs84LineString.get());
							allGeometries.push_back(wgs84LineString.get());
							geometriesSPtr.push_back(wgs84LineString);
						}
						next = its+1;
					}

					boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
						CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
							geometries
					)	);
					submapRoadLeg->insert("arrival_date_time", its->getArrivalDateTime());
					submapRoadLeg->insert("geometry", multiLineString->toString());

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					_serializeHub(its->getRealTimeArrivalVertex()->getHub(), submapArrivalPlace);
					_serializeLatLong(its->getRealTimeArrivalVertex()->getGeometry(), submapArrivalPlace);
					submapRoadLeg->insert("arrival", submapArrivalPlace);

					// Road
					boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
					submapRoadDetails->insert("name", roadName);
					submapRoadDetails->insert("id", road->getAnyRoadPlace()->getKey());

					// Specific leg attributes
					boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
					submapRoadLegAttributes->insert("length", dst);
					submapRoadLegAttributes->insert("road", submapRoadDetails);

					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_PEDESTRIAN)
					{
						submapRoadLeg->insert("walk_attributes", submapRoadLegAttributes);
					}
					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_BIKE)
					{
						submapRoadLeg->insert("bike_attributes", submapRoadLegAttributes);
					}
					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_CAR)
					{
						submapRoadLeg->insert("car_attributes", submapRoadLegAttributes);
					}

					parametersMap->insert("leg", submapRoadLeg);
				}
				else if (NULL != junction)
				{
					// TODO (junction is a walk_leg between 2 pt_leg)
				}
				else if (NULL != ptLine)
				{
					//pt_leg
					boost::shared_ptr<ParametersMap> submapPtLeg(new ParametersMap);
					submapPtLeg->insert("departure_date_time", its->getDepartureDateTime());
					submapPtLeg->insert("arrival_date_time", its->getArrivalDateTime());

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					_serializeHub(its->getRealTimeDepartureVertex()->getHub(), submapDeparturePlace);
					_serializeLatLong(its->getRealTimeDepartureVertex()->getGeometry(), submapDeparturePlace);
					submapPtLeg->insert("departure", submapDeparturePlace);

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					_serializeHub(its->getRealTimeArrivalVertex()->getHub(), submapArrivalPlace);
					_serializeLatLong(its->getRealTimeArrivalVertex()->getGeometry(), submapArrivalPlace);
					submapPtLeg->insert("arrival", submapArrivalPlace);

					boost::shared_ptr<ParametersMap> submapPtAttributes(new ParametersMap);
					_serializePTService(its->getService(), submapPtAttributes);
					submapPtLeg->insert("pt_attributes", submapPtAttributes);

					// Geometry
					boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
					if(geometry.get())
					{
						boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
							*geometry
						)	);
						allGeometries.push_back(wgs84LineString.get());
						geometriesSPtr.push_back(wgs84LineString);

						submapPtLeg->insert("geometry", wgs84LineString->toString());
					}

					parametersMap->insert("leg", submapPtLeg);
				}

				// Next service use
				++its;
			}

			its = journey2.getServiceUses().begin();
			while(journey2.getServiceUses().end() != its)
			{
				const graph::Path* path = its->getService()->getPath();
				const road::RoadPath* road(dynamic_cast<const road::RoadPath*>(path));
				const pt::Junction* junction(dynamic_cast<const pt::Junction*>(path));
				const pt::JourneyPattern* ptLine(dynamic_cast<const pt::JourneyPattern*>(path));

				if (NULL != road)
				{
					// Approach leg
					boost::shared_ptr<ParametersMap> submapRoadLeg(new ParametersMap);
					submapRoadLeg->insert("departure_date_time", its->getDepartureDateTime());

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					_serializeHub(its->getRealTimeDepartureVertex()->getHub(), submapDeparturePlace);
					_serializeLatLong(its->getRealTimeDepartureVertex()->getGeometry(), submapDeparturePlace);
					submapRoadLeg->insert("departure", submapDeparturePlace);

					const road::Road* road(dynamic_cast<const road::RoadPath*>(its->getService()->getPath())->getRoad());

					std::string roadName = _getRoadName(road);
					std::size_t userClassRank = its->getUserClassRank();
					double dst = its->getDistance();
					vector<geos::geom::Geometry*> geometries;
					graph::Journey::ServiceUses::const_iterator next = its+1;
					boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
					if(geometry.get())
					{
						boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
							*geometry
						)	);
						geometries.push_back(wgs84LineString.get());
						allGeometries.push_back(wgs84LineString.get());
						geometriesSPtr.push_back(wgs84LineString);
					}

					// Concatenate road legs on the same road and using the same transportation mean
					while(
						next != journey2.getServiceUses().end() &&
						dynamic_cast<const road::RoadPath*> (next->getService()->getPath())
					)
					{
						std::string nextRoadName(
							_getRoadName(
								dynamic_cast<const road::RoadPath*>(next->getService()->getPath())->getRoad()
						)	);
						std::size_t nextUserClassRank = next->getUserClassRank();

						if((roadName != nextRoadName) || (userClassRank != nextUserClassRank))
						{
							// Road changed or transportation mean changed => do not concatenate next leg
							break;
						}

						++its;
						dst += its->getDistance();
						boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
						if(geometry.get())
						{
							boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
								*geometry
							)	);
							geometries.push_back(wgs84LineString.get());
							allGeometries.push_back(wgs84LineString.get());
							geometriesSPtr.push_back(wgs84LineString);
						}
						next = its+1;
					}

					boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
						CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
							geometries
					)	);
					submapRoadLeg->insert("arrival_date_time", its->getArrivalDateTime());
					submapRoadLeg->insert("geometry", multiLineString->toString());

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					_serializeHub(its->getRealTimeArrivalVertex()->getHub(), submapArrivalPlace);
					_serializeLatLong(its->getRealTimeArrivalVertex()->getGeometry(), submapArrivalPlace);
					submapRoadLeg->insert("arrival", submapArrivalPlace);

					// Road
					boost::shared_ptr<ParametersMap> submapRoadDetails(new ParametersMap);
					submapRoadDetails->insert("name", roadName);
					submapRoadDetails->insert("id", road->getAnyRoadPlace()->getKey());

					// Specific leg attributes
					boost::shared_ptr<ParametersMap> submapRoadLegAttributes(new ParametersMap);
					submapRoadLegAttributes->insert("length", dst);
					submapRoadLegAttributes->insert("road", submapRoadDetails);

					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_PEDESTRIAN)
					{
						submapRoadLeg->insert("walk_attributes", submapRoadLegAttributes);
					}
					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_BIKE)
					{
						submapRoadLeg->insert("bike_attributes", submapRoadLegAttributes);
					}
					if(graph::USER_CLASS_CODE_OFFSET + its->getUserClassRank() == graph::USER_CAR)
					{
						submapRoadLeg->insert("car_attributes", submapRoadLegAttributes);
					}

					parametersMap->insert("leg", submapRoadLeg);
				}
				else if (NULL != junction)
				{
					// TODO (junction is a walk_leg between 2 pt_leg)
				}
				else if (NULL != ptLine)
				{
					//pt_leg
					boost::shared_ptr<ParametersMap> submapPtLeg(new ParametersMap);
					submapPtLeg->insert("departure_date_time", its->getDepartureDateTime());
					submapPtLeg->insert("arrival_date_time", its->getArrivalDateTime());

					// Departure place
					boost::shared_ptr<ParametersMap> submapDeparturePlace(new ParametersMap);
					_serializeHub(its->getRealTimeDepartureVertex()->getHub(), submapDeparturePlace);
					_serializeLatLong(its->getRealTimeDepartureVertex()->getGeometry(), submapDeparturePlace);
					submapPtLeg->insert("departure", submapDeparturePlace);

					// Arrival place
					boost::shared_ptr<ParametersMap> submapArrivalPlace(new ParametersMap);
					_serializeHub(its->getRealTimeArrivalVertex()->getHub(), submapArrivalPlace);
					_serializeLatLong(its->getRealTimeArrivalVertex()->getGeometry(), submapArrivalPlace);
					submapPtLeg->insert("arrival", submapArrivalPlace);

					boost::shared_ptr<ParametersMap> submapPtAttributes(new ParametersMap);
					_serializePTService(its->getService(), submapPtAttributes);
					submapPtLeg->insert("pt_attributes", submapPtAttributes);

					// Geometry
					boost::shared_ptr<geos::geom::LineString> geometry(its->getGeometry());
					if(geometry.get())
					{
						boost::shared_ptr<geos::geom::Geometry> wgs84LineString(CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
							*geometry
						)	);
						allGeometries.push_back(wgs84LineString.get());
						geometriesSPtr.push_back(wgs84LineString);

						submapPtLeg->insert("geometry", wgs84LineString->toString());
					}

					parametersMap->insert("leg", submapPtLeg);
				}

				// Next service use
				++its;
			}

			boost::shared_ptr<geos::geom::MultiLineString> multiLineString(
				CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory().createMultiLineString(
					allGeometries
			)	);
			parametersMap->insert("geometry", multiLineString->toString());
		}

}	}
