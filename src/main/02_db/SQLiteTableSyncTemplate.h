
#ifndef SYNTHESE_SQLiteTableSyncTemplate_H__
#define SYNTHESE_SQLiteTableSyncTemplate_H__

#include <sstream>
#include <string>

#include "01_util/UId.h"
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
		template <class T>
		class SQLiteTableSyncTemplate : public SQLiteTableSync
		{
		public:
			static const std::string TABLE_NAME;

		protected:
			static const int TABLE_ID;
			static const bool HAS_AUTO_INCREMENT;
			static int _autoIncrementValue;

			static boost::shared_ptr<boost::mutex> _idMutex; 

			static uid encodeUId (int gridId, int gridNodeId, long objectId);

			/** Unique ID getter for autoincremented tables.
			*/
			static uid getId(int gridId, int gridNodeId);

			SQLiteTableSyncTemplate( const std::string& tableName, 
				bool allowInsert = true, 
				bool allowRemove = true,
				const std::string& triggerOverrideClause = "1") : SQLiteTableSync(tableName, allowInsert, allowRemove, triggerOverrideClause)
			{			}

			void initAutoIncrement();

		public:
			typedef T ObjectsClass;

			static void load(T* obj, const db::SQLiteResult& rows, int rowId=0);
			static void save(T* obj);
			static T* get(uid key);

			/// @todo See if the template can be used more 


		};

		template <class T>
			T* synthese::db::SQLiteTableSyncTemplate<T>::get(uid key)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "SELECT * "
				<< "FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(key);
			db::SQLiteResult rows = sqlite->execQuery(query.str());
			if (rows.getNbRows() <= 0)
				throw DBEmptyResultException("Object "+ TABLE_NAME + " " + Conversion::ToString(key) + " not found in database.");
			T* object = new T;
			load(object, rows);
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
						<< "SELECT " << util::Conversion::ToString(0x00000000FFFFFFFFLL) << " & MAX(id) AS maxid FROM " << TABLE_NAME
						<< " WHERE (id & " << util::Conversion::ToString(0xFFFFFFFF00000000LL) << ") = " << util::Conversion::ToString(encodeUId(1, 1, 0)); /// @todo GRID and NODEGRID to be replaced by the correct values

					SQLiteResult result = sqlite->execQuery(query.str());

					if (result.getNbRows() > 0 && util::Conversion::ToLongLong(result.getColumn(0, "maxid")) > 0)
						_autoIncrementValue = util::decodeObjectId(util::Conversion::ToLongLong(result.getColumn(0, "maxid"))) + 1;
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

