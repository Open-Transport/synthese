////////////////////////////////////////////////////////////////////////////////
///	Road place class header.
///	@file RoadPlace.h
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "Place.h"
#include "NamedPlaceTemplate.h"
#include "PathGroup.h"
#include "Registrable.h"
#include "Registry.h"
#include "MainRoadChunk.hpp"
#include "ImportableTemplate.hpp"

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace road
	{
		class Road;
		class House;

		////////////////////////////////////////////////////////////////////////
		/// Road place class.
		/// @ingroup m34
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		class RoadPlace:
			public virtual geography::Place,
			public graph::PathGroup,
			public geography::NamedPlaceTemplate<RoadPlace>,
			public impex::ImportableTemplate<RoadPlace>
		{
		public:
			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_X;
			static const std::string DATA_Y;

			/// Chosen registry class.
			typedef util::Registry<RoadPlace> Registry;


		private:

		public:
			/////////////////////////////////////////////////////////////////////
			/// RoadModule Constructor.
			/// Initializes the following default use rules :
			///  - USER_PEDESTRIAN : allowed
			///  - USER_BIKE : allowed
			///  - USER_CAR : allowed
			///  - USER_BUS : allowed
			RoadPlace(
				util::RegistryKeyType id = 0
			);

			//! @name Getters
			//@{
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Modifiers
			//@{
				void addRoad(Road& road);
				void removeRoad(Road& road);
			//@}

			//! @name Services
			//@{
				void toParametersMap(
					util::ParametersMap& pm,
					const CoordinatesSystem* coordinatesSystem,
					const std::string& prefix = std::string()
				) const;

				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual std::string getRuleUserName() const;

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const GraphTypes& whatToSearch
				) const;

				const virtual boost::shared_ptr<geos::geom::Point>& getPoint() const;

				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Gets a house from a number.
				/// Much cases :
				/// <ul>
				///	<li>the number is found inside a road chunk : OK</li>
				///	</ul>
				//////////////////////////////////////////////////////////////////////////
				/// @param houseNumber the number of the house to get
				/// @param numberAtBeginning the number is before the street name
				/// @return auto generated House object corresponding to the point
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.2.0
				boost::shared_ptr<House> getHouse(
					MainRoadChunk::HouseNumber houseNumber,
					bool numberAtBeginning = true
				) const;
			//@}

			//! @name Static algorithms
			//@{
			//@}
		};
}	}

#endif // SYNTHESE_RoadPlace_h__

