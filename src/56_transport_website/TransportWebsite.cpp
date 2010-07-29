
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
#include "PTModule.h"
#include "City.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "AccessParameters.h"
#include "GeographyModule.h"
#include "LexicalMatcher.h"

#include "01_util/Exception.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace std;
	using namespace util;
	using namespace geography;
	using namespace lexmatcher;
	using namespace pt;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<transportwebsite::Site>::KEY("Site");
	}

	namespace transportwebsite
	{
		const string Site::TEMPS_MIN_CIRCULATIONS ("r");
		const string Site::TEMPS_MAX_CIRCULATIONS ("R");

		Site::Site(
			RegistryKeyType id
		):	Registrable(id),
			_startValidityDate(not_a_date_time),
			_endValidityDate(not_a_date_time),
			_displayRoadApproachDetail(true),
			_defaultTemplate(NULL)
		{		
		}



		bool Site::dateControl() const
		{
			date tempDate(day_clock::local_day());
			return tempDate >= _startValidityDate && tempDate <= _endValidityDate;
		}

		

		void Site::setStartDate( const date& dateDebut )
		{
			_startValidityDate = dateDebut;
		}

		void Site::setEndDate( const date& dateFin )
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



		bool Site::getPastSolutionsDisplayed() const
		{
			return _pastSolutionsDisplayed;
		}

		const date Site::getMinUseDate() const
		{
			return day_clock::local_day();
		}

		const date Site::getMaxUseDate() const
		{
			date date(day_clock::local_day());
			date += _useDateRange;
			return date;
		}

		date Site::interpretDate( const std::string& text ) const
		{
			if ( text.empty() )
				return date(not_a_date_time);

			if ( text == TEMPS_MIN_CIRCULATIONS)
				return getMinUseDate();

			if (text == TEMPS_MAX_CIRCULATIONS)
				return getMaxUseDate();

			if (text.size() == 1)
				return day_clock::local_day();
			
			return date(from_string(text));
		}



		void Site::setMaxTransportConnectionsCount( int number )
		{
			_maxTransportConnectionsCount = number;
		}



		int Site::getMaxTransportConnectionsCount() const
		{
			return _maxTransportConnectionsCount;
		}



		graph::AccessParameters Site::getAccessParameters(
			UserClassCode parameter,
			const graph::AccessParameters::AllowedPathClasses& allowedPathClasses
		) const	{
			AccessParameters ap;

			switch(parameter)
			{
			case USER_HANDICAPPED:
				return AccessParameters(
					parameter, false, false, 300, posix_time::minutes(23), 34, _maxTransportConnectionsCount, allowedPathClasses
				);

			case USER_BIKE:
				return AccessParameters(
					parameter, false, false, 3000, posix_time::minutes(23), 201, _maxTransportConnectionsCount, allowedPathClasses
				);

			case USER_PEDESTRIAN:
			default:
				return AccessParameters(
					USER_PEDESTRIAN, false, false, 1000, posix_time::minutes(23), 67, _maxTransportConnectionsCount, allowedPathClasses
				);
			}
		}



		void Site::applyPeriod(
			const HourPeriod& period
			, ptime& startTime
			, ptime& endTime
		) const {
			
			// Updates
			if (period.getEndHour() <= period.getBeginHour())
			{
				endTime += days( 1 );
			}

			endTime = ptime(endTime.date(), period.getEndHour());
			startTime = ptime(startTime.date(), period.getBeginHour());

			// Checks
			if (_pastSolutionsDisplayed == false )
			{
				ptime now(second_clock::local_time());
				if (endTime < now)
				{
					throw ForbiddenDateException();
				}
					
				if(startTime < now) 
				{
					startTime = now;
				}
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



		date_duration Site::getUseDatesRange() const
		{
			return _useDateRange;
		}



		const date& Site::getStartDate() const
		{
			return _startValidityDate;
		}



		const date& Site::getEndDate() const
		{
			return _endValidityDate;
		}
		
		
		
		const Site::CitiesMatcher& Site::getCitiesMatcher () const
		{
			return _citiesMatcher.size() ? _citiesMatcher : GeographyModule::GetCitiesMatcher();
		}
		
		
		
		void Site::addCity(City* city)
		{
			if(!city) return;
			
			// Conflict control
			string name(city->getName());
			CitiesMatcher::Map::const_iterator it(_citiesMatcher.entries().find(name));
			if(it != _citiesMatcher.entries().end())
			{
				string oldName(it->first.getSource());
				City* oldCity(it->second);
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



		const Place* Site::fetchPlace(
			const string& cityName,
			const string& placeName
		) const {
			return extendedFetchPlace(cityName, placeName).placeResult.value;
		}



		Site::ExtendedFetchPlaceResult Site::extendedFetchPlace(
			const std::string& cityName,
			const std::string& placeName
		) const	{
			ExtendedFetchPlaceResult result;

			if (cityName.empty())
				throw Exception("Empty city name");

			CitiesMatcher::MatchResult cities(
				getCitiesMatcher().bestMatches(cityName,1)
			);
			if(cities.empty()) throw Exception("An error has occured in city name search");
			result.cityResult = cities.front();
			result.placeResult.key = result.cityResult.key;
			result.placeResult.score = result.cityResult.score;
			result.placeResult.value = result.cityResult.value;

			assert(result.placeResult.value != NULL);
			
			if (!placeName.empty())
			{
				City::PlacesMatcher::MatchResult places(
					result.cityResult.value->getAllPlacesMatcher().bestMatches(placeName, 1)
				);
				if (!places.empty())
				{
					result.placeResult.key = places.front().key;
					result.placeResult.score = places.front().score;
					result.placeResult.value = places.front().value;
				}
			}

			return result;
		}



		void Site::addRollingStockFilter( RollingStockFilter& value )
		{
			_rollingStockFilters[value.getRank()] = &value;
		}



		void Site::removeRollingStockFilter( RollingStockFilter& value )
		{
			RollingStockFilters::iterator it(_rollingStockFilters.find(value.getRank()));
			if(it != _rollingStockFilters.end())
			{
				_rollingStockFilters.erase(it);
			}
		}



		void Site::clearRollingStockFilters()
		{
			_rollingStockFilters.clear();
		}



		const Site::RollingStockFilters& Site::getRollingStockFilters() const
		{
			return _rollingStockFilters;
		}



		Site::Labels Site::getRollingStockFiltersList(
		) const {
			Labels result;
			BOOST_FOREACH(const RollingStockFilters::value_type& it, _rollingStockFilters)
			{
				result.insert(make_pair(it.first, it.second->getName()));
			}
			return result;
		}



		void Site::clearHourPeriods()
		{
			_periods.clear();
		}



		Site::ExtendedFetchPlaceResult::ExtendedFetchPlaceResult()
		{
			cityResult.value = NULL;
			placeResult.value = NULL;
		}



		Site::ForbiddenDateException::ForbiddenDateException():
			Exception("Forbidden date")
		{

		}
	}
}
