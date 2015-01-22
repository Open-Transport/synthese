
/** PTTimeSlotRoutePlanner class implementation.
	@file PTTimeSlotRoutePlanner.cpp

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

#include "PTTimeSlotRoutePlanner.h"

#include "AlgorithmLogger.hpp"
#include "FreeDRTArea.hpp"
#include "Hub.h"
#include "IntegralSearcher.h"
#include "JourneysResult.h"
#include "Log.h"
#include "NamedPlace.h"
#include "Place.h"
#include "PTModule.h"
#include "RoadModule.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "VAMConverter.hpp"
#include "VertexAccessMap.h"
#include "RoadJourneyPlanner.h"
#include "RoadJourneyPlannerResult.h"
#include "Journey.h"
#include "ServicePointer.h"
#include "Service.h"
#include <geos/geom/Point.h>

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace algorithm;
	using namespace road;
	using namespace graph;
	using namespace geography;
	using namespace algorithm;
	using namespace util;
	using namespace pt;
    using namespace road_journey_planner;

	namespace pt_journey_planner
	{
        PTTimeSlotRoutePlanner::PTTimeSlotRoutePlanner(
            const geography::Place* origin,
            const geography::Place* destination,
            const boost::posix_time::ptime& lowerDepartureTime,
            const boost::posix_time::ptime& higherDepartureTime,
            const boost::posix_time::ptime& lowerArrivalTime,
            const boost::posix_time::ptime& higherArrivalTime,
            const boost::optional<std::size_t>	maxSolutionsNumber,
            const graph::AccessParameters		accessParameters,
            const algorithm::PlanningOrder		planningOrder,
            bool								ignoreReservation,
            const algorithm::AlgorithmLogger& logger,
            boost::optional<boost::posix_time::time_duration> maxTransferDuration,
            boost::optional<double> minMaxDurationRatioFilter,
            bool enableTheoretical,
            bool enableRealTime,
            graph::UseRule::ReservationDelayType reservationRulesDelayType
        ):  TimeSlotRoutePlanner(
                origin->getVertexAccessMap(
                    accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
                ),
                destination->getVertexAccessMap(
                    accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
                ),
                lowerDepartureTime, higherDepartureTime,
                lowerArrivalTime, higherArrivalTime,
                PTModule::GRAPH_ID,
                PTModule::GRAPH_ID,
                optional<posix_time::time_duration>(),
                maxSolutionsNumber,
                accessParameters,
                planningOrder,
                100,
                ignoreReservation,
                logger,
                maxTransferDuration,
                minMaxDurationRatioFilter,
                enableTheoretical,
                enableRealTime,
                reservationRulesDelayType
            ),
            _departurePlace(origin),
            _arrivalPlace(destination),
            _departureParking(NULL),
            _arrivalParking(NULL),
            _startWithCar(false),
            _endWithCar(false)
        {
        }

		PTTimeSlotRoutePlanner::PTTimeSlotRoutePlanner(
			const geography::Place* origin,
			const geography::Place* destination,
            const geography::Place* originParking,
            const geography::Place* destinationParking,
            bool startWithCar,
            bool endWithCar,
			const ptime& lowerDepartureTime,
			const ptime& higherDepartureTime,
			const ptime& lowerArrivalTime,
			const ptime& higherArrivalTime,
			const boost::optional<std::size_t> maxSolutionsNumber,
			const graph::AccessParameters accessParameters,
			const PlanningOrder planningOrder,
			bool ignoreReservation,
			const AlgorithmLogger& logger,
			boost::optional<boost::posix_time::time_duration> maxTransferDuration,
			boost::optional<double> minMaxDurationRatioFilter,
			bool enableTheoretical,
			bool enableRealTime,
			UseRule::ReservationDelayType reservationRulesDelayType
		):	TimeSlotRoutePlanner(
				origin->getVertexAccessMap(
					accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				destination->getVertexAccessMap(
					accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
				),
				lowerDepartureTime, higherDepartureTime,
				lowerArrivalTime, higherArrivalTime,
				PTModule::GRAPH_ID,
				PTModule::GRAPH_ID,
				optional<posix_time::time_duration>(),
				maxSolutionsNumber,
				accessParameters,
				planningOrder,
				100,
				ignoreReservation,
				logger,
				maxTransferDuration,
				minMaxDurationRatioFilter,
				enableTheoretical,
				enableRealTime,
				reservationRulesDelayType
			),
			_departurePlace(origin),
            _arrivalPlace(destination),
            _departureParking(originParking),
            _arrivalParking(destinationParking),
            _startWithCar(startWithCar),
            _endWithCar(endWithCar)
		{
		}


		PTRoutePlannerResult PTTimeSlotRoutePlanner::run() const
		{
			TimeSlotRoutePlanner::Result result;
			_logger.openTimeSlotJourneyPlannerLog();

			// Check if departure and arrival VAMs has contains at least one vertex
			if(_originVam.getMap().empty() ||
				_destinationVam.getMap().empty()
			){
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, false, result);
			}

			// Check if the departure and arrival places are the same
			if(_originVam.intersercts(_destinationVam))
			{
				return PTRoutePlannerResult(_departurePlace, _arrivalPlace, true, result);
			}

			// Search stops around the departure and arrival places using the road network
			// FIXME: Need to handle approcahSpeed = 0 in IntegralSearcher himself
			VertexAccessMap ovam, dvam;
			if(_accessParameters.getApproachSpeed() != 0)
			{
				VAMConverter extenderToPhysicalStops(
					_accessParameters,
					_logger,
					PTModule::GRAPH_ID,
					RoadModule::GRAPH_ID,
					getLowestDepartureTime(),
					getHighestDepartureTime(),
					getLowestArrivalTime(),
					getHighestArrivalTime(),
					_departurePlace,
					_arrivalPlace
				);
				ovam = extenderToPhysicalStops.run(
					_originVam,
					_destinationVam,
					DEPARTURE_TO_ARRIVAL
				);
				dvam = extenderToPhysicalStops.run(
					_destinationVam,
					_originVam,
					ARRIVAL_TO_DEPARTURE
				);
			}
			else
			{
				// FIXME: Need to exclude Roads part of VAM
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, _originVam.getMap())
				{
					const Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PTModule::GRAPH_ID)
					{
						ovam.insert(vertex, itps.second);
					}
				}
				BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, _destinationVam.getMap())
				{
					const Vertex* vertex(itps.first);
					if(vertex->getGraphType() == PTModule::GRAPH_ID)
					{
						dvam.insert(vertex, itps.second);
					}
				}
			}

			// Log the vams
			_logger.logTimeSlotJourneyPlannerApproachMap(true, ovam);
			_logger.logTimeSlotJourneyPlannerApproachMap(false, dvam);

			// Handle of the case of possible full road approach
			if(	ovam.intersercts(dvam)
			){
				Journey resultJourney(ovam.getBestIntersection(dvam));
				ptime departureTime(resultJourney.getFirstDepartureTime());
				if(departureTime.time_of_day().seconds())
				{
					resultJourney.shift(seconds(60 - departureTime.time_of_day().seconds()));
				}
				resultJourney.shift(
					getLowestDepartureTime() - resultJourney.getFirstDepartureTime()
				);
				resultJourney.forceContinuousServiceRange(getHighestArrivalTime() - getLowestDepartureTime());

				result.push_back(resultJourney);
			}

			// Free DRT approach
			{
				_extendByFreeDRT(ovam, dvam, DEPARTURE_TO_ARRIVAL);
				_extendByFreeDRT(dvam, ovam, ARRIVAL_TO_DEPARTURE);
			}


			if(result.empty())
			{
                // no car usage, perform a standard PT journey planning
                if((false == _startWithCar) && (false == _endWithCar))
                {
                    TimeSlotRoutePlanner r(
                        ovam,
                        dvam,
                        getLowestDepartureTime(),
                        getHighestDepartureTime(),
                        getLowestArrivalTime(),
                        getHighestArrivalTime(),
                        _whatToSearch,
                        _graphToUse,
                        _maxDuration,
                        _maxSolutionsNumber,
                        _accessParameters,
                        _planningOrder,
                        70, // 252 km/h TODO take it configurable
                        _ignoreReservation,
                        _logger,
                        _maxTransferDuration,
                        _minMaxDurationRatioFilter,
                        _enableTheoretical,
                        _enableRealTime,
                        _reservationRulesDelayType
                    );
                    return PTRoutePlannerResult(
                        _departurePlace,
                        _arrivalPlace,
                        false,
                        r.run()
                    );
                }
                else
                {
                    // mixed mode journey (car + public transportation)
                    return _computeCarPTJourney();
                }
			}
			else
			{
				TimeSlotRoutePlanner r(
					ovam,
					dvam,
					result.front(),
					_whatToSearch,
					_graphToUse,
					_maxDuration,
					_maxSolutionsNumber,
					_accessParameters,
					_planningOrder,
					100,
					_ignoreReservation,
					_logger,
					_maxTransferDuration,
					_minMaxDurationRatioFilter,
					_enableTheoretical,
					_enableRealTime,
					_reservationRulesDelayType
				);
				return PTRoutePlannerResult(
					_departurePlace,
					_arrivalPlace,
					false,
					r.run()
				);
			}
		}


		void PTTimeSlotRoutePlanner::_extendByFreeDRT(
			VertexAccessMap& vam,
			const VertexAccessMap& destinationVam,
			PlanningPhase direction
		) const	{
#if 0
			// TODO Work in progress...
			
			// Loop on each stop
			BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, vam.getMap())
			{
				// Select stop areas
				const StopPoint* stopPoint(dynamic_cast<const StopPoint*>(itps.first));
				if(!stopPoint)
				{
					continue;
				}

				// Free DRTs loop
				StopArea::FreeDRTs freeDRTs(stopPoint->getConnectionPlace()->getFreeDRTs());
				BOOST_FOREACH(const StopArea::FreeDRTs::value_type& freeDRT, freeDRTs)
				{
					// Select

				}
			}
#endif
		}


        PTRoutePlannerResult PTTimeSlotRoutePlanner::_computeCarPTJourney() const {
            // OVE!!! : temporary change for debug
            /*
            if(true == _startWithCar)
            {
                if(NULL != _departureParking)
                {
                    return _computeCarPTJourneyWithChosenParking();
                }

                else
                {
                    return _computeCarPTJourneyWithBestParking();
                }
            }

            if(true == _endWithCar)
            {
                if(NULL != _arrivalParking)
                {
                    return _computeCarPTJourneyWithChosenParking();
                }

                else
                {
                    return _computeCarPTJourneyWithBestParking();
                }
            }
            */
            return _computeCarPTJourneyWithBestParking();
        }


        PTRoutePlannerResult PTTimeSlotRoutePlanner::_computeCarPTJourneyWithChosenParking() const {
            // Arbitrary delay modelling the transfer time between the relay park and its stop area (in minutes)
            // for better accuracy it should be defined per StopArea in t007_connection_places
            long PARKING_TIME = 5;

            ptime ptLowestDepartureTime  = getLowestDepartureTime();
            ptime ptHighestDepartureTime = getHighestDepartureTime();
            ptime ptLowestArrivalTime    = getLowestArrivalTime();
            ptime ptHighestArrivalTime   = getHighestArrivalTime();

            const Place* ptDeparturePlace = _departurePlace;
            const Place* ptArrivalPlace   = _arrivalPlace;

            TimeSlotRoutePlanner::Result ptJourneys;
            Journey carStartJourney;
            Journey carEndJourney;

            // route planning
            AccessParameters carAccessParams(
                USER_CAR,
                false, false, 30000, posix_time::hours(5), 11.111, // max 30km at ~= 40km/h
                1000
            );


            // 1) check if the first part of the journey shall use the car
            if(NULL != _departureParking)
            {
                // compute journey from departure place to departure parking, using car
                RoadJourneyPlanner rjp(
                    _departurePlace,
                    _departureParking,
                    getLowestDepartureTime(),
                    getHighestDepartureTime(),
                    getLowestArrivalTime(),
                    getHighestArrivalTime(),
                    1,
                    carAccessParams,
                    _planningOrder,
                    _logger
                );

                RoadJourneyPlannerResult results = rjp.run();

                if(!results.getJourneys().empty())
                {
                    // DEBUG : print the journey from departure place to departure parking
                    _printJourneys(results.getJourneys());

                    carStartJourney = results.getJourneys()[0];

                    // compute the duration of the car journey and add parking time
                    time_duration duration = carStartJourney.getDuration();
                    duration = duration + minutes(PARKING_TIME);

                    // add this duration to departure times
                    ptLowestDepartureTime += duration;
                    ptHighestDepartureTime += duration;

                    // for the PT journey the departure place is now the parking
                    ptDeparturePlace = _departureParking;
                }

                else
                {
                    // no route from departure to parking
                    return PTRoutePlannerResult(
                        _departurePlace,
                        _arrivalPlace,
                        false,
                        ptJourneys
                    );
                }
            }

            // 2) check if the last part of the journey shall use the car
            if(NULL != _arrivalParking)
            {
                // compute journey from arrival parking to arrival place, using car
                RoadJourneyPlanner rjp(
                    _arrivalParking,
                    _arrivalPlace,
                    getLowestDepartureTime(),
                    getHighestDepartureTime(),
                    getLowestArrivalTime(),
                    getHighestArrivalTime(),
                    1,
                    carAccessParams,
                    _planningOrder,
                    _logger
                );

                RoadJourneyPlannerResult results = rjp.run();

                if(!results.getJourneys().empty())
                {
                    // compute journey from parking to arrival
                    _printJourneys(results.getJourneys());

                    carEndJourney = results.getJourneys()[0];
                    time_duration duration = carEndJourney.getDuration();
                    duration = duration + minutes(PARKING_TIME);
                    ptLowestArrivalTime -= duration;
                    ptHighestArrivalTime -= duration;

                    ptArrivalPlace = _arrivalParking;
                }

                else
                {
                    // no route from parking to arrival
                    return PTRoutePlannerResult(
                        _departurePlace,
                        _arrivalPlace,
                        false,
                        ptJourneys
                    );
                }
            }

            // 3) compute the vertex access maps for the public transportation departure and arrival places
            VertexAccessMap ptDepartureVam = ptDeparturePlace->getVertexAccessMap(
                        _accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
            );
            VertexAccessMap ptArrivalVam = ptArrivalPlace->getVertexAccessMap(
                        _accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
            );

            VertexAccessMap ptDepartureExtVam, ptArrivalExtVam;
            if(_accessParameters.getApproachSpeed() != 0)
            {
                VAMConverter extenderToPhysicalStops(
                    _accessParameters,
                    _logger,
                    PTModule::GRAPH_ID,
                    RoadModule::GRAPH_ID,
                    ptLowestDepartureTime,
                    ptHighestDepartureTime,
                    ptLowestArrivalTime,
                    ptHighestArrivalTime,
                    ptDeparturePlace,
                    ptArrivalPlace
                );
                ptDepartureExtVam = extenderToPhysicalStops.run(
                    ptDepartureVam,
                    ptArrivalVam,
                    DEPARTURE_TO_ARRIVAL
                );
                ptArrivalExtVam = extenderToPhysicalStops.run(
                    ptArrivalVam,
                    ptDepartureVam,
                    ARRIVAL_TO_DEPARTURE
                );
            }
            else
            {
                // FIXME: Need to exclude Roads part of VAM
                BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, ptDepartureVam.getMap())
                {
                    const Vertex* vertex(itps.first);
                    if(vertex->getGraphType() == PTModule::GRAPH_ID)
                    {
                        ptDepartureExtVam.insert(vertex, itps.second);
                    }
                }
                BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, ptArrivalVam.getMap())
                {
                    const Vertex* vertex(itps.first);
                    if(vertex->getGraphType() == PTModule::GRAPH_ID)
                    {
                        ptArrivalExtVam.insert(vertex, itps.second);
                    }
                }
            }

            // 4) compute the part of the journey using public transportation
            TimeSlotRoutePlanner r(
                ptDepartureExtVam,
                ptArrivalExtVam,
                ptLowestDepartureTime,
                ptHighestDepartureTime,
                ptLowestArrivalTime,
                ptHighestArrivalTime,
                _whatToSearch,
                _graphToUse,
                _maxDuration,
                _maxSolutionsNumber,
                _accessParameters,
                _planningOrder,
                70, // 252 km/h TODO take it configurable
                _ignoreReservation,
                _logger,
                _maxTransferDuration,
                _minMaxDurationRatioFilter,
                _enableTheoretical,
                _enableRealTime,
                _reservationRulesDelayType
            );

            ptJourneys = r.run();

            // foreach public transportation journey, concatenate the car journeys
            BOOST_FOREACH(Journey& ptJourney, ptJourneys)
            {
                if(NULL != _departureParking)
                {
                    // compute the duration between the arrival of the car journey and the PT departure
                    ptime ptDepartureTime = ptJourney.getFirstDepartureTime();
                    ptime parkingArrivalTime = carStartJourney.getFirstArrivalTime();
                    time_duration waitingTime = time_period(parkingArrivalTime, ptDepartureTime).length();
                    // subtract an arbitrary amount of time for parking
                    waitingTime -= minutes(PARKING_TIME);

                    if(!waitingTime.is_not_a_date_time() && !waitingTime.is_negative())
                    {
                        // if the duration is not positive, shift the departure of the car journey to reduce the waiting time
                        // (negative duration should not happen because of the time constraints of the PT journey)
                        Journey shiftedCarJourney = carStartJourney;
                        shiftedCarJourney.shift(waitingTime);
                        ptJourney.prepend(shiftedCarJourney);
                    }

                    else
                    {
                        // should not happen, left for temporary debug
                        std::cout << "/!\\ Bad PT service : departure < parking arrival" << std::endl;
                    }
                }

                if(NULL != _arrivalParking)
                {
                    // compute the duration between PT journey arrival and departure from parking
                    ptime ptArrivalTime = ptJourney.getFirstArrivalTime();
                    ptime parkingDepartureTime = carEndJourney.getFirstDepartureTime();
                    time_duration waitingTime = time_period(ptArrivalTime, parkingDepartureTime).length();
                    // subtract an arbitrary amount of time for parking
                    waitingTime -= minutes(PARKING_TIME);

                    if(!waitingTime.is_not_a_date_time())
                    {
                        // if the duration is not null, shift the departure of the car journey
                        Journey shiftedCarJourney = carEndJourney;
                        shiftedCarJourney.shift(waitingTime.invert_sign());
                        ptJourney.append(shiftedCarJourney);
                    }
                }
            }

            //_printJourneys(ptJourneys);

            return PTRoutePlannerResult(
                _departurePlace,
                _arrivalPlace,
                false,
                ptJourneys
            );
        }


        PTRoutePlannerResult PTTimeSlotRoutePlanner::_computeCarPTJourneyWithBestParking() const {
            // Arbitrary delay modelling the transfer time between the relay park and its stop area (in minutes)
            // for better accuracy it should be defined per StopArea in t007_connection_places
            long PARKING_TIME = 5;

            ptime ptLowestDepartureTime  = getLowestDepartureTime();
            ptime ptHighestDepartureTime = getHighestDepartureTime();
            ptime ptLowestArrivalTime    = getLowestArrivalTime();
            ptime ptHighestArrivalTime   = getHighestArrivalTime();

            const synthese::geography::NamedPlace* const namedDeparturePlace =
                dynamic_cast<const synthese::geography::NamedPlace* const>(_departurePlace);
            const synthese::geography::NamedPlace* const namedArrivalPlace =
                dynamic_cast<const synthese::geography::NamedPlace* const>(_arrivalPlace);

            TimeSlotRoutePlanner::Result ptJourneys;

            // parameters for car approach
            AccessParameters carAccessParams(
                USER_CAR,
                false, false, 30000, posix_time::hours(5), 11.111, // max 30km at ~= 40km/h
                1000
            );

            // consider only the physical stops bound to the stop areas equipped with relay parks
            geography::Place::GraphTypes whatToSearch;
            whatToSearch.insert(PTModule::GRAPH_ID);

            // 1) check if the first part of the journey shall use the car
            if(true == _startWithCar)
            {
                SortableStopAreaSet relayParks;
                VertexAccessMap relayParksMap;

                // gather all the stop areas equipped with relay parks, located within 15km of the departure place
                _findRelayParks(_departurePlace, 10000, relayParks);

                BOOST_FOREACH(const SortableStopAreaSet::value_type& relayPark, relayParks)
                {
                    // OVE!!! : debug
                    std::cout << "+++ Parking " << relayPark.getStopArea()->getName() << " matches (distance="
                              << relayPark.getDistanceToCenter() << ")" << std::endl;
                }

                BOOST_FOREACH(const SortableStopAreaSet::value_type& relayPark, relayParks)
                {
                    // compute journey from departure place to relay park, using car
                    RoadJourneyPlanner rjp(
                        _departurePlace,
                        relayPark.getStopArea(),
                        getLowestDepartureTime(),
                        getHighestDepartureTime(),
                        getLowestArrivalTime(),
                        getHighestArrivalTime(),
                        1,
                        carAccessParams,
                        _planningOrder,
                        _logger
                    );

                    RoadJourneyPlannerResult roadResults = rjp.run();

                    if(!roadResults.getJourneys().empty())
                    {
                        Journey carStartJourney = roadResults.getJourneys()[0];

                        // DEBUG : print the journey from departure place to departure parking
                        _printJourneys(roadResults.getJourneys());

                        // compute the duration of the car journey and add parking time
                        time_duration duration = carStartJourney.getDuration();
                        duration = duration + minutes(PARKING_TIME);

                        // for each physical stop of this stop area, add an entry into the VAM with the car journey, its distance and duration
                        BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stopPoint, relayPark.getStopArea()->getPhysicalStops())
                        {
                            VertexAccess vertexAccess(duration, carStartJourney.getDistance(), carStartJourney);
                            relayParksMap.insert(stopPoint.second, vertexAccess);
                        }

                        // OVE!!! : debug
                        std::cout << "Parking " << relayPark.getStopArea()->getName() << " can be reached from "
                                  << (namedDeparturePlace ? namedDeparturePlace->getName() : "NULL") << " : distance="
                                  << carStartJourney.getDistance() << ", duration=" << duration << std::endl;
                    }

                    else
                    {
                        // OVE!!! : debug
                        std::cerr << "!!! no route from " << (namedDeparturePlace ? namedDeparturePlace->getName() : "NULL") << " to " << relayPark.getStopArea()->getName() << std::endl;
                    }
                }

                // compute the VAM of the arrival place
                VertexAccessMap ptArrivalVam = _arrivalPlace->getVertexAccessMap(
                            _accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
                );
                VertexAccessMap ptArrivalExtVam;

                if(_accessParameters.getApproachSpeed() != 0)
                {
                    // OVE!!! : WTF???
                    VAMConverter extenderToPhysicalStops(
                        _accessParameters,
                        _logger,
                        PTModule::GRAPH_ID,
                        RoadModule::GRAPH_ID,
                        ptLowestDepartureTime,
                        ptHighestDepartureTime,
                        ptLowestArrivalTime,
                        ptHighestArrivalTime,
                        NULL,
                        _arrivalPlace
                    );
                    ptArrivalExtVam = extenderToPhysicalStops.run(
                        ptArrivalVam,
                        relayParksMap,
                        ARRIVAL_TO_DEPARTURE
                    );
                }
                else
                {
                    BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, ptArrivalVam.getMap())
                    {
                        const Vertex* vertex(itps.first);
                        if(vertex->getGraphType() == PTModule::GRAPH_ID)
                        {
                            ptArrivalExtVam.insert(vertex, itps.second);
                        }
                    }
                }

                // perform a PT journey planning from the set of the relay parks to the arrival place
                TimeSlotRoutePlanner r(
                    relayParksMap,
                    ptArrivalExtVam,
                    ptLowestDepartureTime,
                    ptHighestDepartureTime,
                    ptLowestArrivalTime,
                    ptHighestArrivalTime,
                    _whatToSearch,
                    _graphToUse,
                    _maxDuration,
                    _maxSolutionsNumber,
                    _accessParameters,
                    _planningOrder,
                    70, // 252 km/h TODO take it configurable
                    _ignoreReservation,
                    _logger,
                    _maxTransferDuration,
                    _minMaxDurationRatioFilter,
                    _enableTheoretical,
                    _enableRealTime,
                    _reservationRulesDelayType
                );

                ptJourneys = r.run();

                if(0 < ptJourneys.size())
                {
                    // OVE!!! : debug
                    const synthese::geography::NamedPlace* const namedPlace =
                        dynamic_cast<const synthese::geography::NamedPlace* const>(_arrivalPlace);
                    std::cout << "Found " << ptJourneys.size() << " journey(s) to " << (namedPlace ? namedPlace->getName() : "NULL") << std::endl;
                }

                else
                {
                    // OVE!!! : debug
                    std::cout << "Found no solution" << std::endl;
                }
            }

            // 2) check if the last part of the journey shall use the car
            else if(true == _endWithCar)
            {
                // search the arrival parking
                // OVE!!!
                SortableStopAreaSet relayParks;
                VertexAccessMap relayParksMap;

                // gather all the stop areas equipped with relay parks, located within 15km of the arrival place
                _findRelayParks(_arrivalPlace, 10000, relayParks);

                BOOST_FOREACH(const SortableStopAreaSet::value_type& relayPark, relayParks)
                {
                    // OVE!!! : debug
                    std::cout << "+++ Parking " << relayPark.getStopArea()->getName() << " matches (distance="
                              << relayPark.getDistanceToCenter() << ")" << std::endl;
                }

                BOOST_FOREACH(const SortableStopAreaSet::value_type& relayPark, relayParks)
                {
                    // compute journey from departure place to relay park, using car
                    RoadJourneyPlanner rjp(
                        relayPark.getStopArea(),
                        _arrivalPlace,
                        getLowestDepartureTime(),
                        getHighestDepartureTime(),
                        getLowestArrivalTime(),
                        getHighestArrivalTime(),
                        1,
                        carAccessParams,
                        _planningOrder,
                        _logger
                    );

                    RoadJourneyPlannerResult roadResults = rjp.run();

                    if(!roadResults.getJourneys().empty())
                    {
                        Journey carJourney = roadResults.getJourneys()[0];

                        // DEBUG : print the journey from departure place to departure parking
                        _printJourneys(roadResults.getJourneys());

                        // compute the duration of the car journey and add parking time
                        time_duration duration = carJourney.getDuration();
                        duration = duration + minutes(PARKING_TIME);

                        // for each physical stop of this stop area, add an entry into the VAM with the car journey, its distance and duration
                        BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stopPoint, relayPark.getStopArea()->getPhysicalStops())
                        {
                            // OVE!!! : does it really work ?
                            VertexAccess vertexAccess(duration, carJourney.getDistance(), carJourney);
                            relayParksMap.insert(stopPoint.second, vertexAccess);
                        }

                        // OVE!!! : debug
                        std::cout << "Parking " << relayPark.getStopArea()->getName() << " can reached to "
                                  << (namedArrivalPlace ? namedArrivalPlace->getName() : "NULL") << " : distance="
                                  << carJourney.getDistance() << ", duration=" << duration << std::endl;
                    }

                    else
                    {
                        // OVE!!! : debug
                        std::cerr << "!!! no route to " << (namedArrivalPlace ? namedArrivalPlace->getName() : "NULL") << " from " << relayPark.getStopArea()->getName() << std::endl;
                    }
                }

                // compute the VAM of the arrival place
                VertexAccessMap ptDepartureVam = _departurePlace->getVertexAccessMap(
                            _accessParameters, PTModule::GRAPH_ID, RoadModule::GRAPH_ID, 0
                );
                VertexAccessMap ptDepartureExtVam;

                if(_accessParameters.getApproachSpeed() != 0)
                {
                    // OVE!!! : WTF???
                    VAMConverter extenderToPhysicalStops(
                        _accessParameters,
                        _logger,
                        PTModule::GRAPH_ID,
                        RoadModule::GRAPH_ID,
                        ptLowestDepartureTime,
                        ptHighestDepartureTime,
                        ptLowestArrivalTime,
                        ptHighestArrivalTime,
                        _departurePlace,
                        NULL
                    );
                    ptDepartureExtVam = extenderToPhysicalStops.run(
                        ptDepartureVam,
                        relayParksMap,
                        DEPARTURE_TO_ARRIVAL
                    );
                }
                else
                {
                    BOOST_FOREACH(const VertexAccessMap::VamMap::value_type& itps, ptDepartureVam.getMap())
                    {
                        const Vertex* vertex(itps.first);
                        if(vertex->getGraphType() == PTModule::GRAPH_ID)
                        {
                            ptDepartureExtVam.insert(vertex, itps.second);
                        }
                    }
                }

                // perform a PT journey planning from the set of the relay parks to the arrival place
                TimeSlotRoutePlanner r(
                    ptDepartureExtVam,
                    relayParksMap,
                    ptLowestDepartureTime,
                    ptHighestDepartureTime,
                    ptLowestArrivalTime,
                    ptHighestArrivalTime,
                    _whatToSearch,
                    _graphToUse,
                    _maxDuration,
                    _maxSolutionsNumber,
                    _accessParameters,
                    _planningOrder,
                    70, // 252 km/h TODO take it configurable
                    _ignoreReservation,
                    _logger,
                    _maxTransferDuration,
                    _minMaxDurationRatioFilter,
                    _enableTheoretical,
                    _enableRealTime,
                    _reservationRulesDelayType
                );

                ptJourneys = r.run();

                if(0 < ptJourneys.size())
                {
                    // OVE!!! : debug
                    std::cout << "Found " << ptJourneys.size() << " journey(s) from "
                              << (namedDeparturePlace ? namedDeparturePlace->getName() : "NULL") << " to "
                              << (namedArrivalPlace ? namedArrivalPlace->getName() : "NULL") << std::endl;
                }

                else
                {
                    // OVE!!! : debug
                    std::cout << "Found no solution" << std::endl;
                }
            }

            return PTRoutePlannerResult(
                _departurePlace,
                _arrivalPlace,
                false,
                ptJourneys
            );
        }


        void PTTimeSlotRoutePlanner::_printJourneys(const TimeSlotRoutePlanner::Result& journeys) const
        {
#ifdef DEBUG
            // this method displays the details of a journey on the standard output
            if(Log::LEVEL_DEBUG >= Log::GetInstance().getLevel())
            {
                std::stringstream oStream;
                oStream << "Found " << journeys.size() << " journeys" << std::endl;
                int journeyCount = 0;

                BOOST_FOREACH(Journey journey, journeys)
                {
                    std::deque<ServicePointer>& servicePtrs = journey.getServiceUses();
                    journeyCount += 1;

                    oStream << "---------------" << std::endl;
                    oStream << "Journey #" << journeyCount << ": distance=" << journey.getDistance()
                            << ", duration=" << journey.getEffectiveDuration()
                            << ", services=" << servicePtrs.size() << std::endl;

                    const ServicePointer& servicePtr1 = journey.getFirstJourneyLeg();
                    const Service* service1 = servicePtr1.getService();
                    oStream << " * service #1: departure=" << servicePtr1.getDepartureDateTime()
                            << ", arrival=" << servicePtr1.getArrivalDateTime()
                            << ", name=" << service1->getServiceNumber() << std::endl;

                    if(2 < servicePtrs.size())
                    {
                        oStream << " * services ..." << std::endl;
                    }

                    const ServicePointer& servicePtrN = journey.getLastJourneyLeg();
                    const Service* serviceN = servicePtrN.getService();
                    oStream << " * service #" << servicePtrs.size() << ": departure=" << servicePtrN.getDepartureDateTime()
                            << ", arrival=" << servicePtrN.getArrivalDateTime()
                            << ", name=" << serviceN->getServiceNumber() << std::endl;

                    /*
                    // this version prints all the services used by the journey
                    int serviceCount = 0;
                    BOOST_FOREACH(ServicePointer servicePtr, servicePtrs)
                    {
                        serviceCount += 1;
                        const Service* service = servicePtr.getService();
                        oStream << " * service #" << serviceCount << ": departure=" << servicePtr.getDepartureDateTime()
                                   << ", arrival=" << servicePtr.getArrivalDateTime()
                                   << ", name=" << service->getServiceNumber() << std::endl;
                    }
                    */
                }

                util::Log::GetInstance().debug(oStream.str());
            }
#endif
        }


        void PTTimeSlotRoutePlanner::_findRelayParks(const geography::Place* origin, const int maxDistance, SortableStopAreaSet& relayParks) const {
            boost::shared_ptr<geos::geom::Point> originLocation = origin->getPoint();

            if(NULL == originLocation.get())
            {
                // OVE!!! : TODO log
                std::cerr << "Origin has no position, cannot compute the list of relay parks" << std::endl;
                return;
            }

            BOOST_FOREACH(const Registry<StopArea>::value_type& stopArea, Env::GetOfficialEnv().getRegistry<StopArea>())
            {
                if(stopArea.second->getIsRelayPark())
                {
                    boost::shared_ptr<geos::geom::Point> parkingLocation = stopArea.second->getPoint();

                    if(parkingLocation.get())
                    {
                        double deltaX = parkingLocation->getX() - originLocation->getX();
                        double deltaY = parkingLocation->getY() - originLocation->getY();
                        int    distance = round(sqrt(deltaX * deltaX + deltaY * deltaY));

                        if(maxDistance >= distance)
                        {
                            relayParks.insert(SortableStopArea(stopArea.second.get(), distance));
                        }
                    }

                    else
                    {
                        // OVE!!! : TODO log
                        std::cerr << "!!! Parking " << stopArea.second->getName() << " has no position" << std::endl;
                    }
                }
            }
        }


        // OVE!!! : derived from ServiceDensityService

        PTTimeSlotRoutePlanner::SortableStopArea::SortableStopArea(const StopArea* stopArea, int distanceToCenter):
            _stopArea(stopArea),
            _distanceToCenter(distanceToCenter),
            _opCode(_stopArea->getCodeBySources())
        {
        }


        bool PTTimeSlotRoutePlanner::SortableStopArea::operator<(SortableStopArea const &otherStopArea) const
        {
            if(_distanceToCenter != otherStopArea.getDistanceToCenter())
            {
                return _distanceToCenter < otherStopArea.getDistanceToCenter();
            }

            if(_opCode != otherStopArea.getOpCode())
            {
                return _opCode < otherStopArea.getOpCode();
            }

            return _stopArea < otherStopArea.getStopArea();
        }


        std::string PTTimeSlotRoutePlanner::SortableStopArea::getOpCode() const
        {
            return _opCode;
        }


        int PTTimeSlotRoutePlanner::SortableStopArea::getDistanceToCenter() const
        {
            return _distanceToCenter;
        }

        const StopArea* PTTimeSlotRoutePlanner::SortableStopArea::getStopArea() const
        {
            return _stopArea;
        }

}	}
