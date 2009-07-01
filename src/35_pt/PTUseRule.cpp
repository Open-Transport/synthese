
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
#include "DateTime.h"
#include "AccessParameters.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace env;
	using namespace time;

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
			_minDelayMinutes(0),
			_minDelayDays(0),
			_hourDeadLine(TIME_UNKNOWN)
		{
		}


		
		UseRule::AccessCapacity PTUseRule::getAccessCapacity() const
		{
			return _accessCapacity;
		}

	
		void PTUseRule::setMinDelayMinutes (int minDelayMinutes)
		{
			_minDelayMinutes = minDelayMinutes;
		}



		void PTUseRule::setMinDelayDays (int minDelayDays)
		{
			_minDelayDays = minDelayDays;
		}



		void PTUseRule::setMaxDelayDays(
			const optional<size_t> maxDelayDays
		){
			_maxDelayDays = maxDelayDays;
		}



		void PTUseRule::setOriginIsReference (bool originIsReference)
		{
			_originIsReference = originIsReference;
		}

		bool PTUseRule::getOriginIsReference() const
		{
			return _originIsReference;
		}

		int PTUseRule::getMinDelayDays() const
		{
			return _minDelayDays;
		}

		int PTUseRule::getMinDelayMinutes() const
		{
			return _minDelayMinutes;
		}

		const optional<size_t>& PTUseRule::getMaxDelayDays() const
		{
			return _maxDelayDays;
		}





		PTUseRule::ReservationRuleType PTUseRule::getReservationType() const
		{
			return _reservationType;
		}



		void PTUseRule::setReservationType(
			PTUseRule::ReservationRuleType value
		){
			_reservationType = value;
		}



		const Hour& PTUseRule::getHourDeadLine () const
		{
			return _hourDeadLine;
		}



		void PTUseRule::setHourDeadLine(
			const Hour& hourDeadLine
		){
			if (hourDeadLine == Hour(0,0))
				_hourDeadLine = Hour(23,59);
			else
				_hourDeadLine = hourDeadLine;
		}




		DateTime PTUseRule::getReservationDeadLine (
			const DateTime& originDateTime
			, const DateTime& departureTime
		) const {

			const DateTime& referenceTime(
				_originIsReference ?
				originDateTime :
				departureTime
			);

			DateTime minutesMoment = referenceTime;
			DateTime daysMoment = referenceTime;

			if ( _minDelayMinutes ) minutesMoment -= _minDelayMinutes;

			if ( _minDelayDays )
			{
				daysMoment.subDaysDuration( _minDelayDays );
				daysMoment.setHour(Hour(TIME_MAX));
			}

			if ( _hourDeadLine < daysMoment.getHour () )
				daysMoment.setHour( _hourDeadLine );

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
					DateTime reservationTime(TIME_CURRENT);
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
					DateTime reservationTime(TIME_CURRENT);
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
					DateTime reservationTime(TIME_CURRENT);
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
					DateTime reservationTime(TIME_CURRENT);
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
				DateTime reservationTime(TIME_CURRENT);
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
					DateTime reservationTime(TIME_CURRENT);
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

		time::DateTime PTUseRule::getReservationOpeningTime(
			const graph::ServicePointer& servicePointer
		) const {
			if(_maxDelayDays)
			{
				DateTime reservationStartTime(servicePointer.getOriginDateTime());
				reservationStartTime.subDaysDuration(static_cast<int>(*_maxDelayDays));
				reservationStartTime.setHour(Hour(TIME_MIN));
				return reservationStartTime;
			}
			return DateTime(TIME_MIN);
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

		const std::string& PTUseRule::getName() const
		{
			return _name;
		}

		void PTUseRule::setName( const std::string& value )
		{
			_name = value;
		}

		void PTUseRule::setAccessCapacity( UseRule::AccessCapacity value )
		{
			_accessCapacity = value;
		}

		void PTUseRule::setDefaultFare( const env::Fare* value )
		{
			_defaultFare = value;
		}

		const env::Fare* PTUseRule::getDefaultFare() const
		{
			return _defaultFare;
		}
	}
}
