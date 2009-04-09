////////////////////////////////////////////////////////////////////////////////
///	Road place class header.
///	@file RoadPlace.h
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_RoadPlace_h__
#define SYNTHESE_RoadPlace_h__

#include "AddressablePlace.h"
#include "PathGroup.h"
#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace env
	{
		class City;
	}
	
	namespace road
	{
		class Road;
		
		////////////////////////////////////////////////////////////////////////
		/// Road place class.
		/// @ingroup m34
		class RoadPlace
		:	public AddressablePlace,
			public graph::PathGroup
		{
		public:
			// Typedefs
			
			/// Chosen registry class.
			typedef util::Registry<RoadPlace> Registry;

			typedef std::set<const Road*> Roads;
			
		private:
			// Attributes
			Roads _roads;
			
		public:
			/////////////////////////////////////////////////////////////////////
			/// RoadModule Constructor.
			RoadPlace(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string name = std::string(),
				const env::City* city = NULL
			);

			//! @name Getters
			//@{
				const Roads& getRoads() const;
			//@}
		
			//! @name Setters
			//@{
			//@}

			//! @name Modifiers
			//@{
				void addRoad(const Road* road);
				void removeRoad(const Road* road);
			//@}

			//! @name Queries
			//@{
			
				/** getImmediateVertices.
					@param result : all the vertices of all places traversed by the road
					@param accessDirection
					@param accessParameters
					@param returnAddresses
					@param returnPhysicalStops
					@param origin
					@author Hugues Romain
					@date 2008		  	
				*/
				void getImmediateVertices(
					graph::VertexAccessMap& result, 
					const graph::AccessDirection& accessDirection,
					const env::AccessParameters& accessParameters,
					SearchAddresses returnAddresses,
					SearchPhysicalStops returnPhysicalStops,
					const graph::Vertex* origin = 0
				) const;

			//@}
		
			//! @name Static algorithms
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_RoadPlace_h__
 
