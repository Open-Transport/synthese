
/** City class header.
	@file City.h

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

#ifndef SYNTHESE_ENV_CITY_H
#define SYNTHESE_ENV_CITY_H

#include "Object.hpp"

#include "CoordinatesSystem.hpp"
#include "IncludingPlace.h"
#include "LexicalMatcher.h"
#include "NamedPlace.h"
#include "Registry.h"
#include "StringField.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace geography
	{
		class NamedPlace;

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Code)		//!< Unique code identifier for city within its country (france => INSEE code)
		> CitySchema;

		/** City class.

			A city holds in its included places the main connection places
			 (those taken by default when no stop is explicitly chosen).

			@ingroup m32
		*/
		class City:
			public Object<City, CitySchema>,
			public IncludingPlace<NamedPlace>
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<City>	Registry;

			typedef lexical_matcher::LexicalMatcher<boost::shared_ptr<NamedPlace> > PlacesMatcher;

			static const std::string DATA_CITY_ID;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_CITY_CODE;
			static const std::string DATA_CITY_X;
			static const std::string DATA_CITY_Y;

			typedef std::map<std::string, PlacesMatcher> PlacesMatchers;

		private:
			PlacesMatcher	_allPlacesMatcher;
			PlacesMatchers _lexicalMatchers;

		public:
			City(
				util::RegistryKeyType key = 0,
				std::string name = std::string(),
				std::string code = std::string()
			);

			~City ();


			//! @name Getters/Setters
			//@{
				PlacesMatcher& getLexicalMatcher(
					const std::string& key
				);
				const PlacesMatcher& getLexicalMatcher(
					const std::string& key
				) const;

				PlacesMatcher& getAllPlacesMatcher(
				);

				const PlacesMatcher& getAllPlacesMatcher(
				) const;
			//@}

			//! @name Update methods
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if no place belongs to the city.
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2012
				bool empty() const;



				void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const GraphTypes& whatToSearch
				) const;



				template<class T>
				std::vector<boost::shared_ptr<T> > search(
					const std::string& fuzzyName,
					int nbMatches = 10
				) const {
					std::vector<boost::shared_ptr<T> > result;
					const PlacesMatchers::const_iterator itMatcher(_lexicalMatchers.find(T::FACTORY_KEY));
					assert(itMatcher != _lexicalMatchers.end());
					PlacesMatcher::MatchResult matches = itMatcher->second.bestMatches (
						fuzzyName, nbMatches
					);
					BOOST_FOREACH(const PlacesMatcher::MatchResult::value_type& it, matches)
					{
						result.push_back(boost::static_pointer_cast<T>(it.value));
					}
					return result;
				}



				void addPlaceToMatcher(
					PlacesMatcher::Content place
				);



				void removePlaceFromMatcher(
					const geography::NamedPlace& place
				);



				//////////////////////////////////////////////////////////////////////////
				/// Exports the object into a parameters map.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Cities_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @retval the parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Sylvain Pasche
				/// @since 3.3.0
				/// @date 2011
				void toParametersMap(
					util::ParametersMap& pm,
					const CoordinatesSystem* coordinatesSystem,
					const std::string& prefix = std::string()
				) const;


				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual std::string getName() const { return get<Name>(); }


				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif
