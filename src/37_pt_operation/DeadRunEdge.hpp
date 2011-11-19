
/** DeadRunEdge class header.
	@file DeadRunEdge.hpp

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

#ifndef SYNTHESE_pt_operation_DeadRunEdge_hpp__
#define SYNTHESE_pt_operation_DeadRunEdge_hpp__

#include "Edge.h"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	namespace pt_operation
	{
		class DeadRun;
		class Depot;

		/** Dead run edge class.
			@ingroup m37
		*/
		class DeadRunEdge:
			public graph::Edge
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<DeadRunEdge>	Registry;

		private:

		public:
			DeadRunEdge(util::RegistryKeyType id = 0);

			DeadRunEdge(
				DeadRun& parentPath,
				Depot& depot
			);
			DeadRunEdge(
				DeadRun& parentPath,
				pt::StopPoint& stop
			);
			DeadRunEdge(
				double length,
				DeadRun& parentPath,
				Depot& depot
			);
			DeadRunEdge(
				double length,
				DeadRun& parentPath,
				pt::StopPoint& stop
			);
			bool isDepartureAllowed() const;
			bool isArrivalAllowed() const;

		};
	}
}

#endif
