
/** City class implementation.
	@file City.cpp

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

#include "City.h"
#include "Registry.h"
#include "Factory.h"
#include "ParametersMap.h"

#include <assert.h>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace lexical_matcher;
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<geography::City>::KEY("City");
	}

	namespace geography
	{
		const std::string City::DATA_CITY_ID = "city_id";
		const std::string City::DATA_CITY_NAME = "city_name";
		const std::string City::DATA_CITY_CODE = "city_code";
		const std::string City::DATA_CITY_X = "city_x";
		const std::string City::DATA_CITY_Y = "city_y";



		City::City(
			RegistryKeyType key,
			std::string name,
			std::string code
		):	Registrable(key),
			IncludingPlace<NamedPlace>(),
			_name(name),
			_code (code)
		{
			Factory<NamedPlace>::Keys keys(Factory<NamedPlace>::GetKeys());
			BOOST_FOREACH(const string& key, keys)
			{
				_lexicalMatchers.insert(
					make_pair(key, PlacesMatcher())
				);
			}
		}



		City::~City ()
		{}



		void City::getVertexAccessMap(
			VertexAccessMap& result,
			const AccessParameters& accessParameters,
			const GraphTypes& whatToSearch
		) const {

			IncludingPlace<NamedPlace>::getVertexAccessMap(
				result,
				accessParameters,
				whatToSearch
			);

		}



		City::PlacesMatcher& City::getAllPlacesMatcher()
		{
			return _allPlacesMatcher;
		}



		const City::PlacesMatcher& City::getAllPlacesMatcher() const
		{
			return _allPlacesMatcher;
		}



		City::PlacesMatcher& City::getLexicalMatcher(
			const std::string& key
		){
			PlacesMatchers::iterator it(_lexicalMatchers.find(key));
			assert(it != _lexicalMatchers.end());
			return it->second;
		}



		const City::PlacesMatcher& City::getLexicalMatcher(
			const std::string& key
		) const	{
			PlacesMatchers::const_iterator it(_lexicalMatchers.find(key));
			assert(it != _lexicalMatchers.end());
			return it->second;
		}


		void City::toParametersMap(
			ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			const string& prefix
		) const {
			// Id
			pm.insert(prefix + DATA_CITY_ID, getKey());

			// Name
			pm.insert(prefix + DATA_CITY_NAME, getName());

			// Code
			pm.insert(prefix + DATA_CITY_CODE, _code);

			// X and Y
			if(coordinatesSystem && getPoint().get())
			{
				boost::shared_ptr<Point> center(
					coordinatesSystem->convertPoint(*getPoint())
				);
				{
					stringstream s;
					s << std::fixed << center->getX();
					pm.insert(prefix + DATA_CITY_X, s.str());
				}
				{
					stringstream s;
					s << std::fixed << center->getY();
					pm.insert(prefix + DATA_CITY_Y, s.str());
				}
			}
		}



		void City::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {
			static string fakePrefix;
			toParametersMap(
				pm,
				&CoordinatesSystem::GetInstanceCoordinatesSystem(),
				prefix
			);
		}



		void City::addPlaceToMatcher( PlacesMatcher::Content place )
		{
			getLexicalMatcher(place->getFactoryKey()).add(place->getName(), place);
			_allPlacesMatcher.add(place->getNameForAllPlacesMatcher(),place);
		}



		void City::removePlaceFromMatcher(
			const geography::NamedPlace& place
		){
			getLexicalMatcher(place.getFactoryKey()).remove(place.getName());
			_allPlacesMatcher.remove(place.getNameForAllPlacesMatcher());
		}



		bool City::empty() const
		{
			BOOST_FOREACH(const PlacesMatchers::value_type& matcher, _lexicalMatchers)
			{
				if(matcher.second.size() != 0)
				{
					return false;
				}
			}
			return true;
		}
}	}
