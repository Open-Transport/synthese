
/** UtilModule class header.
	@file UtilModule.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_UtilModule_H__
#define SYNTHESE_UtilModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	/**	@defgroup m01Exceptions 01 Exceptions
		@ingroup m01

		@defgroup m01 01 Root technical elements
		@ingroup m0
		
		(Module documentation)
		
		@{
	*/

	/** 01 Various utilities Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace util
	{

		/** 01 Various utilities Module class.
			@author Hugues Romain
			@date 2008
		*/
		class UtilModule : public util::ModuleClass
		{
		private:
			// static Object::Registry _registry;	//!< Objects registry
			
			
		public:
			/** Initialization of the 01 Various utilities module after the automatic database loads.
				@author Hugues Romain
				@date 2008
			*/			
			void initialize();
			
			// static Object::Registry& getObjects();
		};
	}
	/** @} */
}

#endif // SYNTHESE_UtilModule_H__
