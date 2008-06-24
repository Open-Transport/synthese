
/** JourneyComparator class header.
	@file JourneyComparator.h

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

#ifndef SYNTHESE_env_JourneyComparator_h__
#define SYNTHESE_env_JourneyComparator_h__

namespace synthese
{
	namespace env
	{
		class Journey;

		/** JourneyComparator class.
			@ingroup m35

			The journey comparison is used for ordering purposes, in the exploration queue of the route planner for example.

			Each journey is scored upon the following elements :
				- the main part of the score is the "MinSpeedToEnd" which actually is the squared distance from the reached vertex to the goal vertex, divided by the available time to reach it.
				- if the min speed is lower than 100 then the score is 100
				- the score is now divided by the score of the vertex : it depends of the transport offer and the variety of destinations available from the vertex. If a vertex has a high score,
					then the min speed to end is attenuated, and the vertex raises to a better rank in the exploration queue.

			The vertex score is calculated by the following way :
				- each commercial line gives some points, depending of the number of services which belongs to the line :
					- 1 to 10 services lines gives 2 point
					- 10 to 50 services lines gives 3 points
					- 50 to 100 services lines gives 4 points
					- much than 100 services lines gives 5 points
				- if the score is bigger than 100 points, then the score is 100
		*/
		class JourneyComparator
		{
		public:
			bool operator()(const Journey* j1, const Journey* j2) const;

		};
	}
}

#endif // SYNTHESE_env_JourneyComparator_h__
