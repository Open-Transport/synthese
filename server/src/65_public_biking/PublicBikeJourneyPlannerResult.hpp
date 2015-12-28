
/** PublicBikeJourneyPlannerResult class header.
	@file PublicBikeJourneyPlannerResult.h

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

#ifndef SYNTHESE_publicbiking_PublicBikeJourneyPlannerResult_h__
#define SYNTHESE_publicbiking_PublicBikeJourneyPlannerResult_h__

#include <vector>
#include <boost/optional.hpp>

#include "Journey.h"
#include "TimeSlotRoutePlanner.h"
#include "HTMLForm.h"

namespace synthese
{
	namespace geography
	{
		class Place;
	}

	namespace public_biking
	{
		/** Public Bike journey planner result class.
			@ingroup m65
		*/
		class PublicBikeJourneyPlannerResult
		{
		public:

			typedef algorithm::TimeSlotRoutePlanner::Result Journeys;

			PublicBikeJourneyPlannerResult(
				const geography::Place* departurePlace,
				const geography::Place* arrivalPlace,
				bool samePlaces,
				const algorithm::TimeSlotRoutePlanner::Result& journeys
			);

		private:
			//! @name Informations about the computing process
			//@{
				const geography::Place* const		_departurePlace;
				const geography::Place* const		_arrivalPlace;
				bool		_samePlaces;	//!< Whether the route planning was attempted between to identical places (in this case the result is always empty)
			//@}

			//! @name Journeys found
			//@{
				Journeys	_journeys;		//!< List of the result journeys, ordered by departure time
			//@}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Generates an HTML table to display on admin console.
			/// @param stream stream to write the result on
			virtual void displayHTMLTable(
				std::ostream& stream
			) const;

			std::string getTripWKT() const;

			const Journeys& getJourneys() const { return _journeys; }

			void addJourney(graph::Journey newJourney) { _journeys.push_back(newJourney); }
		};
	}
}

#endif // SYNTHESE_publicbiking_PublicBikeJourneyPlannerResult_h__
