
/** TimetableModule class header.
	@file TimetableModule.h

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

#ifndef SYNTHESE_timetables_TimetableModule_h__
#define SYNTHESE_timetables_TimetableModule_h__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m55Actions 55 Actions
		@ingroup m55

		@defgroup m55Pages 55 Pages
		@ingroup m55

		@defgroup m55Functions 55 Functions
		@ingroup m55

		@defgroup m55Exceptions 55 Exceptions
		@ingroup m55

		@defgroup m55Alarm 55 Messages recipient
		@ingroup m55

		@defgroup m55LS 55 Table synchronizers
		@ingroup m55

		@defgroup m55Admin 55 Administration pages
		@ingroup m55

		@defgroup m55Rights 55 Rights
		@ingroup m55

		@defgroup m55Logs 55 DB Logs
		@ingroup m55
		
		@defgroup m55 55 Timetables generator
		@ingroup m5
		
		(Module documentation)
		
		@{
	*/

	/** 55 Timetables generator Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace timetables
	{
		/** TimetableModule class.
		*/
		class TimetableModule:
			public server::ModuleClassTemplate<TimetableModule>
		{
		public:
		};
	}
}

#endif // SYNTHESE_timetables_TimetableModule_h__
