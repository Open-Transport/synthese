
/** TreeModule class header.
	@file TreeModule.h
	@author Hugues
	@date 2010

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

#ifndef SYNTHESE_TreeModule_H__
#define SYNTHESE_TreeModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m20Exceptions 20.01 Exceptions
		@ingroup m20

		@defgroup m20LS 20.10 Table synchronizers
		@ingroup m20

		@defgroup m20Pages 20.11 Interface Pages
		@ingroup m20

		@defgroup m20Library 20.11 Interface Library
		@ingroup m20

		@defgroup m20Rights 20.12 Rights
		@ingroup m20

		@defgroup m20Logs 20.13 DB Logs
		@ingroup m20

		@defgroup m20Admin 20.14 Administration pages
		@ingroup m20

		@defgroup m20Actions 20.15 Actions
		@ingroup m20

		@defgroup m20Functions 20.15 Functions
		@ingroup m20

		@defgroup m20File 20.16 File formats
		@ingroup m20

		@defgroup m20 20 Tree
		@ingroup m1

		@todo Write Module documentation

		@{
	*/

	/** 20 Tree Module namespace.
		@author Hugues Romain
		@date 2010
	*/
	namespace tree
	{

		/** 20 Tree Module class.
			@author Hugues
			@date 2010
		*/
		class TreeModule:
			public server::ModuleClassTemplate<TreeModule>
		{
		private:


		public:
			/** Initialization of the 20 Tree module after the automatic database loads.
				@author Hugues
				@date 2010
			*/
			void initialize();
		};
	}
	/** @} */
}

#endif // SYNTHESE_TreeModule_H__
