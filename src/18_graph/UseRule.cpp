//
// C++ Implementation: UseRule
//
// Description: 
//
//
// Author: Hugues Romain (RCS) <hugues.romain@reseaux-conseil.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "UseRule.h"

#include <limits>

using namespace std;

namespace synthese
{
	using namespace time;
	
	namespace graph
	{
		const UseRule UseRule::ALLOWED(
			UseRule::ACCESS_ALLOWED
		);
		const UseRule UseRule::FORBIDDEN(
			UseRule::ACCESS_FORBIDDEN
		);
		const UseRule UseRule::UNKNOWN(
			UseRule::ACCESS_UNKNOWN
		);
		const UseRule::Capacity UseRule::UNLIMITED_CAPACITY(numeric_limits<UseRule::Capacity>::max());
		
		
		UseRule::UseRule(
			UseRule::Access access
		):	_access(access),
			_capacity(UNLIMITED_CAPACITY),
			_reservationType(RESERVATION_FORBIDDEN),
			_minDelayMinutes(0),
			_minDelayDays(0),
			_maxDelayDays(0),
			_hourDeadLine(TIME_UNKNOWN)
		{
		}
		
		
		
		UseRule::~UseRule()
		{
		}
		
		
		
		UseRule::Access UseRule::getAccess() const
		{
			return _access;
		}
		
		
		UseRule::Capacity UseRule::getCapacity() const
		{
			return _capacity;
		}

	
		void UseRule::setMinDelayMinutes (int minDelayMinutes)
		{
			_minDelayMinutes = minDelayMinutes;
		}



		void UseRule::setMinDelayDays (int minDelayDays)
		{
			_minDelayDays = minDelayDays;
		}



		void UseRule::setMaxDelayDays (int maxDelayDays)
		{
			_maxDelayDays = maxDelayDays;
		}



		void UseRule::setOriginIsReference (bool originIsReference)
		{
			_originIsReference = originIsReference;
		}

		bool UseRule::getOriginIsReference() const
		{
			return _originIsReference;
		}

		int UseRule::getMinDelayDays() const
		{
			return _minDelayDays;
		}

		int UseRule::getMinDelayMinutes() const
		{
			return _minDelayMinutes;
		}

		int UseRule::getMaxDelayDays() const
		{
			return _maxDelayDays;
		}





		UseRule::ReservationRuleType UseRule::getReservationType() const
		{
			return _reservationType;
		}



		void UseRule::setReservationType(
			UseRule::ReservationRuleType value
		){
			_reservationType = value;
		}



		const Hour& UseRule::getHourDeadLine () const
		{
			return _hourDeadLine;
		}



		void UseRule::setHourDeadLine(
			const Hour& hourDeadLine
		){
			if (hourDeadLine == Hour(0,0))
				_hourDeadLine = Hour(23,59);
			else
				_hourDeadLine = hourDeadLine;
		}



		DateTime UseRule::getReservationDeadLine (
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



		DateTime UseRule::getReservationStartTime (
			const DateTime& reservationTime
		) const {
			DateTime reservationStartTime = reservationTime;
		    
			if ( _maxDelayDays )
			{
				reservationStartTime.subDaysDuration( _maxDelayDays );
				reservationStartTime.setHour(Hour(TIME_MIN));
			}

			return reservationStartTime;
		}



		bool UseRule::isRunPossible(
			const DateTime& originDateTime,
			bool stopBelongsToOptionalReservationPlaces,
			const DateTime& reservationTime,
			const DateTime& departureTime
		) const {
			return	_reservationType == RESERVATION_FORBIDDEN
				||	_reservationType == RESERVATION_OPTIONAL
				||	(	_reservationType == RESERVATION_MIXED_BY_DEPARTURE_PLACE &&
						stopBelongsToOptionalReservationPlaces)
				||	isReservationPossible(originDateTime, reservationTime, departureTime)
			;
		}



		bool UseRule::isReservationPossible(
			const DateTime& originDateTime
			, const DateTime& reservationTime
			, const DateTime& departureTime
		) const {
			return	_reservationType != RESERVATION_FORBIDDEN
				&&	reservationTime <= getReservationDeadLine(originDateTime, departureTime )
				&&	reservationTime >= getReservationStartTime (reservationTime)
			;
		}
	}
}