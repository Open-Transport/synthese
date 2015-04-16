
/** PlacesListFunction class implementation.
	@file PlacesListFunction.cpp

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

#include "PlacesListFunction.h"

#include "Request.h"
#include "TransportWebsiteTypes.hpp"
#include "PTServiceConfigTableSync.hpp"
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
#include "Website.hpp"
#include "MimeTypes.hpp"

#include <boost/foreach.hpp>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

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
		const string PlacesListFunction::PARAMETER_CONFIG_ID = "config_id";
		const string PlacesListFunction::PARAMETER_OLD_INPUT("i");
		const string PlacesListFunction::PARAMETER_INPUT("t");
		const string PlacesListFunction::PARAMETER_CITY_TEXT("ct");
		const string PlacesListFunction::PARAMETER_CITY_ID("city_id");
		const string PlacesListFunction::PARAMETER_NUMBER("n");
		const string PlacesListFunction::PARAMETER_PAGE("page_id");
		const string PlacesListFunction::PARAMETER_ITEM_PAGE("item_page_id");

		const string PlacesListFunction::PARAMETER_SRID("srid");

		const string PlacesListFunction::DATA_CITY_ID("city_id");
		const string PlacesListFunction::DATA_CITY_NAME("city_name");
		const string PlacesListFunction::DATA_RESULTS_SIZE("size");
		const string PlacesListFunction::DATA_CONTENT("content");
		const string PlacesListFunction::DATA_PLACES("places");
		const string PlacesListFunction::DATA_PLACE("place");

		const string PlacesListFunction::DATA_NAME("name");
		const string PlacesListFunction::DATA_RANK("rank");



		PlacesListFunction::PlacesListFunction():
			_config(NULL)
		{}



		ParametersMap PlacesListFunction::_getParametersMap() const
		{
			ParametersMap map(_FunctionWithSite::_getParametersMap());
			map.insert(PARAMETER_INPUT, _input);
			if(_cityText.empty())
			{
				map.insert(PARAMETER_CITY_ID, _city.get() ? _city->getKey() : RegistryKeyType(0));
			}
			else
			{
				map.insert(PARAMETER_CITY_TEXT, _cityText);
			}
			if(_n)
			{
				map.insert(PARAMETER_NUMBER, *_n);
			}
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE, _page->getKey());
			}
			if(_itemPage.get())
			{
				map.insert(PARAMETER_ITEM_PAGE, _itemPage->getKey());
			}
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			if(_config)
			{
				map.insert(PARAMETER_CONFIG_ID, _config->getKey());
			}

			return map;
		}



		void PlacesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);

			// Config
			RegistryKeyType configId(map.getDefault<RegistryKeyType>(PARAMETER_CONFIG_ID, 0));
			if(configId) try
			{
				_config = PTServiceConfigTableSync::Get(configId, *_env).get();
			}
			catch (ObjectNotFoundException<PTServiceConfig>&)
			{
				throw RequestException("No such config");
			}

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
			if(!_page.get())
			{
				setOutputFormatFromMap(map, "");
			}

			_input =
				map.getOptional<string>(PARAMETER_INPUT) ?
				map.getDefault<string>(PARAMETER_INPUT) :
				map.getDefault<string>(PARAMETER_OLD_INPUT);

			_n = map.getOptional<size_t>(PARAMETER_NUMBER);
			if (!_input.empty() && !_n)
			{
				throw RequestException("Number of result must be limited");
			}

			if(map.isDefined(PARAMETER_CITY_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CITY_ID));
				if(id > 0) try
				{
					_city = Env::GetOfficialEnv().getEditable<City>(id);
				}
				catch(ObjectNotFoundException<City>&)
				{
					throw RequestException("No such city");
				}
			}
			else
			{
				_cityText = map.get<string>(PARAMETER_CITY_TEXT);
				if(!_cityText.empty())
				{
					GeographyModule::CitiesMatcher matcher;
					if(_config)
					{
						matcher = _config->getCitiesMatcher();
					}
					else
					{
						matcher = (GeographyModule::GetCitiesMatcher());
					}

					GeographyModule::CitiesMatcher::MatchResult cities(
						matcher.bestMatches(_cityText,1)
					);
					if(cities.empty())
					{
						throw RequestException("No city was found");
					}
					_city = cities.front().value;
				}
			}
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Cleaning of template parameters for non CMS output
			if(!_outputFormat.empty())
			{
				_templateParameters.clear();
			}
		}



		util::ParametersMap PlacesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{

			PlacesList placesList;

			if(!_input.empty())
			{
				GeographyModule::CitiesMatcher matcher;
				if(_config)
				{
					matcher = _config->getCitiesMatcher();
				}
				else
				{
					matcher = (GeographyModule::GetCitiesMatcher());
				}

				RoadModule::ExtendedFetchPlacesResult places(
					_city.get() ?
					RoadModule::ExtendedFetchPlaces(_city, _input, *_n) :(
						_cityText.empty() ?
						PTModule::ExtendedFetchPlaces(_input, *_n) :
						RoadModule::ExtendedFetchPlaces(matcher, _cityText, _input, *_n)
				)	);

				if(_page.get() || _itemPage.get() || !_outputFormat.empty())
				{
					BOOST_FOREACH(const RoadModule::ExtendedFetchPlaceResult& place, places)
					{
						placesList.push_back(make_pair(
								dynamic_cast<Registrable*>(place.placeResult.value.get()) ?
								dynamic_cast<Registrable*>(place.placeResult.value.get())->getKey() : 0,
								place.placeResult.key.getSource()
						)	);
					}
				}
				else
				{
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<options xsi:noNamespaceSchemaLocation=\"https://extranet.rcsmobility.com/projects/synthese/repository/raw/doc/include/56_transport_website/places_list.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
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
						stream << "\"";
						if(dynamic_cast<const NamedPlace*>(it.placeResult.value.get()))
						{
							stream << " id=\"" << dynamic_cast<const NamedPlace*>(it.placeResult.value.get())->getKey() << "\"";
						}
						boost::shared_ptr<Point> placePoint;
						double x = 0.0;
						double y = 0.0;
						if(it.placeResult.value.get()->getPoint().get())
						{
							placePoint = _coordinatesSystem->convertPoint(*it.placeResult.value.get()->getPoint());
							if(placePoint.get())
							{
								x = placePoint->getX();
								y = placePoint->getY();
							}
						}
						stream <<
							" score=\"" << it.placeResult.score.phoneticScore << "\"" <<
							" levenshtein=\"" << it.placeResult.score.levenshtein << "\"" <<
							" cityId=\"" << dynamic_cast<const NamedPlace*>(it.placeResult.value.get())->getCity()->getKey() << "\"" <<
							" cityName=\"" << dynamic_cast<const NamedPlace*>(it.placeResult.value.get())->getCity()->getName() << "\"" <<
							" name=\"" << dynamic_cast<const NamedPlace*>(it.placeResult.value.get())->getName() << "\"" <<
							" x=\"" << x << "\"" <<
							" y=\"" << y << "\"" <<
						">";

						if(dynamic_cast<const House*>(it.placeResult.value.get()) && dynamic_cast<const House*>(it.placeResult.value.get())->getHouseNumber())
						{
							stream << *dynamic_cast<const House*>(it.placeResult.value.get())->getHouseNumber() << " ";
						}
						stream << it.placeResult.key.getSource() << "</option>";
					}
					stream << "</options>";
				}
			}
			else if(_city.get())
			{
				if(_page.get() || _itemPage.get() || !_outputFormat.empty())
				{
					size_t c(0);
					BOOST_FOREACH(const LexicalMatcher<boost::shared_ptr<Place> >::Map::value_type& it, _city->getAllPlacesMatcher().entries())
					{
						placesList.push_back(
							make_pair(
								dynamic_pointer_cast<NamedPlace,Place>(it.second)->getKey(),
								dynamic_pointer_cast<NamedPlace,Place>(it.second)->getName()
						)	);
						if(_n && c >= *_n)
						{
							break;
						}
						++c;
					}
				}
				else
				{
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<options xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/places_list.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
						;
					BOOST_FOREACH(const LexicalMatcher<boost::shared_ptr<Place> >::Map::value_type& it, _city->getAllPlacesMatcher().entries())
					{
						stream << "<option type=\"";
						if(	dynamic_cast<StopArea*>(it.second.get()) ||
							dynamic_cast<PlaceAlias*>(it.second.get())
						){
							stream << "stop";
						}
						else if(dynamic_cast<const PublicPlace*>(it.second.get()))
						{
							stream << "publicPlace";
						}
						else if(dynamic_cast<const RoadPlace*>(it.second.get()))
						{
							stream << "street";
						}
						else if(dynamic_cast<const House*>(it.second.get()))
						{
							stream << "address";
						}
						stream << "\"";
						if(dynamic_cast<const NamedPlace*>(it.second.get()))
						{
							stream << " id=\"" << dynamic_cast<const NamedPlace*>(it.second.get())->getKey() << "\"";
						}
						stream << ">";

						if(dynamic_cast<const House*>(it.second.get()) && dynamic_cast<const House*>(it.second.get())->getHouseNumber())
						{
							stream << *dynamic_cast<const House*>(it.second.get())->getHouseNumber() << " ";
						}

						stream << it.first.getSource() << "</option>";
					}
					stream << "</options>";
				}
			}

			ParametersMap pm(getTemplateParameters());

			if(_page.get())
			{
				pm.insert(DATA_RESULTS_SIZE, placesList.size());
				if(_city.get())
				{
					pm.insert(DATA_CITY_ID, _city->getKey());
					pm.insert(DATA_CITY_NAME, _city->getName());
				}

				if(_itemPage.get())
				{
					stringstream content;
					_displayItems(content, placesList, request);
					pm.insert(DATA_CONTENT, content.str());
				}

				_page->display(stream, request, pm);
			}
			else if(_itemPage.get())
			{
				_displayItems(stream, placesList, request);
			}
			else if(!_outputFormat.empty())
			{
				size_t i(0);
				BOOST_FOREACH(const PlacesList::value_type& it, placesList)
				{
					boost::shared_ptr<ParametersMap> placePm(new ParametersMap());

					placePm->insert(DATA_RANK, i);
					placePm->insert(DATA_NAME, it.second);
					placePm->insert(Request::PARAMETER_OBJECT_ID, it.first);
					pm.insert(DATA_PLACE, placePm);
					++i;
				}
				outputParametersMap(
					pm,
					stream,
					DATA_PLACES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/56_pt_website/places_list.xsd"
				);
			}

			return pm;
		}

		void PlacesListFunction::setTextInput( const std::string& text )
		{
			_input = text;
		}

		void PlacesListFunction::setNumber( int number )
		{
			_n = number;
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
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat(MimeTypes::XML);
		}



		void PlacesListFunction::_displayItems(
			std::ostream& stream,
			const PlacesList& items,
			const server::Request& request
		) const {
			size_t i(0);
			BOOST_FOREACH(const PlacesList::value_type& it, items)
			{
				ParametersMap pm(getTemplateParameters());

				pm.insert(DATA_RANK, i);
				pm.insert(DATA_NAME, it.second);
				pm.insert(Request::PARAMETER_OBJECT_ID, it.first);

				_itemPage->display(stream, request, pm);
				++i;
			}
		}
}	}
