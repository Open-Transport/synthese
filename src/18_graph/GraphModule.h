
/** GraphModule class header.
	@file GraphModule.h
	@author Hugues
	@date 2010

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

#ifndef SYNTHESE_GraphModule_H__
#define SYNTHESE_GraphModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	/**	@defgroup m18Actions 18 Actions
		@ingroup m18

		@defgroup m18Pages 18 Pages
		@ingroup m18

		@defgroup m18Functions 18 Functions
		@ingroup m18

		@defgroup m18Exceptions 18 Exceptions
		@ingroup m18

		@defgroup m18Alarm 18 Messages recipient
		@ingroup m18

		@defgroup m18LS 18 Table synchronizers
		@ingroup m18

		@defgroup m18Admin 18 Administration pages
		@ingroup m18

		@defgroup m18Rights 18 Rights
		@ingroup m18

		@defgroup m18Logs 18 DB Logs
		@ingroup m18
		
		@defgroup m18 18 Graph
		@ingroup m1
		
		(Module documentation)
		
		@{
	*/

	/** 18 Graph Module namespace.
		@author Hugues
		@date 2010
	*/
	namespace graph
	{

		/** 18 Graph Module class.
			@author Hugues
			@date 2010
		*/
		class GraphModule : public util::ModuleClass
		{
		private:
			// static Object::Registry _registry;	//!< Objects registry
			
			
		public:
			/** Initialization of the 18 Graph module after the automatic database loads.
				@author Hugues
				@date 2010
			*/			
			void initialize();
			
			// static Object::Registry& getObjects();
		};
	}
	/** @} */
}

#endif // SYNTHESE_GraphModule_H__
