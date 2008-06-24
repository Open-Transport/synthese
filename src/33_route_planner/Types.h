
/** Types class header.
	@file Types.h

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

#ifndef SYNTHESE_routeplanner_Types_h__
#define SYNTHESE_routeplanner_Types_h__

#include <vector>

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
		class Journey;
		class Place;
	}

	namespace routeplanner
	{

		/** @addtogroup m53
			@{
		*/

		typedef std::vector<env::Journey*>			JourneyBoardJourneys;
		typedef std::vector<const env::ConnectionPlace*>	JourneyBoardPlaces;

		struct RoutePlannerResult
		{
			JourneyBoardJourneys	result;
			const env::Place*		departurePlace;
			const env::Place*		arrivalPlace;
		};

		typedef enum { USE_ROADS, DO_NOT_USE_ROADS } UseRoads;
		typedef enum { USE_LINES, DO_NOT_USE_LINES } UseLines;

		/** @} */
	}
}

#endif // SYNTHESE_routeplanner_Types_h__
