
/** ResaModule class header.
	@file ResaModule.h

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

#ifndef SYNTHESE_ResaModule_h__
#define SYNTHESE_ResaModule_h__

#include "01_util/ModuleClass.h"

#include "31_resa/OnlineReservationRule.h"

namespace synthese
{
	/** @defgroup m31 31 Reservation
		@ingroup m3

		The reservation module provides the ability to book seats on transport on demand lines.

		The features of the reservation module are :
			- BoolingScreenFunction : booking screen, available directly in a route planner journey roadmap
			- BoolingConfirmationFunction : booking confirmation
			
		A logged standard user uses the administration panel to access to the following features :
			- edit personal informations (in security module)
			- edit favorites journeys (in routeplanner module)
			- display reservations history
				- cancel a reservation
			- display an integrated route planner (in routeplanner module)

		A logged operator uses the administration panel to access to the following features :
			- personal informations (security module)
			- search a customer (security module)
				- edit customer personal informations (security module)
				- display customer reservations history
					- cancel a reservation
			- display the commercial lines with reservations
				- display the reservation list of the line
					- ServiceReservationsRoadMapFunction : display the detailed reservation list of a service / course (pop up : not in the admin panel, optimized for printing)
			- display an integrated route planner (routeplanner module)
			
			Move the following features in a call center module :
			- display the call center planning
			- display the calls list
				- display a call log

		A logged driver uses the administration panel to access to the following features :
			- display the commercial lines
				- display the reservation list of the line
					- display the detailed reservation list of a service / course

	@{
	*/

	/** 51 Reservation module namespace.
	*/
	namespace resa
	{
		/** 51 Reservation module class.
		*/
		class ResaModule : public util::ModuleClass
		{
		private:
			static OnlineReservationRule::Registry _onlineReservationRules;

		public:
			ResaModule();

			static OnlineReservationRule::Registry& getOnlineReservationRules();

		};
	}
	/** @} */
}

#endif // SYNTHESE_ResaModule_h__
