
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

#include "PlacesListInterfacePage.h"
#include "Types.h"
#include "Site.h"
#include "PlacesListModule.h"
#include "PlaceAlias.h"
#include "RequestException.h"
#include "RoadPlace.h"
#include "PublicPlace.h"
#include "Address.h"
#include "City.h"
#include "EnvModule.h"
#include "PublicTransportStopZoneConnectionPlace.h"

#include "Interface.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace road;
	using namespace util;
	using namespace server;
	using namespace lexmatcher;
	using namespace geography;
	using namespace interfaces;

	template<> const string util::FactorableTemplate<transportwebsite::FunctionWithSite,transportwebsite::PlacesListFunction>::FACTORY_KEY("lp");

	namespace transportwebsite
	{
		const std::string PlacesListFunction::PARAMETER_OLD_INPUT("i");
		const std::string PlacesListFunction::PARAMETER_INPUT("t");
		const std::string PlacesListFunction::PARAMETER_CITY_TEXT("ct");
		const std::string PlacesListFunction::PARAMETER_NUMBER("n");
		const std::string PlacesListFunction::PARAMETER_IS_FOR_ORIGIN("o");
		const std::string PlacesListFunction::PARAMETER_PAGE("p");

		PlacesListFunction::PlacesListFunction()
			: _page(NULL)
		{
		}

		ParametersMap PlacesListFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSite::_getParametersMap());
			map.insert(PARAMETER_INPUT, _input);
			map.insert(PARAMETER_CITY_TEXT, _cityText);
			map.insert(PARAMETER_NUMBER, _n);
			map.insert(PARAMETER_IS_FOR_ORIGIN, _isForOrigin);
			if(_page) map.insert(PARAMETER_PAGE, _page->getFactoryKey());
			return map;
		}

		void PlacesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			FunctionWithSite::_setFromParametersMap(map);
			if(map.getOptional<string>(PARAMETER_PAGE))
			{
				_page = _site->getInterface()->getPage<PlacesListInterfacePage>(map.get<string>(PARAMETER_PAGE));
			}
			_input =
				map.getOptional<string>(PARAMETER_INPUT) ?
				map.get<string>(PARAMETER_INPUT) :
				map.get<string>(PARAMETER_OLD_INPUT);
			_isForOrigin = map.getDefault<bool>(PARAMETER_IS_FOR_ORIGIN, false);
			_n = map.get<int>(PARAMETER_NUMBER);
			_cityText = map.get<string>(PARAMETER_CITY_TEXT);
		}

		void PlacesListFunction::_run( std::ostream& stream ) const
		{
			Site::CitiesMatcher::MatchResult cities(
				_site->getCitiesMatcher().bestMatches(_cityText, 1)
			);
			if (cities.empty())
			{
				return;
			}
			const City* city(cities.front().value);

			City::PlacesMatcher::MatchResult places(
				city->getAllPlacesMatcher().bestMatches(_input, _n)
			);

			if(_page)
			{
				PlacesList placesList;
				BOOST_FOREACH(const City::PlacesMatcher::MatchHit it, places)
				{
					placesList.push_back(make_pair(UNKNOWN_VALUE /*it.value->getKey()*/, it.key.getSource()));
				}

				VariablesMap vm;
				_page->display(stream, vm, placesList, false, _isForOrigin, city, _request);
			}
			else
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
					"<options xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/places_list.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				;
				BOOST_FOREACH(const City::PlacesMatcher::MatchHit it, places)
				{
					stream << "<option type=\"";
					if(	dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(it.value) ||
						dynamic_cast<const PlaceAlias*>(it.value)
					){
						stream << "stop";
					}
					else if(dynamic_cast<const PublicPlace*>(it.value))
					{
						stream << "publicPlace";
					}
					else if(dynamic_cast<const RoadPlace*>(it.value))
					{
						stream << "street";
					}
					else if(dynamic_cast<const Address*>(it.value))
					{
						stream << "address";
					}
					stream << "\" score=\"" << it.score << "\">" << it.key.getSource() << "</option>";
				}
				stream << "</options>";
			}
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



		bool PlacesListFunction::_isAuthorized(
		) const {
			return true;
		}

		std::string PlacesListFunction::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : "text/xml";
		}
	}
}
