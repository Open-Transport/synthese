
/** RoutePlannerModule class header.
	@file RoutePlannerModule.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_RoutePlannerModule_H__
#define SYNTHESE_RoutePlannerModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	/**	@defgroup m53Actions 53 Actions
		@ingroup m53

		@defgroup m53Pages 53 Pages
		@ingroup m53

		@defgroup m53Values 53 Values
		@ingroup m53

		@defgroup m53Functions 53 Functions
		@ingroup m53

		@defgroup m53LS 53 Table synchronizers
		@ingroup m53

		@defgroup m53Admin 53 Administration pages
		@ingroup m53

		@defgroup m53Rights 53 Rights
		@ingroup m53

		@defgroup m53Logs 53 DB Logs
		@ingroup m53
		
		@defgroup m53 53 Route planner.
		@ingroup m5
		
		(Module documentation)
		
		@{
	*/

	/** 53 Route planner. Module namespace.
		@author Hugues Romain
		@date 2007
	*/
	namespace routeplanner
	{

		/** 53 Route planner. Module class.
			@author Hugues Romain
			@date 2007
		*/
		class RoutePlannerModule : public util::ModuleClass
		{
		private:
			
			
		public:
			/** Initialization of the 33 53 Route planner. module after the automatic database loads.
				@author Hugues Romain
				@date 2007
			*/			
//			void initialize();

			virtual std::string getName() const;
		};
	}
	/** @} */
}

#endif // SYNTHESE_RoutePlannerModule_H__
