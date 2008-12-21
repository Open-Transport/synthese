////////////////////////////////////////////////////////////////////////////////
/// SQLiteTableSyncTemplate class header.
///	@file SQLiteTableSyncTemplate.h
///	@author Hugues Romain
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
#include "DbModuleClass.h"
#include "SQLite.h"
#include "SQLiteTableSync.h"
#include "SQLiteResult.h"
#include "DBEmptyResultException.h"
#include "SQLiteException.h"
#include "FactorableTemplate.h"

#include "01_util/Constants.h"
#include "Conversion.h"
#include "Log.h"

#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace db
	{
		/** Table synchronizer template.
			@ingroup m10
		*/
		template <class K>
		class SQLiteTableSyncTemplate : public util::FactorableTemplate<SQLiteTableSync, K>
		{
		public:
			typedef DBEmptyResultException<K>	DBEmptyResultException;

			static const SQLiteTableFormat	TABLE;

		protected:

			//! @name Table run variables
			//@{
				static int _autoIncrementValue;						//!< Value of the last created object
				static boost::shared_ptr<boost::mutex> _idMutex;	//!< Mutex
			//@}


			//! @name Table run variables handlers
			//@{
				static uid encodeUId (long objectId)
				{
					// TODO : plenty of functions should be at SQLiteTableSync level directly.
					// default value is 1 for compatibility
					static int nodeId = util::Conversion::ToInt (DbModuleClass::GetParameter ("dbring_node_id", "1"));
					return util::encodeUId (TABLE.ID, 
						1, // TODO : remove grid id, deprecated with ring nodes
						nodeId, 
						objectId);
				}


				static void _InitAutoIncrement()
				{
					if (!TABLE.HAS_AUTO_INCREMENT)
						return;

					try
					{
						SQLite* sqlite = DBModule::GetSQLite();
						std::stringstream query;
						query
							<< "SELECT " << util::Conversion::ToString((uid) 0x00000000FFFFFFFFLL) 
							<< " & MAX(id) AS maxid FROM " << TABLE.NAME
							<< " WHERE (id & " << util::Conversion::ToString((uid) 0xFFFFFFFF00000000LL) << ") = " 
							<< util::Conversion::ToString(encodeUId(0)); 

						/// @todo GRID and NODEGRID to be replaced by the correct values


						SQLiteResultSPtr result (sqlite->execQuery(query.str()));
						if (result->next ())
						{
							uid maxid = result->getLongLong ("maxid");
							if (maxid > 0) _autoIncrementValue = util::decodeObjectId(maxid) + 1;
						}

					}
					catch (SQLiteException& e)
					{
						util::Log::GetInstance().debug("Table "+ TABLE.NAME +" without preceding id.", e);

					}
					catch (...)
					{
						//					Log::GetInstance().debug("Table "+ getTableName() +" without preceding id.", e);
					}
				}
			//@}


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



			//! @name Data access
			//@{
				/** Gets a result row in the database.
					@param key key of the row to get (corresponds to the id field)
					@return SQLiteResultSPtr The found result row
					@throw DBEmptyResultException if the key was not found in the table
					@author Hugues Romain
					@date 2007				
				*/
				static SQLiteResultSPtr _GetRow( uid key )
				{
					SQLite* sqlite = DBModule::GetSQLite();
					std::stringstream query;
					query
						<< "SELECT * "
						<< "FROM " << TABLE.NAME
						<< " WHERE " << TABLE_COL_ID << "=" << util::Conversion::ToString(key)
						<< " LIMIT 1";
					SQLiteResultSPtr rows (sqlite->execQuery (query.str()));
					if (rows->next() == false)
						throw DBEmptyResultException(key);
					return rows;
				}
			//@}

			SQLiteTableSyncTemplate(
			):	util::FactorableTemplate<SQLiteTableSync, K>()
			{}

			//! @name Virtual access to instantiated static methods
			//@{
				virtual void initAutoIncrement() const { _InitAutoIncrement(); }
				virtual void firstSync(SQLite* sqlite, SQLiteSync* sync) const { _FirstSync(sqlite, sync); }
				virtual void updateSchema(SQLite* sqlite) const { _UpdateSchema(sqlite); }
				virtual const SQLiteTableFormat& getFormat() const { return TABLE; }
				//@}

		protected:
			

			/** Utility method to get a row by id.
			This uses a precompiled statement for performance 
			*/
			virtual SQLiteResultSPtr getRowById(SQLite* sqlite, util::RegistryKeyType id) const
			{
				std::stringstream ss;
				ss << "SELECT * FROM " << TABLE.NAME << " WHERE ROWID=" << id << " LIMIT 1";

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
				if (!TABLE.IGNORE_CALLBACKS_ON_FIRST_SYNC)
				{
					std::stringstream ss;
					ss << "SELECT *  FROM " << TABLE.NAME;
					SQLiteResultSPtr result = sqlite->execQuery (ss.str ());
					K().rowsAdded (sqlite, sync, result, true);
				}
			}


			/** Adapts table in SQLite db to conform to this class 
			* table format.
			* Right now, only new column addition/insertion is supported.
			* Any other change to table schema is not supported yet.
			*/
			static void _UpdateSchema(
				SQLite* sqlite
			){
				std::string tableSchema = TABLE.getSQLFieldsSchema();
				std::string triggerNoUpdate = TABLE.getSQLTriggerNoUpdate();


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
				else if ((GetSQLSchemaDb(sqlite, TABLE.NAME) != tableSchema) ||
						GetTriggerNoUpdateDb(sqlite, TABLE.NAME) == triggerNoUpdate
				){
					std::vector<std::string> dbCols = SQLiteTableSync::GetTableColumnsDb(sqlite, TABLE.NAME);

					// Filter columns that are not in new table format
					std::stringstream colsStr;
					std::stringstream filteredColsStr;
					
					BOOST_FOREACH(const std::string& dbCol, dbCols)
					{
						if (!colsStr.str().empty()) colsStr << ",";
						colsStr << dbCol;

						if (TABLE.hasTableColumn(dbCol))
						{
							if (!filteredColsStr.str().empty()) filteredColsStr << ",";
							filteredColsStr << dbCol;
						}
					}

					std::string buTableName = TABLE.NAME + "_backup";
					std::stringstream str;
					str << "BEGIN TRANSACTION; ";

					// Drop triggers
					if (!GetTriggerNoUpdateDb(sqlite, TABLE.NAME).empty()) str << "DROP TRIGGER " << TABLE.NAME << "_no_update ;";

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
				BOOST_FOREACH(const SQLiteTableFormat::Index& index, TABLE.INDEXES)
				{
					// Search if the index exists
					std::stringstream sql;
					sql << "SELECT sql FROM SQLITE_MASTER WHERE name='" << TABLE.getIndexDBName(index)
						<< "' AND type='index'";

					SQLiteResultSPtr res = sqlite->execQuery (sql.str());

					// The index already exists
					if (res->next ())
					{
						// The index already exists and is identical : do nothing
						if (res->getText("sql") == TABLE.getSQLIndexSchema(index)) continue;

						// Drop the old index
						std::stringstream drop;
						drop << "DROP INDEX " << TABLE.getIndexDBName(index);
						sqlite->execUpdate(drop.str());
					}

					// Creation of the index
					sqlite->execUpdate(TABLE.getSQLIndexSchema(index));
				}
			}

		public:

			static std::string GetFieldValue(
				util::RegistryKeyType id,
				const std::string& field
			){
				std::stringstream query;
				query
					<< "SELECT " << field << " FROM " << TABLE.NAME
					<< " WHERE " << TABLE_COL_ID << "=" << util::Conversion::ToString(id);
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				if (!rows->next())
					throw DBEmptyResultException(id);
				else
					return rows->getText(field);
			}


			static void Remove(util::RegistryKeyType key)
			{
				SQLite* sqlite = DBModule::GetSQLite();
				std::stringstream query;
				query
					<< "DELETE FROM " << TABLE.NAME
					<< " WHERE " << TABLE_COL_ID << "=" << key;
				sqlite->execUpdate(query.str());
			}


		};


		template <class K>
			boost::shared_ptr<boost::mutex> SQLiteTableSyncTemplate<K>::_idMutex(new boost::mutex); 

		template <class K>
			int SQLiteTableSyncTemplate<K>::_autoIncrementValue(1); 
	}
}

#endif // SYNTHESE_SQLiteTableSyncTemplate_H__
