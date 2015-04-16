
/** JunctionStop class implementation.
	@file JunctionStop.cpp

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

#include "JunctionStop.hpp"
#include "Junction.hpp"
#include "StopPoint.hpp"

using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace util;

	namespace util
	{
		template<> const string Registry<pt::JunctionStop>::KEY("JunctionStop");
	}

	namespace pt
	{
		JunctionStop::JunctionStop(
			Junction* parentPath /*= NULL*/,
			StopPoint* fromVertex /*= NULL*/
		):	Registrable(0),
			Edge(parentPath, 0, fromVertex, 0)
		{

		}



		JunctionStop::JunctionStop(
			double length,
			Junction* parentPath,
			StopPoint* fromVertex
		):	Registrable(0),
			Edge(parentPath, 1, fromVertex, length)
		{
		}



		bool JunctionStop::isDepartureAllowed() const
		{
			return getRankInPath() == 0;
		}



		bool JunctionStop::isArrivalAllowed() const
		{
			return getRankInPath() == 1;
		}
	}
}
