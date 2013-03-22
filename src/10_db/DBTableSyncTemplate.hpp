////////////////////////////////////////////////////////////////////////////////
/// DBTableSyncTemplate class header.
///	@file DBTableSyncTemplate.hpp
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#ifndef SYNTHESE_db_DBTableSyncTemplate_hpp__
#define SYNTHESE_db_DBTableSyncTemplate_hpp__

#include "DB.hpp"
#include "DBModule.h"
#include "DBTableSync.hpp"
#include "DBResult.hpp"
#include "DBEmptyResultException.h"
#include "DBException.hpp"
#include "DeleteQuery.hpp"
#include "FactorableTemplate.h"
#include "UtilTypes.h"
#include "Log.h"
#include "CoordinatesSystem.hpp"

#include <sstream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace db
	{
		////////////////////////////////////////////////////////////////////
		/// Table synchronizer template.
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m10
		/// @warning Geometry columns must always be at the end. If non geometry
		/// columns are present after any geometry column, they will be ignored.
		template <class K>
		class DBTableSyncTemplate:
			public util::FactorableTemplate<DBTableSync, K>
		{
		public:
			////////////////////////////////////////////////////////////////////
			/// Format of the table
			/// The access to TABLE is public to allow the use of it in SQL
			/// queries everywhere
			static const DBTableSync::Format TABLE;



			////////////////////////////////////////////////////////////////////
			/// Fields of the table.
			/// To allow to loop on the table without knowing its size,
			/// the last element of the _FIELDS array must be an empty Field
			/// object.
			/// The first field must be the primary key of the table.
			static const Field _FIELDS[];



			////////////////////////////////////////////////////////////////////
			/// Indexes of the table.
			/// To allow to loop on the table without knowing its size,
			/// the last element of the _INDEXES array must be an empty Index
			/// object.
			static DBTableSync::Indexes GetIndexes();



		private:
			//////////////////////////////////////////////////////////////////////////
			/// Replacement of * fields getter to ensure conversion of geometry
			/// columns into WKT format
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static std::string _fieldsGetter;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Public getter of the last attribute.
			/// @author Hugues Romain
			/// @since 3.3.0
			/// @date 2011
			static const std::string GetFieldsGetter() { return _fieldsGetter; }


		private:

			////////////////////////////////////////////////////////////////////
			///	Tests if the table has a field of the specified name.
			///	@param name Name of the searched field
			///	@return bool true if the table has such a field
			static bool _HasField(const std::string& name);



			////////////////////////////////////////////////////////////////////
			/// Adapts the corresponding database table schema to match this class table format.
			/// Right now, only new column addition/insertion is supported.
			/// Any other change to table schema is not supported yet.
			/// @param db Database access.
			/// @author Marc Jambert
			/// @date 2007
			static void _UpdateSchema(
				DB* db
			);



			////////////////////////////////////////////////////////////////////
			/// Adapts the corresponding database table schema to match this class table format.
			/// Right now, only new column addition/insertion is supported.
			/// Any other change to table schema is not supported yet.
			/// @param db Database access.
			/// @author Marc Jambert
			/// @date 2007
			static void _MigrateTableData(
				DB* db
			);

			static bool CanDelete(
				const server::Session* session,
				util::RegistryKeyType object_id
			);

			static void BeforeDelete(
				util::RegistryKeyType id,
				DBTransaction&
			);

			static void AfterDelete(
				util::RegistryKeyType id,
				DBTransaction& transaction
			);

			static void LogRemoval(
				const server::Session* session,
				util::RegistryKeyType id
			);

			////////////////////////////////////////////////////////////////////
			/// Gets a result row in the database.
			/// @param key key of the row to get (corresponds to the id field)
			/// @return DBResultSPtr The found result row
			static DBResultSPtr _GetRowInternal(util::RegistryKeyType key)
			{
				std::stringstream query;
				query <<
					"SELECT " << _fieldsGetter <<
					" FROM " << K::TABLE.NAME <<
					" WHERE " << TABLE_COL_ID << "=" << key <<
					" LIMIT 1";
				return DBModule::GetDB()->execQuery(query.str());
			}



		protected:

			//! @name Table run variables
			//@{
				static util::RegistryObjectType _autoIncrementValue;						//!< Value of the last created object
				static boost::shared_ptr<boost::mutex> _idMutex;	//!< Mutex
			//@}


			//! @name Table run variables handlers
			//@{
				static util::RegistryKeyType encodeUId(
					util::RegistryObjectType objectId
				){
					return util::encodeUId(
						K::TABLE.ID,
						DBModule::GetNodeId(),
						objectId
					);
				}



				////////////////////////////////////////////////////////////////////
				///	Auto increment updater.
				/// The auto increment is updated if :
				///  - the id has been generated by the current node
				///  - the id is greater than the max existing id 
				/// @param id the id to check
				virtual void updateAutoIncrement(
					util::RegistryKeyType id
				) const {
					_UpdateAutoIncrement(id);
				}



				static void _UpdateAutoIncrement(
					util::RegistryKeyType id
				){
					util::RegistryObjectType localId(util::decodeObjectId(id));

					boost::mutex::scoped_lock mutex(*_idMutex);

					if(	!K::TABLE.HAS_AUTO_INCREMENT ||
						util::decodeGridNodeId(id) != DBModule::GetNodeId() ||
						localId < _autoIncrementValue
					){
						return;
					}

					_autoIncrementValue = localId + 1;
				}



				static void _InitAutoIncrement()
				{
					if (!K::TABLE.HAS_AUTO_INCREMENT)
						return;

					try
					{
						DB* db = DBModule::GetDB();
						std::stringstream query;
						query <<
							"SELECT " << 0x00000000FFFFFFFFLL <<
							" & " << TABLE_COL_ID << " AS maxid FROM " << K::TABLE.NAME <<
							" WHERE " << TABLE_COL_ID << ">=" << encodeUId(0) << " AND " <<
							TABLE_COL_ID << "<=" << encodeUId(0xFFFFFFFF) <<
							" ORDER BY " << TABLE_COL_ID << " DESC LIMIT 1"
						;

						DBResultSPtr result (db->execQuery(query.str()));
						if (result->next ())
						{
							util::RegistryKeyType maxid = result->getLongLong("maxid");
							if (maxid > 0)
							{
								_autoIncrementValue = util::decodeObjectId(maxid) + 1;
								util::Log::GetInstance().debug("Auto-increment of table "+ K::TABLE.NAME +" initialized at "+ boost::lexical_cast<std::string>(_autoIncrementValue));
							}
						}

					}
					catch (DBException& e)
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
					@return DBResultSPtr The found result row
					@throw DBEmptyResultException if the key was not found in the table
					@author Hugues Romain
					@date 2007
				*/
				static DBResultSPtr _GetRow(util::RegistryKeyType key)
				{
					DBResultSPtr rows = _GetRowInternal(key);
					if (rows->next() == false) throw DBEmptyResultException<K>(key);
					return rows;
				}
			//@}



			//! @name Virtual access to instantiated static methods
			//@{
				virtual void initAutoIncrement() const { _InitAutoIncrement(); }
				virtual void firstSync(DB* db) const { _FirstSync(db); }
				virtual void updateSchema(DB* db) const { _UpdateSchema(db); }
				virtual const DBTableSync::Format& getFormat() const { return K::TABLE; }

				virtual bool canDelete(
					const server::Session* session,
					util::RegistryKeyType object_id
				) const {
					return K::CanDelete(session, object_id);
				}

				virtual void deleteRecord(
					const server::Session* session,
					util::RegistryKeyType id,
					DBTransaction& transaction,
					bool log = true
				) const {
					DBTableSyncTemplate<K>::Remove(session, id, transaction, log);
				}
			//@}



			/** Utility method to get a row by id.
			*/
			virtual DBResultSPtr getRow(util::RegistryKeyType key) const
			{
				return _GetRowInternal(key);
			}



			////////////////////////////////////////////////////////////////////
			///	Launch all "data insertion" handlers (loads all the data).
			///	@param db
			///	@param sync
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			static void _FirstSync(
				DB* db
			){
				// Callbacks according to what already exists in the table.
				if (!K::TABLE.IGNORE_CALLBACKS_ON_FIRST_SYNC)
				{
					std::stringstream ss;
					ss << "SELECT " << _fieldsGetter << " FROM " << K::TABLE.NAME;
					DBResultSPtr result = db->execQuery (ss.str ());
					K().rowsAdded (db, result);
				}
			}

		public:
			/** Unique ID generator for autoincremented tables.
			*/
			// TODO: rename to GetId() because it's static.
			static util::RegistryKeyType getId()
			{
				boost::mutex::scoped_lock mutex(*_idMutex);

				util::RegistryObjectType retval = _autoIncrementValue++;

				return encodeUId(retval);
			}

			virtual util::RegistryKeyType getNewId() const
			{
				return getId();
			}



			static std::string GetFieldValue(
				util::RegistryKeyType id,
				const std::string& field
			){
				std::stringstream query;
				query <<
					"SELECT " << field << " FROM " << K::TABLE.NAME <<
					" WHERE " << TABLE_COL_ID << "=" << id;
				DBResultSPtr rows = DBModule::GetDB()->execQuery(query.str());
				if (!rows->next())
					throw DBEmptyResultException<K>(id);
				else
					return rows->getText(field);
			}



			static void Remove(
				const server::Session* session,
				util::RegistryKeyType id,
				DBTransaction& transaction,
				bool log = true
			){
				if(log)
				{
					DBTableSyncTemplate<K>::LogRemoval(session, id);
				}
				DBTableSyncTemplate<K>::BeforeDelete(id, transaction);
				DBModule::GetDB()->deleteStmt(id, transaction);
				DBTableSyncTemplate<K>::AfterDelete(id, transaction);
			}
		};




// IMPLEMENTATIONS ==============================================================================


		template<class K>
		std::string DBTableSyncTemplate<K>::_fieldsGetter;



		template <class K>
		bool DBTableSyncTemplate<K>::_HasField(
			const std::string& name
		){
			FieldsList l(K::GetFieldsList());
			BOOST_FOREACH(const FieldsList::value_type& f, l)
			{
				if(f.name == name)
				{
					return true;
			}	}
			return false;
		}



		template <class K>
		void synthese::db::DBTableSyncTemplate<K>::_MigrateTableData(
			DB* db
		){
			std::vector<DB::ColumnInfo> columnInfos = db->getTableColumns(TABLE.NAME);

			// Filter columns that are not in new table format
			std::stringstream colsStr;
			std::stringstream colsWithTypeStr;
			std::stringstream filteredColsStr;

			BOOST_FOREACH(const DB::ColumnInfo& columnInfo, columnInfos)
			{
				if (!colsStr.str().empty()) colsStr << ",";
				colsStr << "\"" << columnInfo.first << "\"";
				if (!colsWithTypeStr.str().empty()) colsWithTypeStr << ",";
				colsWithTypeStr << columnInfo.first << " " << columnInfo.second;

				if (_HasField(columnInfo.first))
				{
					if (!filteredColsStr.str().empty()) filteredColsStr << ",";
					filteredColsStr << columnInfo.first;
				}
			}

			std::string buTableName = TABLE.NAME + "_backup";
			std::stringstream str;
			str << "BEGIN; ";

			// Backup of old data in a temporary table
			str <<
				"CREATE TEMPORARY TABLE \"" << buTableName << "\" (" <<
				colsWithTypeStr.str () << "); " <<
				"INSERT INTO " << buTableName  <<
				" SELECT " << colsStr.str () << " FROM " << TABLE.NAME << "; " <<

				// Deletion of the table with the old schema
				"DROP TABLE " << TABLE.NAME << "; " <<

				// Creation of the table with the new schema
				db->getCreateTableSQL(TABLE.NAME, K::GetFieldsList()) <<

				// Restoration of the data in the table
				"INSERT INTO " << TABLE.NAME << " (" << filteredColsStr.str () << ")" <<
				" SELECT " << filteredColsStr.str () << " FROM " << buTableName << "; " <<

				// Deletion of the temporary table
				"DROP TABLE " << buTableName << "; "
			;

			str << "COMMIT;";

			db->execUpdate(str.str());
		}



		template <class K>
		void synthese::db::DBTableSyncTemplate<K>::_UpdateSchema(
			DB* db
		){
			FieldsList fieldsList(K::GetFieldsList());

			// reset statistics
			TABLE.CreatedTable = false;
			TABLE.MigratedSchema = false;
			TABLE.CreatedIndexes = 0;

			// * Fields getter
			std::stringstream fieldsGetter;
			bool first(true);
			BOOST_FOREACH(const FieldsList::value_type& field, fieldsList)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					fieldsGetter << ",";
				}
				if(field.isGeometry())
				{
					fieldsGetter << "AsText(" << TABLE.NAME << "." << field.name << ") AS " << field.name;
				}
				else
				{
					fieldsGetter << TABLE.NAME << "." << field.name;
				}
			}
			_fieldsGetter = fieldsGetter.str();


			// Check if the table already exists
			if (!db->doesTableExists(TABLE.NAME))
			{
				// Create the table if it does not already exist.
				db->execUpdate(db->getCreateTableSQL(TABLE.NAME, fieldsList));
				TABLE.CreatedTable = true;
			}
			else if (!db->isTableSchemaUpToDate(TABLE.NAME, fieldsList))
			{
				_MigrateTableData(db);
				TABLE.MigratedSchema = true;
			}


			// Indexes
			DBTableSync::Indexes indexes(GetIndexes());
			BOOST_FOREACH(const DBTableSync::Indexes::value_type& index, indexes)
			{
				if (db->doesIndexExist(TABLE.NAME, index))
				{
					// We assume that if the index exists, it is correct. That should be a rather safe bet if
					// backends use an index named from the list of its columns (which all backends do now).
					continue;
				}
				db->createIndex(TABLE.NAME, index, fieldsList);
				TABLE.CreatedIndexes++;
			}

			db->afterUpdateSchema(TABLE.NAME, fieldsList);
		}



		template <class K>
		boost::shared_ptr<boost::mutex> DBTableSyncTemplate<K>::_idMutex(new boost::mutex);

		template <class K>
		util::RegistryObjectType DBTableSyncTemplate<K>::_autoIncrementValue(1);
	}
}

#endif // SYNTHESE_db_DBTableSyncTemplate_hpp__
