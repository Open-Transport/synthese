
/** CityListFunction class implementation.
	@file CityListFunction.cpp

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

#include "CityListFunction.h"

#include "PlacesListInterfacePage.h"
#include "Types.h"
#include "Site.h"
#include "PlacesListModule.h"
#include "RequestException.h"
#include "City.h"
#include "Interface.h"

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

	template<> const string util::FactorableTemplate<FunctionWithSite,CityListFunction>::FACTORY_KEY("lc");
	
	namespace transportwebsite
	{
		const string CityListFunction::PARAMETER_INPUT("t");
		const string CityListFunction::PARAMETER_NUMBER("n");
		const string CityListFunction::PARAMETER_IS_FOR_ORIGIN("o");
		const string CityListFunction::PARAMETER_PAGE("p");
		
		void CityListFunction::run( std::ostream& stream, const Request& request ) const
		{
			Site::CitiesMatcher::MatchResult matches(
				_site->getCitiesMatcher().bestMatches(_input, _n)
			);

			if(_page)
			{
				PlacesList placesList;
				BOOST_FOREACH(LexicalMatcher<const City*>::MatchHit it, matches)
				{
					placesList.push_back(make_pair(it.value->getKey(), it.key.getSource()));
				}

				VariablesMap vm;
				_page->display(stream, vm, placesList, true, _isForOrigin, NULL, &request);
			}
			else
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
					"<options xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/places_list.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				;
				BOOST_FOREACH(LexicalMatcher<const City*>::MatchHit it, matches)
				{
					stream << "<option type=\"city\" score=\"" << it.score.phoneticScore << "\">" << it.key.getSource() << "</option>";
				}
				stream << "</options>";
			}
		}

		ParametersMap CityListFunction::_getParametersMap() const
		{
			ParametersMap pm(FunctionWithSite::_getParametersMap());
			pm.insert(PARAMETER_INPUT, _input);
			pm.insert(PARAMETER_NUMBER, _n);
			pm.insert(PARAMETER_IS_FOR_ORIGIN, _isForOrigin);
			if(_page) pm.insert(PARAMETER_PAGE, _page->getFactoryKey());
			return pm;
		}

		void CityListFunction::_setFromParametersMap( const server::ParametersMap& map )
		{
			FunctionWithSite::_setFromParametersMap(map);

			if(map.getOptional<string>(PARAMETER_PAGE))
			{
				_page = _site->getInterface()->getPage<PlacesListInterfacePage>(map.get<string>(PARAMETER_PAGE));
			}
			_input = map.get<string>(PARAMETER_INPUT);
			_isForOrigin = map.getDefault<bool>(PARAMETER_IS_FOR_ORIGIN, false);
		
			_n = map.get<int>(PARAMETER_NUMBER);
			if (_n < 0)
				throw RequestException("Bad value for number");
		}

		void CityListFunction::setTextInput( const std::string& text )
		{
			_input = text;
		}

		void CityListFunction::setNumber( int number )
		{
			_n = number;
		}

		void CityListFunction::setIsForOrigin( bool isForOrigin )
		{
			_isForOrigin = isForOrigin;
		}


		CityListFunction::CityListFunction():
			_page(NULL)
		{
		}


		bool CityListFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string CityListFunction::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : "text/xml";
		}
	}
}
