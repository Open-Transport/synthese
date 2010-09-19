
/** RoadModule class implementation.
	@file RoadModule.cpp

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

#include "RoadModule.h"
#include "GeographyModule.h"
#include "RoadChunk.h"
#include "House.hpp"
#include "RoadPlace.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace road;
	using namespace server;
	using namespace geography;


	namespace graph
	{
		template<> const GraphIdType GraphModuleTemplate<RoadModule>::GRAPH_ID(1);
	}

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,RoadModule>::FACTORY_KEY("34_road");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<RoadModule>::NAME("Voirie");
		
		template<> void ModuleClassTemplate<RoadModule>::PreInit()
		{
		}
		
		template<> void ModuleClassTemplate<RoadModule>::Init()
		{
		}
		
		template<> void ModuleClassTemplate<RoadModule>::End()
		{
		}
	}

	namespace road
	{
		RoadModule::ExtendedFetchPlaceResult RoadModule::ExtendedFetchPlace(
			const GeographyModule::CitiesMatcher& citiesMatcher,
			const std::string& cityName,
			const std::string& placeName
		){

			ExtendedFetchPlaceResult result;

			if (cityName.empty())
			{
				throw UndeterminedPlaceException(cityName, placeName, UndeterminedPlaceException::EMPTY_CITY);
			}

			GeographyModule::CitiesMatcher::MatchResult cities(
				citiesMatcher.bestMatches(cityName,1)
			);
			if(cities.empty())
			{
				throw UndeterminedPlaceException(cityName, placeName, UndeterminedPlaceException::NO_RESULT_FROM_CITY_SEARCH);
			}
			result.cityResult = cities.front();
			result.placeResult.key = result.cityResult.key;
			result.placeResult.score = result.cityResult.score;
			result.placeResult.value = result.cityResult.value;

			assert(result.placeResult.value != NULL);
			
			if (!placeName.empty())
			{
				vector<string> words;
				split(words, placeName, is_any_of(", "));
				if(words.size() > 1)
				{
					try
					{
						RoadChunk::HouseNumber number(lexical_cast<RoadChunk::HouseNumber>(words[0]));

						string roadName(placeName.substr(words[0].size() + 1));

						City::PlacesMatcher::MatchResult places(
							result.cityResult.value->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(roadName, 1)
						);

						if (!places.empty())
						{
							const RoadPlace& roadPlace(
								dynamic_cast<const RoadPlace&>(*places.front().value)
							);

							shared_ptr<House> house(roadPlace.getHouse(number));

							result.placeResult.key = places.front().key;
							result.placeResult.score = places.front().score;
							result.placeResult.value = house;
						}

						return result;
					}
					catch (bad_lexical_cast)
					{
					}
				}



				City::PlacesMatcher::MatchResult places(
					result.cityResult.value->getAllPlacesMatcher().bestMatches(placeName, 1)
				);
				if (!places.empty())
				{
					result.placeResult.key = places.front().key;
					result.placeResult.score = places.front().score;
					result.placeResult.value = places.front().value;
				}
			}

			return result;

		}



		RoadModule::ExtendedFetchPlaceResult RoadModule::ExtendedFetchPlace( const std::string& cityName, const std::string& placeName )
		{
			return ExtendedFetchPlace(GeographyModule::GetCitiesMatcher(), cityName, placeName);
		}



		boost::shared_ptr<geography::Place> RoadModule::FetchPlace(
			const GeographyModule::CitiesMatcher& citiesMatcher,
			const std::string& cityName,
			const std::string& placeName
		){
			return ExtendedFetchPlace(citiesMatcher, cityName, placeName).placeResult.value;
		}



		boost::shared_ptr<geography::Place> RoadModule::FetchPlace( const std::string& cityName, const std::string& placeName )
		{
			return FetchPlace(GeographyModule::GetCitiesMatcher(), cityName, placeName);
		}

		

		RoadModule::UndeterminedPlaceException::UndeterminedPlaceException(
			const std::string& cityName,
			const std::string& placeName,
			RoadModule::UndeterminedPlaceException::Reason reason
		):	Exception(
				"Undetermined place, because " + string((reason == EMPTY_CITY) ? "city text is empty" : "city search got no results") + " (entered text was city="+ cityName + ", place="+ placeName +")"
			)
		{
		}
}	}
