
/** RoadModule class implementation.
	@file RoadModule.cpp

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

#include "RoadModule.h"
#include "GeographyModule.h"
#include "MainRoadChunk.hpp"
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
	using namespace lexical_matcher;



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
		RoadModule::GeneralRoadsMatcher RoadModule::_generalRoadsMatcher;
		RoadModule::GeneralPublicPlacesMatcher RoadModule::_generalPublicPlacesMatcher;


		RoadModule::ExtendedFetchPlaceResult RoadModule::ExtendedFetchPlace(
			const GeographyModule::CitiesMatcher& citiesMatcher,
			const std::string& cityName,
			const std::string& placeName
		){
			RoadModule::ExtendedFetchPlacesResult result(ExtendedFetchPlaces(citiesMatcher, cityName, placeName, 1));
			if(result.empty())
			{
				throw Exception("No place was found");
			}
			return result.front();
		}



		RoadModule::ExtendedFetchPlaceResult RoadModule::ExtendedFetchPlace(
			const std::string& cityName,
			const std::string& placeName
		){
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



		RoadModule::ExtendedFetchPlacesResult RoadModule::ExtendedFetchPlaces( const std::string& cityName, const std::string& placeName, std::size_t resultsNumber )
		{
			return ExtendedFetchPlaces(GeographyModule::GetCitiesMatcher(), cityName, placeName, resultsNumber);
		}



		RoadModule::ExtendedFetchPlacesResult RoadModule::ExtendedFetchPlaces(
			const geography::GeographyModule::CitiesMatcher& citiesMatcher,
			const std::string& cityName,
			const std::string& placeName,
			std::size_t resultsNumber
		){
			if (cityName.empty())
			{
				throw UndeterminedPlaceException(cityName, placeName, UndeterminedPlaceException::EMPTY_CITY);
			}

			GeographyModule::CitiesMatcher::MatchResult cities(
				citiesMatcher.bestMatches(cityName,50)
			);
			BOOST_FOREACH(const GeographyModule::CitiesMatcher::MatchResult::value_type& city, cities)
			{
				RoadModule::ExtendedFetchPlacesResult result(
					ExtendedFetchPlaces(city, placeName, resultsNumber)
				);
				if(!result.empty())
				{
					return result;
				}
			}
			throw UndeterminedPlaceException(cityName, placeName, UndeterminedPlaceException::NO_RESULT_FROM_CITY_SEARCH);
		}



		RoadModule::ExtendedFetchPlacesResult RoadModule::ExtendedFetchPlaces(
			boost::shared_ptr<geography::City> city,
			const std::string& placeName,
			std::size_t resultsNumber
		){
			GeographyModule::CitiesMatcher::MatchResult::value_type cityResult;
			cityResult.key = FrenchSentence(city->getName()+" "+ city->getCode());
			cityResult.score.levenshtein = 0;
			cityResult.score.phoneticScore = 1;
			cityResult.value = city;
			return ExtendedFetchPlaces(cityResult, placeName, resultsNumber);
		}


		RoadModule::ExtendedFetchPlacesResult RoadModule::ExtendedFetchPlaces(
			const GeographyModule::CitiesMatcher::MatchResult::value_type& cityResult,
			const std::string& placeName,
			std::size_t resultsNumber
		){
			ExtendedFetchPlacesResult result;

			if(placeName.empty())
			{	// Default place of the city
				ExtendedFetchPlaceResult defaultPlaceResult;
				defaultPlaceResult.cityResult = cityResult;
				defaultPlaceResult.placeResult.key = defaultPlaceResult.cityResult.key;
				defaultPlaceResult.placeResult.score = defaultPlaceResult.cityResult.score;
				defaultPlaceResult.placeResult.value = defaultPlaceResult.cityResult.value;
				assert(defaultPlaceResult.placeResult.value != NULL);
				result.push_back(defaultPlaceResult);

				return result;
			}


			vector<string> words;
			split(words, placeName, is_any_of(", "));
			if(words.size() > 1)
			{	// Text points to an address
				bool numberAtBeginning(true);
				MainRoadChunk::HouseNumber number(0);
				try
				{
					number = lexical_cast<MainRoadChunk::HouseNumber>(words[0]);
				}
				catch(bad_lexical_cast)
				{
					try
					{
						number = lexical_cast<MainRoadChunk::HouseNumber>(words[words.size()-1]);
						numberAtBeginning = false;
					}
					catch(bad_lexical_cast)
					{

					}

				}
				if(number > 0)
				{
					string roadName(placeName.substr(words[0].size() + 1));

					City::PlacesMatcher::MatchResult places(
						cityResult.value->getLexicalMatcher(RoadPlace::FACTORY_KEY).bestMatches(roadName, resultsNumber)
					);

					BOOST_FOREACH(const City::PlacesMatcher::MatchResult::value_type& place, places)
					{
						const RoadPlace& roadPlace(
							dynamic_cast<const RoadPlace&>(*place.value)
						);

						shared_ptr<House> house(roadPlace.getHouse(number, numberAtBeginning));

						ExtendedFetchPlaceResult placeResult;
						placeResult.cityResult = cityResult;
						placeResult.placeResult.key = place.key;
						placeResult.placeResult.score = place.score;
						placeResult.placeResult.value = house.get() ? house : place.value;

						result.push_back(placeResult);
					}

					return result;
				}
			}


			// Text points to a stop or a street
			City::PlacesMatcher::MatchResult places(
				cityResult.value->getAllPlacesMatcher().bestMatches(placeName, resultsNumber)
			);
			BOOST_FOREACH(const City::PlacesMatcher::MatchResult::value_type& place, places)
			{
				ExtendedFetchPlaceResult placeResult;
				placeResult.cityResult = cityResult;
				placeResult.placeResult.key = place.key;
				placeResult.placeResult.score = place.score;
				placeResult.placeResult.value = place.value;

				result.push_back(placeResult);
			}

			return result;
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
