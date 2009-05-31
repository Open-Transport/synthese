
/** City class implementation.
	@file City.cpp

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

#include "City.h"
#include "Registry.h"
#include "Factory.h"

#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace lexmatcher;
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<geography::City>::KEY("City");
	}

	namespace geography
	{

		City::City(
			RegistryKeyType key,
			std::string name,
			std::string code
		):	IncludingPlace(),
			_code (code),
			_name(name),
			Registrable(key)
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
		{

		}


		void City::getVertexAccessMap(
			VertexAccessMap& result
			, const AccessDirection& accessDirection
			, const AccessParameters& accessParameters
			, GraphIdType whatToSearch
		) const {

/*			if (_includedPlaces.empty ())
			{
				if (_connectionPlacesMatcher.size () > 0)
				{
					_connectionPlacesMatcher.entries().begin()->second->getVertexAccessMap(
						result, accessDirection, accessParameters, 
						whatToSearch
					);
				}
				else if (_placeAliasesMatcher.size () > 0)
				{
					_placeAliasesMatcher.entries().begin()->second->getVertexAccessMap(
						result, accessDirection, accessParameters
						, whatToSearch
					);
				}
				else if (_publicPlacesMatcher.size () > 0)
				{
					_publicPlacesMatcher.entries().begin()->second->getVertexAccessMap(
						result, accessDirection, accessParameters
						, whatToSearch
					);
				}
				else if (_roadsMatcher.size () > 0)
				{
					_roadsMatcher.entries().begin()->second->getVertexAccessMap(
						result, accessDirection, accessParameters
						, whatToSearch
					);
				}
			}
			else
*/			{
				IncludingPlace::getVertexAccessMap(
					result, accessDirection, accessParameters, 
					whatToSearch
				);
			}
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
	}
}
