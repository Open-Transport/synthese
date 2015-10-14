
/** House class header.
	@file House.hpp

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

#ifndef SYNTHESE_road_House_hpp__
#define SYNTHESE_road_House_hpp__

#include "Object.hpp"
#include "Address.h"
#include "NamedPlaceTemplate.h"
#include "RoadPlace.h"
#include "ImportableTemplate.hpp"
#include "GeometryField.hpp"

namespace synthese
{
	namespace road
	{

		FIELD_INT(Number)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(impex::DataSourceLinks),
			FIELD(RoadPlace),
			FIELD(Number),
			FIELD(PointGeometry)
		> HouseSchema;

		//////////////////////////////////////////////////////////////////////////
		/// House.
		/// A house is a place representing a single address, designed to be
		/// temporarily created at runtime.
		/// For registered houses, use geography::PublicPlace instead.
		/// @image html uml_house.png
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m34
		/// @author Hugues Romain
		/// @since 3.2.0
		/// @date 2010
		class House:
			public Address,
			public Object<House, HouseSchema>,
			public geography::NamedPlaceTemplate<House>,
			public impex::ImportableTemplate<House>
		{
		public:
			static const std::string DATA_ROAD_PREFIX;
			static const std::string DATA_NUMBER;
			static const std::string DATA_X;
			static const std::string DATA_Y;

			typedef util::Registry<House>	Registry;

			bool _numberAtBeginning;

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
				HouseNumber houseNumber,
				bool numberAtBeginning = true,
				std::string separator = std::string(" ")
			);



			House(
				RoadChunk& chunk,
				double metricOffset,
				bool numberAtBeginning = true,
				std::string separator = std::string(" ")
			);


			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param key identifier (default=0)
			/// @param geometry (default unknown) will not be cloned !
			/// @param codeBySource code of the crossing in the data source (default empty)
			/// @param source data source (default NULL)
			House(
				util::RegistryKeyType key = 0
			);

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Gets the vertices directly reachable from the house.
				///	Two vertices are returned if the graph type is road :
				///	<ul>
				///	<li>the beginning of the two chunk linked with the house</li>
				///	<li>the beginning of the reversed chunk from the preceding one</li>
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
				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const;



				virtual void setRoadChunkFromRoadPlace(
					boost::shared_ptr<RoadPlace> roadPlace,
					double maxDistance = 200
				);



				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Export of house into parameters map.
				void toParametersMap(
					util::ParametersMap& pm,
					const CoordinatesSystem* coordinatesSystem,
					const std::string& prefix = std::string()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Export of house into parameters map.
				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;


				virtual bool allowUpdate(const server::Session* session) const;
				virtual bool allowCreate(const server::Session* session) const;
				virtual bool allowDelete(const server::Session* session) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				virtual bool _getCityNameBeforePlaceName() const;
			//@}
		};
}	}

#endif // SYNTHESE_road_House_hpp__
