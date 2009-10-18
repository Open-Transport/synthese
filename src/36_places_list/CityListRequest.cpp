
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

#include "PlacesListInterfacePage.h"
#include "Types.h"
#include "Site.h"
#include "PlacesListModule.h"
#include "RequestException.h"
#include "City.h"
#include "Interface.h"
#include "Conversion.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geography;
	using namespace server;
	using namespace interfaces;
	using namespace util;
	using namespace lexmatcher;
	using namespace transportwebsite;

	template<> const string util::FactorableTemplate<FunctionWithSite,CityListRequest>::FACTORY_KEY("lc");
	
	namespace transportwebsite
	{
		const string CityListRequest::PARAMETER_INPUT("t");
		const string CityListRequest::PARAMETER_NUMBER("n");
		const string CityListRequest::PARAMETER_IS_FOR_ORIGIN("o");
		
		void CityListRequest::_run( ostream& stream ) const
		{
			PlacesList placesList;
			
			Site::CitiesMatcher::MatchResult matches(
				_site->getCitiesMatcher().findCombined(_input, _n)
			);
			//for(LexicalMatcher<const City*>::MatchResult::iterator it(matches.begin());
			//	it != matches.end();
			//	++it
			BOOST_FOREACH(LexicalMatcher<const City*>::MatchHit it, matches)
			{
				placesList.push_back(make_pair(it.value->getKey(), it.key.getSource()));
			}

			VariablesMap vm;
			_page->display(stream, vm, placesList, true, _isForOrigin, NULL, _request);
		}

		ParametersMap CityListRequest::_getParametersMap() const
		{
			ParametersMap pm(FunctionWithSite::_getParametersMap());
			pm.insert(PARAMETER_INPUT, _input);
			pm.insert(PARAMETER_NUMBER, _n);
			pm.insert(PARAMETER_IS_FOR_ORIGIN, _isForOrigin);
			return pm;
		}

		void CityListRequest::_setFromParametersMap( const server::ParametersMap& map )
		{
			FunctionWithSite::_setFromParametersMap(map);

			_page = _site->getInterface()->getPage<PlacesListInterfacePage>();
			_input = map.getString(PARAMETER_INPUT, true, FACTORY_KEY);
			_isForOrigin = map.getBool(PARAMETER_IS_FOR_ORIGIN, false, false, FACTORY_KEY);
		
			_n = map.getInt(PARAMETER_NUMBER, true, FACTORY_KEY);
			if (_n < 0)
				throw RequestException("Bad value for number");
		}

		void CityListRequest::setTextInput( const std::string& text )
		{
			_input = text;
		}

		void CityListRequest::setNumber( int number )
		{
			_n = number;
		}

		void CityListRequest::setIsForOrigin( bool isForOrigin )
		{
			_isForOrigin = isForOrigin;
		}



		bool CityListRequest::_isAuthorized(
		) const {
			return true;
		}

		std::string CityListRequest::getOutputMimeType() const
		{
			return _page->getMimeType();
		}
	}
}
