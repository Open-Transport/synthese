//////////////////////////////////////////////////////////////////////////
/** PTServiceConfig class implementation.
	@file PTServiceConfig.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "PTServiceConfig.hpp"

#include "AccessParameters.h"
#include "PTConstants.h"
#include "PTModule.h"
#include "City.h"
#include "StopArea.hpp"
#include "GeographyModule.h"
#include "LexicalMatcher.h"
#include "RoadChunk.h"
#include "RoadPlace.h"
#include "House.hpp"
#include "Env.h"

#include "Exception.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace geography;
	using namespace lexical_matcher;
	using namespace pt;
	using namespace graph;
	using namespace road;
	using namespace pt_website;

	CLASS_DEFINITION(PTServiceConfig, "t086_pt_services_configurations", 86)
	FIELD_DEFINITION_OF_TYPE(OnlineBookingActivated, "online_booking", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(UseOldData, "use_old_data", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(MaxConnections, "max_connections", SQL_INTEGER)

	// Use dates range
	FIELD_DEFINITION_OF_TYPE(UseDatesRange, "use_dates_range", SQL_INTEGER)
	

	// Display road approach detail
	FIELD_DEFINITION_OF_TYPE(DisplayRoadApproachDetails, "display_road_approach_detail", SQL_BOOLEAN)

	namespace pt_website
	{
		const string PTServiceConfig::TEMPS_MIN_CIRCULATIONS ("r");
		const string PTServiceConfig::TEMPS_MAX_CIRCULATIONS ("R");



		PTServiceConfig::PTServiceConfig(
			RegistryKeyType id
		):	Registrable(id),
			Object<PTServiceConfig, PTServiceConfigSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(OnlineBookingActivated, true),
					FIELD_VALUE_CONSTRUCTOR(UseOldData, false),
					FIELD_VALUE_CONSTRUCTOR(MaxConnections, 10),
					FIELD_VALUE_CONSTRUCTOR(UseDatesRange, 365),
					FIELD_DEFAULT_CONSTRUCTOR(HourPeriods),
					FIELD_VALUE_CONSTRUCTOR(DisplayRoadApproachDetails, true)
			)	)
		{}



		const date PTServiceConfig::getMinUseDate() const
		{
			return day_clock::local_day();
		}



		const date PTServiceConfig::getMaxUseDate() const
		{
			date date(day_clock::local_day());
			date += get<UseDatesRange>();
			return date;
		}



		date PTServiceConfig::interpretDate( const std::string& text ) const
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



		graph::AccessParameters PTServiceConfig::getAccessParameters(
			UserClassCode parameter,
			const graph::AccessParameters::AllowedPathClasses& allowedPathClasses,
			const graph::AccessParameters::AllowedNetworks& allowedNetworks
		) const	{
			switch(parameter)
			{
			case USER_HANDICAPPED:
				return AccessParameters(
					parameter, false, false, 300, posix_time::minutes(23), 0.556, get<MaxConnections>(), allowedPathClasses, allowedNetworks
				);

			case USER_BIKE:
				return AccessParameters(
					parameter, false, false, 3000, posix_time::minutes(23), 4.167, get<MaxConnections>(), allowedPathClasses, allowedNetworks
				);

			case USER_PEDESTRIAN:
			default:
				return AccessParameters(
					USER_PEDESTRIAN, false, false, 1000, posix_time::minutes(23), 1.111, get<MaxConnections>(), allowedPathClasses, allowedNetworks
				);
			}
		}



		void PTServiceConfig::applyPeriod(
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
			if (get<UseOldData>() == false )
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



		const GeographyModule::CitiesMatcher& PTServiceConfig::getCitiesMatcher () const
		{
			return _citiesMatcher.size() ? _citiesMatcher : GeographyModule::GetCitiesMatcher();
		}



		void PTServiceConfig::addCity(boost::shared_ptr<City> city)
		{
			if(!city) return;

			// Conflict control
			string name(city->getName());
			GeographyModule::CitiesMatcher::Map::const_iterator it(_citiesMatcher.entries().find(name));
			if(it != _citiesMatcher.entries().end())
			{
				string oldName(it->first.getSource());
				boost::shared_ptr<City> oldCity(it->second);
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



		const boost::shared_ptr<Place> PTServiceConfig::fetchPlace(
			const string& cityName,
			const string& placeName
		) const {
			return extendedFetchPlace(cityName, placeName).placeResult.value;
		}



		RoadModule::ExtendedFetchPlaceResult PTServiceConfig::extendedFetchPlace(
			const std::string& cityName,
			const std::string& placeName
		) const	{
			return RoadModule::ExtendedFetchPlace(
				getCitiesMatcher(),
				cityName,
				placeName
			);
		}



		void PTServiceConfig::addRollingStockFilter( RollingStockFilter& value )
		{
			_rollingStockFilters[value.getRank()] = &value;
		}



		void PTServiceConfig::removeRollingStockFilter( RollingStockFilter& value )
		{
			RollingStockFilters::iterator it(_rollingStockFilters.find(value.getRank()));
			if(it != _rollingStockFilters.end())
			{
				_rollingStockFilters.erase(it);
			}
		}



		void PTServiceConfig::clearRollingStockFilters()
		{
			_rollingStockFilters.clear();
		}



		PTServiceConfig::Labels PTServiceConfig::getRollingStockFiltersList(
		) const {
			Labels result;
			BOOST_FOREACH(const RollingStockFilters::value_type& it, _rollingStockFilters)
			{
				result.insert(make_pair(it.first, it.second->getName()));
			}
			return result;
		}



		PTServiceConfig::ForbiddenDateException::ForbiddenDateException():
		Exception("Forbidden date")
		{
		}
}	}
