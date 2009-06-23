
/** CityTableSync class header.
	@file CityTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_CITYTABLESYNC_H
#define SYNTHESE_ENVLSSQL_CITYTABLESYNC_H

#include "City.h"

#include <string>
#include <iostream>

#include "SQLiteAutoRegisterTableSyncTemplate.h"

namespace synthese
{
	namespace geography
	{
		/** City SQLite table synchronizer.
			@ingroup m14LS refLS

			- on insert : insert entry in associator
			- on update : update entry in associator
			- on delete : X
		*/
		class CityTableSync : public db::SQLiteAutoRegisterTableSyncTemplate<CityTableSync,City>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_CODE;

			CityTableSync ();
			~CityTableSync ();

			static void Search(
				util::Env& env,
				boost::optional<std::string> exactName = boost::optional<std::string>(),
				boost::optional<std::string> likeName = boost::optional<std::string>(),
				boost::optional<std::string> code = boost::optional<std::string>(),
				int first = 0,
				int number = 0,
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);

			static boost::shared_ptr<City> GetEditableFromCode(
				const std::string& code,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif
