
/** SQLiteDB class header.
	@file SQLiteDB.h
	@author Sylvain Pasche

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

#ifndef SYNTHESE_db_sqlite_SQLiteDB_h__
#define SYNTHESE_db_sqlite_SQLiteDB_h__

#include "DB.hpp"
#include "FactorableTemplate.h"
#include "FrameworkTypes.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/thread/tss.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <spatialite/sqlite3.h>

namespace synthese
{
	namespace db
	{
		class DBRecord;
		class DBTableSync;

		typedef std::vector<sqlite3_stmt*> PreparedStatements;

		struct SQLiteTSS
		{
			sqlite3* handle;
#ifdef DO_VERIFY_TRIGGER_EVENTS
			std::vector<DB::DBModifEvent> events;
#endif
			PreparedStatements replaceStatements;
			PreparedStatements deleteStatements;
		};

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

			class DBRecordCellBindConvertor:
				public boost::static_visitor<>
			{
				sqlite3_stmt& _stmt;
				size_t _i;

			public:
				DBRecordCellBindConvertor(
					sqlite3_stmt& stmt,
					size_t i
				);

				void operator()(const int& i) const;
				void operator()(const bool& i) const;
				void operator()(const double& d) const;
#ifndef _WINDOWS
				void operator()(const size_t& s) const;
#endif
				void operator()(const util::RegistryKeyType& id) const;
				void operator()(const boost::optional<std::string>& str) const;
				void operator()(const boost::optional<Blob>& blob) const;
				void operator()(const boost::shared_ptr<geos::geom::Geometry>& geom) const;
			};

			class RequestExecutor:
				public boost::static_visitor<>
			{
				SQLiteDB& _db;

			public:
				RequestExecutor(
					SQLiteDB& db
				);

				/// SQL
				void operator()(const std::string& d);

				/// Replace
				void operator()(const DBRecord& r);

				/// Delete
				void operator()(util::RegistryKeyType id);
			};

		public:

			SQLiteDB();
			virtual ~SQLiteDB();

			virtual void initForStandaloneUse();
			virtual void preInit();
			virtual void saveRecord(
				const DBRecord& record
			);
			virtual void deleteRow(
				util::RegistryKeyType id
			);

			virtual DBResultSPtr execQuery(const SQLData& sql);
			virtual void execTransaction(
				const DBTransaction& transaction
			);

			virtual void initSpatialRefSysTable();
			virtual std::string getSQLType(FieldType fieldType);
			virtual bool doesTableExists(const std::string& tableName);
			virtual bool isTableSchemaUpToDate(const std::string& tableName, const FieldsList& fields);
			virtual std::vector<ColumnInfo> getTableColumns(const std::string& tableName);
			virtual std::string getCreateTableSQL(const std::string& tableName, const FieldsList& fields);
			virtual void afterUpdateSchema(const std::string& tableName, const FieldsList& fields);
			virtual bool doesIndexExist(const std::string& tableName, const DBTableSync::Index& index);
			virtual void createIndex(const std::string& tableName, const DBTableSync::Index& index, const FieldsList& fields);
			virtual const std::string getSQLDateFormat(const std::string& format, const std::string& expr);
			virtual const std::string getSQLConvertInteger(const std::string& expr);
			virtual bool isBackend(Backend backend);
			
		protected:

			virtual void _doExecUpdate(const SQLData& sql);

		private:
			void _initPreparedStatements() const;
			sqlite3_stmt* _getReplaceStatement(util::RegistryTableType tableId) const;
			sqlite3_stmt* _getDeleteStatement(util::RegistryTableType tableId) const;
			sqlite3* _getHandle() const;
			static void _ThrowIfError(sqlite3* handle, int retCode, const std::string& message);
			SQLiteTSS* _getSQLiteTSS() const;
			SQLiteTSS* _initSQLiteTSS() const;
			
			friend class SQLiteResult;
			friend void cleanupTSS(SQLiteTSS* tss);
		};
	}
}


#endif // SYNTHESE_db_sqlite_SQLiteDB_h__
