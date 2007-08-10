
/** PlacesListFunction class implementation.
	@file PlacesListFunction.cpp

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

#include "PlacesListFunction.h"

#include "36_places_list/PlacesListInterfacePage.h"
#include "36_places_list/Types.h"
#include "36_places_list/Site.h"
#include "36_places_list/PlacesListModule.h"

#include "30_server/RequestException.h"

#include "15_env/EnvModule.h"
#include "15_env/City.h"

#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace lexmatcher;
	using namespace env;
	using namespace interfaces;

	namespace transportwebsite
	{
		const std::string PlacesListFunction::PARAMETER_INPUT("i");
		const std::string PlacesListFunction::PARAMETER_CITY_ID("ci");
		const std::string PlacesListFunction::PARAMETER_CITY_TEXT("ct");
		const std::string PlacesListFunction::PARAMETER_NUMBER("n");
		const std::string PlacesListFunction::PARAMETER_IS_FOR_ORIGIN("o");
		
		ParametersMap PlacesListFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSite::_getParametersMap());
			map.insert(make_pair(PARAMETER_INPUT, _input));
			map.insert(make_pair(PARAMETER_CITY_ID, _cityId));
			map.insert(make_pair(PARAMETER_CITY_TEXT, _cityText));
			map.insert(make_pair(PARAMETER_NUMBER, Conversion::ToString(_n)));
			map.insert(make_pair(PARAMETER_IS_FOR_ORIGIN, Conversion::ToString(_isForOrigin)));
			return map;
		}

		void PlacesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			FunctionWithSite::_setFromParametersMap(map);
			_page = _site->getInterface()->getPage<PlacesListInterfacePage>();

			ParametersMap::const_iterator it;

			/// @todo Parameters parsing
			it = map.find(PARAMETER_INPUT);
			if (it == map.end())
				throw RequestException("Text input not specified");
			_input = it->second;

			it = map.find(PARAMETER_CITY_ID);
			if (it == map.end())
				throw RequestException("City ID not specified");
			_cityId = it->second;
			if (EnvModule::getCities().contains(Conversion::ToLongLong(_cityId)))
				_city = EnvModule::getCities().get(Conversion::ToLongLong(_cityId));
			else
			{
				it = map.find(PARAMETER_INPUT);
				if (it == map.end())
					throw RequestException("City text not specified");
				_cityText = it->second;
			}

			it = map.find(PARAMETER_IS_FOR_ORIGIN);
			if (it == map.end())
				throw RequestException("Is for origin status not specified");
			_isForOrigin = Conversion::ToBool(it->second);

			it = map.find(PARAMETER_NUMBER);
			if (it == map.end())
				throw RequestException("Number not specified");
			_n = Conversion::ToInt(it->second);

		}

		void PlacesListFunction::_run( std::ostream& stream ) const
		{
			shared_ptr<const City> city(_city);
			if (!city.get())
			{
				CityList cities(EnvModule::guessCity(_cityText, 1));
				if (cities.empty())
				{
					return;
				}
				city = cities.front();
			}

			PlacesList placesList;
			LexicalMatcher<const ConnectionPlace*>::MatchResult places(city->getConnectionPlacesMatcher().bestMatches(_input, _n));

			for(LexicalMatcher<const ConnectionPlace*>::MatchResult::const_iterator it(places.begin()); it != places.end(); ++it)
			{
				placesList.push_back(make_pair(it->value->getKey(), it->value->getName()));
			}

			VariablesMap vm;
			_page->display(stream, vm, placesList, false, _isForOrigin, city, _request);
		}

		void PlacesListFunction::setTextInput( const std::string& text )
		{
			_input = text;
		}

		void PlacesListFunction::setNumber( int number )
		{
			_n = number;
		}

		void PlacesListFunction::setIsForOrigin( bool isForOrigin )
		{
			_isForOrigin = isForOrigin;
		}

		void PlacesListFunction::setCityTextInput( const std::string& text )
		{
			_cityText = text;
		}

		void PlacesListFunction::setCityIdInput( const std::string& text )
		{
			_cityId = text;
		}
	}
}
