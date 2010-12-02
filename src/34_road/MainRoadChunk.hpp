
/** MainRoadChunk class header.
	@file MainRoadChunk.hpp

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

#ifndef SYNTHESE_road_MainRoadChunk_hpp__
#define SYNTHESE_road_MainRoadChunk_hpp__

#include "RoadChunk.h"

namespace synthese
{
	namespace road
	{
		class MainRoadPart;
		class ReverseRoadChunk;

		//////////////////////////////////////////////////////////////////////////
		/// MainRoadChunk class.
		///	@ingroup m34
		///	@author Hugues Romain
		///	@date 2010
		///	@since 3.2.0
		class MainRoadChunk:
			public RoadChunk
		{
		public:
			typedef unsigned int HouseNumber;

			typedef boost::optional<std::pair<HouseNumber, HouseNumber> > HouseNumberBounds;

			typedef enum {
				ODD = 'O',
				EVEN = 'E',
				ALL = 'A'
			} HouseNumberingPolicy;

			/// Chosen registry class.
			typedef util::Registry<RoadChunk>	Registry;


		private:
			HouseNumberBounds _leftHouseNumberBounds;
			HouseNumberBounds _rightHouseNumberBounds;
			HouseNumberingPolicy	_leftHouseNumberingPolicy;
			HouseNumberingPolicy	_rightHouseNumberingPolicy;

			boost::shared_ptr<ReverseRoadChunk> _reverseRoadChunk;

			static bool _testIfHouseNumberBelongsToBounds(
				const HouseNumberBounds& bounds,
				const HouseNumberingPolicy& policy,
				HouseNumber houseNumber
			);

			//////////////////////////////////////////////////////////////////////////
			/// @pre bounds are defined
			/// @pre startOffset <= endOffset
			static RoadChunk::MetricOffset _computeMetricOffset(
				const HouseNumberBounds& bounds,
				HouseNumber houseNumber,
				MetricOffset startOffset,
				MetricOffset endOffset
			);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param id identifier (default 0)
			/// @param fromCrossing crossing where the chunk begins (default NULL)
			/// @param rankInRoad rank of the chunk in the road path (default unknown)
			/// @param road road which the chunk belongs to (default NULL)
			/// @param metricOffset distance between the the chunk beginning and the road beginning (default unknown)
			MainRoadChunk(
				util::RegistryKeyType id = 0,
				Crossing* fromCrossing = NULL,
				int rankInRoad = UNKNOWN_VALUE,
				MainRoadPart* road = NULL,
				double metricOffset = UNKNOWN_VALUE
			);

			//! @name Setters
			//@{
				void setLeftHouseNumberBounds(const HouseNumberBounds& value){ _leftHouseNumberBounds = value; }
				void setRightHouseNumberBounds(const HouseNumberBounds& value){ _rightHouseNumberBounds = value; }
				void setLeftHouseNumberingPolicy(const HouseNumberingPolicy& value){ _leftHouseNumberingPolicy = value; }
				void setRightHouseNumberingPolicy(const HouseNumberingPolicy& value){ _rightHouseNumberingPolicy = value; }
				void setReverseRoadChunk(boost::shared_ptr<ReverseRoadChunk> value){ _reverseRoadChunk = value; }
			//@}

			//! @name Getters
			//@{
				const HouseNumberBounds& getLeftHouseNumberBounds() const { return _leftHouseNumberBounds; }
				const HouseNumberBounds& getRightHouseNumberBounds() const { return _rightHouseNumberBounds; }
				const HouseNumberingPolicy& getLeftHouseNumberingPolicy() const { return _leftHouseNumberingPolicy; }
				const HouseNumberingPolicy& getRightHouseNumberingPolicy() const { return _rightHouseNumberingPolicy; }
				const ReverseRoadChunk* getReverseRoadChunk() const { return _reverseRoadChunk.get(); }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Tests if a numeric house number belongs to the chunk
				/// @param houseNumber the house number to test
				/// @return true if the house number belongs to the chunk
				/// @author Hugues Romain
				/// @since 3.2.0
				bool testIfHouseNumberBelongsToChunk(
					HouseNumber houseNumber
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Computes metric offset of a valid house number
				/// @param houseNumber the house number
				/// @return the metric offset of the house in the path
				/// @pre houseNumber must be valid, the house number bounds of the chunk must be
				/// defined. Use testIfHouseNumberBelongsToChunk to check this pre-condition.
				/// @author Hugues Romain
				/// @since 3.2.0
				MetricOffset getHouseNumberMetricOffset(
					HouseNumber houseNumber
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_road_MainRoadChunk_hpp__
