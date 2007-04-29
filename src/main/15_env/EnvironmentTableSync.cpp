
/** EnvironmentTableSync class implementation.
	@file EnvironmentTableSync.cpp

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

#include <sqlite/sqlite3.h>

#include "01_util/Conversion.h"
#include "01_util/UId.h"

#include "02_db/SQLiteException.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/EnvironmentLinkTableSync.h"
#include "15_env/EnvironmentTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Environment>::TABLE_NAME = "t000_environments";
		template<> const int SQLiteTableSyncTemplate<Environment>::TABLE_ID = 0;
		template<> const bool SQLiteTableSyncTemplate<Environment>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Environment>::load(Environment* object, const db::SQLiteResult& rows, int rowId/*=0*/ )
		{
			object->setKey(Conversion::ToLongLong(rows.getColumn(rowId, TABLE_COL_ID)));
			object->setName(rows.getColumn(rowId, EnvironmentTableSync::TABLE_COL_NAME));
		}

		template<> void SQLiteTableSyncTemplate<Environment>::save(Environment* object)
		{
			/// @todo Implement
/*			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					<< DisplayTypeTableSync::TABLE_COL_NAME << "=" << Conversion::ToSQLiteString(object->getName())
					<< "," << DisplayTypeTableSync::TABLE_COL_INTERFACE_ID << "="  << Conversion::ToString(object->getInterface()->getKey())
					<< "," << DisplayTypeTableSync::TABLE_COL_ROWS_NUMBER << "="  << Conversion::ToString(object->getRowNumber())
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId(1,1));
				query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					<< "," << Conversion::ToSQLiteString(object->getName())
					<< "," << Conversion::ToString(object->getInterface()->getKey())
					<< "," << Conversion::ToString(object->getRowNumber())
					<< ")";
			}
			sqlite->execUpdate(query.str());
*/		}

	}

	namespace env
	{
		const string EnvironmentTableSync::TABLE_COL_NAME = "name";

		EnvironmentTableSync::EnvironmentTableSync ()
		: SQLiteTableSyncTemplate<Environment> (true, true, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (TABLE_COL_ID, "INTEGER");
			addTableColumn (TABLE_COL_NAME, "TEXT");
		}



		EnvironmentTableSync::~EnvironmentTableSync ()
		{

		}

		    
		void 
		EnvironmentTableSync::rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						synthese::db::SQLiteSync* sync,
						const synthese::db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows (); ++i)
			{
				uid envId = Conversion::ToLongLong (rows.getColumn (i, TABLE_COL_ID));
				if (EnvModule::getEnvironments().contains (envId)) continue;
				shared_ptr<Environment> newEnv(new Environment (envId));
				load(newEnv.get(), rows, i);
				EnvModule::getEnvironments().add (newEnv);
			}
		}



		void 
		EnvironmentTableSync::rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						synthese::db::SQLiteSync* sync,
						const synthese::db::SQLiteResult& rows)
		{
			// Not supported right now...
			throw synthese::db::SQLiteException ("Updating an environment row is not supported right now.");
		}



		void 
		EnvironmentTableSync::rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
						synthese::db::SQLiteSync* sync,
						const synthese::db::SQLiteResult& rows)
		{
			// Look in environment link tables for each row id
			for (int i=0; i<rows.getNbRows (); ++i)
			{
				std::string envId = rows.getColumn (i, TABLE_COL_ID);

				// Remove all environment links
				sqlite->execQuery ("DELETE FROM " + ENVIRONMENT_LINKS_TABLE_NAME + " WHERE " 
						+ ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + "=" + envId);
				
				// Remove the environment
				EnvModule::getEnvironments().remove (Conversion::ToLongLong (envId));
			
			}
		}
			
			






	}
}
