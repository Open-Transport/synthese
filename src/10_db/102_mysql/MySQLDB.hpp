
/** MySQLDB class header.
	@file MySQLDB.hpp
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

#ifndef SYNTHESE_db_mysql_MySQLDB_h__
#define SYNTHESE_db_mysql_MySQLDB_h__

#include "DB.hpp"
#include "FactorableTemplate.h"
#include "01_util/ConcurrentQueue.hpp"

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
		class MySQLDB : public util::FactorableTemplate<DB, MySQLDB>
		{
		private:

			MYSQL* _connection;
			// Recursive because we might need to run sub-requests when reinitializing the db connection.
			boost::recursive_mutex _connectionMutex;
			boost::thread_specific_ptr<bool> _mysqlThreadInitialized;
			const std::string _secretToken;
			ConcurrentQueue<DBModifEvent> _modifEventQueue;
			boost::shared_ptr<boost::thread> _modifEventsThread;

		public:

			MySQLDB();
			virtual ~MySQLDB();

			virtual void preInit();
			virtual void init();

			virtual DBResultSPtr execQuery(const SQLData& sql);
			virtual void execTransaction(
				const DBTransaction& transaction
			);

			virtual void initDatabase();
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

			const std::string& getSecretToken();
			void addDBModifEvent(std::string table, std::string type, util::RegistryKeyType id);

		protected:

			virtual void _doExecUpdate(const SQLData& sql);
#ifdef DO_VERIFY_TRIGGER_EVENTS
			virtual bool _DBModifCheckEnabled()
			{
				return false;
			}
#endif
		private:

			void _initConnection();
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
