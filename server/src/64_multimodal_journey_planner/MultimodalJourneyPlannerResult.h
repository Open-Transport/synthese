
/** MultimodalJourneyPlannerResult class header.
	@file MultimodalJourneyPlannerResult.h

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

#ifndef SYNTHESE_multimodal_journey_planner_MultimodalRoutePlannerResult_h__
#define SYNTHESE_multimodal_journey_planner_MultimodalRoutePlannerResult_h__

#include <vector>
#include <boost/optional.hpp>

#include "Journey.h"

namespace synthese
{
	namespace multimodal_journey_planner
	{
		/** Multimodal journey planner result class.
			@ingroup m64
		*/
		class MultimodalJourneyPlannerResult
		{
		public:

			typedef std::deque<graph::Journey> Result;

			MultimodalJourneyPlannerResult(
			);

		private:
			//! @name Informations about the computing process
			//@{
			//@}

			//! @name Journeys found
			//@{
				Result	_journeys;		//!< List of the result journeys, ordered by type and departure time
			//@}

		public:
			const Result& getJourneys() const { return _journeys; }
		};
	}
}

#endif // SYNTHESE_routeplanner_PTRoutePlannerResult_h__
