
/** SQLiteDB class header.
	@file SQLiteDB.hpp
	@author Sylvain Pasche

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

#ifndef SYNTHESE_db_sqlite_SQLiteDB_h__
#define SYNTHESE_db_sqlite_SQLiteDB_h__

#include "DB.hpp"
#include "FactorableTemplate.h"

#include <boost/filesystem/path.hpp>
#include <boost/thread/tss.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <spatialite/sqlite3.h>

namespace synthese
{
	namespace db
	{
		class DBTableSync;

		typedef struct
		{
			sqlite3* handle;
#ifdef DO_VERIFY_TRIGGER_EVENTS
			std::vector<DB::DBModifEvent> events;
#endif
		} SQLiteTSS;

		//////////////////////////////////////////////////////////////////////////
		/// SQLite database backend.
		///
		/// @author Sylvain Pasche
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class SQLiteDB :
			 public util::FactorableTemplate<DB, SQLiteDB>
		{
		private:

			boost::filesystem::path _databaseFile;
			mutable boost::thread_specific_ptr<SQLiteTSS> _tss;
#ifdef DO_VERIFY_TRIGGER_EVENTS
			boost::recursive_mutex _updateMutex;
#endif

		public:

			SQLiteDB();
			virtual ~SQLiteDB();

			virtual void preInit();

			virtual DBResultSPtr execQuery(const SQLData& sql);
			virtual void execTransaction(
				const DBTransaction& transaction
			);

			virtual void initSpatialRefSysTable();
			virtual std::string getSQLType(FieldType fieldType);
			virtual bool doesTableExists(const std::string& tableName);
			virtual bool isTableSchemaUpToDate(const std::string& tableName, const DBTableSync::Field fields[]);
			virtual std::vector<ColumnInfo> getTableColumns(const std::string& tableName);
			virtual std::string getCreateTableSQL(const std::string& tableName, const DBTableSync::Field fields[]);
			virtual void afterUpdateSchema(const std::string& tableName, const DBTableSync::Field fields[]);
			virtual bool doesIndexExist(const std::string& tableName, const DBTableSync::Index& index);
			virtual void createIndex(const std::string& tableName, const DBTableSync::Index& index, const DBTableSync::Field fields[]);
			virtual const std::string getSQLDateFormat(const std::string& format, const std::string& expr);
			virtual const std::string getSQLConvertInteger(const std::string& expr);
			virtual const bool isBackend(Backend backend);

		protected:

			virtual void _doExecUpdate(const SQLData& sql);

		private:
			static void _ThrowIfError(sqlite3* handle, int retCode, const std::string& message);
			SQLiteTSS* _getSQLiteTSS() const;
			SQLiteTSS* _initSQLiteTSS() const;
			sqlite3* _getHandle() const;

			friend class SQLiteResult;
			friend void cleanupTSS(SQLiteTSS* tss);
		};
	}
}


#endif // SYNTHESE_db_sqlite_SQLiteDB_h__
