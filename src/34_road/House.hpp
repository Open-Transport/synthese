
/** House class header.
	@file House.hpp

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

#ifndef SYNTHESE_road_House_hpp__
#define SYNTHESE_road_House_hpp__

#include "Address.h"
#include "NamedPlaceTemplate.h"

namespace synthese
{
	namespace road
	{
		//////////////////////////////////////////////////////////////////////////
		/// House.
		/// A house is a place representing a single address, designed to be
		/// temporarily created on runtime.
		/// For registered houses, use geography::PublicPlace instead.
		/// @image html uml_house.png
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m34
		/// @author Hugues Romain
		/// @since 3.2.0
		/// @date 2010
		class House:
			public Address,
			public geography::NamedPlaceTemplate<House>
		{
		public:
			House();

			//////////////////////////////////////////////////////////////////////////
			/// Constructor from parameter values.
			///	@param chunk the road chunk where the house is attached
			/// @param houseNumber number of the house
			/// @param houseAtBeginning the number must be placed before the road name
			/// @param separator between the number and the road name
			/// @pre the house number must correspond to the road chunk (test it with
			///	RoadChunk::testIfHouseNumberBelongsToChunk)
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			House(
				RoadChunk& chunk,
				RoadChunk::HouseNumber houseNumber,
				bool numberAtBeginning = true,
				std::string separator = std::string(" ")
			);


			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the vertices directly reachable from the house.
				///	Two vertices are returned if the graph type is road : 
				///	<ul>
				///	<li>the beginning of the two chunk linked with the house</li>
				///	<li>the beginning ot the revered chunk from the preceding one</li>
				///	</ul>
				///	Approach durations are computed from the accessParameters.
				//////////////////////////////////////////////////////////////////////////
				/// @retval result the two vertices
				/// @param accessParameters parameters of access
				/// @param whatToSearch graph of the returned vertices
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const GraphTypes& whatToSearch
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the point representing the place.
				/// @return the point of the address
				virtual boost::shared_ptr<geos::geom::Point> getPoint() const;



				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const;
			//@}
		};
	}
}

#endif // SYNTHESE_road_House_hpp__
