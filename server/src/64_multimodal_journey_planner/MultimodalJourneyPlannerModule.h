
/** MultimodalJourneyPlannerModule class header.
	@file MultimodalJourneyPlannerModule.h
	@author Camille Hue
	@date 2015

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

#ifndef SYNTHESE_MultimodalJourneyPlannerModule_H__
#define SYNTHESE_MultimodalJourneyPlannerModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 64 Mulitmodal journey planner module namespace.
	///	@author Camille Hue
	///	@date 2015
	/// @ingroup m64
	namespace multimodal_journey_planner
	{
		//////////////////////////////////////////////////////////////////////////
		/// 64 multimodal_journey_planner Module class.
		///	@author Camille Hue
		///	@date 2015
		class MultimodalJourneyPlannerModule:
			public server::ModuleClassTemplate<MultimodalJourneyPlannerModule>
		{
		private:


		public:
			/** Initialization of the 64 multimodal_journey_planner module after the automatic database loads.
				@author Camille Hue
				@date 2015
			*/
			//void initialize();

		};
	}
	/** @} */
}

#endif // SYNTHESE_MultimodalJourneyPlannerModule_H__
