
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
#include "House.hpp"
#include "HTMLTable.h"
#include "MimeTypes.hpp"
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
		const string PlacesListService::PARAMETER_CLASS_FILTER = "class_filter";
		const string PlacesListService::PARAMETER_CLASS_PAGE_ID = "class_page_id";
		const string PlacesListService::PARAMETER_ITEM_PAGE_ID = "item_page_id";
		const string PlacesListService::PARAMETER_MIN_SCORE = "min_score";
		const string PlacesListService::PARAMETER_NUMBER = "number";
		const string PlacesListService::PARAMETER_OUTPUT_FORMAT = "output_format";
		const string PlacesListService::PARAMETER_SITE_ID = "site_id";
		const string PlacesListService::PARAMETER_SORTED = "sorted";
		const string PlacesListService::PARAMETER_TEXT = "text";
		const string PlacesListService::PARAMETER_SRID = "srid";

		const string PlacesListService::DATA_ADDRESS = "address";
		const string PlacesListService::DATA_ADDRESSES = "addresses";
		const string PlacesListService::DATA_BEST_PLACE = "best_place";
		const string PlacesListService::DATA_CITIES = "cities";
		const string PlacesListService::DATA_CITY = "city";
		const string PlacesListService::DATA_CLASS = "class";
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
			_citiesWithAtLeastAStop(true),
			_minScore(0)
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

			// Min score
			if(_minScore > 0)
			{
				map.insert(PARAMETER_MIN_SCORE, _minScore);
			}

			// Class filter
			if(!_classFilter.empty())
			{
				map.insert(PARAMETER_CLASS_FILTER, _classFilter);
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

			// Min score
			_minScore = map.getDefault<double>(PARAMETER_MIN_SCORE, _minScore);

			// Class filter
			_classFilter = map.getDefault<string>(PARAMETER_CLASS_FILTER);

			// Set coordinate system if provided else default SRID is used
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

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
				// Output format
				_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT, MimeTypes::XML);
				if(_outputFormat == VALUE_JSON)
				{
					_outputFormat = MimeTypes::JSON;
				}
				if(_outputFormat == VALUE_XML)
				{
					_outputFormat = MimeTypes::XML;
				}
			}
		}



		ParametersMap PlacesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// ParametersMap populating
			ParametersMap result;
			if(_sorted || !_classFilter.empty())
			{
				if(_city.get())
				{
					// Stops
					if(_classFilter.empty() || _classFilter == DATA_STOP)
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(StopArea::FACTORY_KEY).bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore
						)	);
						result.insert(DATA_STOPS, pm);
					}

					// Roads
					if(_classFilter.empty() || _classFilter == DATA_ROAD)
					{
						bool done(false);
						vector<string> words;
						split(words, _text, is_any_of(", "));
						if(words.size() > 1)
						{	// Text points to an address
							try
							{
								MainRoadChunk::HouseNumber number(lexical_cast<MainRoadChunk::HouseNumber>(words[0]));

								string roadName(_text.substr(words[0].size() + 1));

								City::PlacesMatcher::MatchResult places(
									_city->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(
										roadName,
										_number ? *_number : 0,
										_minScore
								)	);

								// Transformation into house places list
								LexicalMatcher<shared_ptr<NamedPlace> >::MatchResult newList;
								BOOST_FOREACH(const City::PlacesMatcher::MatchResult::value_type& place, places)
								{
									const RoadPlace& roadPlace(
										dynamic_cast<const RoadPlace&>(*place.value)
									);

									shared_ptr<House> house(roadPlace.getHouse(number));

									LexicalMatcher<shared_ptr<NamedPlace> >::MatchHit houseResult;
									houseResult.key = place.key;
									houseResult.score = place.score;
									houseResult.value = house.get() ?
										dynamic_pointer_cast<NamedPlace, House>(house) :
										place.value
									;

									newList.push_back(houseResult);
								}

								// Registration
								shared_ptr<ParametersMap> pm(new ParametersMap);
								_registerItems<NamedPlace>(
									*pm,
									newList
								);
								result.insert(DATA_ADDRESSES, pm);
								done = true;
							}
							catch (bad_lexical_cast)
							{
							}
						}

						// Roads if not address
						if(!done)
						{
							shared_ptr<ParametersMap> pm(new ParametersMap);
							_registerItems<NamedPlace>(
								*pm,
								_city->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(
									_text,
									_number ? *_number : 0,
									_minScore
							)	);
							result.insert(DATA_ROADS, pm);
						}
					}

					// Public places
					if(_classFilter.empty() || _classFilter == DATA_PUBLIC_PLACE)
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(PublicPlace::FACTORY_KEY).bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore
						)	);
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
					if(_classFilter.empty() || _classFilter == DATA_CITY)
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<City>(
							*pm,
							GeographyModule::GetCitiesMatcher().bestMatches(
								_text,
								(_number && !_citiesWithAtLeastAStop) ? *_number : 0,
								_minScore
						)	);
						result.insert(DATA_CITIES, pm);
					}

					// Stops
					if(_classFilter.empty() || _classFilter == DATA_STOP)
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<StopArea>(
							*pm,
							PTModule::GetGeneralStopsMatcher().bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore
						)	);
						result.insert(DATA_STOPS, pm);
					}

					// Roads
					if(_classFilter.empty() || _classFilter == DATA_ROAD)
					{
						bool done(false);
						vector<string> words;
						split(words, _text, is_any_of(", "));
						if(words.size() > 1)
						{	// Text points to an address
							try
							{
								MainRoadChunk::HouseNumber number(lexical_cast<MainRoadChunk::HouseNumber>(words[0]));

								string roadName(_text.substr(words[0].size() + 1));

								RoadModule::GeneralRoadsMatcher::MatchResult places(
									RoadModule::GetGeneralRoadsMatcher().bestMatches(
										roadName,
										_number ? *_number : 0,
										_minScore
								)	);

								// Transformation into house places list
								LexicalMatcher<shared_ptr<NamedPlace> >::MatchResult newList;
								BOOST_FOREACH(const RoadModule::GeneralRoadsMatcher::MatchResult::value_type& place, places)
								{
									const RoadPlace& roadPlace(
										dynamic_cast<const RoadPlace&>(*place.value)
									);

									shared_ptr<House> house(roadPlace.getHouse(number));

									LexicalMatcher<shared_ptr<NamedPlace> >::MatchHit houseResult;
									houseResult.key = place.key;
									houseResult.score = place.score;
									houseResult.value = house.get() ?
										dynamic_pointer_cast<NamedPlace, House>(house) :
										place.value
									;

									newList.push_back(houseResult);
								}

								// Registration
								shared_ptr<ParametersMap> pm(new ParametersMap);
								_registerItems<NamedPlace>(
									*pm,
									newList
								);
								result.insert(DATA_ADDRESSES, pm);
								done = true;
							}
							catch (bad_lexical_cast)
							{
							}
						}

						// Roads if not address
						if(!done)
						{
							shared_ptr<ParametersMap> pm(new ParametersMap);
							_registerItems<RoadPlace>(
								*pm,
								RoadModule::GetGeneralRoadsMatcher().bestMatches(
									_text,
									_number ? *_number : 0,
									_minScore
							)	);
							result.insert(DATA_ROADS, pm);
						}
					}

					// Public places
					if(_classFilter.empty() || _classFilter == DATA_PUBLIC_PLACE)
					{
						shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<PublicPlace>(
							*pm,
							RoadModule::GetGeneralPublicPlacesMatcher().bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore
						)	);
						result.insert(DATA_PUBLIC_PLACES, pm);
					}
				}
			}
			else
			{
				shared_ptr<ParametersMap> pm(new ParametersMap);
				if(_city.get())
				{ /// TODO take into account of _classFilter
					_registerItems<NamedPlace>(
						*pm,
						_city->getAllPlacesMatcher().bestMatches(
							_text,
							_number ? *_number : 0,
							_minScore
					)	);
				}
				else if(_site.get())
				{
					 /// TODO
				}
				else
				{ /// TODO take into account of _classFilter
					_registerItems<Place>(
						*pm,
						GeographyModule::GetGeneralAllPlacesMatcher().bestMatches(
							_text,
							_number ? *_number : 0,
							_minScore
					)	);
				}
				result.insert(DATA_PLACES, pm);
			}

			// Best place
			shared_ptr<ParametersMap> bestMap;
			string className;
			if(_sorted)
			{
				shared_ptr<ParametersMap> bestPlace(new ParametersMap);

				// Stop areas
				if(	result.hasSubMaps(DATA_STOPS) &&
					(*result.getSubMaps(DATA_STOPS).begin())->hasSubMaps(DATA_STOP)
				){
					bestMap = *(*result.getSubMaps(DATA_STOPS).begin())->getSubMaps(DATA_STOP).begin();
					className = DATA_STOP;
				}

				// Cities
				if(	result.hasSubMaps(DATA_CITIES) &&
					(*result.getSubMaps(DATA_CITIES).begin())->hasSubMaps(DATA_CITY)
				){
					shared_ptr<ParametersMap> cityBestMap(
						*(*result.getSubMaps(DATA_CITIES).begin())->getSubMaps(DATA_CITY).begin()
					);
					if(	!bestMap.get() ||
						cityBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						cityBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						cityBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN)
					){
						bestMap = cityBestMap;
						className = DATA_CITY;
					}
				}

				// Public places
				if(	result.hasSubMaps(DATA_PUBLIC_PLACES) &&
					(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->hasSubMaps(DATA_PUBLIC_PLACE)
				){
					shared_ptr<ParametersMap> ppBestMap(
						*(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->getSubMaps(DATA_PUBLIC_PLACE).begin()
					);
					if(	!bestMap.get() ||
						ppBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						ppBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						ppBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN)
					){
						bestMap = ppBestMap;
						className = DATA_PUBLIC_PLACE;
					}
				}

				// Streets
				if(	result.hasSubMaps(DATA_ROADS) &&
					(*result.getSubMaps(DATA_ROADS).begin())->hasSubMaps(DATA_ROAD)
				){
					shared_ptr<ParametersMap> roadBestMap(
						*(*result.getSubMaps(DATA_ROADS).begin())->getSubMaps(DATA_ROAD).begin()
					);
					if(	!bestMap.get() ||
						roadBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						roadBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						roadBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN)
					){
						bestMap = roadBestMap;
						className = DATA_ROAD;
					}
				}

				// Addresses
				if(	result.hasSubMaps(DATA_ADDRESSES) &&
					(*result.getSubMaps(DATA_ADDRESSES).begin())->hasSubMaps(DATA_ADDRESS)
				){
					shared_ptr<ParametersMap> addressBestMap(
						*(*result.getSubMaps(DATA_ADDRESSES).begin())->getSubMaps(DATA_ADDRESS).begin()
					);
					if(	!bestMap.get() ||
						addressBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						addressBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						addressBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN)
					){
						bestMap = addressBestMap;
						className = DATA_ADDRESS;
					}
				}

				// Registration on the result
				if(bestMap.get())
				{
					bestPlace->insert(className, bestMap);
				}
				result.insert(DATA_BEST_PLACE, bestPlace);
			}


			// Output
			if(_itemPage.get())
			{
				// Sorted results or not
				if(_sorted)
				{
					if(_classPage.get())
					{
						_displayClass(
							stream,
							result,
							request,
							bestMap,
							className
						);
					}
					else
					{
						size_t rank(0);
						if(	result.hasSubMaps(DATA_CITIES) &&
							(*result.getSubMaps(DATA_CITIES).begin())->hasSubMaps(DATA_CITY)
						){
							_displayItems(
								stream,
								DATA_CITY,
								(*result.getSubMaps(DATA_CITIES).begin())->getSubMaps(DATA_CITY),
								request,
								rank
							);
						}
						if(	result.hasSubMaps(DATA_STOPS) &&
							(*result.getSubMaps(DATA_STOPS).begin())->hasSubMaps(DATA_STOP)
						){
							_displayItems(
								stream,
								DATA_STOP,
								(*result.getSubMaps(DATA_STOPS).begin())->getSubMaps(DATA_STOP),
								request,
								rank
							);
						}
						if(	result.hasSubMaps(DATA_ADDRESSES) &&
							(*result.getSubMaps(DATA_ADDRESSES).begin())->hasSubMaps(DATA_ADDRESS)
						){
							_displayItems(
								stream,
								DATA_ADDRESS,
								(*result.getSubMaps(DATA_ADDRESSES).begin())->getSubMaps(DATA_ADDRESS),
								request,
								rank
							);
						}
						if(	result.hasSubMaps(DATA_ROADS) &&
							(*result.getSubMaps(DATA_ROADS).begin())->hasSubMaps(DATA_ROAD)
						){
							_displayItems(
								stream,
								DATA_ROAD,
								(*result.getSubMaps(DATA_ROADS).begin())->getSubMaps(DATA_ROAD),
								request,
								rank
							);
						}
						if(	result.hasSubMaps(DATA_PUBLIC_PLACES) &&
							(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->hasSubMaps(DATA_ADDRESS)
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
					if(pm.hasSubMaps(DATA_CITY))
					{
						_displayItems(
							stream,
							DATA_CITY,
							pm.getSubMaps(DATA_CITY),
							request,
							rank
						);
					}
					if(pm.hasSubMaps(DATA_STOP))
					{
						_displayItems(
							stream,
							DATA_STOP,
							pm.getSubMaps(DATA_STOP),
							request,
							rank
						);
					}
					if(pm.hasSubMaps(DATA_ADDRESS))
					{
						_displayItems(
							stream,
							DATA_ADDRESS,
							pm.getSubMaps(DATA_ADDRESS),
							request,
							rank
						);
					}
					if(pm.hasSubMaps(DATA_ROAD))
					{
						_displayItems(
							stream,
							DATA_ROAD,
							pm.getSubMaps(DATA_ROAD),
							request,
							rank
						);
					}
					if(pm.hasSubMaps(DATA_PUBLIC_PLACE))
					{
						_displayItems(
							stream,
							DATA_PUBLIC_PLACE,
							pm.getSubMaps(DATA_PUBLIC_PLACE),
							request,
							rank
						);
					}
				}
			}
			else if(_outputFormat == MimeTypes::JSON)
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
			else if(_outputFormat == MimeTypes::XML)
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
			else if(_outputFormat == MimeTypes::CSV)
			{
				result.outputCSV(
					stream,
					DATA_PLACES
				);
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
			return _itemPage.get() ? _itemPage->getMimeType() : _outputFormat;
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

			BOOST_FOREACH(const shared_ptr<ParametersMap>& item, maps)
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
			const util::ParametersMap & result,
			const server::Request& request,
			shared_ptr<ParametersMap> bestPlace,
			const string& bestPlaceClassName
		) const {
			ParametersMap classMap(getTemplateParameters());

			//Insert best place
			if(bestPlace.get())
			{
				stringstream bestPlaceStream;
				vector<shared_ptr<ParametersMap> > bestPlaceMap;
				bestPlaceMap.push_back(bestPlace);
				_displayItems(
					bestPlaceStream,
					bestPlaceClassName,
					bestPlaceMap,
					request
				);
				classMap.insert(DATA_BEST_PLACE, bestPlaceStream.str());
			}

			if(result.hasSubMaps(DATA_CITIES) &&
				(*result.getSubMaps(DATA_CITIES).begin())->hasSubMaps(DATA_CITY)
			){
				stringstream cities;
				_displayItems(
					cities,
					DATA_CITY,
					(*result.getSubMaps(DATA_CITIES).begin())->getSubMaps(DATA_CITY),
					request
				);
				classMap.insert(DATA_CITIES, cities.str());
			}
			if(result.hasSubMaps(DATA_STOPS) &&
				(*result.getSubMaps(DATA_STOPS).begin())->hasSubMaps(DATA_STOP)
			){
				stringstream stops;
				_displayItems(
					stops,
					DATA_STOP,
					(*result.getSubMaps(DATA_STOPS).begin())->getSubMaps(DATA_STOP),
					request
				);
				classMap.insert(DATA_STOPS, stops.str());
			}
			if(result.hasSubMaps(DATA_ADDRESSES) &&
				(*result.getSubMaps(DATA_ADDRESSES).begin())->hasSubMaps(DATA_ADDRESS)
			){
				stringstream adresses;
				_displayItems(
					adresses,
					DATA_ADDRESS,
					(*result.getSubMaps(DATA_ADDRESSES).begin())->getSubMaps(DATA_ADDRESS),
					request
				);
				classMap.insert(DATA_ADDRESSES, adresses.str());
			}
			if(result.hasSubMaps(DATA_ROADS) &&
				(*result.getSubMaps(DATA_ROADS).begin())->hasSubMaps(DATA_ROAD)
			){
				stringstream roads;
				_displayItems(
					roads,
					DATA_ROAD,
					(*result.getSubMaps(DATA_ROADS).begin())->getSubMaps(DATA_ROAD),
					request
				);
				classMap.insert(DATA_ROADS, roads.str());
			}
			if(result.hasSubMaps(DATA_PUBLIC_PLACES) &&
				(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->hasSubMaps(DATA_PUBLIC_PLACE)
			){
				stringstream publicPlaces;
				_displayItems(
					publicPlaces,
					DATA_PUBLIC_PLACE,
					(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->getSubMaps(DATA_PUBLIC_PLACE),
					request
				);
				classMap.insert(DATA_PUBLIC_PLACES, publicPlaces.str());
			}
			// Display
			_classPage->display(stream, request, classMap);
		}


		PlacesListService::PlaceResult PlacesListService::getPlaceFromBestResult(
			const ParametersMap& result
		) const {

			PlacesListService::PlaceResult placeResult;

			if(_sorted)
			{
				shared_ptr<ParametersMap> bestPlaceMap(*result.getSubMaps(DATA_BEST_PLACE).begin());

				// Class of the best place
				ParametersMap::SubMapsKeys keys(bestPlaceMap->getSubMapsKeys());
				if(keys.empty())
				{
					return placeResult;
				}
				string className(*keys.begin());
				shared_ptr<ParametersMap> itemMap(
					*bestPlaceMap->getSubMaps(className).begin()
				);
				placeResult.score.phoneticScore = itemMap->get<double>(DATA_PHONETIC_SCORE);
				placeResult.score.levenshtein = itemMap->get<size_t>(DATA_LEVENSHTEIN);
				placeResult.key = itemMap->get<string>(DATA_KEY);

				// City
				if(className == DATA_CITY)
				{
					placeResult.value = static_pointer_cast<Place, City>(
						Env::GetOfficialEnv().getEditable<City>(
							itemMap->get<RegistryKeyType>(
								City::DATA_CITY_ID
					)	)	);
				}
				else if(className == DATA_STOP)
				{
					placeResult.value = static_pointer_cast<Place, StopArea>(
						Env::GetOfficialEnv().getEditable<StopArea>(
							itemMap->get<RegistryKeyType>(
								StopArea::DATA_STOP_ID
					)	)	);
				}
				else if(className == DATA_ROAD)
				{
					placeResult.value = static_pointer_cast<Place, RoadPlace>(
						Env::GetOfficialEnv().getEditable<RoadPlace>(
							itemMap->get<RegistryKeyType>(
								RoadPlace::DATA_ID
					)	)	);
				}
				else if(className == DATA_PUBLIC_PLACE)
				{
					placeResult.value = static_pointer_cast<Place, PublicPlace>(
						Env::GetOfficialEnv().getEditable<PublicPlace>(
							itemMap->get<RegistryKeyType>(
								PublicPlace::DATA_ID
					)	)	);
				}
				else if(className == DATA_ADDRESS)
				{
					const shared_ptr<RoadPlace>& roadPlace(
						Env::GetOfficialEnv().getEditable<RoadPlace>(
							itemMap->get<RegistryKeyType>(
								House::DATA_ROAD_PREFIX + RoadPlace::DATA_ID
					)	)	);
					placeResult.value = static_pointer_cast<Place, House>(
						roadPlace->getHouse(
							itemMap->get<RegistryKeyType>(
								House::DATA_NUMBER
					)	)	);
				}
			}

			return placeResult;
		}
}	}
