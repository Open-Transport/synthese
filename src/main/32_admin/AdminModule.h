
/** AdminModule class header.
	@file AdminModule.h

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

#ifndef SYNTHESE_AdminModule_H__
#define SYNTHESE_AdminModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
/**	@defgroup m14Actions 14 Actions
		@ingroup m14

		@defgroup m14Pages 14 Pages
		@ingroup m14

		@defgroup m14Functions 14 Functions
		@ingroup m14

		@defgroup m14Admin 14 Administration pages
		@ingroup m14

		@defgroup m14Rights 14 Rights
		@ingroup m14

		@defgroup m14Logs 14 DB Logs
		@ingroup m14

		@defgroup m14 14 Generic administration console
		@ingroup m1
		@{
	*/

	/** 14 Generic administration console module namespace.
	*/
	namespace admin
	{
		/** 14 Generic administration console module class.
		*/
		class AdminModule : public util::ModuleClass
		{
		private:

		public:
			static const std::string TABLE_COL_ID;
			static const std::string ICON_PATH_INTERFACE_VARIABLE;

			void initialize();
		};
	}
	/** @} */
}

#endif // SYNTHESE_AdminModule_H__
