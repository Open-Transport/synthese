
/** ReachableFromCrossing class header.
	@file ReachableFromCrossing.hpp

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

#ifndef SYNTHESE_road_ReachableFromCrossing_hpp__
#define SYNTHESE_road_ReachableFromCrossing_hpp__

#include "VertexAccessMap.h"
#include "GraphTypes.h"

namespace synthese
{
	namespace road
	{
		class Crossing;

		/** ReachableFromCrossing class.
			@ingroup m34
			@since 3.2.0
			@author Hugues Romain
		*/
		class ReachableFromCrossing
		{
		public:

			virtual graph::VertexAccess getVertexAccess(const Crossing& crossing) const = 0;
			virtual graph::GraphIdType getGraphType() const = 0;

		};
	}
}

#endif // SYNTHESE_road_ReachableFromCrossing_hpp__
