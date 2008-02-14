
/** SQLiteTableSyncTemplate class header.
	@file SQLiteTableSyncTemplate.h

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

#ifndef SYNTHESE_SQLiteTableSyncTemplate_H__
#define SYNTHESE_SQLiteTableSyncTemplate_H__


#include "02_db/DBModule.h"
#include "02_db/DbModuleClass.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/DBEmptyResultException.h"
#include "02_db/SQLiteException.h"

#include "01_util/FactorableTemplate.h"

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "01_util/UId.h"
#include "01_util/Constants.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"


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

			static const std::string	TABLE_NAME;		//!< Table name in the database
			static const int			TABLE_ID;		//!< Table ID used by util::encodeID and util::decodeTableId

		protected:
			static const bool HAS_AUTO_INCREMENT;
			static int _autoIncrementValue;

			static boost::shared_ptr<boost::mutex> _idMutex; 

			static uid encodeUId (long objectId);

			/** Unique ID getter for autoincremented tables.
			*/
			static uid getId();

			SQLiteTableSyncTemplate()	
				: util::FactorableTemplate<SQLiteTableSync, K>() 
			{}

			SQLiteTableSyncTemplate(const SQLiteTableSync::Args& args)
				: util::FactorableTemplate<SQLiteTableSync, K>(args)
			{}

			void initAutoIncrement();

		public:

			virtual const std::string& getTableName() const { return TABLE_NAME; }
			virtual std::string getTriggerOverrideClause() const { return TRIGGERS_ENABLED_CLAUSE; }


			static std::string GetFieldValue(uid id, const std::string& field);

		protected:
			
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
					<< "FROM " << TABLE_NAME
					<< " WHERE " << TABLE_COL_ID << "=" << util::Conversion::ToString(key)
					<< " LIMIT 1";
				SQLiteResultSPtr rows (sqlite->execQuery (query.str()));
				if (rows->next() == false)
					throw DBEmptyResultException(key);
				return rows;
			}


	
		public:

			static void Remove(uid key);

		};

		template <class K>
		void synthese::db::SQLiteTableSyncTemplate<K>::Remove( uid key )
		{
			SQLite* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "DELETE FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << util::Conversion::ToString(key);
			sqlite->execUpdate(query.str());
		}


		template <class K>
		std::string SQLiteTableSyncTemplate<K>::GetFieldValue(uid id, const std::string& field)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "SELECT " << field << " FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << util::Conversion::ToString(id);
			SQLiteResultSPtr rows = sqlite->execQuery(query.str());
			if (!rows->next())
				throw DBEmptyResultException(id);
			else
				return rows->getText(field);
		}


		template <class K>
			boost::shared_ptr<boost::mutex> SQLiteTableSyncTemplate<K>::_idMutex(new boost::mutex); 

		template <class K>
			int SQLiteTableSyncTemplate<K>::_autoIncrementValue(1); 

		template <class K>
			uid SQLiteTableSyncTemplate<K>::getId()
		{			
			boost::mutex::scoped_lock mutex(*_idMutex);

			//	if (it == _autoIncrementValues.end())
			//		throw Exception("Autoincrement not initialized for table "+ getTableName());

			//	int retval = it->second++;
			int retval = _autoIncrementValue++;

			return encodeUId(retval);
		}


		template <class K>
			uid SQLiteTableSyncTemplate<K>::encodeUId (long objectId)
		{
		    // TODO : plenty of functions should be at SQLiteTableSync level directly.
		    // default value is 1 for compatibility
			static int nodeId = util::Conversion::ToInt (DbModuleClass::GetParameter ("dbring_node_id", "1"));
			return util::encodeUId (TABLE_ID, 
							  1, // TODO : remove grid id, deprecated with ring nodes
							  nodeId, 
							  objectId);
		}



		template <class K>
			void SQLiteTableSyncTemplate<K>::initAutoIncrement()
		{
			if (HAS_AUTO_INCREMENT)
			{
				try
				{
					SQLite* sqlite = DBModule::GetSQLite();
					std::stringstream query;
					query
					    << "SELECT " << util::Conversion::ToString((uid) 0x00000000FFFFFFFFLL) 
					    << " & MAX(id) AS maxid FROM " << TABLE_NAME
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
					util::Log::GetInstance().debug("Table "+ getTableName() +" without preceding id.", e);

				}
				catch (...)
				{
//					Log::GetInstance().debug("Table "+ getTableName() +" without preceding id.", e);
				}
			}
		}

	}
}

#endif // SYNTHESE_SQLiteTableSyncTemplate_H__

