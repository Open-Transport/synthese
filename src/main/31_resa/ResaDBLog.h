
/** ResaDBLog class header.
	@file ResaDBLog.h

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

#ifndef SYNTHESE_RESA_DB_LOG
#define SYNTHESE_RESA_DB_LOG

#include <string>

#include "13_dblog/DBLog.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace resa
	{
		/** Reservation Log.
			@ingroup m31Logs refLogs

		*/
		class ResaDBLog : public util::FactorableTemplate<dblog::DBLog, SecurityLog>
		{
			typedef enum { LOGIN_ENTRY = 10, USER_ADMIN_ENTRY = 20, PROFILE_ADMIN_ENTRY = 30 } _EntryType;

		public:
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;
			DBLog::ColumnsVector parse(const dblog::DBLogEntry::Content& cols ) const;

			static void addUserLogin(const User* user);
			static void addUserAdmin(
				const User* user
				, const User* subject
				, const std::string& text
			);
			static void addProfileAdmin(
				const User* user
				, const Profile* subject
				, const std::string& text
			);
		};
	}
}

#endif

