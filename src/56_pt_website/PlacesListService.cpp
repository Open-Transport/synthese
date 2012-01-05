
//////////////////////////////////////////////////////////////////////////////////////////
///	PlacesListService class implementation.
///	@file PlacesListService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PlacesListService.hpp"

#include "City.h"
#include "GeographyModule.h"
#include "HTMLTable.h"
#include "PTModule.h"
#include "PublicPlace.h"
#include "Request.h"
#include "RequestException.h"
#include "RoadPlace.h"
#include "StopArea.hpp"
#include "TransportWebsite.h"
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace cms;
	using namespace geography;
	using namespace html;
	using namespace lexical_matcher;
	using namespace pt;
	using namespace road;
	using namespace security;
	using namespace server;
	using namespace util;

	template<> const string util::FactorableTemplate<Function,pt_website::PlacesListService>::FACTORY_KEY("places_list");
	
	namespace pt_website
	{
		const string PlacesListService::PARAMETER_CITY_ID = "city_id";
		const string PlacesListService::PARAMETER_CITIES_WITH_AT_LEAST_A_STOP = "cities_with_at_least_a_stop";
		const string PlacesListService::PARAMETER_CLASS_PAGE_ID = "class_page_id";
		const string PlacesListService::PARAMETER_ITEM_PAGE_ID = "item_page_id";
		const string PlacesListService::PARAMETER_OUTPUT_FORMAT = "output_format";
		const string PlacesListService::PARAMETER_SITE_ID = "site_id";
		const string PlacesListService::PARAMETER_SORTED = "sorted";
		const string PlacesListService::PARAMETER_TEXT = "text";
		const string PlacesListService::PARAMETER_NUMBER = "number";

		const string PlacesListService::DATA_ADDRESS = "address";
		const string PlacesListService::DATA_ADDRESSES = "addresses";
		const string PlacesListService::DATA_CITIES = "cities";
		const string PlacesListService::DATA_CITY = "city";
		const string PlacesListService::DATA_CLASS = "class";
		const string PlacesListService::DATA_CONTENT = "content";
		const string PlacesListService::DATA_KEY = "key";
		const string PlacesListService::DATA_LEVENSHTEIN = "levenshtein";
		const string PlacesListService::DATA_PHONETIC_SCORE = "phonetic_score";
		const string PlacesListService::DATA_PHONETIC_STRING = "phonetic_string";
		const string PlacesListService::DATA_PLACES = "places";
		const string PlacesListService::DATA_PUBLIC_PLACE = "public_place";
		const string PlacesListService::DATA_PUBLIC_PLACES = "public_places";
		const string PlacesListService::DATA_RANK = "rank";
		const string PlacesListService::DATA_ROAD = "road";
		const string PlacesListService::DATA_ROADS = "roads";
		const string PlacesListService::DATA_STOP = "stop";
		const string PlacesListService::DATA_STOPS = "stops";

		const string PlacesListService::VALUE_JSON = "json";
		const string PlacesListService::VALUE_XML = "xml";



		PlacesListService::PlacesListService():
			_sorted(true),
			_citiesWithAtLeastAStop(true)
		{}



		ParametersMap PlacesListService::_getParametersMap() const
		{
			ParametersMap map;

			// Text
			if(!_text.empty())
			{
				map.insert(PARAMETER_TEXT, _text);
			}

			// Sorted (default is true)a
			if(!_sorted)
			{
				map.insert(PARAMETER_SORTED, _sorted);
			}

			// Site filter
			if(_site.get())
			{
				map.insert(PARAMETER_SITE_ID, _site->getKey());
			}

			// City filter
			if(_city.get())
			{
				map.insert(PARAMETER_CITY_ID, _city->getKey());
			}

			// Cities with at least a stop (default is true)a
			if(!_citiesWithAtLeastAStop)
			{
				map.insert(PARAMETER_CITIES_WITH_AT_LEAST_A_STOP, _citiesWithAtLeastAStop);
			}

			// Number
			if(_number)
			{
				map.insert(PARAMETER_NUMBER, *_number);
			}

			// Output format
			if(_itemPage.get())
			{
				map.insert(PARAMETER_ITEM_PAGE_ID, _itemPage->getKey());
				if(_classPage.get())
				{
					map.insert(PARAMETER_CLASS_PAGE_ID, _classPage->getKey());
				}
			}
			else
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}

			return map;
		}



		void PlacesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Text
			_text = map.getDefault<string>(PARAMETER_TEXT);

			// Number
			_number = map.getOptional<size_t>(PARAMETER_NUMBER);

			// Site filter
			RegistryKeyType siteId(map.getDefault<RegistryKeyType>(PARAMETER_SITE_ID, 0));
			if(siteId > 0) try
			{
				_site = Env::GetOfficialEnv().get<TransportWebsite>(siteId);
			}
			catch(ObjectNotFoundException<TransportWebsite>&)
			{
				throw RequestException("No such site");
			}

			// City filter
			RegistryKeyType cityId(map.getDefault<RegistryKeyType>(PARAMETER_CITY_ID, 0));
			if(cityId > 0) try
			{
				_city = Env::GetOfficialEnv().get<City>(cityId);
			}
			catch(ObjectNotFoundException<City>&)
			{
				throw RequestException("No such city");
			}

			// Sorted
			_sorted = map.getDefault<bool>(PARAMETER_SORTED, _sorted);

			// Cities with at least a stop
			_citiesWithAtLeastAStop = map.getDefault<bool>(PARAMETER_CITIES_WITH_AT_LEAST_A_STOP, _citiesWithAtLeastAStop);

			// Output
			if(map.isDefined(PARAMETER_ITEM_PAGE_ID))
			{
				try
				{
					_itemPage = Env::GetOfficialEnv().get<Webpage>(
						map.get<RegistryKeyType>(PARAMETER_ITEM_PAGE_ID)
					);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such item page");

				}

				if(map.isDefined(PARAMETER_CLASS_PAGE_ID))
				{
					try
					{
						_classPage = Env::GetOfficialEnv().get<Webpage>(
							map.get<RegistryKeyType>(PARAMETER_CLASS_PAGE_ID)
						);
					}
					catch (ObjectNotFoundException<Webpage>&)
					{
						throw RequestException("No such class page");
					}
				}
			}
			else
			{
				_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT, VALUE_XML);
			}
		}



		ParametersMap PlacesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// ParametersMap populating
			ParametersMap result;
			if(_sorted)
			{
				if(_city.get())
				{
					// Stops
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(StopArea::FACTORY_KEY).bestMatches(_text, _number ? *_number : 0)
						);
						result.insert(DATA_STOPS, pm);
					}

					// Roads
					{ /// TODO catch addresses
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(
								_text,
								_number ? *_number : 0
							)
						);
						result.insert(DATA_ROADS, pm);
					}

					// Public places
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(PublicPlace::FACTORY_KEY).bestMatches(_text, _number ? *_number : 0)
						);
						result.insert(DATA_PUBLIC_PLACES, pm);
					}
				}
				else if(_site.get())
				{
					 /// TODO implement it
				}
				else
				{
					// Cities
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<City>(
							*pm,
							GeographyModule::GetCitiesMatcher().bestMatches(
								_text,
								(_number && !_citiesWithAtLeastAStop) ? *_number : 0
						)	);
						result.insert(DATA_CITIES, pm);
					}

					// Stops
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<StopArea>(
							*pm,
							PTModule::GetGeneralStopsMatcher().bestMatches(_text, _number ? *_number : 0)
						);
						result.insert(DATA_STOPS, pm);
					}

					// Roads
					{ /// TODO catch addresses
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<RoadPlace>(
							*pm,
							RoadModule::GetGeneralRoadsMatcher().bestMatches(_text, _number ? *_number : 0)
						);
						result.insert(DATA_ROADS, pm);
					}

					// Public places
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<PublicPlace>(
							*pm,
							RoadModule::GetGeneralPublicPlacesMatcher().bestMatches(_text, _number ? *_number : 0)
						);
						result.insert(DATA_PUBLIC_PLACES, pm);
					}
				}
			}
			else
			{
				shared_ptr<ParametersMap> pm(new ParametersMap);
				if(_city.get())
				{
					_registerItems<NamedPlace>(
						*pm,
						_city->getAllPlacesMatcher().bestMatches(_text, _number ? *_number : 0)
					);
				}
				else if(_site.get())
				{
					 /// TODO
				}
				else
				{
					_registerItems<Place>(
						*pm,
						GeographyModule::GetGeneralAllPlacesMatcher().bestMatches(_text, _number ? *_number : 0)
					);
				}
				result.insert(DATA_PLACES, pm);
			}

			// Output
			if(_itemPage.get())
			{
				// Sorted results or not
				if(_sorted)
				{
					if(_classPage.get())
					{
						if(!result.getSubMaps(DATA_CITIES).empty()
						){
							_displayClass(
								stream,
								DATA_CITY,
								(*result.getSubMaps(DATA_CITIES).begin())->getSubMaps(DATA_CITY),
								request
							);
						}
						if(!result.getSubMaps(DATA_STOPS).empty()
						){
							_displayClass(
								stream,
								DATA_STOP,
								(*result.getSubMaps(DATA_STOPS).begin())->getSubMaps(DATA_STOP),
								request
							);
						}
						if(!result.getSubMaps(DATA_ADDRESSES).empty()
						){
							_displayClass(
								stream,
								DATA_ADDRESS,
								(*result.getSubMaps(DATA_ADDRESSES).begin())->getSubMaps(DATA_ADDRESS),
								request
							);
						}
						if(!result.getSubMaps(DATA_ROADS).empty()
						){
							_displayClass(
								stream,
								DATA_ROAD,
								(*result.getSubMaps(DATA_ROADS).begin())->getSubMaps(DATA_ROAD),
								request
							);
						}
						if(!result.getSubMaps(DATA_PUBLIC_PLACES).empty()
						){
							_displayClass(
								stream,
								DATA_PUBLIC_PLACE,
								(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->getSubMaps(DATA_PUBLIC_PLACE),
								request
							);
						}
					}
					else
					{
						size_t rank(0);
						if(!result.getSubMaps(DATA_CITIES).empty()
						){
							_displayItems(
								stream,
								DATA_CITY,
								(*result.getSubMaps(DATA_CITIES).begin())->getSubMaps(DATA_CITY),
								request,
								rank
							);
						}
						if(!result.getSubMaps(DATA_STOPS).empty()
						){
							_displayItems(
								stream,
								DATA_STOP,
								(*result.getSubMaps(DATA_STOPS).begin())->getSubMaps(DATA_STOP),
								request,
								rank
							);
						}
						if(!result.getSubMaps(DATA_ADDRESSES).empty()
						){
							_displayItems(
								stream,
								DATA_ADDRESS,
								(*result.getSubMaps(DATA_ADDRESSES).begin())->getSubMaps(DATA_ADDRESS),
								request,
								rank
							);
						}
						if(!result.getSubMaps(DATA_ROADS).empty()
						){
							_displayItems(
								stream,
								DATA_ROAD,
								(*result.getSubMaps(DATA_ROADS).begin())->getSubMaps(DATA_ROAD),
								request,
								rank
							);
						}
						if(!result.getSubMaps(DATA_PUBLIC_PLACES).empty()
						){
							_displayItems(
								stream,
								DATA_PUBLIC_PLACE,
								(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->getSubMaps(DATA_PUBLIC_PLACE),
								request,
								rank
							);
						}
					}
				}
				else
				{
					size_t rank(0);
					const ParametersMap& pm(
						**result.getSubMaps(DATA_PLACES).begin()
					);
					_displayItems(
						stream,
						DATA_CITY,
						pm.getSubMaps(DATA_CITY),
						request,
						rank
					);
					_displayItems(
						stream,
						DATA_STOP,
						pm.getSubMaps(DATA_STOP),
						request,
						rank
					);
					_displayItems(
						stream,
						DATA_ADDRESS,
						pm.getSubMaps(DATA_ADDRESS),
						request,
						rank
					);
					_displayItems(
						stream,
						DATA_ROAD,
						pm.getSubMaps(DATA_ROAD),
						request,
						rank
					);
					_displayItems(
						stream,
						DATA_PUBLIC_PLACE,
						pm.getSubMaps(DATA_PUBLIC_PLACE),
						request,
						rank
					);
				}
			}
			else if(_outputFormat == VALUE_JSON)
			{
				if(_sorted)
				{
					result.outputJSON(
						stream,
						DATA_PLACES
					);
				}
				else
				{
					(*result.getSubMaps(DATA_PLACES).begin())->outputJSON(
						stream,
						DATA_PLACES
					);
				}
			}
			else if(_outputFormat == VALUE_XML)
			{
				if(_sorted)
				{
					result.outputXML(
						stream,
						DATA_PLACES,
						true,
						"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/56_pt_website/sorted_places_list.xsd"
					);
				}
				else
				{
					(*result.getSubMaps(DATA_PLACES).begin())->outputXML(
						stream,
						DATA_PLACES,
						true,
						"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/56_pt_website/unsorted_places_list.xsd"
					);
				}
			}

			return result;
		}
		
		
		
		bool PlacesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PlacesListService::getOutputMimeType() const
		{
			if(_itemPage.get())
			{
				return _itemPage->getMimeType();
			}
			if(_outputFormat == VALUE_JSON)
			{
				return "application/json";
			}
			return "text/xml";
		}



		void PlacesListService::_displayItems(
			std::ostream& stream,
			const std::string& className,
			const std::vector<boost::shared_ptr<util::ParametersMap> >& maps,
			const server::Request& request
		) const {
			size_t rank(0);
			_displayItems(stream, className, maps, request, rank);
		}



		void PlacesListService::_displayItems(
			std::ostream& stream,
			const std::string& className,
			const std::vector<boost::shared_ptr<util::ParametersMap> >& maps,
			const server::Request& request,
			std::size_t& rank
		) const	{

			BOOST_FOREACH(shared_ptr<ParametersMap> item, maps)
			{
				// Template parameters
				item->merge(getTemplateParameters());

				// Class
				item->insert(DATA_CLASS, className);

				// Rank
				item->insert(DATA_RANK, rank++);

				// Display
				_itemPage->display(stream, request, *item);
			}
		}



		void PlacesListService::_displayClass(
			std::ostream& stream,
			const std::string& className,
			const std::vector<boost::shared_ptr<util::ParametersMap> >& maps,
			const server::Request& request
		) const	{
			ParametersMap classMap(getTemplateParameters());

			// Content
			stringstream content;
			_displayItems(
				stream,
				className,
				maps,
				request
			);
			classMap.insert(DATA_CONTENT, content.str());

			// Display
			_classPage->display(stream, request, classMap);
		}
}	}
