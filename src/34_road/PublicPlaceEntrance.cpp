
/** PublicPlaceEntrance class implementation.
	@file PublicPlaceEntrance.cpp

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

#include "PublicPlaceEntrance.hpp"

#include "ReverseRoadChunk.hpp"
#include "RoadModule.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace road;
	using namespace util;

	namespace util
	{
		template<> const string Registry<PublicPlaceEntrance>::KEY("PublicPlaceEntrance");
	}

	namespace road
	{
		PublicPlaceEntrance::PublicPlaceEntrance(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			_publicPlace(NULL)
		{}



		graph::VertexAccess PublicPlaceEntrance::getVertexAccess(
			const Crossing& crossing
		) const	{
			if(getRoadChunk())
			{
				if(getRoadChunk()->getFromCrossing() == &crossing)
				{
					return VertexAccess(minutes(static_cast<long>(getMetricOffset() / 50)), getMetricOffset());
				}
				if(	getRoadChunk()->getReverseRoadChunk() &&
					getRoadChunk()->getReverseRoadChunk()->getFromCrossing() == &crossing
				){
					return VertexAccess(
						minutes(
							static_cast<long>(
								(getRoadChunk()->getEndMetricOffset() - getRoadChunk()->getMetricOffset() - getMetricOffset()) / 50
						)	),
						getRoadChunk()->getEndMetricOffset() - getRoadChunk()->getMetricOffset() - getMetricOffset()
					);
				}
			}
			return VertexAccess();
		}



		graph::GraphIdType PublicPlaceEntrance::getGraphType() const
		{
			return RoadModule::GRAPH_ID;
		}
}	}
