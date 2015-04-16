
/** AdminModule class header.
	@file AdminModule.h

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

#ifndef SYNTHESE_AdminModule_H__
#define SYNTHESE_AdminModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	namespace server
	{
		class Request;
	}

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
		class AdminModule:
			public server::ModuleClassTemplate<AdminModule>
		{
		private:

		public:
			static const std::string TABLE_COL_ID;
			static const std::string CSS_TIME_INPUT;
			static const std::string CSS_2DIGIT_INPUT;
		};
	}
	/** @} */
}

#endif // SYNTHESE_AdminModule_H__
