/** ImpEx module types.
	@file ImpExTypes.hpp

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

#ifndef SYNTHESE_impex_ImpExTypes_hpp__
#define SYNTHESE_impex_ImpExTypes_hpp__

namespace synthese
{
	namespace impex
	{
		typedef enum
		{
			IMPORT_LOG_ALL = 0,
			IMPORT_LOG_DEBG = 10,
			IMPORT_LOG_LOAD = 20,
			IMPORT_LOG_CREA = 25,
			IMPORT_LOG_INFO = 30,
			IMPORT_LOG_WARN = 40,
			IMPORT_LOG_NOTI = 50,
			IMPORT_LOG_ERROR = 60,
			IMPORT_LOG_NOLOG = 99
		} ImportLogLevel;
	}
}

#endif // SYNTHESE_impex_ImpExTypes_hpp__

