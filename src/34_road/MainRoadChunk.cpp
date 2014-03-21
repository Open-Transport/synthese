
/** MainRoadChunk class implementation.
	@file MainRoadChunk.cpp

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

#include "MainRoadChunk.hpp"
#include "ReverseRoadChunk.hpp"
#include "MainRoadPart.hpp"
#include "House.hpp"

using namespace std;

namespace synthese
{
	using namespace graph;

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
		):	Registrable(id),
			RoadChunk(
				id,
				fromCrossing,
				rankInRoad,
				road,
				metricOffset
			),
			_reverseRoadChunk(NULL)
		{}



		bool MainRoadChunk::_testIfHouseNumberBelongsToBounds(
			const HouseNumberBounds& bounds,
			const HouseNumberingPolicy& policy,
			HouseNumber houseNumber
		){
			if(!bounds)
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
				_testIfHouseNumberBelongsToBounds(_rightHouseNumberBounds, _rightHouseNumberingPolicy, houseNumber) ||
				_houses.count(houseNumber);
			;
		}


		MetricOffset MainRoadChunk::_computeMetricOffset(
			const HouseNumberBounds& bounds,
			HouseNumber houseNumber,
			MetricOffset startOffset,
			MetricOffset endOffset
		){
			assert(bounds);
			assert(startOffset <= endOffset);

			return startOffset + (endOffset - startOffset) * (
					   (bounds->first < bounds->second) ?
					   ((double)(houseNumber - (bounds->first - 1)) / (double)((bounds->second + 1) - (bounds->first - 1))) :
					   ((double)((bounds->first + 1) - houseNumber) / (double)((bounds->first + 1) - (bounds->second - 1)))
				   )
			;
		}



		MetricOffset MainRoadChunk::getHouseNumberMetricOffset(
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
				return _computeMetricOffset(_rightHouseNumberBounds, houseNumber, getMetricOffset(), getEndMetricOffset());
			}

			return getMetricOffset();
		}

		MainRoadChunk::HouseNumber MainRoadChunk::getHouseNumberFromOffset(
			double metricOffset
		) const {
			double relativePosition = (metricOffset - this->getMetricOffset()) / (this->getEndMetricOffset() - this->getMetricOffset());
			HouseNumberBounds bounds = getLeftHouseNumberBounds();

			if(!bounds)
			{
				return 0;
			}
			if(relativePosition > 1)
			{
				return bounds->second;
			}
			else if(relativePosition < 0)
			{
				return bounds->first;
			}
			else
			{
				HouseNumber closestHouseNumber(
					(bounds->first < bounds->second) ?
						ceil((relativePosition * ((bounds->second + 1) - (bounds->first - 1))) + (bounds->first - 1)) :
						floor((bounds->first + 1) - (relativePosition * ((bounds->first + 1) - (bounds->second - 1))))
				);

				switch(getLeftHouseNumberingPolicy())
				{
					case ODD:
						return closestHouseNumber - (closestHouseNumber % 2) + 1;
					case EVEN:
						return closestHouseNumber - (closestHouseNumber % 2);
					case ALL:
					default:
						return closestHouseNumber;
				}
			}
		}



		MainRoadPart* MainRoadChunk::getMainRoadPart() const
		{
			return static_cast<MainRoadPart*>(getRoad());
		}



		void MainRoadChunk::addHouse(House& house)
		{
			if(house.getHouseNumber())
			{
				_houses[*house.getHouseNumber()] = &house;
			}
		}



		void MainRoadChunk::removeHouse(House& house)
		{
			if(house.getHouseNumber())
			{
				_houses.erase(*house.getHouseNumber());
			}
		}
}	}
