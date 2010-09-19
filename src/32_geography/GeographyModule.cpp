
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
#include "CoordinatesSystem.hpp"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"

#include <sstream>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace geography;
	using namespace server;
	using namespace db;


	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,GeographyModule>::FACTORY_KEY("32_geography");
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
			_citiesMatcher.add(city->getName() +" "+ city->getCode(), city);

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
			_citiesMatcher.remove(city->getName() +" "+ city->getCode());

			stringstream ss;
			boost::iostreams::filtering_ostream out;
			out.push (T9Filter());
			out.push (ss);
			out << city->getName() << flush;

			_citiesT9Matcher.remove(ss.str());
		}



		GeographyModule::CityList GeographyModule::GuessCity (
			const std::string& fuzzyName,
			int nbMatches,
			bool t9
		){
			CityList result;
			CitiesMatcher::MatchResult matches = (t9 ? _citiesT9Matcher : _citiesMatcher).bestMatches(fuzzyName, nbMatches);
			BOOST_FOREACH(const CitiesMatcher::MatchResult::value_type& it, matches)
			{
				result.push_back(it.value);
			}
			return result;
		}



		const GeographyModule::CitiesMatcher& GeographyModule::GetCitiesMatcher()
		{
			return _citiesMatcher;
		}
	}
}
