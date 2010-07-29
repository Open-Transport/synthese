
/** JourneyTemplates class header.
	@file JourneyTemplates.h

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

#ifndef SYNTHESE_algorithm_JourneyTemplates_h__
#define SYNTHESE_algorithm_JourneyTemplates_h__

#include <set>
#include <vector>

#include "GraphTypes.h"

namespace synthese
{
	namespace graph
	{
		class Hub;
		class Journey;
	}

	namespace algorithm
	{
		/** JourneyTemplates class.
			@ingroup m33
		*/
		class JourneyTemplates
		{
		public:
			typedef std::set<std::vector<const graph::Hub*> > Hubs;

		private:
			const graph::GraphIdType _graphToUse;
			Hubs _hubs;

		public:
			JourneyTemplates(const graph::GraphIdType graphToUse);

			void addResult(const graph::Journey& value);
			bool testJourneySimilarity(const graph::Journey& value) const;
		};
	}
}

#endif // SYNTHESE_algorithm_JourneyTemplates_h__
