////////////////////////////////////////////////////////////////////////////////
/// SQLiteTableSyncTemplate class header.
///	@file SQLiteTableSyncTemplate.h
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_SQLiteTableSyncTemplate_H__
#define SYNTHESE_SQLiteTableSyncTemplate_H__

#include "DBModule.h"
#include "SQLite.h"
#include "SQLiteTableSync.h"
#include "SQLiteResult.h"
#include "DBEmptyResultException.h"
#include "SQLiteException.h"
#include "FactorableTemplate.h"
#include "UtilTypes.h"
#include "Log.h"

#include <limits>
#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#undef max

namespace synthese
{
	namespace db
	{
		////////////////////////////////////////////////////////////////////
		/// Table synchronizer template.
		///	@ingroup m10
		template <class K>
		class SQLiteTableSyncTemplate:
			public util::FactorableTemplate<SQLiteTableSync, K>
		{
		public:
			////////////////////////////////////////////////////////////////////
			/// Format of the table
			/// The access to TABLE is public to allow the use of it in SQL 
			/// queries everywhere
			static const SQLiteTableSync::Format TABLE;


			////////////////////////////////////////////////////////////////////
			/// Fields of the table.
			/// To allow to loop on the table without knowing its size,
			/// the last element of the _FIELDS array must be an empty Field
			/// object.
			/// The first field must be the primary key of the table.
			static const SQLiteTableSync::Field _FIELDS[];



			////////////////////////////////////////////////////////////////////
			/// Indexes of the table.
			/// To allow to loop on the table without knowing its size,
			/// the last element of the _INDEXES array must be an empty Index
			/// object.
			static const SQLiteTableSync::Index _INDEXES[];

			
		private:
			////////////////////////////////////////////////////////////////////
			///	Gets the SQL schema of the table.
			///	@return the SQL schema of the table
			///	@author Marc Jambert
			/// @warning this method considers that the first field is always
			/// the primary key of the table. To avoid this behavior, do not
			/// use the SQLiteTableSyncTemplate class.
			static std::string _GetSQLFieldsSchema();



			////////////////////////////////////////////////////////////////////
			/// Creates the SQL statement to create an index in the database given a certain format.
			///	@param index The index to create
			///	@return std::string The SQL statement
			///	@author Hugues Romain
			///	@date 2007
			static std::string _GetSQLIndexSchema(const SQLiteTableSync::Index& index);

			
			
			////////////////////////////////////////////////////////////////////
			///	Creates the SQL schema of the table related triggers.
			///	@return SQL statement
			///	@author Marc Jambert
			static std::string _GetSQLTriggerNoUpdate();



			////////////////////////////////////////////////////////////////////
			///	Tests if the table has a field of the specified name.
			///	@param name Name of the searched field
			///	@return bool true if the table has such a field
			static bool _HasField(const std::string& name);

			
			
			////////////////////////////////////////////////////////////////////
			///	Creates the database wide name of an index.
			///	@param index index
			///	@return name of the index
			///	@author Hugues Romain
			static std::string _GetIndexName(const SQLiteTableSync::Index& index);



			////////////////////////////////////////////////////////////////////
			/// Adapts table in SQLite db to conform to this class table format.
			/// Right now, only new column addition/insertion is supported.
			/// Any other change to table schema is not supported yet.
			/// @param sqlite SQLite access.
			/// @author Marc Jambert
			/// @date 2007
			static void _UpdateSchema(
				SQLite* sqlite
			);



		protected:

			//! @name Table run variables
			//@{
				static int _autoIncrementValue;						//!< Value of the last created object
				static boost::shared_ptr<boost::mutex> _idMutex;	//!< Mutex
			//@}


			//! @name Table run variables handlers
			//@{
				static util::RegistryKeyType encodeUId(
					long objectId
				){
					// TODO : plenty of functions should be at SQLiteTableSync level directly.
					// default value is 1 for compatibility
					static int nodeId = boost::lexical_cast<int>(
						server::ModuleClass::GetParameter("dbring_node_id", "1")
					);
					return util::encodeUId(
						K::TABLE.ID,
						1, // TODO : remove grid id, deprecated with ring nodes
						nodeId,
						objectId
					);
				}



				static void _InitAutoIncrement()
				{
					if (!K::TABLE.HAS_AUTO_INCREMENT)
						return;

					try
					{
						SQLite* sqlite = DBModule::GetSQLite();
						std::stringstream query;
						query
							<< "SELECT " << 0x00000000FFFFFFFFLL
							<< " & " << TABLE_COL_ID << " AS maxid FROM " << K::TABLE.NAME
							<< " WHERE " << TABLE_COL_ID << ">=" << encodeUId(0) << " AND "
							<< TABLE_COL_ID << "<=" << encodeUId(std::numeric_limits<long>::max()) <<
							" ORDER BY " << TABLE_COL_ID << " DESC LIMIT 1"
						;

						SQLiteResultSPtr result (sqlite->execQuery(query.str()));
						if (result->next ())
						{
							util::RegistryKeyType maxid = result->getLongLong("maxid");
							if (maxid > 0) _autoIncrementValue = util::decodeObjectId(maxid) + 1;
						}

					}
					catch (SQLiteException& e)
					{
						util::Log::GetInstance().debug("Table "+ K::TABLE.NAME +" without preceding id.", e);

					}
					catch (...)
					{
						//					Log::GetInstance().debug("Table "+ getTableName() +" without preceding id.", e);
					}
				}
			//@}



			//! @name Data access
			//@{
				/** Gets a result row in the database.
					@param key key of the row to get (corresponds to the id field)
					@return SQLiteResultSPtr The found result row
					@throw DBEmptyResultException if the key was not found in the table
					@author Hugues Romain
					@date 2007				
				*/
				static SQLiteResultSPtr _GetRow(util::RegistryKeyType key )
				{
					SQLite* sqlite = DBModule::GetSQLite();
					std::stringstream query;
					query
						<< "SELECT * "
						<< "FROM " << K::TABLE.NAME
						<< " WHERE " << TABLE_COL_ID << "=" << key
						<< " LIMIT 1";
					SQLiteResultSPtr rows (sqlite->execQuery (query.str()));
					if (rows->next() == false) throw DBEmptyResultException<K>(key);
					return rows;
				}
			//@}


			//! @name Virtual access to instantiated static methods
			//@{
				virtual void initAutoIncrement() const { _InitAutoIncrement(); }
				virtual void firstSync(SQLite* sqlite, SQLiteSync* sync) const { _FirstSync(sqlite, sync); }
				virtual void updateSchema(SQLite* sqlite) const { _UpdateSchema(sqlite); }
				virtual const SQLiteTableSync::Format& getFormat() const { return K::TABLE; }
			//@}



			/** Utility method to get a row by id.
			This uses a precompiled statement for performance 
			*/
			virtual SQLiteResultSPtr getRowById(SQLite* sqlite, util::RegistryKeyType id) const
			{
				std::stringstream ss;
				ss << "SELECT * FROM " << K::TABLE.NAME << " WHERE ROWID=" << id << " LIMIT 1";

				return sqlite->execQuery (ss.str (), false); 
			}

			
			////////////////////////////////////////////////////////////////////
			///	Launch all "data insertion" handlers (loads all the data).
			///	@param sqlite
			///	@param sync
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			static void _FirstSync(
				SQLite* sqlite, 
				SQLiteSync* sync
			){
				// Callbacks according to what already exists in the table.
				if (!K::TABLE.IGNORE_CALLBACKS_ON_FIRST_SYNC)
				{
					std::stringstream ss;
					ss << "SELECT *  FROM " << K::TABLE.NAME;
					SQLiteResultSPtr result = sqlite->execQuery (ss.str (), true);
					K().rowsAdded (sqlite, sync, result);
				}
			}

		public:
			/** Unique ID generator for autoincremented tables.
			*/
			static util::RegistryKeyType getId()
			{			
				boost::mutex::scoped_lock mutex(*_idMutex);

				//	if (it == _autoIncrementValues.end())
				//		throw Exception("Autoincrement not initialized for table "+ getTableName());

				//	int retval = it->second++;
				int retval = _autoIncrementValue++;

				return encodeUId(retval);
			}



			static std::string GetFieldValue(
				util::RegistryKeyType id,
				const std::string& field
			){
				std::stringstream query;
				query
					<< "SELECT " << field << " FROM " << K::TABLE.NAME
					<< " WHERE " << TABLE_COL_ID << "=" << id;
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				if (!rows->next())
					throw DBEmptyResultException<K>(id);
				else
					return rows->getText(field);
			}


			static void Remove(util::RegistryKeyType key)
			{
				SQLite* sqlite = DBModule::GetSQLite();
				std::stringstream query;
				query
					<< "DELETE FROM " << K::TABLE.NAME
					<< " WHERE " << TABLE_COL_ID << "=" << key;
				sqlite->execUpdate(query.str());
			}
		};



// IMPLEMENTATIONS ==============================================================================


		template <class K>
		std::string synthese::db::SQLiteTableSyncTemplate<K>::_GetIndexName(
			const SQLiteTableSync::Index& index
		){
			std::stringstream s;
			s << TABLE.NAME;
			BOOST_FOREACH(const std::string& field, index.fields)
			{
				s << "_" << field;
			}
			return s.str();
		}




		template <class K>
		std::string synthese::db::SQLiteTableSyncTemplate<K>::_GetSQLIndexSchema(
			const SQLiteTableSync::Index& index
		){
			// Creation of the statement
			std::stringstream s;
			s	<< "CREATE INDEX " << _GetIndexName(index)
				<< " ON " << TABLE.NAME << "(";
			BOOST_FOREACH(const std::string& field, index.fields)
			{
				if (field != index.fields[0])
					s << ",";
				s << field;
			}
			s << ")";

			return s.str();
		}



		template <class K>
		bool synthese::db::SQLiteTableSyncTemplate<K>::_HasField(
			const std::string& name
		){
			for(size_t i(0); !_FIELDS[i].empty(); ++i)
			{
				if(_FIELDS[i].name == name) return true;
			}
			return false;
		}



		template <class K>
		std::string synthese::db::SQLiteTableSyncTemplate<K>::_GetSQLTriggerNoUpdate(
		) {
			std::vector<std::string> nonUpdatableColumns;
			for(size_t i(0); !_FIELDS[i].empty(); ++i)
			{
				if(_FIELDS[i].updatable) continue;

				nonUpdatableColumns.push_back(_FIELDS[i].name);
			}

			// If no non updatable field, return empty trigger
			if (nonUpdatableColumns.empty()) return std::string();

			std::stringstream columnList;
			BOOST_FOREACH(const std::string& field, nonUpdatableColumns)
			{
				if (!columnList.str().empty())
					columnList << ", ";
				columnList << field;
			}

			std::stringstream sql;
			sql << "CREATE TRIGGER "
				<< TABLE.NAME << "_no_update"
				<< " BEFORE UPDATE OF "
				<< columnList.str() << " ON " << TABLE.NAME
				<< " BEGIN SELECT RAISE (ABORT, 'Update of "
				<< columnList.str() << " in " << TABLE.NAME << " is forbidden.') WHERE "
				<< TABLE.TRIGGER_OVERRIDE_CLAUSE << "; END";

			return sql.str();
		}



		template <class K>
		void synthese::db::SQLiteTableSyncTemplate<K>::_UpdateSchema(
			SQLite* sqlite
		) {
			std::string tableSchema = _GetSQLFieldsSchema();
			std::string triggerNoUpdate = _GetSQLTriggerNoUpdate();

			// Check if the table already exists
			std::string sql = "SELECT * FROM SQLITE_MASTER WHERE name='" + TABLE.NAME + "' AND type='table'";
			SQLiteResultSPtr res = sqlite->execQuery (sql);
			if (res->next () == false)
			{
				// Create the table if it does not already exist.
				sqlite->execUpdate (tableSchema);

				// Insert some triggers to prevent non allowed operations
				if (!triggerNoUpdate.empty()) sqlite->execUpdate (triggerNoUpdate);
			}
			else if(
				(SQLiteTableSync::GetSQLSchemaDb(sqlite, TABLE.NAME) != tableSchema) ||
				SQLiteTableSync::GetTriggerNoUpdateDb(sqlite, TABLE.NAME) != triggerNoUpdate
			){
				std::vector<std::string> dbCols = SQLiteTableSync::GetTableColumnsDb(sqlite, TABLE.NAME);

				// Filter columns that are not in new table format
				std::stringstream colsStr;
				std::stringstream filteredColsStr;

				BOOST_FOREACH(const std::string& dbCol, dbCols)
				{
					if (!colsStr.str().empty()) colsStr << ",";
					colsStr << dbCol;

					if (_HasField(dbCol))
					{
						if (!filteredColsStr.str().empty()) filteredColsStr << ",";
						filteredColsStr << dbCol;
					}
				}

				std::string buTableName = TABLE.NAME + "_backup";
				std::stringstream str;
				str << "BEGIN TRANSACTION; ";

				// Drop triggers
				if (!SQLiteTableSync::GetTriggerNoUpdateDb(sqlite, TABLE.NAME).empty())
				{
					str << "DROP TRIGGER " << K::TABLE.NAME << "_no_update ;";
				}

				// Convert table schema (through temporary table)
				str << "CREATE TEMPORARY TABLE " << buTableName << " (" 
					<< colsStr.str () << "); ";
				str << "INSERT INTO " << buTableName 
					<< " SELECT " << colsStr.str () << " FROM " << TABLE.NAME << "; ";
				str << "DROP TABLE " << TABLE.NAME << "; ";
				str << tableSchema << "; ";
				str << "INSERT INTO " << TABLE.NAME << " (" << filteredColsStr.str () << ")"
					<< " SELECT " << filteredColsStr.str () << " FROM " << buTableName << "; ";
				str << "DROP TABLE " << buTableName << "; ";

				// Redefine triggers
				if (triggerNoUpdate != "") str << triggerNoUpdate << " ;";

				str << "COMMIT;";

				sqlite->execUpdate (str.str ());
			}

			// Indexes
			for(size_t i(0); !_INDEXES[i].empty(); ++i)
			{
				// Search if the index exists
				std::stringstream sql;
				sql << "SELECT sql FROM SQLITE_MASTER WHERE name='" << _GetIndexName(_INDEXES[i])
					<< "' AND type='index'";

				SQLiteResultSPtr res = sqlite->execQuery (sql.str());

				// The index already exists
				if (res->next ())
				{
					// The index already exists and is identical : do nothing
					if (res->getText("sql") == _GetSQLIndexSchema(_INDEXES[i])) continue;

					// Drop the old index
					std::stringstream drop;
					drop << "DROP INDEX " << _GetIndexName(_INDEXES[i]);
					sqlite->execUpdate(drop.str());
				}

				// Creation of the index
				sqlite->execUpdate(_GetSQLIndexSchema(_INDEXES[i]));
			}
		}



		template <class K>
		std::string synthese::db::SQLiteTableSyncTemplate<K>::_GetSQLFieldsSchema(
		){
			std::stringstream sql;
			sql << "CREATE TABLE " << TABLE.NAME << " ("
				<< _FIELDS[0].name << " " << _FIELDS[0].getSQLType()
				<< " UNIQUE PRIMARY KEY ON CONFLICT ROLLBACK";

			for(size_t i(1); !_FIELDS[i].empty(); ++i)
			{
				sql << ", " << _FIELDS[i].name << " " << _FIELDS[i].getSQLType();
			}
			sql << ")";
			return sql.str();
		}


		template <class K>
			boost::shared_ptr<boost::mutex> SQLiteTableSyncTemplate<K>::_idMutex(new boost::mutex); 

		template <class K>
			int SQLiteTableSyncTemplate<K>::_autoIncrementValue(1); 
	}
}

#endif // SYNTHESE_SQLiteTableSyncTemplate_H__
