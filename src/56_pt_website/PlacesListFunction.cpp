
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
#include "TransportWebsiteTypes.hpp"
#include "TransportWebsite.h"
#include "TransportWebsiteModule.h"
#include "PlaceAlias.h"
#include "RequestException.h"
#include "RoadPlace.h"
#include "PublicPlace.h"
#include "House.hpp"
#include "City.h"
#include "PTModule.h"
#include "StopArea.hpp"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace pt;
	using namespace road;
	using namespace util;
	using namespace server;
	using namespace lexical_matcher;
	using namespace geography;
	using namespace pt_website;
	using namespace cms;

	template<> const string util::FactorableTemplate<PlacesListFunction::_FunctionWithSite,PlacesListFunction>::FACTORY_KEY("lp");

	namespace pt_website
	{
		const std::string PlacesListFunction::PARAMETER_OLD_INPUT("i");
		const std::string PlacesListFunction::PARAMETER_INPUT("t");
		const std::string PlacesListFunction::PARAMETER_CITY_TEXT("ct");
		const std::string PlacesListFunction::PARAMETER_NUMBER("n");
		const std::string PlacesListFunction::PARAMETER_IS_FOR_ORIGIN("o");
		const std::string PlacesListFunction::PARAMETER_PAGE("p");
		const string PlacesListFunction::PARAMETER_ITEM_PAGE("ip");



		PlacesListFunction::PlacesListFunction()
		{
		}



		ParametersMap PlacesListFunction::_getParametersMap() const
		{
			ParametersMap map(_FunctionWithSite::_getParametersMap());
			map.insert(PARAMETER_INPUT, _input);
			map.insert(PARAMETER_CITY_TEXT, _cityText);
			map.insert(PARAMETER_NUMBER, _n);
			map.insert(PARAMETER_IS_FOR_ORIGIN, _isForOrigin);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE, _page->getKey());
			}
			if(_itemPage.get())
			{
				map.insert(PARAMETER_ITEM_PAGE, _itemPage->getKey());
			}
			return map;
		}



		void PlacesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);
			optional<RegistryKeyType> pageId(map.getOptional<RegistryKeyType>(PARAMETER_PAGE));
			if (pageId) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*pageId);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such web page");
			}
			optional<RegistryKeyType> itemPageId(map.getOptional<RegistryKeyType>(PARAMETER_ITEM_PAGE));
			if (itemPageId) try
			{
				_itemPage = Env::GetOfficialEnv().get<Webpage>(*itemPageId);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such web page");
			}
			_input =
				map.getOptional<string>(PARAMETER_INPUT) ?
				map.get<string>(PARAMETER_INPUT) :
				map.get<string>(PARAMETER_OLD_INPUT);
			_isForOrigin = map.getDefault<bool>(PARAMETER_IS_FOR_ORIGIN, false);
			_n = map.get<int>(PARAMETER_NUMBER);
			_cityText = map.get<string>(PARAMETER_CITY_TEXT);
		}



		void PlacesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{
			const TransportWebsite* site(dynamic_cast<const TransportWebsite*>(_site.get()));
			if(!site) throw RequestException("Incorrect site");

			RoadModule::ExtendedFetchPlacesResult places(
				RoadModule::ExtendedFetchPlaces(site->getCitiesMatcher(), _cityText, _input, _n)
			);
		
			if(_page.get())
			{
				PlacesList placesList;
				BOOST_FOREACH(const RoadModule::ExtendedFetchPlaceResult& place, places)
				{
					placesList.push_back(make_pair(
						dynamic_cast<Registrable*>(place.placeResult.value.get()) ? 
						dynamic_cast<Registrable*>(place.placeResult.value.get())->getKey() : 0,
						place.placeResult.key.getSource()
					)	);
				}

				PlacesListInterfacePage::DisplayPlacesList(
					stream,
					_page,
					_itemPage,
					request,
					placesList,
					_isForOrigin,
					places.front().cityResult.value.get()
				);
			}
			else
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<options xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/places_list.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				;
				BOOST_FOREACH(const RoadModule::ExtendedFetchPlaceResult& it, places)
				{
					stream << "<option type=\"";
					if(	dynamic_cast<StopArea*>(it.placeResult.value.get()) ||
						dynamic_cast<PlaceAlias*>(it.placeResult.value.get())
					){
						stream << "stop";
					}
					else if(dynamic_cast<const PublicPlace*>(it.placeResult.value.get()))
					{
						stream << "publicPlace";
					}
					else if(dynamic_cast<const RoadPlace*>(it.placeResult.value.get()))
					{
						stream << "street";
					}
					else if(dynamic_cast<const House*>(it.placeResult.value.get()))
					{
						stream << "address";
					}
					stream <<
						"\" score=\"" << it.placeResult.score.phoneticScore << "\"" <<
						" levenshtein=\"" << it.placeResult.score.levenshtein << "\">";
					
					if(dynamic_cast<const House*>(it.placeResult.value.get()) && dynamic_cast<const House*>(it.placeResult.value.get())->getHouseNumber())
					{
						stream << *dynamic_cast<const House*>(it.placeResult.value.get())->getHouseNumber() << " ";
					}
					stream << it.placeResult.key.getSource() << "</option>";
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



		bool PlacesListFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string PlacesListFunction::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : "text/xml";
		}
	}
}
