
/** 59 road_journey_planner module class header.
	@file RoadJourneyPlannerModule.hpp
	@author Hugues Romain
	@date 2010
	@since 3.2.0

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

#ifndef SYNTHESE_RoadJourneyPlannerModule_H__
#define SYNTHESE_RoadJourneyPlannerModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 59 road_journey_planner Module namespace.
	///	@author Hugues Romain
	///	@date 2010
	/// @ingroup m59
	/// @since 3.2.0
	namespace road_journey_planner
	{
		/**	@defgroup m59Exceptions 59.01 Exceptions
			@ingroup m59

			@defgroup m59LS 59.10 Table synchronizers
			@ingroup m59

			@defgroup m59Pages 59.11 Interface Pages
			@ingroup m59

			@defgroup m59Library 59.11 Interface Library
			@ingroup m59

			@defgroup m59Rights 59.12 Rights
			@ingroup m59

			@defgroup m59Logs 59.13 DB Logs
			@ingroup m59

			@defgroup m59Admin 59.14 Administration pages
			@ingroup m59

			@defgroup m59Actions 59.15 Actions
			@ingroup m59

			@defgroup m59Functions 59.15 Functions
			@ingroup m59

			@defgroup m59File 59.16 File formats
			@ingroup m59

			@defgroup m59Alarm 59.17 Messages recipient
			@ingroup m59

			@defgroup m59 59 road_journey_planner
			@ingroup m5

			@todo Write Module documentation

			@{
		*/


		//////////////////////////////////////////////////////////////////////////
		/// 59 road_journey_planner Module class.
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		class RoadJourneyPlannerModule:
			public server::ModuleClassTemplate<RoadJourneyPlannerModule>
		{
		private:

		public:

		};
	}
	/** @} */
}

#endif // SYNTHESE_RoadJourneyPlannerModule_H__
