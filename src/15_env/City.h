
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
	namespace road
	{
		class Road;
	}
	
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
		class PlaceAlias;
		class PublicPlace;

		/** City class.

		A city holds in its included places the main connection places
		 (those taken by default when no stop is explicitly chosen).

		@ingroup m35
		*/
		class City
		:	public IncludingPlace
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<City>	Registry;

		private:
			lexmatcher::LexicalMatcher<const PublicTransportStopZoneConnectionPlace*> _connectionPlacesMatcher;
			lexmatcher::LexicalMatcher<const PublicPlace*> _publicPlacesMatcher;
			lexmatcher::LexicalMatcher<const road::Road*> _roadsMatcher;
			lexmatcher::LexicalMatcher<const PlaceAlias*> _placeAliasesMatcher;
			lexmatcher::LexicalMatcher<const Place*> _allPlacesMatcher;

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

				lexmatcher::LexicalMatcher<const PublicTransportStopZoneConnectionPlace*>& getConnectionPlacesMatcher ();
				const lexmatcher::LexicalMatcher<const PublicTransportStopZoneConnectionPlace*>& getConnectionPlacesMatcher () const;

				lexmatcher::LexicalMatcher<const PublicPlace*>& getPublicPlacesMatcher ();
				const lexmatcher::LexicalMatcher<const PublicPlace*>& getPublicPlacesMatcher () const;
			    
				lexmatcher::LexicalMatcher<const road::Road*>& getRoadsMatcher ();
				const lexmatcher::LexicalMatcher<const road::Road*>& getRoadsMatcher () const;

				lexmatcher::LexicalMatcher<const PlaceAlias*>& getPlaceAliasesMatcher ();
				const lexmatcher::LexicalMatcher<const PlaceAlias*>& getPlaceAliasesMatcher () const;

				lexmatcher::LexicalMatcher<const Place*>& getAllPlacesMatcher ();
				const lexmatcher::LexicalMatcher<const Place*>& getAllPlacesMatcher () const;
			//@}

			//! @name Update methods
			//@{

			//@}

			//! @name Query methods
			//@{
				void getImmediateVertices(
					graph::VertexAccessMap& result, 
					const graph::AccessDirection& accessDirection,
					const AccessParameters& accessParameters,
					SearchAddresses returnAddresses
					, SearchPhysicalStops returnPhysicalStops
					, const graph::Vertex* origin = NULL
				) const;
			    
				std::vector<const road::Road*> searchRoad(
					const std::string& fuzzyName, int nbMatches = 10
				) const;
			//@}
		};
	}
}

#endif
