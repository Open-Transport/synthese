
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

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "01_util/UId.h"
#include "01_util/Constants.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/DBEmptyResultException.h"
#include "02_db/SQLiteException.h"
#include "02_db/SQLiteTableSync.h"

namespace synthese
{
	using namespace util;

	namespace db
	{
		/** Table synchronizer template.
			@ingroup m02
		*/
		template <class T>
		class SQLiteTableSyncTemplate : public SQLiteTableSync
		{
		public:
			static const std::string	TABLE_NAME;		//!< Table name in the database
			static const int			TABLE_ID;		//!< Table ID used by util::encodeID and util::decodeTableId

		protected:
			static const bool HAS_AUTO_INCREMENT;
			static int _autoIncrementValue;

			static boost::shared_ptr<boost::mutex> _idMutex; 

			static uid encodeUId (int gridId, int gridNodeId, long objectId);

			/** Unique ID getter for autoincremented tables.
			*/
			static uid getId(int gridId, int gridNodeId);

			SQLiteTableSyncTemplate(
				bool allowInsert = true, 
				bool allowRemove = true,
				const std::string& triggerOverrideClause = "1",
				bool ignoreCallbacksOnFirstSync = false)
				: SQLiteTableSync(TABLE_NAME, allowInsert, allowRemove, triggerOverrideClause, ignoreCallbacksOnFirstSync)
			{			}

			void initAutoIncrement();

		public:
			typedef T ObjectsClass;

			static void load(T* obj, const db::SQLiteResult& rows, int rowId=0);
			
			
			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007
				
				The object is recognized by its key :
					- if the object has already a key, then the corresponding record is replaced
					- if the object does not have any kay, then the autoincrement function generates one for it.
			*/
			static void save(T* obj);

			/** Object fetcher.
				@param key UID of the object
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException<T> if the object was not found
			*/
			static boost::shared_ptr<T> get(uid key);

			static boost::shared_ptr<T> createEmpty();
			static void remove(uid key);

		};

		template <class T>
			void synthese::db::SQLiteTableSyncTemplate<T>::remove(uid key)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "DELETE FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(key);
			sqlite->execUpdate(query.str());
		}

		template <class T>
		boost::shared_ptr<T> synthese::db::SQLiteTableSyncTemplate<T>::createEmpty()
		{
			boost::shared_ptr<T> object(new T);
			save(object.get());
			return object;
		}

		template <class T>
		boost::shared_ptr<T> synthese::db::SQLiteTableSyncTemplate<T>::get(uid key)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "SELECT * "
				<< "FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(key)
				<< " LIMIT 1";
			db::SQLiteResult rows = sqlite->execQuery(query.str());
			if (rows.getNbRows() <= 0)
				throw DBEmptyResultException<T>(key, "ID not found in database.");
			boost::shared_ptr<T> object(new T());
			load(object.get(), rows);
			return object;
		}

		template <class T>
			boost::shared_ptr<boost::mutex> SQLiteTableSyncTemplate<T>::_idMutex(new boost::mutex); 

		template <class T>
			int SQLiteTableSyncTemplate<T>::_autoIncrementValue(1); 

		template <class T>
			uid SQLiteTableSyncTemplate<T>::getId( int gridId, int gridNodeId )
		{			
			boost::mutex::scoped_lock mutex(*_idMutex);

			//	if (it == _autoIncrementValues.end())
			//		throw Exception("Autoincrement not initialized for table "+ getTableName());

			//	int retval = it->second++;
			int retval = _autoIncrementValue++;

			return encodeUId(gridId, gridNodeId, retval);
		}


		template <class T>
			uid SQLiteTableSyncTemplate<T>::encodeUId (int gridId, int gridNodeId, long objectId)
		{
			return synthese::util::encodeUId (TABLE_ID, gridId, gridNodeId, objectId);
		}

		template <class T>
			void SQLiteTableSyncTemplate<T>::initAutoIncrement()
		{
			if (HAS_AUTO_INCREMENT)
			{
				try
				{
					const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
					std::stringstream query;
					query
					    << "SELECT " << util::Conversion::ToString((uid) 0x00000000FFFFFFFFLL) << " & MAX(id) AS maxid FROM " << TABLE_NAME
					    << " WHERE (id & " << util::Conversion::ToString((uid) 0xFFFFFFFF00000000LL) << ") = " << util::Conversion::ToString(encodeUId(1, 1, 0)); /// @todo GRID and NODEGRID to be replaced by the correct values

					SQLiteResult result = sqlite->execQuery(query.str());

					if (result.getNbRows() > 0 && util::Conversion::ToLongLong(result.getColumn(0, "maxid")) > 0)
					{
					    _autoIncrementValue = util::decodeObjectId(util::Conversion::ToLongLong(result.getColumn(0, "maxid"))) + 1;
					}
					
				}
				catch (SQLiteException& e)
				{
					Log::GetInstance().debug("Table "+ getTableName() +" without preceding id.", e);

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

