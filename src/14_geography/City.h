
/** City class header.
	@file City.h

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

#ifndef SYNTHESE_ENV_CITY_H
#define SYNTHESE_ENV_CITY_H


#include "Registry.h"
#include "LexicalMatcher.h"
#include "IncludingPlace.h"

#include <string>
#include <iostream>
#include <vector>


namespace synthese
{
	namespace geography
	{
		class NamedPlace;

		/** City class.

			A city holds in its included places the main connection places
			 (those taken by default when no stop is explicitly chosen).

			@ingroup m14
		*/
		class City:
			public IncludingPlace,
			public util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<City>	Registry;

			typedef lexmatcher::LexicalMatcher<const Place*> LexicalMatcher;

		private:
			typedef std::map<std::string, LexicalMatcher> Lexicalmatchers;
			
			LexicalMatcher	_allPlacesMatcher;
			Lexicalmatchers _lexicalMatchers;
			
			std::string _name;
			std::string _code; //!< Unique code identifier for city within its country (france => INSEE code)

		public:

			City(
				util::RegistryKeyType key = UNKNOWN_VALUE,
				std::string name = std::string(),
				std::string code = std::string()
			);
		    
			~City ();


			//! @name Getters/Setters
			//@{
				const std::string& getCode () const { return _code; }
				void setCode (const std::string& code) { _code = code; }
				const std::string& getName() const { return _name; }
				void setName(const std::string& code) { _name = code; }

				LexicalMatcher& getLexicalMatcher(
					const std::string& key
				);
				const LexicalMatcher& getLexicalMatcher(
					const std::string& key
				) const;

				LexicalMatcher& getAllPlacesMatcher(
				);

				const LexicalMatcher& getAllPlacesMatcher(
				) const;
			//@}

			//! @name Update methods
			//@{

			//@}

			//! @name Query methods
			//@{
				void getVertexAccessMap(
					graph::VertexAccessMap& result, 
					const graph::AccessDirection& accessDirection,
					const graph::AccessParameters& accessParameters,
					graph::GraphIdType whatToSearch
				) const;
				
				template<class T>
				std::vector<const T*> search(
					const std::string& fuzzyName,
					int nbMatches = 10
				) const {
					std::vector<const T*> result;
					const LexicalMatcher<const NamedPlace*>& matcher(_lexicalMatchers[T::FACTORY_KEY]);
					LexicalMatcher<const NamedPlace*>::MatchResult matches = matcher.bestMatches (
						fuzzyName, nbMatches
						);
					BOOST_FOREACH(const LexicalMatcher<const NamedPlace*>::MatchResult::value_type& it, matches)
					{
						result.push_back(static_cast<const T*>(it.value));
					}
					return result;
				}


				template<class T>
				void addPlaceToMatcher(
					const T* place
				){
					getLexicalMatcher(place->getFactoryKey()).add(place->getName(), place);
					_allPlacesMatcher.add(place->getNameForAllPlacesMatcher(),place);
				}


				template<class T>
				void removePlaceFromMatcher(
					const T* place
				){
					getLexicalMatcher(place->getFactoryKey()).remove(place->getName());
					_allPlacesMatcher.remove(place->getNameForAllPlacesMatcher());
				}

			//@}
		};
	}
}

#endif
