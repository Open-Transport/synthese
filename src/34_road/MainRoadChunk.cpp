
/** MainRoadChunk class implementation.
	@file MainRoadChunk.cpp

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

#include "MainRoadChunk.hpp"
#include "ReverseRoadChunk.hpp"
#include "MainRoadPart.hpp"

using namespace std;

namespace synthese
{
	namespace util
	{
		template<> const string Registry<road::MainRoadChunk>::KEY("MainRoadChunk");
	}

	namespace road
	{

	    const MainRoadChunk::HouseNumber MainRoadChunk::HouseNumber_MAX(UINT_MAX);

		MainRoadChunk::MainRoadChunk(
			util::RegistryKeyType id /*= 0*/,
			Crossing* fromCrossing /*= NULL*/,
			int rankInRoad /*= UNKNOWN_VALUE*/,
			MainRoadPart* road /*= NULL*/,
			double metricOffset /*= UNKNOWN_VALUE */
		): Registrable(id),
		   RoadChunk(
			id,
			fromCrossing,
			rankInRoad,
			road,
			metricOffset
		){
		}


		bool MainRoadChunk::_testIfHouseNumberBelongsToBounds(
			const HouseNumberBounds& bounds,
			const HouseNumberingPolicy& policy,
			HouseNumber houseNumber
		){
			if(bounds)
			{
				return false;
			}

			if( (	bounds->first <= bounds->second &&
					(houseNumber < bounds->first || houseNumber > bounds->second)
				) || (
					bounds->first >= bounds->second &&
					(houseNumber > bounds->first || houseNumber < bounds->second)
			)	)
			{
				return false;
			}

			switch(policy)
			{
			case ALL: return true;
			case ODD: return houseNumber % 2;
			case EVEN: return !(houseNumber % 2); 
			}

			// Never execute
			return false;
		}


		bool MainRoadChunk::testIfHouseNumberBelongsToChunk(
			HouseNumber houseNumber
		) const	{
			return
				_testIfHouseNumberBelongsToBounds(_leftHouseNumberBounds, _leftHouseNumberingPolicy, houseNumber) ||
				_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber)
			;
		}


		RoadChunk::MetricOffset MainRoadChunk::_computeMetricOffset(
			const HouseNumberBounds& bounds,
			HouseNumber houseNumber,
			MetricOffset startOffset,
			MetricOffset endOffset
		){
			assert(bounds);
			assert(startOffset <= endOffset);

			if(startOffset == endOffset || bounds->first == bounds->second)
			{
				return startOffset;
			}

			return
				startOffset + (endOffset - startOffset) * (
					(bounds->first < bounds->second) ?
					((houseNumber - bounds->first) / (bounds->second - bounds->first)) :
					((bounds->first - houseNumber) / (bounds->first - bounds->second))
				)
			;
		}


		RoadChunk::MetricOffset MainRoadChunk::getHouseNumberMetricOffset(
			HouseNumber houseNumber
		) const	{

			// Left side
			if(	_testIfHouseNumberBelongsToBounds(_leftHouseNumberBounds, _leftHouseNumberingPolicy, houseNumber))
			{
				return _computeMetricOffset(_leftHouseNumberBounds, houseNumber, getMetricOffset(), getEndMetricOffset());
			}

			// Right side
			if(	_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber))
			{
				return _computeMetricOffset(_leftHouseNumberBounds, houseNumber, getMetricOffset(), getEndMetricOffset());
			}

			assert(false);
			return getMetricOffset();
		}

		MainRoadPart* MainRoadChunk::getMainRoadPart() const
		{
			return static_cast<MainRoadPart*>(getRoad());
		}
}	}
