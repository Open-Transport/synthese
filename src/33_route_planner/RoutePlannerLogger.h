
/** RoutePlannerLogger class header.
	@file RoutePlannerLogger.h

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

#ifndef SYNTHESE_routeplanner_RoutePlannerLogger_h__
#define SYNTHESE_routeplanner_RoutePlannerLogger_h__

#include "HTMLTable.h"
#include "JourneysResult.h"
#include "JourneyComparator.h"
#include "Journey.h"

namespace synthese
{
	namespace algorithm
	{
		/** RoutePlannerLogger class.
			@ingroup m53
		*/
		class RoutePlannerLogger
		{
			typedef std::map<const graph::Vertex*, boost::shared_ptr<graph::Journey> > Map;
			typedef std::vector<boost::shared_ptr<graph::Journey> > Vector;

			std::ostream& _stream;
			html::HTMLTable _t;
			graph::Journey _result;

			std::size_t _searchNumber;
			Map _lastTodo;
			Vector _todoBeforeClean;

		public:
			RoutePlannerLogger(
				std::ostream& stream,
				const JourneysResult<graph::JourneyComparator> emptyTodo,
				const graph::Journey& result
			);

			void open();
			void recordIntegralSearch(const JourneysResult<graph::JourneyComparator>& todo);
			void recordCleanup(const JourneysResult<graph::JourneyComparator>& todo);
			void recordNewResult(const graph::Journey& result);
			void close();
		};
	}
}

#endif // SYNTHESE_routeplanner_RoutePlannerLogger_h__
