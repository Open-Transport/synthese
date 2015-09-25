
/** MySQLDB class header.
	@file MySQLDB.hpp
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

#ifndef SYNTHESE_db_mysql_MySQLDB_h__
#define SYNTHESE_db_mysql_MySQLDB_h__

#define HAVE_INT64 // Let MySQL not to typedef int64 (conflicts with libgeos)
#include "DB.hpp"
#include "DBRecord.hpp"
#include "FactorableTemplate.h"
#include "01_util/ConcurrentQueue.hpp"

#include <my_global.h>
#include <mysql.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>

struct st_mysql;
typedef struct st_mysql MYSQL;

namespace synthese
{
	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// MySQL database backend.
		///
		/// @author Sylvain Pasche
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class MySQLDB:
			public util::FactorableTemplate<DB, MySQLDB>
		{
		private:

			MYSQL* _connection;
			// Recursive because we might need to run sub-requests when reinitializing the db connection.
			boost::recursive_mutex _connectionMutex;
			boost::thread_specific_ptr<bool> _mysqlThreadInitialized;
			const std::string _secretToken;
			bool _hasNotifyHTTPFunction;
			ConcurrentQueue<DBModifEvent> _modifEventQueue;
			boost::shared_ptr<boost::thread> _modifEventsThread;

			typedef std::vector<MYSQL_STMT*> PreparedStatements;
			mutable PreparedStatements _replaceStatements;
			mutable PreparedStatements _deleteStatements;

			class DBRecordCellBindConvertor:
				public boost::static_visitor<>
			{
				MYSQL_BIND& _bnd;

			public:
				DBRecordCellBindConvertor(
					MYSQL_BIND& bnd
				);

				void operator()(const int& i) const;
				void operator()(const bool& i) const;
				void operator()(const double& d) const;
#ifndef _WIN32
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
				MySQLDB& _db;

			public:
				RequestExecutor(
					MySQLDB& db
				);

				/// SQL
				void operator()(const std::string& d);

				/// Replace
				void operator()(const DBRecord& r);

				/// Delete
				void operator()(util::RegistryKeyType id);
			};

		public:

			MySQLDB();
			virtual ~MySQLDB();

			virtual void initForStandaloneUse();
			virtual void preInit();
			virtual void init();
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

			virtual void initDatabase();
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

			const std::string& getSecretToken();
			void addDBModifEvent(std::string table, std::string type, util::RegistryKeyType id);

		protected:

			virtual void _doExecUpdate(const SQLData& sql);
#ifdef DO_VERIFY_TRIGGER_EVENTS
			virtual bool _dbModifCheckEnabled()
			{
				return false;
			}
#endif
		private:

			void _initPreparedStatements() const;
			MYSQL_STMT* _getReplaceStatement(util::RegistryTableType tableId) const;
			MYSQL_STMT* _getDeleteStatement(util::RegistryTableType tableId) const;
			void _initConnection(bool initTriggerMetadata = true);
			void _initTriggerMetadata();
			//////////////////////////////////////////////////////////////////////////
			/// Execute a MySQL query, retrying if the server has gone away (e.g. if it
			/// was restarted or the connection was dropped).
			/// @param sql The query to execute.
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			void _doQuery(const SQLData& sql);
			void _modifEventsDispatcherThread();
			void _ensureThreadInitialized();
			void _throwException(const std::string& message);

			friend class MySQLResult;
		};
	}
}


#endif // SYNTHESE_db_mysql_MySQLDB_h__
