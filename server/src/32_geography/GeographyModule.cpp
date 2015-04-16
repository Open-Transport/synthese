
/** GeographyModule class implementation.
	@file GeographyModule.cpp

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

#include "GeographyModule.h"
#include "City.h"
#include "T9Filter.h"
#include "Exception.h"
#include "NamedPlace.h"
#include "CoordinatesSystem.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "MapSourceTableSync.hpp"

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
		template<> const string FactorableTemplate<ModuleClass, GeographyModule>::FACTORY_KEY = "32_geography";
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<GeographyModule>::NAME = "Géographie";


		template<> void ModuleClassTemplate<GeographyModule>::PreInit()
		{
			RegisterParameter(GeographyModule::MODULE_PARAM_CITY_NAME_BEFORE_PLACE_NAME, "1", &GeographyModule::ParameterCallback);
		}



		template<> void ModuleClassTemplate<GeographyModule>::Init()
		{
			Env env;
			MapSourceTableSync::SearchResult sources(
				MapSourceTableSync::Search(
					env,
					string("OSM"),
					0,
					1
			)	);
			if(sources.empty())
			{
				boost::shared_ptr<MapSource> osmSource(new MapSource(21110623253299200ULL));
				osmSource->setName("OSM");
				osmSource->setCoordinatesSystem(CoordinatesSystem::GetCoordinatesSystem(900913));
				osmSource->setType(MapSource::OSM);
				MapSourceTableSync::Save(osmSource.get());
			}
		}

		template<> void ModuleClassTemplate<GeographyModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<GeographyModule>::End()
		{
			UnregisterParameter(GeographyModule::MODULE_PARAM_CITY_NAME_BEFORE_PLACE_NAME);
		}



		template<> void ModuleClassTemplate<GeographyModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<GeographyModule>::CloseThread(
			
			){
		}
	}


	namespace geography
	{
		const string GeographyModule::MODULE_PARAM_CITY_NAME_BEFORE_PLACE_NAME = "city_name_before_place_name";

		bool GeographyModule::_cityNameBeforePlaceName = true;
		GeographyModule::GeneralAllPlacesMatcher GeographyModule::_generalAllPlacesMatcher;
		GeographyModule::CitiesMatcher GeographyModule::_citiesMatcher;
		GeographyModule::CitiesMatcher GeographyModule::_citiesT9Matcher;



		void GeographyModule::AddToCitiesMatchers(
			GeographyModule::CitiesMatcher::Content city
		){
			// Declarations
			const string key(city->getName());

			_citiesMatcher.add(key, city);
			_generalAllPlacesMatcher.add(key, city);

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



		void GeographyModule::ParameterCallback(
			const std::string& name,
			const std::string& value
		){
			if(name == MODULE_PARAM_CITY_NAME_BEFORE_PLACE_NAME)
			{
				_cityNameBeforePlaceName = !value.empty() && lexical_cast<bool>(value);
			}
		}
}	}
