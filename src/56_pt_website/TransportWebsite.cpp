
/** TransportWebsite class implementation.
	@file TransportWebsite.cpp

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

#include "TransportWebsite.h"
#include "Registry.h"
#include "PTConstants.h"
#include "PTModule.h"
#include "City.h"
#include "StopArea.hpp"
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
	using namespace lexical_matcher;
	using namespace pt;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<pt_website::TransportWebsite>::KEY("TransportWebsite");
	}

	namespace pt_website
	{
		const string TransportWebsite::TEMPS_MIN_CIRCULATIONS ("r");
		const string TransportWebsite::TEMPS_MAX_CIRCULATIONS ("R");

		TransportWebsite::TransportWebsite(
			RegistryKeyType id
		):	Registrable(id),
			_displayRoadApproachDetail(true)
		{		
		}



		const date TransportWebsite::getMinUseDate() const
		{
			return day_clock::local_day();
		}



		const date TransportWebsite::getMaxUseDate() const
		{
			date date(day_clock::local_day());
			date += _useDateRange;
			return date;
		}



		date TransportWebsite::interpretDate( const std::string& text ) const
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



		graph::AccessParameters TransportWebsite::getAccessParameters(
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



		void TransportWebsite::applyPeriod(
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

		void TransportWebsite::addHourPeriod( const HourPeriod& hourPeriod )
		{
			_periods.push_back(hourPeriod);
		}


		
		const TransportWebsite::CitiesMatcher& TransportWebsite::getCitiesMatcher () const
		{
			return _citiesMatcher.size() ? _citiesMatcher : GeographyModule::GetCitiesMatcher();
		}
		
		
		
		void TransportWebsite::addCity(City* city)
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



		const Place* TransportWebsite::fetchPlace(
			const string& cityName,
			const string& placeName
		) const {
			return extendedFetchPlace(cityName, placeName).placeResult.value;
		}



		TransportWebsite::ExtendedFetchPlaceResult TransportWebsite::extendedFetchPlace(
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



		void TransportWebsite::addRollingStockFilter( RollingStockFilter& value )
		{
			_rollingStockFilters[value.getRank()] = &value;
		}



		void TransportWebsite::removeRollingStockFilter( RollingStockFilter& value )
		{
			RollingStockFilters::iterator it(_rollingStockFilters.find(value.getRank()));
			if(it != _rollingStockFilters.end())
			{
				_rollingStockFilters.erase(it);
			}
		}



		void TransportWebsite::clearRollingStockFilters()
		{
			_rollingStockFilters.clear();
		}



		TransportWebsite::Labels TransportWebsite::getRollingStockFiltersList(
		) const {
			Labels result;
			BOOST_FOREACH(const RollingStockFilters::value_type& it, _rollingStockFilters)
			{
				result.insert(make_pair(it.first, it.second->getName()));
			}
			return result;
		}



		void TransportWebsite::clearHourPeriods()
		{
			_periods.clear();
		}



		TransportWebsite::ExtendedFetchPlaceResult::ExtendedFetchPlaceResult()
		{
			cityResult.value = NULL;
			placeResult.value = NULL;
		}



		TransportWebsite::ForbiddenDateException::ForbiddenDateException():
			Exception("Forbidden date")
		{

		}
	}
}
