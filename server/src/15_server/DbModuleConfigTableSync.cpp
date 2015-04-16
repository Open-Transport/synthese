
/** DbModuleConfigTableSync class implementation.
	@file DbModuleConfigTableSync.cpp

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


#include "DbModuleConfigTableSync.h"

#include "ModuleClass.h"
#include "Profile.h"
#include "ServerAdminRight.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,DbModuleConfigTableSync>::FACTORY_KEY(
		"999 db config"
	);

	namespace server
	{
		const string DbModuleConfigTableSync::COL_PARAMNAME("param_name");
		const string DbModuleConfigTableSync::COL_PARAMVALUE("param_value");
	}

	namespace db
	{
	    template<> const DBTableSync::Format DBTableSyncTemplate<DbModuleConfigTableSync>::TABLE(
			"t999_config", false, false
			);

		template<> const Field DBTableSyncTemplate<DbModuleConfigTableSync>::_FIELDS[]=
		{
			Field(DbModuleConfigTableSync::COL_PARAMNAME, SQL_TEXT_FIXED),
			Field(DbModuleConfigTableSync::COL_PARAMVALUE, SQL_TEXT_FIXED),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DbModuleConfigTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<DbModuleConfigTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ServerAdminRight>(WRITE);
		}



		template<> void DBTableSyncTemplate<DbModuleConfigTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DbModuleConfigTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DbModuleConfigTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}

	namespace server
	{
		void
		DbModuleConfigTableSync::rowsAdded(
			DB* db,
			const DBResultSPtr& rows
		) const {
			while (rows->next ())
			{
				ModuleClass::SetParameter (rows->getText (COL_PARAMNAME),
						     rows->getText (COL_PARAMVALUE));
			}
		}



		void DbModuleConfigTableSync::rowsUpdated(
			DB* db,
			const DBResultSPtr& rows
		) const {
		    rowsAdded (db, rows);
		}



		void
		DbModuleConfigTableSync::rowsRemoved(
			DB* db,
			const RowIdList& rowIds
		) const {
		}



		const std::string& DbModuleConfigTableSync::getTableName() const
		{
			return TABLE.NAME;
		}
	}
}

