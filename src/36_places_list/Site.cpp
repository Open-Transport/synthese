
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
#include "Registry.h"
#include "PTConstants.h"
#include "EnvModule.h"
#include "City.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "AccessParameters.h"

#include "LexicalMatcher.h"

#include "DateTime.h"

#include "01_util/Exception.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace geography;
	using namespace interfaces;
	using namespace lexmatcher;
	using namespace env;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<transportwebsite::Site>::KEY("Site");
	}

	namespace transportwebsite
	{
		const string Site::TEMPS_MIN_CIRCULATIONS ("r");
		const string Site::TEMPS_MAX_CIRCULATIONS ("R");

		Site::Site(RegistryKeyType id)
			: Registrable(id)
			, _startValidityDate(TIME_UNKNOWN)
			, _endValidityDate(TIME_UNKNOWN)
		{
			
		}

		const Interface* Site::getInterface() const
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

		void Site::setInterface(const Interface* interf )
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



		graph::AccessParameters Site::getAccessParameters(AccessibilityParameter parameter) const
		{
			AccessParameters ap;

			switch(parameter)
			{
			case HANDICCAPED_ACCESSIBILITY:
				return AccessParameters(
					USER_HANDICAPPED, false, false, 300, 23, 34, _maxTransportConnectionsCount
				);

			case BIKE_ACCESSIBILITY:
				return AccessParameters(
					USER_BIKE_IN_PT, false, false, 3000, 23, 201, _maxTransportConnectionsCount
				);

			default:
				return AccessParameters(
					USER_PEDESTRIAN, false, false, 1000, 23, 67, _maxTransportConnectionsCount
				);
			}
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

		const Place* Site::fetchPlace(
			const string& cityName,
			const string& placeName
		) const {
			const Place* place(NULL);

			if (cityName.empty())
				throw Exception("Empty city name");

			CitiesMatcher::MatchResult cities(
				_citiesMatcher.bestMatches(cityName,1)
			);
			if(cities.empty()) throw Exception("An error has occured in city name search");
			const City* city(cities.front().value);
			place = city;
			assert(place != NULL);

			if (!placeName.empty())
			{
				City::PlacesMatcher::MatchResult places(
					city->getAllPlacesMatcher().bestMatches(placeName, 1)
				);
				if (!places.empty())
				{
					place = places.front().value;
				}
			}

			return place;
		}

		const std::string& Site::getName() const
		{
			return _name;
		}



		int Site::getUseDatesRange() const
		{
			return _useDateRange;
		}



		const time::Date& Site::getStartDate() const
		{
			return _startValidityDate;
		}



		const time::Date& Site::getEndDate() const
		{
			return _endValidityDate;
		}
		
		
		
		const Site::CitiesMatcher& Site::getCitiesMatcher () const
		{
			return _citiesMatcher;
		}
		
		
		
		void Site::addCity(const City* city)
		{
			if(!city) return;
			
			// Conflict control
			string name(city->getName());
			CitiesMatcher::Map::const_iterator it(_citiesMatcher.entries().find(name));
			if(it != _citiesMatcher.entries().end())
			{
				string oldName(it->first);
				const City* oldCity(it->second);
				_citiesMatcher.remove(oldName);
				_citiesMatcher.add(oldName + " (" + oldCity->getCode().substr(0,2) + ")", oldCity);
				name += " (" + city->getCode().substr(0,2) + ")";
			}
			
			// Already resolved conflict control
			//TODO Implement it
			
			_citiesMatcher.add(name, city);
			
						
/*			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (T9Filter());
			out.push (ss);
			out << city->getName() << flush;
			
			_citiesT9Matcher.add(ss.str(), city->getKey());
*/
		}
	}
}
