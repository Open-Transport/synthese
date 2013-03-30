
/** DB class header.
	@file DB.hpp
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

#ifndef SYNTHESE_db_Db_hpp__
#define SYNTHESE_db_Db_hpp__

#include "CoordinatesSystem.hpp"
#include "FactoryBase.h"
#include "DBResult.hpp"
#include "DBTableSync.hpp"
#include "Field.hpp"

#include <boost/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>

// If this define is set, database triggers will be enabled and events fired with the db
// trigger will be compared against the one we fire, to check that we fire the same events.
// It should be removed in the future, once we are sure the new trigger system works
// as well as the database trigger one.
//#ifdef _DEBUG
//#define DO_VERIFY_TRIGGER_EVENTS 0 //1
//#endif

#ifdef DO_VERIFY_TRIGGER_EVENTS
#include <boost/unordered_set.hpp>
#endif

namespace synthese
{
	class ObjectBase;
	class Exception;

	namespace db
	{
		class DB;
		class DBRecord;
		class DBTransaction;

		typedef std::string SQLData;

		//////////////////////////////////////////////////////////////////////////
		/// Base database class to be extended by each backend.
		///
		/// @author Sylvain Pasche
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class DB :
			public util::FactoryBase<DB>
		{

		public:
			typedef std::pair<std::string, std::string> ColumnInfo;

			class InvalidConnectionStringException :
				public synthese::Exception
			{
			public:
				InvalidConnectionStringException(const std::string& message) :
					Exception(message)
				{
				}
			};

			struct ConnectionInfo
			{
				std::string backend;
				std::string path;
				std::string host;
				std::string user;
				std::string passwd;
				std::string db;
				std::string triggerHost;
				int port;
				bool debug;
				bool triggerCheck;
				bool noTrigger;

				ConnectionInfo(const std::string& connectionString);
			};

			typedef enum
			{
				MODIF_INSERT,
				MODIF_UPDATE,
				MODIF_DELETE
			} DBModifType;

			struct DBModifEvent
			{
				DBModifEvent(std::string table, DBModifType type, util::RegistryKeyType id);
				DBModifEvent();
				std::string table;
				DBModifType type;
				util::RegistryKeyType id;
			};

			enum Backend {
				SQLITE_BACKEND = 0,
				MYSQL_BACKEND = 1
			};

		protected:

			bool _schemaUpdated;
			boost::shared_ptr<ConnectionInfo> _connInfo;
			// XXX does it need to be recursive?
			boost::recursive_mutex _tableSynchronizersMutex;
			bool _standalone;

		private:

			static const CoordinatesSystem::SRID _STORAGE_COORD_SYSTEM_SRID;
			static const CoordinatesSystem::SRID _DEFAULT_INSTANCE_COORD_SYSTEM_SRID;
#ifdef DO_VERIFY_TRIGGER_EVENTS
			boost::unordered_set<DBModifEvent> _recordedEvents;
#endif

		public:

			DB();
			virtual ~DB();

			void setConnectionInfo(boost::shared_ptr<ConnectionInfo> connInfo);
			void setStandaloneUse(bool standalone);

			//////////////////////////////////////////////////////////////////////////
			/// Initialize the database to be used in standalone mode (See DBModule::GetDBForStandaloneUse())
			/// @author Sylvain Pasche
			/// @date 2012
			/// @since 3.3.0
			virtual void initForStandaloneUse() = 0;
			virtual void preInit();
			virtual void init();
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Executes a query which updates a blob field.
			/// This should uses prepared statements instead of textual SQL query.
			virtual void saveRecord(
				const DBRecord& record
			) = 0;

			virtual void deleteRow(
				util::RegistryKeyType id
			) = 0;


			virtual DBResultSPtr execQuery(const SQLData& sql) = 0;
			virtual void execTransaction(const DBTransaction& transaction) = 0;
			void execUpdate(
				const SQLData& sql,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);

			void addDBModifEvent(const DBModifEvent& modifEvent, boost::optional<DBTransaction&> transaction);
#ifdef DO_VERIFY_TRIGGER_EVENTS
			void checkModificationEvents();
#endif

			//////////////////////////////////////////////////////////////////////////
			/// First method called after connected to the database. Backends can override this
			/// to run setup things globally.
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual void initDatabase();

			//////////////////////////////////////////////////////////////////////////
			/// Initialize the spatial_ref_sys table and other things needed for spatial capabilities.
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual void initSpatialRefSysTable() = 0;

			//////////////////////////////////////////////////////////////////////////
			/// Returns the SQL type name corresponding to the given FieldType type.
			/// @param fieldType Type identifier
			/// @return std::string Corresponding type for this backend
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual std::string getSQLType(FieldType fieldType) = 0;

			//////////////////////////////////////////////////////////////////////////
			/// Checks if a given table exists in the database.
			/// @param tableName Table name to check for existence
			/// @return bool True if the given table exists
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual bool doesTableExists(const std::string& tableName) = 0;

			//////////////////////////////////////////////////////////////////////////
			/// Checks if a table with its list of fields match the schema of the same table in the database.
			/// @param tableName Table name to check for schema match
			/// @param fieldsList List of table fields
			/// @return bool True if the table schema matches the given fields
			/// @pre The table to check for schema match must exist in the database
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual bool isTableSchemaUpToDate(
				const std::string& tableName,
				const FieldsList& fields
			) = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Retrieves a list of columns with their types for a given table.
			/// @param tableName Table name to check retrieve columns
			/// @return vector<ColumnInfo> List of pairs of column name and type name
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual std::vector<ColumnInfo> getTableColumns(const std::string& tableName) = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the SQL code to execute to create the table in the database.
			/// @param tableName Table name to create
			/// @param fields List of table fields
			/// @return the SQL command which creates the table in the database
			/// @author Hugues Romain, Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual std::string getCreateTableSQL(
				const std::string& tableName,
				const FieldsList& fields
			) = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Called as the last step of the schema update/creation process of a table. Can be overridden
			/// by a database backend to do some finalization work.
			/// @param tableName Table name being processed
			/// @param fields List of table fields
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual void afterUpdateSchema(
				const std::string& tableName,
				const FieldsList& fields
			);



			//////////////////////////////////////////////////////////////////////////
			/// Checks if an index already exists.
			/// @param tableName Table name containing the index
			/// @param index The index to check for exstence
			/// @return bool True if the given index already exists
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual bool doesIndexExist(const std::string& tableName, const DBTableSync::Index& index) = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Creates an index in the database given a certain format.
			/// @param tableName Table to create an index for
			/// @param index The index to create
			/// @author Hugues Romain, Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual void createIndex(
				const std::string& tableName,
				const DBTableSync::Index& index,
				const FieldsList& fields
			) = 0;


			//////////////////////////////////////////////////////////////////////////
			// SQL generator helpers


			//////////////////////////////////////////////////////////////////////////
			/// Returns SQL code to format a date. The format should follow the
			/// syntax of SQLite strftime (http://www.sqlite.org/lang_datefunc.html)
			/// and MySQL DATE_FORMAT (http://dev.mysql.com/doc/refman/5.1/en/date-and-time-functions.html#function_date-format).
			/// @param format Date format specifier (without quotes)
			/// @param expr SQL expression returning a date
			/// @return string SQL expression for formatting a date
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual const std::string getSQLDateFormat(const std::string& format, const std::string& expr) = 0;

			//////////////////////////////////////////////////////////////////////////
			/// Returns SQL code to convert an expression to an integer.
			/// @param expr SQL expression returning a date
			/// @return string SQL expression for converting to integer
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual const std::string getSQLConvertInteger(const std::string& expr) = 0;

			//////////////////////////////////////////////////////////////////////////
			/// Check if this database matches the given backend.
			/// Should only be used as a last resort if it's not possible to use
			/// generic SQL code or the SQL generator helpers above.
			/// @return bool
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			virtual bool isBackend(Backend backend) = 0;

		protected:

			virtual void _doExecUpdate(const SQLData& sql) = 0;

			////////////////////////////////////////////////////////////////////
			/// Creates the database wide name of an index.
			/// @param tableName table
			/// @param index index
			/// @return name of the index
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.3.0
			std::string _getIndexName(const std::string& tableName, const DBTableSync::Index& index);

#ifdef DO_VERIFY_TRIGGER_EVENTS
			void _recordDBModifEvents(const std::vector<DBModifEvent>& modifEvents);
			virtual bool _dbModifCheckEnabled()
			{
				return _connInfo->triggerCheck;
			}
#endif

			////////////////////////////////////////////////////////////////////
			/// Must be called after the transaction is executed to dispatch
			/// modification events to table syncs
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			void _finishTransaction(const DBTransaction& transaction);

			////////////////////////////////////////////////////////////////////
			/// Calls the rows{Added,Updated,Removed} method on the corresponding table sync from
			/// the given database change event.
			/// @param modifEvent Modification event
			/// @author Sylvain Pasche
			/// @date 2011
			/// @since 3.3.0
			void _dispatchDBModifEvent(const DBModifEvent& modifEvent);

		public:
			void replaceStmt(
				ObjectBase& o,
				boost::optional<DBTransaction&> transaction
			);

			void replaceStmt(
				util::RegistryKeyType objectId,
				const DBRecord& r,
				boost::optional<DBTransaction&> transaction
			);

			void deleteStmt(
				util::RegistryKeyType objectId,
				boost::optional<DBTransaction&> transaction
			);
		};
	}
}

#endif // SYNTHESE_db_Db_hpp__
