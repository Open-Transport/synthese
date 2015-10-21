
/** DeadRun class implementation.
	@file DeadRun.cpp

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

#include "DeadRunEdge.hpp"
#include "DeadRun.hpp"
#include "PTUseRule.h"
#include "StopPoint.hpp"
#include "Depot.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<pt_operation::DeadRunEdge>::KEY("DeadRunEdge");
	}

	namespace pt_operation
	{
		DeadRunEdge::DeadRunEdge(
			RegistryKeyType id /*= 0*/
		):	Registrable(0),
			Edge(NULL, 0, NULL, 0)
		{}



		DeadRunEdge::DeadRunEdge(
			DeadRun& parentPath,
			Depot& depot
		):	Registrable(0),
			Edge(&parentPath, 0, &depot, 0)
		{}



		DeadRunEdge::DeadRunEdge(
			DeadRun& parentPath,
			StopPoint& stop
		):	Registrable(0),
			Edge(&parentPath, 0, &stop, 0)
		{}



		DeadRunEdge::DeadRunEdge(
			double length,
			DeadRun& parentPath,
			Depot& depot
		):	Registrable(0),
			Edge(&parentPath, 1, &depot, length)			
		{}



		DeadRunEdge::DeadRunEdge(
			double length,
			DeadRun& parentPath,
			StopPoint& stop
		):	Registrable(0),
			Edge(&parentPath, 1, &stop, length)
		{}



		bool DeadRunEdge::isDepartureAllowed() const
		{
			return false;
		}



		bool DeadRunEdge::isArrivalAllowed() const
		{
			return false;
		}

	}
}
