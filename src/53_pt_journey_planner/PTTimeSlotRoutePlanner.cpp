
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
			_arrivalParking(NULL)
		{
		}

		PTTimeSlotRoutePlanner::PTTimeSlotRoutePlanner(
			const geography::Place* origin,
			const geography::Place* destination,
			const geography::Place* originParking,
			const geography::Place* destinationParking,
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
			_arrivalParking(destinationParking)
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
				if((NULL == _departureParking) && (NULL == _arrivalParking))
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
					return _computeMixedModeJourney();
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



		PTRoutePlannerResult PTTimeSlotRoutePlanner::_computeMixedModeJourney() const {

			long PARKING_TIME = 5;
			ptime ptLowestDepartureTime  = getLowestDepartureTime();
			ptime ptHighestDepartureTime = getHighestDepartureTime();
			ptime ptLowestArrivalTime	= getLowestArrivalTime();
			ptime ptHighestArrivalTime   = getHighestArrivalTime();

			const Place* ptDeparturePlace = _departurePlace;
			const Place* ptArrivalPlace   = _arrivalPlace;

			TimeSlotRoutePlanner::Result ptJourneys;
			Journey carStartJourney;
			Journey carEndJourney;

			// 1) check if the first part of the journey shall use the car
			if(NULL != _departureParking)
			{
				// route planning
				AccessParameters ap(
					USER_CAR,
					false, false, 30000, posix_time::hours(5), 11.111, // ~= 40km/h
					1000
				);

				// compute journey from departure place to departure parking, using car
				RoadJourneyPlanner rjp(
					_departurePlace,
					_departureParking,
					getLowestDepartureTime(),
					getHighestDepartureTime(),
					getLowestArrivalTime(),
					getHighestArrivalTime(),
					1,
					ap,
					_planningOrder,
					_logger
				);

				RoadJourneyPlannerResult results = rjp.run();

				if(!results.getJourneys().empty())
				{
					// compute journey from departure to parking
					_printJourneys(results.getJourneys());

					carStartJourney = results.getJourneys()[0];
					time_duration duration = carStartJourney.getDuration();
					duration = duration + minutes(PARKING_TIME);
					ptLowestDepartureTime += duration;
					ptHighestDepartureTime += duration;

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
				// Route planning
				AccessParameters ap(
					USER_CAR,
					false, false, 30000, posix_time::hours(5), 11.111, // ~= 40km/h
					1000
				);

				// compute journey from arrival parking to arrival place, using car
				RoadJourneyPlanner rjp(
					_arrivalParking,
					_arrivalPlace,
					getLowestDepartureTime(),
					getHighestDepartureTime(),
					getLowestArrivalTime(),
					getHighestArrivalTime(),
					1,
					ap,
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
					_departurePlace,
					_arrivalPlace
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

}	}
