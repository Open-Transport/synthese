
/** Site class implementation.
	@file Site.cpp

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

#include "Site.h"

#include "04_time/DateTime.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace env;
	using namespace interfaces;

	namespace transportwebsite
	{
		const string Site::TEMPS_MIN_CIRCULATIONS ("r");
		const string Site::TEMPS_MAX_CIRCULATIONS ("R");

		Site::Site(uid id)
			: Registrable<uid, Site>(id)
			, _startValidityDate(TIME_UNKNOWN)
			, _endValidityDate(TIME_UNKNOWN)
		{
			
		}

		shared_ptr<const Interface> Site::getInterface() const
		{
			return _interface;
		}

		bool Site::getOnlineBookingAllowed() const
		{
			return _onlineBookingAllowed;
		}

		bool Site::dateControl() const
		{
			Date tempDate(TIME_CURRENT);
			return tempDate >= _startValidityDate && tempDate <= _endValidityDate;
		}

		void Site::setInterface(shared_ptr<const Interface> interf )
		{
			_interface = interf;
		}

		void Site::setStartDate( const synthese::time::Date& dateDebut )
		{
			_startValidityDate = dateDebut;
		}

		void Site::setEndDate( const synthese::time::Date& dateFin )
		{
			_endValidityDate = dateFin;
		}

		void Site::setOnlineBookingAllowed( const bool valeur )
		{
			_onlineBookingAllowed = valeur;
		}

		void Site::setPastSolutionsDisplayed( bool pastSolutions)
		{
			_pastSolutionsDisplayed = pastSolutions;
		}

		void Site::setName( const std::string& name )
		{
			_name = name;
		}

		bool Site::getPastSolutionsDisplayed() const
		{
			return _pastSolutionsDisplayed;
		}

		const time::Date Site::getMinUseDate() const
		{
			/// @todo customize
			return Date(TIME_CURRENT);
		}

		const time::Date Site::getMaxUseDate() const
		{
			/// @todo customize 14
			Date date(TIME_CURRENT);
			date += 14;
			return date;
		}

		Date Site::interpretDate( const std::string& text ) const
		{
			if ( text.empty() )
				return Date(TIME_UNKNOWN);

			if ( text == TEMPS_MIN_CIRCULATIONS)
				return getMinUseDate();

			if (text == TEMPS_MAX_CIRCULATIONS)
				return getMaxUseDate();

			if (text.size() == 1)
				return Date(text[ 0 ]);
			
			return Date::FromString(text);
		}



		void Site::setMaxTransportConnectionsCount( int number )
		{
			_maxTransportConnectionsCount = number;
		}



		int Site::getMaxTransportConnectionsCount() const
		{
			return _maxTransportConnectionsCount;
		}



		AccessParameters Site::getDefaultAccessParameters() const
		{
			AccessParameters ap;
			ap.maxTransportConnectionCount = _maxTransportConnectionsCount;
			ap.approachSpeed = 67; // m/min = 4 km/h
			ap.maxApproachDistance = 1500;
			ap.maxApproachTime = 23;
			return ap;
		}



		void Site::applyPeriod(
			const HourPeriod& period
			, DateTime& startTime
			, DateTime& endTime
		) const {
			
			// Updates
			if (period.getEndHour() <= period.getBeginHour())
			{
				endTime.addDaysDuration ( 1 );
			}

			endTime.setHour(period.getEndHour());
			startTime.setHour(period.getBeginHour());

			// Checks
			if (_pastSolutionsDisplayed == false )
			{
				DateTime now(TIME_CURRENT);
				assert(endTime >= now);

				if ( startTime < now) 
					startTime = now;
			}
		}

		void Site::addHourPeriod( const HourPeriod& hourPeriod )
		{
			_periods.push_back(hourPeriod);
		}

		const Site::Periods& Site::getPeriods() const
		{
			return _periods;
		}

		void Site::setUseDateRange( int range )
		{
			_useDateRange = range;
		}
	}
}
