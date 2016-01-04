
/** MySQLDB class implementation.
	@file MySQLDB.cpp
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

// At the beginning (macro test)
#include "ServerModule.h"

#include "01_util/threads/Thread.h"
#include "102_mysql/MySQLDB.hpp"
#include "102_mysql/MySQLException.hpp"
#include "102_mysql/MySQLResult.hpp"
#include "DBModule.h"
#include "DBRecord.hpp"
#include "DBTransaction.hpp"
#include "FactorableTemplate.h"
#include "Log.h"
#include "UtilTypes.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <errmsg.h>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTWriter.h>
#include "01_util/Stacktrace.h"

using namespace geos::io;
using namespace std;
using boost::bind;
using boost::lexical_cast;
using boost::recursive_mutex;
using boost::shared_ptr;
using boost::thread;


namespace synthese
{
	using namespace db;
	using namespace util;

	namespace
	{
		void dummyCallback(const std::string& name, const std::string& value)
		{
		}

		string genRandomString(size_t len)
		{
			static const char alphanum[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			string s;

			for (size_t i = 0; i < len; ++i)
			{
				s += alphanum[rand() % (sizeof(alphanum) - 1)];
			}

			return s;
		}

		void cleanupThread(bool* initialized)
		{
			Log::GetInstance().debug("Cleaning up MySQL thread");
			mysql_thread_end();
		}
	}

	namespace util
	{
		template<> const string FactorableTemplate<DB, MySQLDB>::FACTORY_KEY("mysql");
	}

	namespace db
	{
		MySQLDB::MySQLDB() :
			_connection(NULL),
			_mysqlThreadInitialized(cleanupThread),
			_secretToken(genRandomString(10)),
			_hasNotifyHTTPFunction(true)
		{
		}



		MySQLDB::~MySQLDB()
		{
			if (_modifEventsThread)
			{
				server::ServerModule::KillThread(
					lexical_cast<string>(_modifEventsThread->get_id()),
					false
				);
				// The modifEvents thread might throw an exception if it is killed after
				// this object is destroyed. Sleeping here should help with that issue.
				// FIXME: however it seems to still fail sometimes, a better solution is needed.
				util::Thread::Sleep(200);
			}

			try
			{
				if(_connection && !_standalone)
					execUpdate("TRUNCATE trigger_metadata;");
			}
			catch (MySQLException&)
			{
				/* ignored */
			}

			// Cleaning prepared statements
			BOOST_FOREACH(PreparedStatements::value_type& replaceStatement, _replaceStatements)
			{
				if (replaceStatement)
					mysql_stmt_close(replaceStatement);
			}
			BOOST_FOREACH(PreparedStatements::value_type& deleteStatement, _deleteStatements)
			{
				if (deleteStatement)
					mysql_stmt_close(deleteStatement);
			}

			// Connection closing
			if (_connection)
			{
				mysql_close(_connection);
			}
		}



		void MySQLDB::initForStandaloneUse()
		{
			Log::GetInstance().info("Using MySQL client library version " + std::string(mysql_get_client_info()));

			if (!mysql_thread_safe())
			{
				throw MySQLException("MySQL client not compiled as thread-safe.");
			}

			_initConnection(false);
		}



		void MySQLDB::preInit()
		{
			initForStandaloneUse();

			// TODO: is the thread deleted properly on module unload?
			if (_modifEventsThread)
			{
				server::ServerModule::KillThread(
					lexical_cast<string>(_modifEventsThread->get_id()),
					false
				);
				// The modifEvents thread might throw an exception if it is killed after
				// this object is destroyed. Sleeping here should help with that issue.
				// FIXME: however it seems to still fail sometimes, a better solution is needed.
				util::Thread::Sleep(200);
			}
			_modifEventsThread = server::ServerModule::AddThread(
				bind(&MySQLDB::_modifEventsDispatcherThread, this),
				"MySQL db modification events dispatcher"
			);

			DB::preInit();
		}



		void MySQLDB::init()
		{
			// Filling of the trigger_metadata table needs to be done in init because the port parameter
			// might not be available earlier.
			_initTriggerMetadata();

			DB::init();
		}



		DBResultSPtr MySQLDB::execQuery(const SQLData& sql)
		{
			_ensureThreadInitialized();

			Log::GetInstance().trace("MySQLDB::execQuery " + sql);

			return DBResultSPtr(new MySQLResult(this, sql));
		}



		void MySQLDB::execTransaction(const DBTransaction& transaction)
		{
			_doExecUpdate("BEGIN");
			RequestExecutor executor(*this);
			BOOST_FOREACH(const DBTransaction::Queries::value_type& query, transaction.getQueries())
			{
				boost::apply_visitor( executor, query );
			}
			_doExecUpdate("COMMIT");
			
			DB::_finishTransaction(transaction);
		}



		void MySQLDB::_doExecUpdate(const SQLData& sql)
		{
			_ensureThreadInitialized();

			if(	Log::GetInstance().getLevel() > Log::LEVEL_TRACE &&
				sql.find("spatial_ref_sys") == sql.npos
			){
				Log::GetInstance().trace("MySQLDB::execUpdate " + sql);
			}

			{
				boost::recursive_mutex::scoped_lock lock(_connectionMutex);

				_doQuery(sql);

				// Consume remaining results in case of a multi statement query.
				int status;
				do
				{
					MYSQL_RES* result = mysql_store_result(_connection);
					if (result)
					{
						mysql_free_result(result);
					}
					else if (mysql_field_count(_connection) != 0)
					{
						_throwException("Could not retrieve result set");
						break;
					}

					if ((status = mysql_next_result(_connection)) > 0)
					{
						_throwException("Could not execute statement");
					}
				} while (status == 0);
			}
		}



		void MySQLDB::initDatabase()
		{
			if(_connInfo->noTrigger)
			{
				_hasNotifyHTTPFunction = false;
			}
			else
			{
				try
				{
					execUpdate("SELECT notify_synthese_http('', '');");
				}
				catch (const MySQLException&)
				{
					_hasNotifyHTTPFunction = false;
					Log::GetInstance().warn(
						"Couldn't find the notify_synthese_http function. Direct "
						"database modifications won't be supported (and could "
						"corrupt data). See https://extranet.rcsmobility.com/projects/synthese/wiki/MySQL_Installation "
						"for instructions."
					);
				}
			}

			std::stringstream sql;
			sql <<
				"DROP TABLE IF EXISTS trigger_metadata;" <<
				"CREATE TABLE trigger_metadata (" <<
				"  url varchar(100) DEFAULT NULL," <<
				"  secret_token varchar(100) DEFAULT NULL," <<
				"  synthese_conn_id INT DEFAULT NULL" <<
				");";
			if (_hasNotifyHTTPFunction)
			{
				sql <<
					"DROP PROCEDURE IF EXISTS notify_synthese;" <<

					"CREATE PROCEDURE notify_synthese(IN tablename VARCHAR(50), IN type VARCHAR(10), IN id BIGINT)" <<
					"proc: BEGIN" <<
					"  DECLARE _url, _secret_token VARCHAR(100);" <<
					"  DECLARE _post_data VARCHAR(512);" <<
					"  DECLARE _synthese_conn_id, _dummy INTEGER;" <<
					"  SELECT url, secret_token, synthese_conn_id FROM trigger_metadata INTO " <<
					"    _url, _secret_token, _synthese_conn_id;" <<
					"  IF ISNULL(_url) OR CONNECTION_ID() = _synthese_conn_id THEN" <<
					"    LEAVE proc;" <<
					"  END IF;" <<
					"  SELECT CONCAT('nr=1&a=MySQLDBModifiedAction&actionParamst=', _secret_token, '&actionParamtb=', " <<
					"    tablename, '&actionParamty=', type, '&actionParamid=', id) INTO _post_data;" <<
					"  SELECT notify_synthese_http(_url, _post_data) INTO _dummy;" <<
					"END;";
			}
			execUpdate(sql.str());
		}



		void MySQLDB::initSpatialRefSysTable()
		{
			execUpdate(
				"CREATE TABLE IF NOT EXISTS spatial_ref_sys ("
				"  srid INTEGER NOT NULL PRIMARY KEY, "
				"  auth_name VARCHAR(256) NOT NULL, "
				"  auth_srid INTEGER NOT NULL, "
				"  ref_sys_name VARCHAR(256), "
				"  proj4text VARCHAR(2048) NOT NULL "
				")"
			);
		}



		void MySQLDB::_initPreparedStatements(
		) const {
			// Runs only once at the first thread creation
			if(!_replaceStatements.empty())
			{
				return;
			}

			// Clear the statements
			size_t tablesNumber(
				DBModule::GetTablesById().rbegin()->first + 1
			);
			_replaceStatements.resize(tablesNumber, NULL);
			_deleteStatements.resize(tablesNumber, NULL);

			// Loop on tables
			BOOST_FOREACH(const DBModule::TablesByIdMap::value_type& it, DBModule::GetTablesById())
			{
				// Replace statement
				stringstream query;
				query << "REPLACE INTO " << it.second->getFormat().NAME << " VALUES(";
				bool first(true);
				BOOST_FOREACH(const Field& field, it.second->getFieldsList())
				{
					if(first)
					{
						first = false;
					}
					else
					{
						query << ",";
					}
					if(field.isGeometry())
					{
						query << "GeomFromText(?)";
					}
					else
					{
						query << "?";
					}
				}
				query << ")";
				string queryStr(query.str());
				MYSQL_STMT* stmt(
					mysql_stmt_init(_connection)
				);
				mysql_stmt_prepare(
					stmt,
					queryStr.c_str(),
					static_cast<int>(queryStr.size())
				);
				_replaceStatements[it.first] = stmt;

				// Delete statement
				stringstream deleteQuery;
				deleteQuery << "DELETE FROM " << it.second->getFormat().NAME << " WHERE " << TABLE_COL_ID << "=?";
				string deleteQueryStr(deleteQuery.str());
				MYSQL_STMT* deleteStmt(
					mysql_stmt_init(_connection)
				);
				mysql_stmt_prepare(
					deleteStmt,
					deleteQueryStr.c_str(),
					static_cast<int>(deleteQueryStr.size())
				);
				_deleteStatements[it.first] = deleteStmt;
			}
		}



		void MySQLDB::saveRecord(
			const DBRecord& record
		){
			// Auto increment update
			RegistryKeyType objectId(
				boost::get<RegistryKeyType>(record.getContent().at(0))
			);
			record.getTable()->updateAutoIncrement(objectId);

			// Preparation of a MySQL bindable object
			boost::recursive_mutex::scoped_lock lock(_connectionMutex);
			size_t fieldsNumber(record.getTable()->getFieldsList().size());
			MYSQL_BIND* bnd = new MYSQL_BIND[fieldsNumber];
			my_bool* isNullArray = new my_bool[fieldsNumber];
			unsigned long* lengthArray = new unsigned long[fieldsNumber];
			memset(bnd, 0, fieldsNumber * sizeof(MYSQL_BIND));
			memset(isNullArray, 0, fieldsNumber * sizeof(my_bool));
			memset(lengthArray, 0, fieldsNumber * sizeof(unsigned long));
			for(size_t i(0); i<fieldsNumber; ++i)
			{
				bnd[i].is_null = isNullArray+i;
				bnd[i].length = lengthArray+i;
				DBRecordCellBindConvertor visitor(*(bnd+i));
				apply_visitor(visitor, record.getContent().at(i));
			}

			// Running the prepared statement
			MYSQL_STMT* stmt(_getReplaceStatement(record.getTable()->getFormat().ID));
			if(mysql_stmt_bind_param(stmt, bnd))
			{
				string errorMsg(mysql_stmt_error(stmt));
				Log::GetInstance().warn(errorMsg);
			}
			else
			{
				bool ok(true);
				for(size_t i(0); i<fieldsNumber; ++i)
				{
					unsigned long length(*bnd[i].length);
					if(	length >= 10240)
					{
						for(size_t offset(0); offset<length; offset += 10240)
						{
							if(	mysql_stmt_send_long_data(
									stmt,
									i,
									static_cast<char*>(bnd[i].buffer) + offset,
									(offset + 10240 >=length) ? length-offset : 10240

							)	){
								Log::GetInstance().warn("Error sending data");
								ok = false;
								break;
							}
						}
					}
					if(!ok)
					{
						break;
					}
				}
				if(ok)
				{
					if(mysql_stmt_execute(stmt))
					{
						string errorMsg(mysql_stmt_error(stmt));
						Log::GetInstance().warn(errorMsg);
					}
				}
			}

			// Cleaning
			delete[] isNullArray;
			delete[] lengthArray;
			delete[] bnd;
			mysql_stmt_reset(stmt);
		}



		void MySQLDB::deleteRow( util::RegistryKeyType id )
		{
			boost::recursive_mutex::scoped_lock lock(_connectionMutex);
			MYSQL_BIND bnd[1];
			memset(bnd, 0, sizeof(bnd));
			DBRecordCellBindConvertor visitor(*bnd);
			visitor(id);
			MYSQL_STMT* stmt(_getDeleteStatement(decodeTableId(id)));
			if(mysql_stmt_bind_param(stmt, bnd))
			{
				string errorMsg(mysql_stmt_error(stmt));
				Log::GetInstance().warn(errorMsg);
			}
			else
			{
				if(mysql_stmt_execute(stmt))
				{
					string errorMsg(mysql_stmt_error(stmt));
					Log::GetInstance().warn(errorMsg);
				}
			}
		}



		std::string MySQLDB::getSQLType(FieldType fieldType)
		{
			// Important Note:
			// The types returned here should be the types that will appear in the
			// INFORMATION_SCHEMA.COLUMNS table, (don't use alias types).
			// This is required for detecting schema changes.

			switch(fieldType)
			{
			case SQL_INTEGER:
				return "BIGINT(20)";
			case SQL_DOUBLE:
				return "DOUBLE";
			case SQL_TEXT:
				// TODO: use VARCHAR in the future for better perf. That will require
				// maximum length annotation on field declaration.
				return "TEXT";
			case SQL_TEXT_FIXED:
				return "VARCHAR(200)";
			case SQL_BOOLEAN:
				return "TINYINT(1)";
			case SQL_DATETIME:
				// TODO: use a date type instead of a string in the future
				//return "DATETIME";
				return "VARCHAR(100)";
			case SQL_DATE:
				// TODO: use a date type instead of a string in the future
				//return "DATE";
				return "VARCHAR(100)";
			case SQL_TIME:
				// TODO: use a date type instead of a string in the future
				//return "TIME";
				return "VARCHAR(100)";
			case SQL_GEOM_GEOMETRYCOLLECTION:
				return "GEOMETRYCOLLECTION";
			case SQL_GEOM_LINESTRING:
				return "LINESTRING";
			case SQL_GEOM_MULTILINESTRING:
				return "MULTILINESTRING";
			case SQL_GEOM_MULTIPOINT:
				return "MULTIPOINT";
			case SQL_GEOM_MULTIPOLYGON:
				return "MULTIPOLYGON";
			case SQL_GEOM_POINT:
				return "POINT";
			case SQL_GEOM_POLYGON:
				return "POLYGON";
			case SQL_BLOB:
				return "LONGBLOB";
			}
			return string();
		}



		bool MySQLDB::doesTableExists(const std::string& tableName)
		{
			std::string sql = "SELECT * FROM INFORMATION_SCHEMA.TABLES WHERE "
				" TABLE_SCHEMA='" + _connInfo->db + "' AND TABLE_NAME='" + tableName + "'";
			DBResultSPtr res = execQuery(sql);
			return res->next() == true;
		}



		bool MySQLDB::isTableSchemaUpToDate(
			const std::string& tableName,
			const FieldsList& fields
		){
			std::stringstream expectedSchema;
			for(size_t i(1); i!=fields.size(); ++i)
			{
				expectedSchema << ", ";
				expectedSchema << fields[i].name;
				expectedSchema << " " << getSQLType(fields[i].type);
			}
			std::stringstream actualSchema;
			std::vector<DB::ColumnInfo> columnInfos = getTableColumns(tableName);
			size_t index(0);
			BOOST_FOREACH(const DB::ColumnInfo& columnInfo, columnInfos)
			{
				// Skip primary key
				if (index++ == 0)
					continue;
				actualSchema << ", ";
				actualSchema << columnInfo.first;
				actualSchema << " " << columnInfo.second;
			}

			if (!boost::iequals(actualSchema.str(), expectedSchema.str()))
			{
				Log::GetInstance().debug(
					"MySQLDB::isTableSchemaUpToDate: schema is not up to date: "
					"actual: '" + boost::to_lower_copy(actualSchema.str()) + "' "
					"expected: '" + boost::to_lower_copy(expectedSchema.str()) + "'");
				return false;
			}
			Log::GetInstance().debug("MySQLDB::isTableSchemaUpToDate: schema is up to date.");
			return true;
		}



		std::vector<DB::ColumnInfo> MySQLDB::getTableColumns(const std::string& tableName)
		{
			std::stringstream sql;

			sql << "SELECT COLUMN_NAME, COLUMN_TYPE " <<
				"FROM INFORMATION_SCHEMA.COLUMNS " <<
				"WHERE TABLE_SCHEMA = '" << _connInfo->db << "' AND " <<
				"  TABLE_NAME = '" << tableName << "' " <<
				"ORDER BY ORDINAL_POSITION";

			DBResultSPtr res = execQuery(sql.str());
			std::vector<DB::ColumnInfo> columnInfos;
			while (res->next())
			{
				columnInfos.push_back(
					std::pair<std::string, std::string>(
						res->getText(0), res->getText(1)
					)
				);
			}

			return columnInfos;
		};



		std::string MySQLDB::getCreateTableSQL(
			const std::string& tableName,
			const FieldsList& fields
		){
			std::stringstream sql;

			sql <<
				"CREATE TABLE \"" << tableName << "\" (\"" <<
				fields[0].name << "\" " << getSQLType(fields[0].type) <<
				" PRIMARY KEY";

			for(size_t i(1); i!=fields.size(); ++i)
			{
				sql << ", \"";
				sql << fields[i].name;
				sql << "\" " << getSQLType(fields[i].type);
			}
			sql << ") DEFAULT CHARSET=utf8;";

			// Add spatial indexes
			// TODO: fix so that MySQL doesn't return this error:
			// 'All parts of a SPATIAL index must be NOT NULL (errno=1252)'
			/*
			for(size_t i(1); !fields[i].empty(); ++i)
			{
				if (fields[i].isGeometry())
					sql <<
						"ALTER TABLE \"" << tableName <<
						"\" ADD SPATIAL INDEX(" << fields[i].name << ");";
			}
			*/

			return sql.str();
		}



		void MySQLDB::afterUpdateSchema(
			const std::string& tableName,
			const FieldsList& fields
		){
			// Ignore tables without an identifier column.
			if (fields[0].name != TABLE_COL_ID)
			{
				return;
			}
			std::stringstream sql;
			const string modifTypes[] = {"insert", "update", "delete"};
			BOOST_FOREACH(const string& modifType, modifTypes)
			{
				const string triggerName = tableName + "_" + modifType;
				sql <<
					"DROP TRIGGER IF EXISTS " << triggerName << ";";
			}
			execUpdate(sql.str());

			if (!_hasNotifyHTTPFunction)
				return;

			sql.str("");
			BOOST_FOREACH(const string& modifType, modifTypes)
			{
				const string triggerName = tableName + "_" + modifType;
				sql <<
					"CREATE TRIGGER " << triggerName <<
					"  AFTER " << modifType << " ON " << tableName <<
					"  FOR EACH ROW CALL notify_synthese('" <<
					tableName << "', '" << modifType << "', " <<
					(modifType == "delete" ? "OLD" : "NEW") << ".id);";
			}
			execUpdate(sql.str());
		}



		bool MySQLDB::doesIndexExist(const std::string& tableName, const DBTableSync::Index& index)
		{
			std::stringstream sql;
			sql <<
				"SELECT COUNT(INDEX_NAME) " <<
				"FROM INFORMATION_SCHEMA.STATISTICS " <<
				"WHERE TABLE_SCHEMA='" << _connInfo->db << "' AND " <<
				"  INDEX_NAME='" << _getIndexName(tableName, index) << "'";

			DBResultSPtr res = execQuery(sql.str());
			res->next();
			unsigned int columnCount = res->getInt(0);

			if (columnCount == 0)
				return false;

			if (columnCount != index.fields.size())
			{
				throw MySQLException(
					"Existing index has an incorrect number of columns. This shouldn't happen. "
					" column count: " + lexical_cast<string>(columnCount) + " expected: " +
					lexical_cast<string>(index.fields.size())
				);
			}
			return true;
		}



		void MySQLDB::createIndex(
			const std::string& tableName,
			const DBTableSync::Index& index,
			const FieldsList& fields
		){
			// MySQL requires a length to be specified in the index for types BLOB and TEXT.
			// The code below checks the type of the column index and adds a length if needed.

			// TODO: this value is arbitrary. Do some performance tests to find the best trade-off
			// between performance and index size.
			const int TEXT_OR_BLOB_INDEX_LENGTH = 20;
			std::map<std::string, std::string> fieldToType;
			for(size_t i(1); i!=fields.size(); ++i)
			{
				fieldToType[fields[i].name] = getSQLType(fields[i].type);
			}

			std::stringstream s;
			s	<<
				"CREATE INDEX " << _getIndexName(tableName, index) <<
				" ON " << tableName << "(";
			BOOST_FOREACH(const std::string& field, index.fields)
			{
				if (field != index.fields[0])
					s << ",";
				s << field;
				if (fieldToType.find(field) == fieldToType.end())
				{
					throw MySQLException("Index uses column (" + field + ") not listed in the list of fields");
				}
				std::string type = fieldToType[field];
				if (type != "TEXT" && type != "BLOB")
					continue;
				s << "(" << lexical_cast<string>(TEXT_OR_BLOB_INDEX_LENGTH) << ") ";
			}
			s << ")";

			execUpdate(s.str());
		}



		const std::string MySQLDB::getSQLDateFormat(const std::string& format, const std::string& expr)
		{
			return "DATE_FORMAT(" + expr + ", '" + format + "')";
		}



		const std::string MySQLDB::getSQLConvertInteger(const std::string& expr)
		{
			return "CAST(" + expr + " AS UNSIGNED)";
		}



		bool MySQLDB::isBackend(Backend backend)
		{
			return backend == MYSQL_BACKEND;
		}



		const std::string& MySQLDB::getSecretToken()
		{
			return _secretToken;
		}



		// Note: SQL shouldn't be executed here, otherwise it will deadlock the MySQL server which
		// is waiting for the trigger completion.
		// That's why we have to use another thread which will dispatch the DBModifEvents which
		// can potentially execute SQL.
		void MySQLDB::addDBModifEvent(std::string table, std::string type, util::RegistryKeyType id)
		{
			if (_schemaUpdated == false) return;

			recursive_mutex::scoped_lock lock(_tableSynchronizersMutex);

			DBModifType modifType;
			if (type == "insert") { modifType = MODIF_INSERT; }
			else if (type == "update") { modifType = MODIF_UPDATE; }
			else if (type == "delete") { modifType = MODIF_DELETE; }
			else { throw MySQLException("Unknown database modification type " + type); }

			Log::GetInstance().debug("MySQLDB::addDBModifEvent: queue an event");

			_modifEventQueue.push(DBModifEvent(table, modifType, id));
		}



		void MySQLDB::_initConnection(bool initTriggerMetadata)
		{
			if (_connection != NULL)
			{
				mysql_close(_connection);
			}

			_connection = mysql_init(NULL);
			if (_connection == NULL)
			{
				throw MySQLException("mysql_init() failed");
			}

			if (mysql_real_connect(
					_connection, _connInfo->host.c_str(), _connInfo->user.c_str(),
					_connInfo->passwd.c_str(), _connInfo->db.c_str(),
					_connInfo->port, NULL, CLIENT_MULTI_STATEMENTS
				) == NULL)
			{
				_throwException("Can't connect to MySQL server");
			}

			// Everything should be handled in utf8.
			execUpdate("SET NAMES utf8;");

			// Use more standard syntax:
			// ANSI_QUOTES: use single quotes for quoting identifier.
			// NO_BACKSLASH_ESCAPES: backslash is not considered a special character.
			execUpdate("SET sql_mode='ANSI_QUOTES,NO_BACKSLASH_ESCAPES';");

			if (initTriggerMetadata)
			{
				_initTriggerMetadata();
			}
		}



		void MySQLDB::_initTriggerMetadata()
		{
			execUpdate("TRUNCATE trigger_metadata;");
			string port = DBModule::GetParameter(server::ServerModule::MODULE_PARAM_PORT);
			string triggerHost = _connInfo->triggerHost;
			if (triggerHost.empty())
				triggerHost = "localhost";
			string triggerURL = "http://" + triggerHost + ":" + port + "/";
			std::stringstream sql;
			sql <<
				"INSERT INTO trigger_metadata VALUES ('" <<
				triggerURL << "', '" << _secretToken << "', CONNECTION_ID()"
				");";
			execUpdate(sql.str());
		}



		void MySQLDB::_doQuery(const SQLData& sql)
		{
			const int NUM_QUERY_RETRY = 40;

			for (int i = 1; i <= NUM_QUERY_RETRY; i++)
			{
				if (mysql_query(_connection, sql.c_str()))
				{
					if (mysql_errno(_connection) == CR_SERVER_GONE_ERROR)
					{
						bool isFatal(false);

						Log::GetInstance().warn(
							"MySQLDB has gone away, reinitializing connection (" +
							lexical_cast<string>(i) + " / " + lexical_cast<string>(NUM_QUERY_RETRY) + ")"
						);
						util::Thread::Sleep(500);
						try
						{
							_initConnection();
						}
						catch (const MySQLException& e)
						{
							// Don't retry if there's a wrong db error. That's a situation that can happen in the tests.
							const unsigned int ER_BAD_DB_ERROR = 1049;
							isFatal = e.getErrno() == ER_BAD_DB_ERROR;

							Log::GetInstance().warn(
								string("Exception while trying to initialize connection again (") +
								(isFatal ? "" : "not ") + "fatal)", e
							);
						}
						if (!isFatal)
							continue;
					}
					_throwException("MySQL error in mysql_query()");
				}
				break;
			}
		}



		void MySQLDB::_modifEventsDispatcherThread()
		{
			DBModifEvent modifEvent;

			while (true)
			{
				Log::GetInstance().debug("MySQLDB::_modifEventsDispatcherThread: waiting for DBModifEvent");
				_modifEventQueue.wait_and_pop(modifEvent);
				// Apparently, the updated data is visible only after the trigger has finished running.
				// The trigger is not finished running during the HTTP request to synthese. So we need
				// to wait a bit here before dispatching the event, so that we can be sure the trigger
				// is finished and that a potential load will see the updated data. That's sucky.
				util::Thread::Sleep(200);
				Log::GetInstance().debug(
					"MySQLDB::_modifEventsDispatcherThread: got a modif event. "
					" table: " + modifEvent.table +
					" type: " + lexical_cast<string>(modifEvent.type) +
					" id: " + lexical_cast<string>(modifEvent.id));
				_dispatchDBModifEvent(modifEvent);
			}
		}



		void MySQLDB::_ensureThreadInitialized()
		{
			if (_mysqlThreadInitialized.get())
				return;

			Log::GetInstance().debug("Initializing MySQL thread");
			_mysqlThreadInitialized.reset(new bool(true));
			mysql_thread_init();
		}



		void MySQLDB::_throwException(const std::string& message)
		{
			throw MySQLException(
				message + " : " + std::string(mysql_error(_connection)) +
				" (errno=" + lexical_cast<string>(mysql_errno(_connection)) + ")",
				mysql_errno(_connection)
			);
		}



		MYSQL_STMT* MySQLDB::_getReplaceStatement(
			RegistryTableType tableId
		) const	{
			_initPreparedStatements();
			return _replaceStatements[tableId];
		}



		MYSQL_STMT* MySQLDB::_getDeleteStatement(
			RegistryTableType tableId
		) const {
			_initPreparedStatements();
			return _deleteStatements[tableId];
		}



		MySQLDB::DBRecordCellBindConvertor::DBRecordCellBindConvertor(
			MYSQL_BIND& bnd
		):	_bnd(bnd)
		{}



		void MySQLDB::DBRecordCellBindConvertor::operator()( const int& i ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_LONG;
			_bnd.buffer = static_cast<void*>(const_cast<int*>(&i));
		}

#ifndef _WIN32

		void MySQLDB::DBRecordCellBindConvertor::operator()( const size_t& s ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_LONG;
			_bnd.buffer = static_cast<void*>(const_cast<void*>(static_cast<const void*>(&s)));
		}

#endif

		void MySQLDB::DBRecordCellBindConvertor::operator()( const bool& d ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_TINY;
			_bnd.buffer = static_cast<void*>(const_cast<bool*>(&d));
		}



		void MySQLDB::DBRecordCellBindConvertor::operator()( const double& d ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_DOUBLE;
			_bnd.buffer = static_cast<void*>(const_cast<double*>(&d));
		}



		void MySQLDB::DBRecordCellBindConvertor::operator()( const util::RegistryKeyType& id ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_LONGLONG;
			_bnd.buffer = static_cast<void*>(const_cast<RegistryKeyType*>(&id));
		}



		void MySQLDB::DBRecordCellBindConvertor::operator()( const boost::optional<std::string>& str ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_BLOB;
			if(str)
			{
				_bnd.buffer = static_cast<void*>(const_cast<char*>(str->c_str()));
				*_bnd.length = str->size();
			}
			else
			{
				*_bnd.is_null = true;
			}
		}



		void MySQLDB::DBRecordCellBindConvertor::operator()( const boost::optional<Blob>& blob ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_BLOB;
			if(blob)
			{
				_bnd.buffer = static_cast<void*>(const_cast<char*>(blob->first));
				*_bnd.length = blob->second;
			}
			else
			{
				*_bnd.is_null = true;
			}
		}



		void MySQLDB::DBRecordCellBindConvertor::operator()( const boost::shared_ptr<geos::geom::Geometry>& geom ) const
		{
			_bnd.buffer_type = MYSQL_TYPE_STRING;
			string str;
			if(geom)
			{
				boost::shared_ptr<geos::geom::Geometry> projected(geom);
				if(	CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
					static_cast<CoordinatesSystem::SRID>(geom->getSRID())
				){
					projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*geom);
				}

				WKTWriter wkt;
				str = wkt.write(projected.get());
			}
			char* persistentStr = (char*) malloc(str.size() * sizeof(char));
			memcpy(persistentStr, str.c_str(), str.size());
			_bnd.buffer = static_cast<void*>(persistentStr);
			*_bnd.length = str.size();
		}



		MySQLDB::RequestExecutor::RequestExecutor( MySQLDB& db ):
			_db(db)
		{

		}



		void MySQLDB::RequestExecutor::operator()( const std::string& d )
		{
			_db._doExecUpdate(d);
		}



		void MySQLDB::RequestExecutor::operator()( const DBRecord& r )
		{
			_db.saveRecord(r);
		}



		void MySQLDB::RequestExecutor::operator()(
			util::RegistryKeyType id
		){
			_db.deleteRow(id);
		}
}	}
