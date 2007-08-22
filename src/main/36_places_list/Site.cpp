
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

#include "15_env/EnvModule.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/BikeCompliance.h"
#include "15_env/ReservationRule.h"
#include "15_env/City.h"
#include "15_env/ConnectionPlace.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "04_time/DateTime.h"

#include "01_util/Exception.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace env;
	using namespace interfaces;
	using namespace lexmatcher;

	namespace util
	{
		template<> typename Registrable<uid,transportwebsite::Site>::Registry Registrable<uid,transportwebsite::Site>::_registry;
	}

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
			return Date(TIME_CURRENT);
		}

		const time::Date Site::getMaxUseDate() const
		{
			Date date(TIME_CURRENT);
			date += _useDateRange;
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



		AccessParameters Site::getAccessParameters(AccessibilityParameter parameter) const
		{
			AccessParameters ap;

			switch(parameter)
			{
			case HANDICCAPED_ACCESSIBILITY:
				{
					HandicappedCompliance* hc(new HandicappedCompliance);
					hc->setCompliant(true);
					ap.complyer.setHandicappedCompliance(hc);
					BikeCompliance* bc(new BikeCompliance);
					bc->setCompliant(logic::indeterminate);
					ap.complyer.setBikeCompliance(bc);
					ap.approachSpeed = 34; // m/min = 2 km/h
					ap.maxApproachDistance = 300;
				}
				break;

			case BIKE_ACCESSIBILITY:
				{
					HandicappedCompliance* hc(new HandicappedCompliance);
					hc->setCompliant(logic::indeterminate);
					ap.complyer.setHandicappedCompliance(hc);
					BikeCompliance* bc(new BikeCompliance);
					bc->setCompliant(true);
					ap.complyer.setBikeCompliance(bc);
					ap.approachSpeed = 201; // m/min = 12 km/h
					ap.maxApproachDistance = 3000;
				}
				break;

			default:
				{
					ap.approachSpeed = 67; // m/min = 4 km/h
					ap.maxApproachDistance = 1000;
				}
			}

			// Temporary
			ReservationRule* resa(new ReservationRule);
			resa->setCompliant(boost::logic::indeterminate);
			ap.complyer.setReservationRule(resa);
			ap.maxApproachTime = 23;

			ap.maxTransportConnectionCount = _maxTransportConnectionsCount;
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

		const env::Place* Site::fetchPlace(
			const std::string& cityName
			, const std::string& placeName
		) const {
			const Place* place(NULL);

			if (cityName.empty())
				throw Exception("Empty city name");

			shared_ptr<const City> city;
			CityList cityList = EnvModule::guessCity(cityName, 1);
			if (cityName.empty())
				throw Exception("An error has occured in city name search");
			city = cityList.front();
			place = city.get();
			assert(place != NULL);

			if (!placeName.empty())
			{
				LexicalMatcher<const ConnectionPlace*>::MatchResult places = city->getConnectionPlacesMatcher().bestMatches(placeName, 1);
				if (!places.empty())
				{
					place = places.front().value;
				}
			}

			return place;		
		}
	}
}
