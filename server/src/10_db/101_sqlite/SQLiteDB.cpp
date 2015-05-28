
/** SQLiteDB class implementation.
	@file SQLiteDB.cpp

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

#include "SQLiteDB.h"

#include "DBModule.h"
#include "DBRecord.hpp"
#include "DBTableSync.hpp"
#include "DBTransaction.hpp"
#include "Log.h"
#include "Conversion.h"
#include "101_sqlite/SQLiteException.hpp"
#include "101_sqlite/SQLiteResult.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/geom/Geometry.h>
#include <geos/io/WKTWriter.h>
#include <spatialite.h>

using namespace std;
using namespace boost;
using namespace geos::io;

namespace synthese
{
	using namespace db;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<DB, SQLiteDB>::FACTORY_KEY("sqlite");
	}

	namespace db
	{
		void cleanupTSS(SQLiteTSS* tss)
		{
			// FIXME: tss is sometimes null on Windows during unit tests. Needs to be investigated.
			if (!tss)
			{
				Log::GetInstance().warn("tss is null, not calling sqlite3_close");
				return;
			}

			// Finalize the prepared statements
			BOOST_FOREACH(sqlite3_stmt* stmt, tss->replaceStatements)
			{
				sqlite3_finalize(stmt);
			}
			BOOST_FOREACH(sqlite3_stmt* stmt, tss->deleteStatements)
			{
				sqlite3_finalize(stmt);
			}

			// Closing the SQLite "connection"
			SQLiteDB::_ThrowIfError(tss->handle, sqlite3_close(tss->handle), "Cannot close SQLite handle");

			// Memory clean
			delete tss;
		}



		int sqliteBusyHandler(void* arg, int nbCalls)
		{
			// Return a non-zero value so that a retry is made, waiting for SQLite not ot be busy anymore...
			return 1;
		}



#ifdef DO_VERIFY_TRIGGER_EVENTS
		void sqliteUpdateHook(void* userData, int opType, const char* dbName, const char* tbName, sqlite_int64 rowId)
		{
			// WARNING : the update hook is invoked only when working with the connection
			// created inside the body of this thread (initialize).
			SQLiteTSS* tss = (SQLiteTSS*)userData;

			DB::DBModifType modifType;
			if (opType == SQLITE_INSERT) { modifType = DB::MODIF_INSERT; }
			else if (opType == SQLITE_UPDATE) { modifType = DB::MODIF_UPDATE; }
			else if (opType == SQLITE_DELETE) { modifType = DB::MODIF_DELETE; }
			else { throw SQLiteException("Unknown database modification type " + opType); }

			tss->events.push_back(DB::DBModifEvent(tbName, modifType, rowId));
		}



		void sqliteRollbackHook(void* arg)
		{
			SQLiteTSS* tss = (SQLiteTSS*)arg;
			tss->events.clear();
		}
#endif



		SQLiteDB::SQLiteDB() :
			_databaseFile(),
			_tss(&cleanupTSS)
		{
		}



		SQLiteDB::~SQLiteDB()
		{
		}



		void SQLiteDB::_ThrowIfError(sqlite3* handle, int retCode, const std::string& message)
		{
			if (retCode == SQLITE_OK || retCode == SQLITE_DONE)
			{
				return;
			}
			throw SQLiteException(message + " (errmsg='" + sqlite3_errmsg(handle) + "' code=" + lexical_cast<string>(retCode) + ")");
		}

        
        void trace_callback( void* udp, const char* sql )
        {
			Log::GetInstance().trace(sql);
        }
        
		void SQLiteDB::initForStandaloneUse()
		{
			if (_connInfo->path.empty())
				_connInfo->path = "config.db3";
			_databaseFile = _connInfo->path;

			spatialite_init(1);
			Log::GetInstance().info("Using lib SQLite version " + std::string(sqlite3_libversion()));
		}



		void SQLiteDB::preInit()
		{
			initForStandaloneUse();
			DB::preInit();
		}



		SQLiteTSS* SQLiteDB::_getSQLiteTSS() const
		{
			if (_tss.get() == 0)
			{
				// Create the sqlite handle

				sqlite3* handle;
				int retc = sqlite3_open(_databaseFile.string().c_str(), &handle);
				_ThrowIfError(handle, retc, "Cannot open SQLite handle to " + _databaseFile.string());

				// int
				sqlite3_busy_handler(handle, &sqliteBusyHandler, 0);

				//lint --e{429}
				SQLiteTSS* tss = new SQLiteTSS();

#ifdef DO_VERIFY_TRIGGER_EVENTS
				sqlite3_update_hook(handle, &sqliteUpdateHook, tss);
				sqlite3_rollback_hook(handle, &sqliteRollbackHook, tss);
#endif

				tss->handle = handle;
                if (DBModule::IsSqlTraceActive())
                {
                    sqlite3_trace(tss->handle, trace_callback, 0);
                }
				_tss.reset(tss);
			}
			return _tss.get();
		}



		// TODO: inline once DO_VERIFY_TRIGGER_EVENTS is removed.
		SQLiteTSS* SQLiteDB::_initSQLiteTSS() const
		{
			SQLiteTSS* tss = _getSQLiteTSS();
#ifdef DO_VERIFY_TRIGGER_EVENTS
			tss->events.clear();
#endif
			return tss;
		}



		DBResultSPtr SQLiteDB::execQuery(const SQLData& sql)
		{
			Log::GetInstance().trace("SQLiteDB::execQuery " + sql);

			sqlite3_stmt* st;
			int retc = sqlite3_prepare_v2(_getHandle(), sql.c_str(), static_cast<int>(sql.length()), &st, 0);

			_ThrowIfError(_getHandle(), retc, "Error compiling '" + sql + "'");

			return DBResultSPtr(new SQLiteResult(_getHandle(), st, sql));
		}



		sqlite3* SQLiteDB::_getHandle() const
		{
			return _getSQLiteTSS()->handle;
		}



		void SQLiteDB::execTransaction(
			const DBTransaction& transaction
		){
#ifdef DO_VERIFY_TRIGGER_EVENTS
			// Lock this method so that no database update can start before hooks
			// have finished their execution. The mutex is recursive so that
			// an update can still be called inside hook callback.
			boost::recursive_mutex::scoped_lock lock(_updateMutex);
#endif

			_initSQLiteTSS();

			int retc = sqlite3_exec(_getHandle(), "BEGIN TRANSACTION;", 0, 0, 0);

			_ThrowIfError(_getHandle(), retc, "Error executing batch update when opening transaction");

			try
			{
				RequestExecutor executor(*this);
				BOOST_FOREACH(const DBTransaction::Queries::value_type& query, transaction.getQueries())
				{
					boost::apply_visitor( executor, query );
				}

				retc = sqlite3_exec(_getHandle(), "COMMIT;", 0, 0, 0);
				_ThrowIfError(_getHandle(), retc, "Error executing batch update when commiting transaction, database may be locked.");
			}
			// FIXME: could we get anything other than SQLiteException here?
			catch (const SQLiteException& e)
			{
				Log::GetInstance().warn("Exception during batch update: " + e.getMessage());

				retc = sqlite3_exec(_getHandle(), "ROLLBACK;", 0, 0, 0);
				_ThrowIfError(
					_getHandle(),
					retc,
					"Error executing batch update when rollback transaction, database may be locked. Original exception: " + e.getMessage()
				);
				throw SQLiteException(
					"Error executing batch update when reading transaction, database may be locked. Original exception: " + e.getMessage()
				);
			}

#ifdef DO_VERIFY_TRIGGER_EVENTS
			_recordDBModifEvents(tss->events);
#endif
			DB::_finishTransaction(transaction);
		}



		void SQLiteDB::_doExecUpdate(const SQLData& sql)
		{
#ifdef DO_VERIFY_TRIGGER_EVENTS
			// Lock this method so that no database update can start before hooks
			// have finished their execution. The mutex is recursive so that
			// an update can still be called inside hook callback.
			boost::recursive_mutex::scoped_lock lock(_updateMutex);
#endif
			// Do a batch execution (no precompilation since it can contains more than one
			// statement which is impossible to validate wihtout executing them one by one, given one database state)
			assert(sql.size() > 0);

			_initSQLiteTSS();

			RequestExecutor rx(*this);
			rx(sql);

#ifdef DO_VERIFY_TRIGGER_EVENTS
			_recordDBModifEvents(tss->events);
#endif
		}



		void SQLiteDB::initSpatialRefSysTable()
		{
			// Initialization of SpatiaLite (inspired from official init_spatialite-2.3.sql)
			execUpdate("SELECT InitSpatialMetaData();");
		}



		std::string SQLiteDB::getSQLType(FieldType fieldType)
		{
			switch(fieldType)
			{
			case SQL_INTEGER:
				return "INTEGER";
			case SQL_DOUBLE:
				return "REAL";
			case SQL_TEXT:
			case SQL_TEXT_FIXED:
				return "TEXT";
			case SQL_BOOLEAN:
				return "BOOLEAN";
			case SQL_DATETIME:
				return "TIMESTAMP";
			case SQL_DATE:
				return "DATE";
			case SQL_TIME:
				return "TIME";
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
				return "BLOB";
			}
			return string();
		}



		bool SQLiteDB::doesTableExists(const std::string& tableName)
		{
			std::string sql = "SELECT * FROM SQLITE_MASTER WHERE name='" + tableName + "' AND type='table'";
			DBResultSPtr res = this->execQuery(sql);
			return res->next() == true;
		}



		bool SQLiteDB::isTableSchemaUpToDate(
			const std::string& tableName,
			const FieldsList& fields
		){
			// In the SQLite backend, the check to see if the schema is up to date is made by comparing the
			// "CREATE TABLE" SQL returned by SQLite with a "CREATE TABLE" string that we build ourselves and
			// that should match the value returned by SQLite if the schema is up to date.

			std::stringstream sql;
			sql <<
				"CREATE TABLE " << tableName << " (\"" <<
				fields[0].name << "\" " << getSQLType(fields[0].type) <<
				" UNIQUE PRIMARY KEY ON CONFLICT ROLLBACK";

			for(size_t i(1); i!=fields.size(); ++i)
			{
				sql << ", \"";
				sql << fields[i].name;
				sql << "\" " << getSQLType(fields[i].type);
			}
			sql << ")";
			std::string expectedCreateTableSQL = sql.str();

			DBResultSPtr result = this->execQuery(
				"SELECT sql FROM SQLITE_MASTER where type='table' and name='" + tableName + "'"
			);
			if (!result->next())
				return false;
			std::string actualCreateTableSQL = result->getText(0);

			if (!boost::iequals(actualCreateTableSQL, expectedCreateTableSQL))
			{
				Log::GetInstance().debug(
					"SQLiteDB::isTableSchemaUpToDate: schema is not up to date: "
					"actual: '" + boost::to_lower_copy(actualCreateTableSQL) + "' "
					"expected: '" + boost::to_lower_copy(expectedCreateTableSQL) + "'");
				return false;
			}

			Log::GetInstance().debug("SQLiteDB::isTableSchemaUpToDate: schema is up to date.");
			return true;
		}



		std::vector<DB::ColumnInfo> SQLiteDB::getTableColumns(const std::string& tableName)
		{
			std::vector<DB::ColumnInfo> columnInfos;
			std::string sql = "PRAGMA TABLE_INFO (" + tableName + ")";
			DBResultSPtr result = execQuery(sql);
			while (result->next())
			{
				columnInfos.push_back(
					std::pair<std::string, std::string>(
						result->getText(1), result->getText(2)
					)
				);
			}

			return columnInfos;
		};



		std::string SQLiteDB::getCreateTableSQL(
			const std::string& tableName,
			const FieldsList& fields
		){
			std::stringstream sql;

			// Init and primary key
			sql << "CREATE TABLE " << tableName << " (\""
				<< fields[0].name << "\" " << getSQLType(fields[0].type)
				<< " UNIQUE PRIMARY KEY ON CONFLICT ROLLBACK";

			// Non geometry columns
			size_t i(1);
			for(; i!=fields.size() && !fields[i].isGeometry(); ++i)
			{
				sql << ", \"";
				sql << fields[i].name;
				sql << "\" " << getSQLType(fields[i].type);
			}
			sql << ");";

			// Geometry columns
			for(; i!=fields.size() && fields[i].isGeometry(); ++i)
			{
				sql << "SELECT AddGeometryColumn('" << tableName << "','" <<
					fields[i].name << "'," << CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() <<
					",'" << getSQLType(fields[i].type) << "',2);" <<
					"SELECT CreateSpatialIndex('" << tableName << "','" <<
					fields[i].name << "');"
				;
			}

			// Safety check
			if(i!=fields.size())
			{
				util::Log::GetInstance().error(
					"Fields are present after geometry column and will be ignored (possible data loss)."
				);
			}

			return sql.str();
		}



		void SQLiteDB::afterUpdateSchema(
			const std::string& tableName,
			const FieldsList& fields
		){
			// Fix of the Spatialite bug
			bool hasGeometryCol(false);
			for(std::size_t i(0); i!=fields.size(); ++i)
			{
				if(fields[i].isGeometry())
				{
					hasGeometryCol = true;
					break;
				}
			}

			if(!hasGeometryCol)
				return;

			std::stringstream sql;

			// Remove Spatialite default trigger & create a custom one which allow REPLACE command
			sql << " BEGIN; DROP TRIGGER IF EXISTS gii_" << tableName << "_geometry;" << std::endl;

			sql << " CREATE TRIGGER gii_" << tableName << "_geometry" << std::endl
				<< " AFTER INSERT ON " << tableName << std::endl
				<< " FOR EACH ROW BEGIN" << std::endl
				<< " DELETE FROM idx_" << tableName << "_geometry WHERE pkid=NEW.ROWID;" << std::endl
				<< " INSERT INTO idx_" << tableName << "_geometry (pkid, xmin, xmax, ymin, ymax)" << std::endl
				<< " VALUES (NEW.ROWID,MbrMinX(NEW.geometry), MbrMaxX(NEW.geometry), MbrMinY(NEW.geometry), MbrMaxY(NEW.geometry));" << std::endl
				<< " END; COMMIT;" << std::endl;
			execUpdate(sql.str());
		}



		bool SQLiteDB::doesIndexExist(const std::string& tableName, const DBTableSync::Index& index)
		{
			std::stringstream sql;
			sql << "SELECT sql FROM SQLITE_MASTER WHERE name='" << _getIndexName(tableName, index)
				<< "' AND type='index'";

			DBResultSPtr res = execQuery(sql.str());

			return res->next() == true;
		}



		void SQLiteDB::createIndex(
			const std::string& tableName,
			const DBTableSync::Index& index,
			const FieldsList& fields
		){
			std::stringstream s;
			s	<< "CREATE INDEX " << _getIndexName(tableName, index)
				<< " ON " << tableName << "(";
			BOOST_FOREACH(const std::string& field, index.fields)
			{
				if (field != index.fields[0])
					s << ",";
				s << field;
			}
			s << ")";

			execUpdate(s.str());
		}



		const std::string SQLiteDB::getSQLDateFormat(const std::string& format, const std::string& expr)
		{
			return "strftime('" + format + "'," + expr + ")";
		}



		const std::string SQLiteDB::getSQLConvertInteger(const std::string& expr)
		{
			return "CAST(" + expr + " AS INTEGER)";
		}



		bool SQLiteDB::isBackend(Backend backend)
		{
			return backend == SQLITE_BACKEND;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Creation of the replace and delete prepared statements.
		/// They are stored in thread specific storage, because they are thread-
		/// specific (use of the SQLite handle associated to the current thread)
		void SQLiteDB::_initPreparedStatements(
		) const {
			if(DBModule::GetTablesById().empty())
			{
				return;
			}

			// Thread specific statements
			PreparedStatements& replaceStatements(_getSQLiteTSS()->replaceStatements);
			PreparedStatements& deleteStatements(_getSQLiteTSS()->deleteStatements);

			// Prepared statements are already initialized
			if(!replaceStatements.empty())
			{
				return;
			}

			// Clear the statements
			size_t tablesNumber(
				DBModule::GetTablesById().rbegin()->first + 1
			);
			replaceStatements.resize(tablesNumber, NULL);
			deleteStatements.resize(tablesNumber, NULL);

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
						query << "GeomFromText(?," << CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() << ")";
					}
					else
					{
						query << "?";
					}
				}
				query << ")";
				string queryStr(query.str());
				sqlite3_stmt* stmt;
				sqlite3_prepare_v2(
					_getHandle(),
					queryStr.c_str(),
					static_cast<int>(queryStr.size()),
					&stmt,
					0
				);
				replaceStatements[it.first] = stmt;

				// Delete statement
				stringstream deleteQuery;
				deleteQuery << "DELETE FROM " << it.second->getFormat().NAME << " WHERE " << TABLE_COL_ID << "=?";
				string deleteQueryStr(deleteQuery.str());
				sqlite3_stmt* deleteStmt;
				sqlite3_prepare_v2(
					_getHandle(),
					deleteQueryStr.c_str(),
					static_cast<int>(deleteQueryStr.size()),
					&deleteStmt,
					0
				);
				deleteStatements[it.first] = deleteStmt;
			}
		}



		void SQLiteDB::saveRecord(
			const DBRecord& record
		){
#ifdef DO_VERIFY_TRIGGER_EVENTS
			// Lock this method so that no database update can start before hooks
			// have finished their execution. The mutex is recursive so that
			// an update can still be called inside hook callback.
			boost::recursive_mutex::scoped_lock lock(_updateMutex);
#endif
			_initSQLiteTSS();

			// Auto increment update
			RegistryKeyType objectId(
				boost::get<RegistryKeyType>(record.getContent().at(0))
			);
			record.getTable()->updateAutoIncrement(objectId);

			size_t fieldsNumber(record.getTable()->getFieldsList().size());
			sqlite3_stmt* stmt(_getReplaceStatement(record.getTable()->getFormat().ID));
			for(size_t i(0); i<fieldsNumber; ++i)
			{
				DBRecordCellBindConvertor visitor(*stmt, i+1);
				boost::apply_visitor(visitor, record.getContent().at(i));
			}

			int retc = sqlite3_step(stmt);

			_ThrowIfError(_getHandle(), retc, "Error executing prepared statement");

			retc = sqlite3_reset(stmt);

			_ThrowIfError(_getHandle(), retc, "Error resetting prepared statement");

#ifdef DO_VERIFY_TRIGGER_EVENTS
			_recordDBModifEvents(tss->events);
#endif
		}


		void SQLiteDB::deleteRow( util::RegistryKeyType id )
		{
			sqlite3_stmt* stmt(_getDeleteStatement(decodeTableId(id)));
			DBRecordCellBindConvertor visitor(*stmt, 1);
			visitor(id);

			int retc = sqlite3_step(stmt);

			_ThrowIfError(_getHandle(), retc, "Error executing prepared statement");

			retc = sqlite3_reset(stmt);

			_ThrowIfError(_getHandle(), retc, "Error resetting prepared statement");
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets the replace prepared statement available for the current thread.
		/// @param the id of the table on which the request replaces elements
		/// @return the replace prepared statement
		sqlite3_stmt* SQLiteDB::_getReplaceStatement( util::RegistryTableType tableId ) const
		{
			_initPreparedStatements();
			return _getSQLiteTSS()->replaceStatements[tableId];
		}



		//////////////////////////////////////////////////////////////////////////
		/// Gets the delete prepared statement available for the current thread.
		/// @param the id of the table on which the request deletes elements
		/// @return the delete prepared statement
		sqlite3_stmt* SQLiteDB::_getDeleteStatement( util::RegistryTableType tableId ) const
		{
			_initPreparedStatements();
			return _getSQLiteTSS()->deleteStatements[tableId];
		}



		SQLiteDB::DBRecordCellBindConvertor::DBRecordCellBindConvertor(
			sqlite3_stmt& stmt,
			size_t i
		):	_stmt(stmt),
			_i(i)
		{}



		void SQLiteDB::DBRecordCellBindConvertor::operator()( const int& i ) const
		{
			sqlite3_bind_int(&_stmt, static_cast<int>(_i), i);
		}



		void SQLiteDB::DBRecordCellBindConvertor::operator()( const bool& i ) const
		{
			sqlite3_bind_int(&_stmt, static_cast<int>(_i), static_cast<int>(i));
		}


		
#ifndef _WIN32
		void SQLiteDB::DBRecordCellBindConvertor::operator()(const size_t& s) const
		{
			sqlite3_bind_int(&_stmt, static_cast<int>(_i), static_cast<int>(s));
		}
#endif


		void SQLiteDB::DBRecordCellBindConvertor::operator()( const double& d ) const
		{
			sqlite3_bind_double(&_stmt, static_cast<int>(_i), d);
		}


		void SQLiteDB::DBRecordCellBindConvertor::operator()(const util::RegistryKeyType& id) const
		{
			sqlite3_bind_int64(&_stmt, static_cast<int>(_i), id);
		}


		void SQLiteDB::DBRecordCellBindConvertor::operator()( const boost::optional<std::string>& str ) const
		{
			if(str)
			{
				sqlite3_bind_text(&_stmt, static_cast<int>(_i), str->c_str(), static_cast<int>(str->size()), NULL);
			}
			else
			{
				sqlite3_bind_null(&_stmt, static_cast<int>(_i));
			}
		}



		void SQLiteDB::DBRecordCellBindConvertor::operator()( const boost::optional<Blob>& blob ) const
		{
			if(blob)
			{
				sqlite3_bind_text(&_stmt, static_cast<int>(_i), blob->first, static_cast<int>(blob->second), NULL);
			}
			else
			{
				sqlite3_bind_null(&_stmt, static_cast<int>(_i));
			}
		}



		void SQLiteDB::DBRecordCellBindConvertor::operator()( const boost::shared_ptr<geos::geom::Geometry>& geom ) const
		{
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
			sqlite3_bind_text(&_stmt, static_cast<int>(_i), persistentStr, static_cast<int>(str.size()), free);
		}



		SQLiteDB::RequestExecutor::RequestExecutor( SQLiteDB& db ):
			_db(db)
		{

		}



		void SQLiteDB::RequestExecutor::operator()( const std::string& sql )
		{
			if(	Log::GetInstance().getLevel() > Log::LEVEL_TRACE &&
				sql.find("spatial_ref_sys") == sql.npos
			){
				Log::GetInstance().trace("SQLiteDB::exec SQL " + sql);
			}
			int retc(
				sqlite3_exec(_db._getHandle(), sql.c_str (), 0, 0, 0)
			);
			_ThrowIfError(_db._getHandle(), retc, "Error executing batch update '" + Conversion::ToTruncatedString(sql) + "'");
		}



		void SQLiteDB::RequestExecutor::operator()( const DBRecord& record )
		{
			_db.saveRecord(record);
		}



		void SQLiteDB::RequestExecutor::operator()( util::RegistryKeyType id )
		{
			_db.deleteRow(id);
		}
}	}
