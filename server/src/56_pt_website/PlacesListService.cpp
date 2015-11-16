
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
#include "Road.h"
#include "RoadChunkEdge.hpp"
#include "RoadPlace.h"
#include "StopArea.hpp"
#include "PTServiceConfigTableSync.hpp"
#include "Webpage.h"
#include "RoadChunkTableSync.h"
#include <geos/geom/LineString.h>
#include <geos/linearref/LengthIndexedLine.h>

#ifndef UNIX
#include <geos/util/math.h>
#endif

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::util;
using namespace geos::linearref;

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
		const string PlacesListService::PARAMETER_CONFIG_ID = "config_id";
		const string PlacesListService::PARAMETER_CITY_ID = "city_id";
		const string PlacesListService::PARAMETER_CITIES_WITH_AT_LEAST_A_STOP = "cities_with_at_least_a_stop";
		const string PlacesListService::PARAMETER_CLASS_FILTER = "class_filter";
		const string PlacesListService::PARAMETER_CLASS_PAGE_ID = "class_page_id";
		const string PlacesListService::PARAMETER_ITEM_PAGE_ID = "item_page_id";
		const string PlacesListService::PARAMETER_MIN_SCORE = "min_score";
		const string PlacesListService::PARAMETER_NUMBER = "number";
		const string PlacesListService::PARAMETER_SORTED = "sorted";
		const string PlacesListService::PARAMETER_TEXT = "text";
		const string PlacesListService::PARAMETER_SRID = "srid";
		const string PlacesListService::PARAMETER_PHONETIC = "phonetic";

		const string PlacesListService::PARAMETER_COORDINATES_XY = "coordinates_xy";
		const string PlacesListService::PARAMETER_MAX_DISTANCE = "maxDistance";
		const string PlacesListService::PARAMETER_COMPATIBLE_USER_CLASSES_LIST = "acList";
		const string PlacesListService::PARAMETER_DATA_SOURCE_FILTER = "data_source_filter";

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
		const string PlacesListService::DATA_ORIGIN_X = "origin_x";
		const string PlacesListService::DATA_ORIGIN_Y = "origin_y";
		const string PlacesListService::DATA_DISTANCE_TO_ORIGIN = "distance_to_origin";



		PlacesListService::PlacesListService():
			_config(NULL),
			_sorted(true),
			_citiesWithAtLeastAStop(true),
			_minScore(0),
			_coordinatesSystem(NULL),
			_maxDistance(300),
			_requiredUserClasses(),
			_phonetic(true)
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
			if(_config)
			{
				map.insert(PARAMETER_CONFIG_ID, _config->getKey());
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

			//Coordonates XY
			if(!_coordinatesXY.empty())
			{
				map.insert(PARAMETER_COORDINATES_XY, _coordinatesXY);
			}

			//maxDistance
			if(_maxDistance>0)
			{
				map.insert(PARAMETER_MAX_DISTANCE, _maxDistance);
			}
			
			// dataSourceFilter
			if(_dataSourceFilter)
			{
				map.insert(PARAMETER_DATA_SOURCE_FILTER, _dataSourceFilter->getKey());
			}

			// Phonetic search
			map.insert(PARAMETER_PHONETIC, _phonetic);

			return map;
		}



		void PlacesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Text
			_text = map.getDefault<string>(PARAMETER_TEXT);

			// Number
			_number = map.getOptional<size_t>(PARAMETER_NUMBER);

			// Site filter
			RegistryKeyType configId(map.getDefault<RegistryKeyType>(PARAMETER_CONFIG_ID, 0));
			if(configId > 0) try
			{
				_config = Env::GetOfficialEnv().get<PTServiceConfig>(configId).get();
			}
			catch(ObjectNotFoundException<PTServiceConfig>&)
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

			// Build originPoint
			_coordinatesXY = map.getDefault<string>(PARAMETER_COORDINATES_XY);
			_maxDistance=map.getDefault<double>(PARAMETER_MAX_DISTANCE, _maxDistance);

			if(!_coordinatesXY.empty())
			{
				_parseCoordinates(false);
			}

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
				setOutputFormatFromMap(map, MimeTypes::XML);
			}

			string userClassList(map.getDefault<string>(PARAMETER_COMPATIBLE_USER_CLASSES_LIST));
			try
			{
				if(!userClassList.empty())
				{
					vector<string> userClassVect;
					split(userClassVect, userClassList, is_any_of(",; "));
					BOOST_FOREACH(string& userClass, userClassVect)
					{
						graph::UserClassCode code = lexical_cast<graph::UserClassCode>(userClass);
						// Ignore invalid user class codes
						if(code > graph::USER_CLASS_CODE_OFFSET && code < (graph::USER_CLASS_CODE_OFFSET + graph::USER_CLASSES_VECTOR_SIZE))
							_requiredUserClasses.insert(code);
					}		
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Bad user class code in acList parameter.");
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER)) try
			{
				_dataSourceFilter = Env::GetOfficialEnv().get<impex::DataSource>(map.get<RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER));
			}
			catch (ObjectNotFoundException<impex::DataSource>&)
			{
				throw RequestException("No such data source");
			}

			_phonetic = map.getDefault<bool>(PARAMETER_PHONETIC, _phonetic);
		}



		ParametersMap PlacesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// ParametersMap populating
			ParametersMap result;

			if(!_coordinatesXY.empty())
			{
				//Best place, which is near the originPoint
				boost::shared_ptr<Point> originPoint = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*_originPoint);
				RoadChunkTableSync::SearchResult roadChunks = RoadChunkTableSync::SearchByMaxDistance(
					*originPoint.get(),
					_maxDistance,//distance  to originPoint
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);

				BOOST_FOREACH(const RoadChunkTableSync::SearchResult::value_type& roadChunk, roadChunks)
				{
					RoadChunk& chunk(static_cast<RoadChunk&>(*roadChunk));

					boost::shared_ptr<LineString> chunkGeometry = chunk.getRealGeometry();
					if(!chunkGeometry || chunkGeometry->isEmpty())
					{
						continue;
					}

					bool compatibleWithUserClasses(true);
					BOOST_FOREACH(graph::UserClassCode userClassCode, _requiredUserClasses)
					{
						if(	!chunk.getForwardEdge().isCompatibleWith(graph::AccessParameters(userClassCode)) &&
							!chunk.getReverseEdge().isCompatibleWith(graph::AccessParameters(userClassCode))
						){
							compatibleWithUserClasses = false;
							break;
						}
					}

					if(!compatibleWithUserClasses)
					{
						continue;
					}

					if(!chunk.getRoad() || !chunk.getRoad()->get<RoadPlace>() || chunk.getRoad()->get<RoadPlace>()->getName().empty())
					{
						continue;
					}

					LengthIndexedLine indexedLine(chunkGeometry.get());
					double metricOffset = indexedLine.project(*(originPoint->getCoordinate())) + chunk.getMetricOffset();
					boost::shared_ptr<House> house(new House(*roadChunk, metricOffset, true));

					string name;
					if(!house->getHouseNumber() || *(house->getHouseNumber()) == 0)
					{
						name = chunk.getRoad()->get<RoadPlace>()->getName();
					}
					else
					{
						name = (house.get())->getName();
					}

					if(name.empty())
					{
						continue;
					}

					addHouse(_houseMap, house, name);
				}

				// Best place
				if(!_houseMap.empty())
				{
					size_t nbResult = 0;
					vector<lexical_matcher::LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit > houseList, roadList;
					vector<int> distanceHouseList;
					set<string> insertedRoadName;

					BOOST_FOREACH(const HouseMapType::value_type& it, _houseMap)
					{
						boost::shared_ptr<House> house = it.second;

						if(house.get())
						{
							//house number not available, give road instead
							if(*(house.get())->getHouseNumber() == 0)
							{
								//Don't insert same road twice
								string roadName = (house.get())->getRoadChunk()->getRoad()->get<RoadPlace>()->getName();
								set<string>::iterator frenchIt = insertedRoadName.find(roadName);
								if(frenchIt == insertedRoadName.end())
								{
									LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit roadResult;
									insertedRoadName.insert(roadName);
									roadResult.key = roadName;
									FrenchSentence::ComparisonScore score;
									int distanceToOrigin = it.first.getDistanceToOriginPoint();
									//Hack : levenstein and score are used to order results, so we change them to order by distance
									score.levenshtein = distanceToOrigin;
									score.phoneticScore = 1;
									distanceHouseList.push_back(distanceToOrigin);
									roadResult.score = score;
									roadResult.value = dynamic_pointer_cast<NamedPlace,House>(house);
									//Avoid house name like 0, xxx
									house->setName(roadName);
									houseList.push_back(roadResult);
									nbResult++;
								}
							}
							else
							{
								lexical_matcher::LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit houseResult;

								houseResult.key = (house.get())->getName();
								FrenchSentence::ComparisonScore score;
								//Hack : levenstein and score are used to order results, so we change them to order by distance	
								int distanceToOrigin = it.first.getDistanceToOriginPoint();
								score.levenshtein = distanceToOrigin;
								score.phoneticScore = 1;
								houseResult.score = score;
								houseResult.value = dynamic_pointer_cast<NamedPlace,House>(house);

								distanceHouseList.push_back(distanceToOrigin);
								houseList.push_back(houseResult);

								nbResult++;
							}
						}

						if(_number && nbResult >= *_number)
						{
							break;
						}
					}

					if (!houseList.empty())
					{
						// Registration
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							houseList,
							&distanceHouseList
						);
						
						//adresses
						result.insert(DATA_ADDRESSES, pm);
					}

				}
			}
			else if(_sorted || !_classFilter.empty())
			{
				if(_city.get())
				{
					// Stops
					if(_classFilter.empty() || _classFilter == DATA_STOP)
					{
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);

						LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchResult stopResult;
						LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchResult newStopResult;

						stopResult = _city->getLexicalMatcher(StopArea::FACTORY_KEY).bestMatches(
								_text,
								0,
								_minScore,
								_phonetic
						);

						BOOST_FOREACH(const lexical_matcher::LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit& item, stopResult)
						{
							const pt::StopArea* stop(dynamic_cast<const pt::StopArea*>(&(*item.value)));
							if(stop)
							{
								if(_dataSourceFilter && !stop->hasLinkWithSource(*_dataSourceFilter))
									continue;
							}

							newStopResult.push_back(item);

							if(_number && (newStopResult.size() >= (*_number)))
							{
								break;
							}
						}

						_registerItems<NamedPlace>(
							*pm,
							newStopResult
						);

						result.insert(DATA_STOPS, pm);
					}

					// Roads
					if(_classFilter.empty() || _classFilter == DATA_ROAD || _classFilter == DATA_ADDRESS)
					{
						vector<string> words;
						split(words, _text, is_any_of(", "));
						if(words.size() > 1)
						{	// Text points to an address
							HouseNumber number(0);
							string roadName;
							try
							{
								number = lexical_cast<HouseNumber>(words[0]);
								roadName = _text.substr(words[0].size() + 1);
							}
							catch (bad_lexical_cast)
							{
								// Try number at the end
								try
								{
									number = lexical_cast<HouseNumber>(words[words.size() - 1]);
									roadName = _text.substr(0, _text.size() - words[words.size() - 1].size() - 1);
								}
								catch (bad_lexical_cast)
								{
								}
							}

							if (!roadName.empty())
							{
								City::PlacesMatcher::MatchResult places(
									_city->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(
										roadName,
										_number ? *_number : 0,
										_minScore,
										_phonetic
								)	);

								// Transformation into house places list
								LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchResult houseList;
								BOOST_FOREACH(const City::PlacesMatcher::MatchResult::value_type& place, places)
								{
									const RoadPlace& roadPlace(
										dynamic_cast<const RoadPlace&>(*place.value)
									);

									boost::shared_ptr<House> house(roadPlace.getHouse(number));

									LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit houseResult;
									houseResult.key = place.key;
									houseResult.score = place.score;
									houseResult.value = house.get() ?
										dynamic_pointer_cast<NamedPlace, House>(house) :
										place.value
									;

									houseList.push_back(houseResult);
								}

								// Registration
								boost::shared_ptr<ParametersMap> pm(new ParametersMap);
								_registerItems<NamedPlace>(
									*pm,
									houseList
								);
								result.insert(DATA_ADDRESSES, pm);
							}
						}

						// Roads if not address
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore,
								_phonetic
						)	);
						result.insert(DATA_ROADS, pm);
					}

					// Public places
					if(_classFilter.empty() || _classFilter == DATA_PUBLIC_PLACE)
					{
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<NamedPlace>(
							*pm,
							_city->getLexicalMatcher(PublicPlace::FACTORY_KEY).bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore,
								_phonetic
						)	);
						result.insert(DATA_PUBLIC_PLACES, pm);
					}
				}
				else if(_config)
				{
					/// TODO implement it
				}
				else
				{
					// Cities
					if(_classFilter.empty() || _classFilter == DATA_CITY)
					{
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<City>(
							*pm,
							GeographyModule::GetCitiesMatcher().bestMatches(
								_text,
								(_number && !_citiesWithAtLeastAStop) ? *_number : 0,
								_minScore,
								_phonetic
						)	);
						result.insert(DATA_CITIES, pm);
					}

					// Stops
					if(_classFilter.empty() || _classFilter == DATA_STOP)
					{
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);

						LexicalMatcher<boost::shared_ptr<StopArea> >::MatchResult stopResult;
						LexicalMatcher<boost::shared_ptr<StopArea> >::MatchResult newStopResult;

						stopResult = PTModule::GetGeneralStopsMatcher().bestMatches(
								_text,
								0,
								_minScore,
								_phonetic
						);

						BOOST_FOREACH(const lexical_matcher::LexicalMatcher<boost::shared_ptr<StopArea> >::MatchHit& item, stopResult)
						{
							const pt::StopArea* stop(dynamic_cast<const pt::StopArea*>(&(*item.value)));
							if(stop)
							{
								if(_dataSourceFilter && !stop->hasLinkWithSource(*_dataSourceFilter))
									continue;
							}

							newStopResult.push_back(item);

							if(_number && (newStopResult.size() >= (*_number)))
							{
								break;
							}
						}

						_registerItems<StopArea>(
							*pm,
							newStopResult
						);

						result.insert(DATA_STOPS, pm);
					}

					// Roads
					if(_classFilter.empty() || _classFilter == DATA_ROAD || _classFilter == DATA_ADDRESS)
					{
						LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchResult roadList;
						vector<string> words;
						split(words, _text, is_any_of(", "));
						if(words.size() > 1)
						{	// Text points to an address
							HouseNumber number(0);
							string roadName;
							try
							{
								number = lexical_cast<HouseNumber>(words[0]);
								roadName = _text.substr(words[0].size() + 1);
							}
							catch (bad_lexical_cast)
							{
								// Try number at the end
								try
								{
									number = lexical_cast<HouseNumber>(words[words.size() - 1]);
									roadName = _text.substr(0, _text.size() - words[words.size() - 1].size() - 1);
								}
								catch (bad_lexical_cast)
								{
								}
							}

							if (!roadName.empty())
							{
								RoadModule::GeneralRoadsMatcher::MatchResult places(
									RoadModule::GetGeneralRoadsMatcher().bestMatches(
										roadName,
										_number ? *_number : 0,
										_minScore,
										_phonetic
								)	);

								// Transformation into house places list
								LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchResult houseList;
								BOOST_FOREACH(const RoadModule::GeneralRoadsMatcher::MatchResult::value_type& place, places)
								{
									const RoadPlace& roadPlace(
										dynamic_cast<const RoadPlace&>(*place.value)
									);

									boost::shared_ptr<House> house(roadPlace.getHouse(number));

									LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit houseResult;
									houseResult.key = place.key;
									houseResult.score = place.score;
									// If house.get() this is a ADRESS else this is a ROAD
									if(house.get())
									{
										houseResult.value = dynamic_pointer_cast<NamedPlace, House>(house);
										houseList.push_back(houseResult);
									}
									else
									{
										houseResult.value = place.value;
										roadList.push_back(houseResult);
									}
								}

								// Registration
								boost::shared_ptr<ParametersMap> pm(new ParametersMap);
								_registerItems<NamedPlace>(
									*pm,
									houseList
								);
								result.insert(DATA_ADDRESSES, pm);
							}
						}

						// Roads if not address
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);
						RoadModule::GeneralRoadsMatcher::MatchResult roads(
							RoadModule::GetGeneralRoadsMatcher().bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore,
								_phonetic
						)	);
						// We add road results to the roadList
						BOOST_FOREACH(const RoadModule::GeneralRoadsMatcher::MatchResult::value_type& road, roads)
						{
							LexicalMatcher<boost::shared_ptr<NamedPlace> >::MatchHit roadResult;

							roadResult.key = road.key;
							roadResult.score = road.score;
							roadResult.value = road.value;

							roadList.push_back(roadResult);
						}
						_registerItems<NamedPlace>(
							*pm,
							roadList
						);
						result.insert(DATA_ROADS, pm);
					}

					// Public places
					if(_classFilter.empty() || _classFilter == DATA_PUBLIC_PLACE)
					{
						boost::shared_ptr<ParametersMap> pm(new ParametersMap);
						_registerItems<PublicPlace>(
							*pm,
							RoadModule::GetGeneralPublicPlacesMatcher().bestMatches(
								_text,
								_number ? *_number : 0,
								_minScore,
								_phonetic
						)	);
						result.insert(DATA_PUBLIC_PLACES, pm);
					}
				}
			}
			else
			{
				boost::shared_ptr<ParametersMap> pm(new ParametersMap);
				if(_city.get())
				{ /// TODO take into account of _classFilter
					_registerItems<NamedPlace>(
						*pm,
						_city->getAllPlacesMatcher().bestMatches(
							_text,
							_number ? *_number : 0,
							_minScore,
							_phonetic
					)	);
				}
				else if(_config)
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
							_minScore,
							_phonetic
					)	);
				}
				result.insert(DATA_PLACES, pm);
			}

			// Best place
			boost::shared_ptr<ParametersMap> bestMap;
			string className;
			if(_sorted)
			{
				boost::shared_ptr<ParametersMap> bestPlace(new ParametersMap);

				// Stop areas
				if(result.hasSubMaps(DATA_STOPS) &&
					(*result.getSubMaps(DATA_STOPS).begin())->hasSubMaps(DATA_STOP)
				){
					bestMap = *(*result.getSubMaps(DATA_STOPS).begin())->getSubMaps(DATA_STOP).begin();
					className = DATA_STOP;
				}

				// Cities
				if(result.hasSubMaps(DATA_CITIES) &&
					(*result.getSubMaps(DATA_CITIES).begin())->hasSubMaps(DATA_CITY)
				){
					boost::shared_ptr<ParametersMap> cityBestMap(
						*(*result.getSubMaps(DATA_CITIES).begin())->getSubMaps(DATA_CITY).begin()
					);
					if(!bestMap.get() ||
						cityBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						(cityBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						cityBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN))
					){
						bestMap = cityBestMap;
						className = DATA_CITY;
					}
				}

				// Public places
				if(result.hasSubMaps(DATA_PUBLIC_PLACES) &&
					(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->hasSubMaps(DATA_PUBLIC_PLACE)
				){
					boost::shared_ptr<ParametersMap> ppBestMap(
						*(*result.getSubMaps(DATA_PUBLIC_PLACES).begin())->getSubMaps(DATA_PUBLIC_PLACE).begin()
					);
					if(!bestMap.get() ||
						ppBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						(ppBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						ppBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN))
					){
						bestMap = ppBestMap;
						className = DATA_PUBLIC_PLACE;
					}
				}

				// Streets
				if(	result.hasSubMaps(DATA_ROADS) &&
					(*result.getSubMaps(DATA_ROADS).begin())->hasSubMaps(DATA_ROAD)
				){
					boost::shared_ptr<ParametersMap> roadBestMap(
						*(*result.getSubMaps(DATA_ROADS).begin())->getSubMaps(DATA_ROAD).begin()
					);
					if(	!bestMap.get() ||
						roadBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						(roadBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						roadBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN))
					){
						bestMap = roadBestMap;
						className = DATA_ROAD;
					}
				}

				// Addresses
				if(	result.hasSubMaps(DATA_ADDRESSES) &&
					(*result.getSubMaps(DATA_ADDRESSES).begin())->hasSubMaps(DATA_ADDRESS)
				){
					boost::shared_ptr<ParametersMap> addressBestMap(
						*(*result.getSubMaps(DATA_ADDRESSES).begin())->getSubMaps(DATA_ADDRESS).begin()
					);
					if(	!bestMap.get() ||
						addressBestMap->get<double>(DATA_PHONETIC_SCORE) > bestMap->get<double>(DATA_PHONETIC_SCORE) ||
						(addressBestMap->get<double>(DATA_PHONETIC_SCORE) == bestMap->get<double>(DATA_PHONETIC_SCORE) &&
						addressBestMap->get<double>(DATA_LEVENSHTEIN) < bestMap->get<double>(DATA_LEVENSHTEIN))
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
						if(result.hasSubMaps(DATA_CITIES) &&
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
						if(result.hasSubMaps(DATA_STOPS) &&
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
						if(result.hasSubMaps(DATA_ADDRESSES) &&
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
						if(result.hasSubMaps(DATA_ROADS) &&
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
						if(result.hasSubMaps(DATA_PUBLIC_PLACES) &&
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
			else
			{
				outputParametersMap(
					result,
					stream,
					DATA_PLACES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/56_pt_website/sorted_places_list.xsd",
					_sorted,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/56_pt_website/unsorted_places_list.xsd"
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
			return _itemPage.get() ? _itemPage->getMimeType() : getOutputMimeTypeFromOutputFormat();
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
		) const {

			BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& item, maps)
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
			boost::shared_ptr<ParametersMap> bestPlace,
			const string& bestPlaceClassName
		) const {
			ParametersMap classMap(getTemplateParameters());

			size_t rank(0);

			//min distance to origin
			if(_originPoint)
			{
					//origin
					classMap.insert(DATA_ORIGIN_X, _originPoint->getX());
					classMap.insert(DATA_ORIGIN_Y, _originPoint->getY());
			}

			//Insert best place
			if(bestPlace.get())
			{
				stringstream bestPlaceStream;
				vector<boost::shared_ptr<ParametersMap> > bestPlaceMap;
				bestPlaceMap.push_back(bestPlace);
				_displayItems(
					bestPlaceStream,
					bestPlaceClassName,
					bestPlaceMap,
					request,
					rank
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
					request,
					rank
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
					request,
					rank
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
					request,
					rank
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
					request,
					rank
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
					request,
					rank
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
				boost::shared_ptr<ParametersMap> bestPlaceMap(*result.getSubMaps(DATA_BEST_PLACE).begin());

				// Class of the best place
				ParametersMap::SubMapsKeys keys(bestPlaceMap->getSubMapsKeys());
				if(keys.empty())
				{
					return placeResult;
				}
				string className(*keys.begin());
				boost::shared_ptr<ParametersMap> itemMap(
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
					const boost::shared_ptr<RoadPlace>& roadPlace(
						Env::GetOfficialEnv().getEditable<RoadPlace>(
							itemMap->get<RegistryKeyType>(
								House::DATA_ROAD_PREFIX + RoadPlace::DATA_ID
					)	)	);
					HouseNumber houseNumber = itemMap->get<HouseNumber>(House::DATA_NUMBER);
					if (!_houseMap.empty())
					{
						placeResult.value = _houseMap.begin()->second;
					}
					else if(houseNumber != 0)
					{
						placeResult.value = static_pointer_cast<Place, House>(
							roadPlace->getHouse(houseNumber)
						);
					}
					else
					{
						// _houseMap is empty so give back the road
						placeResult.value = roadPlace;
					}
				}
			}

			return placeResult;
		}

		//Sort house by distance to originPoint
		PlacesListService::SortHouseByDistanceToOriginPoint::SortHouseByDistanceToOriginPoint(const boost::shared_ptr<House> house, int distanceToOriginPoint, string name):
			_house(house),
			_distanceToOriginPoint(distanceToOriginPoint),
			_name(name)
		{
		}

		bool PlacesListService::SortHouseByDistanceToOriginPoint::operator<(SortHouseByDistanceToOriginPoint const &otherHouse) const
		{
			return (_distanceToOriginPoint == otherHouse.getDistanceToOriginPoint() )? _name <  otherHouse.getName() :_distanceToOriginPoint < otherHouse.getDistanceToOriginPoint();
		}

		int PlacesListService::SortHouseByDistanceToOriginPoint::getDistanceToOriginPoint() const
		{
			return _distanceToOriginPoint;
		}

		const boost::shared_ptr<House> PlacesListService::SortHouseByDistanceToOriginPoint::getHouse() const
		{
			return _house;
		}

		int PlacesListService::CalcDistanceToOriginPoint(const boost::shared_ptr<House> & house) const
		{
			//return value
			int distanceToOriginPoint = 0;

			if(_originPoint && house.get())
			{
				boost::shared_ptr<LineString> gp = (house.get())->getRoadChunk()->getGeometry();
				boost::shared_ptr<Point> originPoint = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*_originPoint);

				if(gp.get())
				{
					distanceToOriginPoint = gp->distance(originPoint.get());
				}
			}

			//return value
			return round(distanceToOriginPoint);
		}

		void PlacesListService::addHouse(
				HouseMapType& houseMap,
				const boost::shared_ptr<House> house,
				string name
			) const {

			int distanceToOriginPoint = CalcDistanceToOriginPoint(house);
			SortHouseByDistanceToOriginPoint keyHouse(house, distanceToOriginPoint, name);
			houseMap.insert(pair<const SortHouseByDistanceToOriginPoint,const boost::shared_ptr<House> >(keyHouse,house));
		}

		void PlacesListService::_parseCoordinates(bool invertXY) {
			vector< string > parsed_coordinatesXY;
			split(parsed_coordinatesXY, _coordinatesXY, is_any_of(",; ") );

			if(parsed_coordinatesXY.size() != 2)
			{
				throw RequestException("Malformed COORDINATES_XY.");
			}

			try
			{
				if (invertXY)
				{
					_originPoint = _coordinatesSystem->createPoint(lexical_cast<double>(parsed_coordinatesXY[1]), lexical_cast<double>(parsed_coordinatesXY[0]));
				}
				else
				{
					_originPoint = _coordinatesSystem->createPoint(lexical_cast<double>(parsed_coordinatesXY[0]), lexical_cast<double>(parsed_coordinatesXY[1]));
				}
			}
			catch(bad_lexical_cast)
			{
				throw RequestException("Malformed COORDINATES_XY.");
			}
		}
}	}
