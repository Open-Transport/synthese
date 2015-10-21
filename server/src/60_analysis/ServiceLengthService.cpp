
//////////////////////////////////////////////////////////////////////////////////////////
///	ServiceLengthService class implementation.
///	@file ServiceLengthService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "ServiceLengthService.hpp"

#include "CommercialLine.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "DRTArea.hpp"
#include "JourneyPattern.hpp"
#include "PTUseRule.h"
#include "Request.h"
#include "RequestException.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "ScheduledService.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace graph;
	using namespace pt;
	using namespace resa;
	using namespace server;
	using namespace security;
	using namespace util;

	template<>
	const string FactorableTemplate<Function,analysis::ServiceLengthService>::FACTORY_KEY = "service_length";
	
	namespace analysis
	{
		const string ServiceLengthService::PARAMETER_DATE = "date";
		const string ServiceLengthService::ATTR_PLANNED_DISTANCE = "planned_distance";
		const string ServiceLengthService::ATTR_REAL_DISTANCE = "real_distance";
		
		const string ServiceLengthService::TAG_LEG = "leg";
		const string ServiceLengthService::ATTR_DEPARTURE_STOP_ID = "departure_stop_id";
		const string ServiceLengthService::ATTR_DEPARTURE_STOP_NAME = "departure_stop_name";
		const string ServiceLengthService::ATTR_ARRIVAL_STOP_ID = "arrival_stop_id";
		const string ServiceLengthService::ATTR_ARRIVAL_STOP_NAME = "arrival_stop_name";
		const string ServiceLengthService::ATTR_LENGTH = "length";
		const string ServiceLengthService::ATTR_PASSENGERS = "passengers";


		ParametersMap ServiceLengthService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void ServiceLengthService::_setFromParametersMap(const ParametersMap& map)
		{
			// Service
			try
			{
				_service = Env::GetOfficialEnv().get<ScheduledService>(
					map.get<RegistryKeyType>(
						Request::PARAMETER_OBJECT_ID
				)	);
			}
			catch (ObjectNotFoundException<ScheduledService>&)
			{
				throw RequestException("No such service");
			}

			// Date
			 _date = from_string(map.get<string>(PARAMETER_DATE));
		}

		ParametersMap ServiceLengthService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Declarations
			ParametersMap map;
			const JourneyPattern* journeyPattern(
				_service->getRoute()
			);

			// Planned distance
			MetricOffset distance(0);
			Path::Edges edges(journeyPattern->getEdges());
			if(!edges.empty() && (*edges.rbegin())->getMetricOffset())
			{
				distance = (*edges.rbegin())->getMetricOffset();
			}
			if(!distance)
			{
				distance = journeyPattern->getPlannedLength();
			}
			map.insert(ATTR_PLANNED_DISTANCE, distance);

			// Real distance
			if(	dynamic_cast<const PTUseRule*>(
					&_service->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
				) &&
				dynamic_cast<const PTUseRule&>(
					_service->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
				).getReservationType() != pt::RESERVATION_RULE_FORBIDDEN
			){
				// Getting the reservations
				Env env;
				date maxDate(_date + days(1));
				ReservationTableSync::SearchResult resas(
					ReservationTableSync::Search(
						env,
						journeyPattern->getCommercialLine()->getKey(),
						_date,
						maxDate,
						optional<string>(),
						false,
						true,
						true,
						0,
						optional<size_t>(),
						UP_LINKS_LOAD_LEVEL,
						_service->getKey()
				)	);

				distance = 0;
				const StopArea* lastStop(NULL);
				Legs legs;
				size_t passengersAtLastStop(0);
				BOOST_FOREACH(const Path::Edges::value_type& edge, journeyPattern->getEdges())
				{
					ReservationPoints reservationPoints;

					// Search for reservations to do
					bool isArea(!dynamic_cast<DesignatedLinePhysicalStop*>(edge));
					bool isDeparture(edge->isDeparture());
					bool isArrival(edge->isArrival());
					BOOST_FOREACH(const boost::shared_ptr<Reservation>& resa, resas)
					{
						if(isDeparture)
						{
							const StopArea* stopArea(
								Env::GetOfficialEnv().get<StopArea>(
									resa->get<DeparturePlaceId>()
								).get()
							);
							if(	(isArea && dynamic_cast<DRTArea*>(edge->getFromVertex())->contains(*stopArea)) ||
								(!isArea && dynamic_cast<StopPoint*>(edge->getFromVertex())->getConnectionPlace() == stopArea)
							){
								AddReservation(reservationPoints, *resa, true);									
							}
						}
						if(isArrival)
						{
							const StopArea* stopArea(
								Env::GetOfficialEnv().get<StopArea>(
									resa->get<ArrivalPlaceId>()
								).get()
							);
							if(	(isArea && dynamic_cast<DRTArea*>(edge->getFromVertex())->contains(*stopArea)) ||
								(!isArea && dynamic_cast<StopPoint*>(edge->getFromVertex())->getConnectionPlace() == stopArea)
							){
								AddReservation(reservationPoints, *resa, false);									
							}
						}
					}

					// First stop as last point
					if(!lastStop)
					{
						if(!dynamic_cast<DesignatedLinePhysicalStop*>(edge))
						{
							throw RequestException("Invalid journey pattern");
						}
						lastStop = dynamic_cast<const StopPoint*>(edge->getFromVertex())->getConnectionPlace();
						passengersAtLastStop = GetPassengers(reservationPoints, *lastStop, true);
						reservationPoints.erase(lastStop);
					}

					// Building the legs
					while(!reservationPoints.empty())
					{
						// Choosing the nearest stop
						MetricOffset bestDistance(0);
						const StopArea* bestPlace(NULL);
						BOOST_FOREACH(const ReservationPoints::value_type& point, reservationPoints)
						{
							MetricOffset dst(point.first->getPoint()->distance(lastStop->getPoint().get()));
							if(!bestPlace || dst < bestDistance)
							{
								bestDistance = dst;
								bestPlace = point.first;
							}
						}

						// Building the leg
						Leg leg;
						leg.startStop = lastStop;
						leg.endStop = bestPlace;
						leg.passengers = passengersAtLastStop;
						leg.distance = bestDistance;
						legs.push_back(leg);

						// Informations for next leg
						lastStop = bestPlace;
						passengersAtLastStop = passengersAtLastStop
							+ GetPassengers(reservationPoints, *bestPlace, true)
							- GetPassengers(reservationPoints, *bestPlace, false);
						reservationPoints.erase(bestPlace);
					}
				}

				// Output
				distance = 0;
				BOOST_FOREACH(const Leg& leg, legs)
				{
					distance += leg.distance;

					boost::shared_ptr<ParametersMap> legPM(new ParametersMap);
					legPM->insert(ATTR_DEPARTURE_STOP_ID, leg.startStop->getKey());
					legPM->insert(ATTR_DEPARTURE_STOP_NAME, leg.startStop->getFullName());
					legPM->insert(ATTR_ARRIVAL_STOP_ID, leg.endStop->getKey());
					legPM->insert(ATTR_ARRIVAL_STOP_NAME, leg.endStop->getFullName());
					legPM->insert(ATTR_LENGTH, leg.distance);
					legPM->insert(ATTR_PASSENGERS, leg.passengers);

					map.insert(TAG_LEG, legPM);
				}
			}
			map.insert(ATTR_REAL_DISTANCE, distance);

			return map;
		}
		
		
		
		bool ServiceLengthService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ServiceLengthService::getOutputMimeType() const
		{
			return "text/html";
		}



		void ServiceLengthService::AddReservation(
			ReservationPoints& points,
			const Reservation& resa,
			bool departure
		){
			const StopArea* stopArea(
				Env::GetOfficialEnv().get<StopArea>(
					departure ?
					resa.get<DeparturePlaceId>() :
					resa.get<ArrivalPlaceId>()
				).get()
			);

			ReservationPoints::iterator it(points.find(stopArea));
			if(it == points.end())
			{
				ReservationPoint point;
				point.insert(make_pair(&resa, departure));
				points.insert(
					make_pair(
						stopArea,
						point
				)	);
			}
			else
			{
				it->second.insert(make_pair(&resa, departure));
			}
		}



		size_t ServiceLengthService::GetPassengers(
			const ReservationPoints& points,
			const pt::StopArea& stopArea,
			bool departure
		){
			ReservationPoints::const_iterator it(points.find(&stopArea));
			if(it == points.end())
			{
				return 0;
			}
			size_t result(0);
			BOOST_FOREACH(const ReservationPoint::value_type& resa, it->second)
			{
				if(resa.second == departure)
				{
					result += resa.first->get<Transaction>()->get<Seats>();
				}
			}
			return result;
		}
}	}
