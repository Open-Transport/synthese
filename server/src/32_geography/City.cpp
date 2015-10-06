
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

#include "CityTableSync.h"
#include "Factory.h"
#include "GeographyModule.h"
#include "GlobalRight.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Registry.h"
#include "User.h"

#include <assert.h>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace lexical_matcher;
	using namespace util;
	using namespace geography;
	using namespace graph;

	CLASS_DEFINITION(City, "t006_cities", 6)
	FIELD_DEFINITION_OF_OBJECT(City, "city_id", "city_ids")

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
			Object<City, CitySchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_VALUE_CONSTRUCTOR(Name, name),
					FIELD_VALUE_CONSTRUCTOR(Code, code)
			)	),
			IncludingPlace<NamedPlace>()
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

			// TODO remove this method and move the additional elements into addAdditionalParameters
			pm.insert(prefix + Key::FIELD.name, get<Key>());
			pm.insert(prefix + Name::FIELD.name, get<Name>());
			pm.insert(prefix + Code::FIELD.name, get<Code>());

			// Id
			pm.insert(prefix + DATA_CITY_ID, getKey());

			// Name
			pm.insert(prefix + DATA_CITY_NAME, get<Name>());

			// Code
			pm.insert(prefix + DATA_CITY_CODE, get<Code>());

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



		bool City::loadFromRecord( const Record& record, util::Env& env )
		{
			bool updated(false);

			// Name
			if(record.isDefined(CityTableSync::TABLE_COL_NAME))
			{
				string value(
					record.get<string>(CityTableSync::TABLE_COL_NAME)
				);
				if(value != getName())
				{
					updated = true;
					set<Name>(value);
				}
			}

			// Code
			if(record.isDefined(CityTableSync::TABLE_COL_CODE))
			{
				string value(
					record.get<string>(CityTableSync::TABLE_COL_CODE)
				);
				if(value != get<Code>())
				{
					set<Code>(value);
					updated = true;
				}
			}

			return updated;
		}



		void City::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(&env == &Env::GetOfficialEnv())
			{
				// Add to cities and all places matcher
				GeographyModule::AddToCitiesMatchers(env.getEditableSPtr(this));
			}
		}



		void City::unlink()
		{
			// Remove from cities matcher
			GeographyModule::RemoveFromCitiesMatchers(Env::GetOfficialEnv().getEditableSPtr(this));

			// Adds a city with same name if necessary
			CityTableSync::SearchResult others(
				CityTableSync::Search(
					Env::GetOfficialEnv(),
					get<Name>()
			)	);
			BOOST_FOREACH(boost::shared_ptr<City> other, others)
			{
				if(other->getKey() != getKey())
				{
					GeographyModule::AddToCitiesMatchers(other);
					break;
				}
			}
		}


		bool City::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool City::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool City::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
