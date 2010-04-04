
/** PTUseRule class implementation.
	@file PTUseRule.cpp

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

#include "PTUseRule.h"
#include "Edge.h"
#include "Line.h"
#include "CommercialLine.h"
#include "ServicePointer.h"
#include "ServiceUse.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "AccessParameters.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace env;

	namespace util
	{
		template<> const std::string Registry<pt::PTUseRule>::KEY("PTUseRule");
	}

	namespace pt
	{

		PTUseRule::PTUseRule(
			util::RegistryKeyType key /*= UNKNOWN_VALUE */
		):	Registrable(key),
			_accessCapacity(0),
			_reservationType(RESERVATION_RULE_FORBIDDEN),
			_minDelayMinutes(minutes(0)),
			_minDelayDays(days(0)),
			_maxDelayDays(not_a_date_time),
			_hourDeadLine(not_a_date_time),
			_defaultFare(NULL)
		{
		}



		void PTUseRule::setHourDeadLine(
			const time_duration& hourDeadLine
		){
			if (hourDeadLine == time_duration(0,0,0))
				_hourDeadLine = time_duration(23,59,59);
			else
				_hourDeadLine = hourDeadLine;
		}




		ptime PTUseRule::getReservationDeadLine (
			const ptime& originDateTime
			, const ptime& departureTime
		) const {

			const ptime& referenceTime(
				_originIsReference ?
				originDateTime :
				departureTime
			);

			ptime minutesMoment = referenceTime;
			ptime daysMoment = referenceTime;

			if ( _minDelayMinutes.total_seconds() ) minutesMoment -= _minDelayMinutes;

			if ( _minDelayDays.days() )
			{
				daysMoment = daysMoment - _minDelayDays + time_duration(23,59,59);
			}

			if ( _hourDeadLine < daysMoment.time_of_day() )
				daysMoment = ptime(daysMoment.date(), _hourDeadLine);

			if ( minutesMoment < daysMoment )
				return minutesMoment;
			else
				return daysMoment;
		}



		UseRule::RunPossibilityType PTUseRule::isRunPossible(
			const graph::ServicePointer& servicePointer
		) const {

			if(_accessCapacity && *_accessCapacity == 0)
			{
				return RUN_NOT_POSSIBLE;
			}

			switch(_reservationType)
			{
			case RESERVATION_RULE_FORBIDDEN:
			case RESERVATION_RULE_OPTIONAL:
				return RUN_POSSIBLE;

			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE:
				if(servicePointer.getMethod() == ARRIVAL_TO_DEPARTURE)
				{
					return RUN_DEPENDING_ON_DEPARTURE_PLACE;
				}
				else
				{
					const Edge* departureEdge(servicePointer.getEdge());
					if(static_cast<const Line*>(departureEdge->getParentPath())->getCommercialLine()->isOptionalReservationPlace(
						static_cast<const PublicTransportStopZoneConnectionPlace*>(departureEdge->getHub())
					)){
						return RUN_POSSIBLE;
					}
					else
					{
						return
							IsReservationPossible(getReservationAvailability(servicePointer)) ?
							RUN_POSSIBLE :
							RUN_NOT_POSSIBLE
						;
					}
				}

			case RESERVATION_RULE_COMPULSORY:
				return
					IsReservationPossible(getReservationAvailability(servicePointer)) ?
					RUN_POSSIBLE :
					RUN_NOT_POSSIBLE
				;
			}
			
			return RUN_NOT_POSSIBLE;
		}



		UseRule::RunPossibilityType PTUseRule::isRunPossible(
			const graph::ServiceUse& serviceUse
		) const	{
			if(_accessCapacity && *_accessCapacity == 0)
			{
				return RUN_NOT_POSSIBLE;
			}

			switch(_reservationType)
			{
			case RESERVATION_RULE_FORBIDDEN:
			case RESERVATION_RULE_OPTIONAL:
				return RUN_POSSIBLE;

			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE:
			{
				const Edge* departureEdge(serviceUse.getDepartureEdge());
				if(static_cast<const Line*>(departureEdge->getParentPath())->getCommercialLine()->isOptionalReservationPlace(
					static_cast<const PublicTransportStopZoneConnectionPlace*>(departureEdge->getHub())
				)){
					return RUN_POSSIBLE;
				}
				else
				{
					return
						IsReservationPossible(getReservationAvailability(serviceUse)) ?
						RUN_POSSIBLE :
						RUN_NOT_POSSIBLE
					;
				}
			}

			case RESERVATION_RULE_COMPULSORY:
				return
					IsReservationPossible(getReservationAvailability(serviceUse)) ?
					RUN_POSSIBLE :
					RUN_NOT_POSSIBLE
				;
			}
			
			return RUN_NOT_POSSIBLE;
		}



		UseRule::ReservationAvailabilityType PTUseRule::getReservationAvailability(
			const ServicePointer& servicePointer
		) const	{
			switch(_reservationType)
			{
			case RESERVATION_RULE_FORBIDDEN:
				return RESERVATION_FORBIDDEN;

			case RESERVATION_RULE_OPTIONAL:
				{
					ptime reservationTime(second_clock::local_time());
					if(	reservationTime < getReservationOpeningTime(servicePointer)
					){
						return RESERVATION_OPTIONAL_TOO_EARLY;
					}
					if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getActualDateTime())
					){
						return RESERVATION_OPTIONAL_TOO_LATE;
					}
					if(servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
					{
						return RESERVATION_OPTIONAL_POSSIBLE;
					}
					if(reservationTime <= getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getOriginDateTime()))
					{
						return RESERVATION_OPTIONAL_POSSIBLE;
					}
					return RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;
				}

			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE:
				if(servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
				{
					const Edge* departureEdge(servicePointer.getEdge());
					ptime reservationTime(second_clock::local_time());
					if(static_cast<const Line*>(departureEdge->getParentPath())->getCommercialLine()->isOptionalReservationPlace(
						static_cast<const PublicTransportStopZoneConnectionPlace*>(departureEdge->getHub())
					)){
						if(	reservationTime < getReservationOpeningTime(servicePointer)
						){
							return RESERVATION_OPTIONAL_TOO_EARLY;
						}
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getActualDateTime())
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						return RESERVATION_OPTIONAL_POSSIBLE;
					} else {
						if(	reservationTime < getReservationOpeningTime(servicePointer)
						){
							return RESERVATION_COMPULSORY_TOO_EARLY;
						}
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getActualDateTime())
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
						}
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
				}
				return RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;

			case RESERVATION_RULE_COMPULSORY:
				{
					ptime reservationTime(second_clock::local_time());
					if(	reservationTime < getReservationOpeningTime(servicePointer)
					){
						return RESERVATION_COMPULSORY_TOO_EARLY;
					}
					if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getActualDateTime())
					){
						return RESERVATION_COMPULSORY_TOO_LATE;
					}
					if (servicePointer.getMethod() == DEPARTURE_TO_ARRIVAL)
					{
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
					if(reservationTime <= getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getOriginDateTime())
					){
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
					return RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;

				}
			}
			
			return RESERVATION_FORBIDDEN;
		}



		UseRule::ReservationAvailabilityType PTUseRule::getReservationAvailability(
			const graph::ServiceUse& serviceUse
		) const	{
			switch(_reservationType)
			{
			case RESERVATION_RULE_FORBIDDEN:
				return RESERVATION_FORBIDDEN;

			case RESERVATION_RULE_OPTIONAL:
				{
					ptime reservationTime(second_clock::local_time());
					if(	reservationTime < getReservationOpeningTime(serviceUse)
						){
							return RESERVATION_OPTIONAL_TOO_EARLY;
					}
					if(reservationTime > getReservationDeadLine(serviceUse.getOriginDateTime(), serviceUse.getDepartureDateTime())
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
					}
					return RESERVATION_OPTIONAL_POSSIBLE;
				}

			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE:
			{
				ptime reservationTime(second_clock::local_time());
				const Edge* departureEdge(serviceUse.getDepartureEdge());
				if(static_cast<const Line*>(departureEdge->getParentPath())->getCommercialLine()->isOptionalReservationPlace(
					static_cast<const PublicTransportStopZoneConnectionPlace*>(departureEdge->getHub())
				)){
					if(	reservationTime < getReservationOpeningTime(serviceUse)
					){
						return RESERVATION_OPTIONAL_TOO_EARLY;
					}
					if(reservationTime > getReservationDeadLine(serviceUse.getOriginDateTime(), serviceUse.getDepartureDateTime())
					){
						return RESERVATION_OPTIONAL_TOO_LATE;
					}
					return RESERVATION_OPTIONAL_POSSIBLE;
				} else {
					if(	reservationTime < getReservationOpeningTime(serviceUse)
					){
						return RESERVATION_COMPULSORY_TOO_EARLY;
					}
					if(reservationTime > getReservationDeadLine(serviceUse.getOriginDateTime(), serviceUse.getDepartureDateTime())
					){
						return RESERVATION_COMPULSORY_TOO_LATE;
					}
					return RESERVATION_COMPULSORY_POSSIBLE;
				}
			}
			
			case RESERVATION_RULE_COMPULSORY:
				{
					ptime reservationTime(second_clock::local_time());
					if(	reservationTime < getReservationOpeningTime(serviceUse)
						){
							return RESERVATION_COMPULSORY_TOO_EARLY;
					}
					if(reservationTime > getReservationDeadLine(serviceUse.getOriginDateTime(), serviceUse.getDepartureDateTime())
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
					}
					return RESERVATION_COMPULSORY_POSSIBLE;
				}
			}
			
			return RESERVATION_FORBIDDEN;
		}



		ptime PTUseRule::getReservationOpeningTime(
			const graph::ServicePointer& servicePointer
		) const {
			if(_maxDelayDays)
			{
				return ptime(servicePointer.getOriginDateTime().date(), -hours(24 * _maxDelayDays->days()));
			}
			return neg_infin;
		}

		bool PTUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			if(_accessCapacity && *_accessCapacity == 0) return false;

			// 			if (_fare && complyer.getFare()->isCompliant() == logic::tribool(true) && complyer.getFare().get() != _fare)
			// 				return false;

			if (accessParameters.getDRTOnly() &&
				_reservationType == RESERVATION_RULE_FORBIDDEN
			) return false;

			if (accessParameters.getWithoutDRT() &&
				_reservationType != RESERVATION_RULE_FORBIDDEN
			) return false;

			return true;
		}



		PTUseRule::ReservationRuleTypesList PTUseRule::GetTypesList()
		{
			ReservationRuleTypesList result;
			result.push_back(make_pair(RESERVATION_RULE_FORBIDDEN, GetTypeName(RESERVATION_RULE_FORBIDDEN)));
			result.push_back(make_pair(RESERVATION_RULE_OPTIONAL, GetTypeName(RESERVATION_RULE_OPTIONAL)));
			result.push_back(make_pair(RESERVATION_RULE_COMPULSORY, GetTypeName(RESERVATION_RULE_COMPULSORY)));
			result.push_back(make_pair(RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE, GetTypeName(RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE)));
			return result;
		}



		std::string PTUseRule::GetTypeName( ReservationRuleType type )
		{
			switch(type)
			{
			case RESERVATION_RULE_FORBIDDEN: return "Réservation impossible";
			case RESERVATION_RULE_COMPULSORY: return "Réservation obligatoire";
			case RESERVATION_RULE_OPTIONAL: return "Réservation possible";
			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE: return "Réservation possible, obligatoire au départ de certains arrêts";
			}
			assert(false);
			return string();
		}
	}
}
