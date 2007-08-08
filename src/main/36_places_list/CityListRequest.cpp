
/** CityListRequest class implementation.
	@file CityListRequest.cpp

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

#include "CityListRequest.h"

#include "36_places_list/PlacesListInterfacePage.h"
#include "36_places_list/Types.h"
#include "36_places_list/Site.h"
#include "36_places_list/PlacesListModule.h"

#include "30_server/RequestException.h"

#include "15_env/EnvModule.h"

#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace server;
	using namespace interfaces;
	using namespace util;
	
	namespace transportwebsite
	{
		const string CityListRequest::PARAMETER_INPUT("t");
		const string CityListRequest::PARAMETER_INPUT_ID("i");
		const string CityListRequest::PARAMETER_NUMBER("n");
		const string CityListRequest::PARAMETER_SITE("s");
		const string CityListRequest::PARAMETER_IS_FOR_ORIGIN("o");

		void CityListRequest::_run( ostream& stream ) const
		{
			/// @todo Read city list from site
			CityList tbCommunes(EnvModule::guessCity(_input, _n ));
			PlacesList placesList;
			for(CityList::const_iterator it(tbCommunes.begin()); it != tbCommunes.end(); ++it)
				placesList.push_back(make_pair((*it)->getKey(), (*it)->getName()));

			VariablesMap vm;
			_page->display(stream, vm, placesList, _request);
		}

		ParametersMap CityListRequest::_getParametersMap() const
		{
			ParametersMap pm;
			pm.insert(make_pair(PARAMETER_INPUT, _input));
			pm.insert(make_pair(PARAMETER_NUMBER, Conversion::ToString(_n)));
			pm.insert(make_pair(PARAMETER_SITE, Conversion::ToString(_site->getKey())));
			return pm;
		}

		void CityListRequest::_setFromParametersMap( const server::ParametersMap& map )
		{
			FunctionWithSite::_setFromParametersMap(map);

			ParametersMap::const_iterator it;

			_page = _site->getInterface()->getPage<PlacesListInterfacePage>();

			it = map.find(PARAMETER_INPUT);
			if (it == map.end())
				throw RequestException("Text input not specified");
			_input = it->second;

			it = map.find(PARAMETER_NUMBER);
			if (it == map.end())
				throw RequestException("Number not specified");
			_n = Conversion::ToInt(it->second);
			if (_n < 0)
				throw RequestException("Bad value for number");
		}
	}
}
