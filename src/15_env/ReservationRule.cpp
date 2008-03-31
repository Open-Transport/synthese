
/** ReservationRule class implementation.
	@file ReservationRule.cpp

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

#include "15_env/ReservationRule.h"

#include "04_time/Schedule.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace util
	{
		template<> typename Registrable<uid,env::ReservationRule>::Registry Registrable<uid,env::ReservationRule>::_registry;
		template<> boost::shared_ptr<const env::ReservationRule> RegistrableWithNeutralElement<uid,env::ReservationRule>::_neutral(new env::ReservationRule);
	}

	namespace env
	{
		ReservationRule::ReservationRule()
		: RegistrableWithNeutralElement<uid,ReservationRule> ()
		, Compliance(false, UNKNOWN_VALUE)
		, _online(false)
		, _minDelayMinutes(0)
		, _minDelayDays(0)
		, _maxDelayDays(0)
		, _hourDeadLine(TIME_UNKNOWN)
		{
		}

		ReservationRule::~ReservationRule()
		{
		}




		const synthese::time::Hour& 
		ReservationRule::getHourDeadLine () const
		{
			return _hourDeadLine;
		}



		void
		ReservationRule::setHourDeadLine (const synthese::time::Hour& hourDeadLine)
		{
			if (hourDeadLine == Hour(0,0))
				_hourDeadLine = Hour(23,59);
			else
				_hourDeadLine = hourDeadLine;
		}




		const std::string& 
		ReservationRule::getPhoneExchangeOpeningHours () const
		{
			return _phoneExchangeOpeningHours;
		}



		void
		ReservationRule::setPhoneExchangeOpeningHours (const std::string& phoneExchangeOpeningHours)
		{
			_phoneExchangeOpeningHours = phoneExchangeOpeningHours;
		}




		const std::string& 
		ReservationRule::getWebSiteUrl () const
		{
			return _webSiteUrl;
		}


		void
		ReservationRule::setWebSiteUrl (const std::string& webSiteUrl)
		{
			_webSiteUrl = webSiteUrl;
		}




		void
		ReservationRule::setPhoneExchangeNumber (const std::string& phoneExchangeNumber)
		{
			_phoneExchangeNumber = phoneExchangeNumber;
		}




		const std::string& 
		ReservationRule::getPhoneExchangeNumber () const
		{
			return _phoneExchangeNumber;
		}




		DateTime ReservationRule::getReservationDeadLine (
			const DateTime& originDateTime
			, const DateTime& departureTime
		) const {
			
			const DateTime& referenceTime = _originIsReference
				? originDateTime
				: departureTime;

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



		DateTime ReservationRule::getReservationStartTime (
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





		bool ReservationRule::isRunPossible (
			const DateTime& originDateTime
			, const DateTime& reservationTime
			, const DateTime& departureTime
		) const {
			return (isCompliant() != true)
			|| isReservationPossible (originDateTime, reservationTime, departureTime );

		}




		bool 
		ReservationRule::isReservationPossible (
			const DateTime& originDateTime
			, const DateTime& reservationTime
			, const DateTime& departureTime
		) const {
			return reservationTime <= getReservationDeadLine(originDateTime, departureTime )
			&& reservationTime >= getReservationStartTime (reservationTime);
		}




		const std::string& 
		ReservationRule::getDescription () const
		{
			return _description;
		}




		void 
		ReservationRule::setDescription (const std::string& description)
		{
			_description = description;
		}



		void 
		ReservationRule::setMinDelayMinutes (int minDelayMinutes)
		{
			_minDelayMinutes = minDelayMinutes;
		}



		void ReservationRule::setMinDelayDays (int minDelayDays)
		{
			_minDelayDays = minDelayDays;
		}





		void ReservationRule::setMaxDelayDays (int maxDelayDays)
		{
			_maxDelayDays = maxDelayDays;
		}




		void 
		ReservationRule::setOnline (bool online)
		{
			_online = online;
		}
		 


		void 
		ReservationRule::setOriginIsReference (bool originIsReference)
		{
			_originIsReference = originIsReference;
		}

	}
}
