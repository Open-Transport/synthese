
/** GeographyModule class implementation.
	@file GeographyModule.cpp

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

#include "GeographyModule.h"
#include "City.h"
#include "T9Filter.h"
#include "Exception.h"
#include "NamedPlace.h"

#include <sstream>
#include <boost/iostreams/filtering_stream.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace geography;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,GeographyModule>::FACTORY_KEY("14_geography");
	}
	
	
	namespace server
	{
		template<> const string ModuleClassTemplate<GeographyModule>::NAME("Géographie");
	
		template<> void ModuleClassTemplate<GeographyModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<GeographyModule>::Init()
		{
		}
		
		template<> void ModuleClassTemplate<GeographyModule>::End()
		{
		}
	}


	namespace geography
	{
		GeographyModule::CitiesMatcher GeographyModule::_citiesMatcher; 
		GeographyModule::CitiesMatcher GeographyModule::_citiesT9Matcher;



		void GeographyModule::AddToCitiesMatchers(
			GeographyModule::CitiesMatcher::Content city
		){
			_citiesMatcher.add(city->getName (), city);

			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (T9Filter());
			out.push (ss);
			out << city->getName() << flush;

			_citiesT9Matcher.add(ss.str(), city);
		}



		void GeographyModule::RemoveFromCitiesMatchers(
			GeographyModule::CitiesMatcher::Content city
		){
			_citiesMatcher.remove(city->getName());

			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (T9Filter());
			out.push (ss);
			out << city->getName() << flush;

			_citiesT9Matcher.remove(ss.str());
		}



		const Place* GeographyModule::FetchPlace(
			const std::string& cityName,
			const std::string& placeName
		){
			const Place* place(NULL);

			if (cityName.empty())
				throw Exception("Empty city name");

			CityList cityList = GuessCity(cityName, 1);
			if (cityName.empty())
				throw Exception("An error has occured in city name search");
			CitiesMatcher::Content city(cityList.front());
			place = city;
			assert(place != NULL);

			if (!placeName.empty())
			{
				City::PlacesMatcher::MatchResult places(city->getAllPlacesMatcher().bestMatches(placeName, 1));
				if (!places.empty())
				{
					place = places.front().value;
				}
			}

			return place;		
		}



		GeographyModule::CityList GeographyModule::GuessCity (
			const std::string& fuzzyName,
			int nbMatches,
			bool t9
		){
			CityList result;
			CitiesMatcher::MatchResult matches = (t9 ? _citiesT9Matcher : _citiesMatcher).bestMatches (fuzzyName, nbMatches);
			BOOST_FOREACH(const CitiesMatcher::MatchResult::value_type& it, matches)
			{
				result.push_back(it.value);
			}
			return result;
		}

	}
}
