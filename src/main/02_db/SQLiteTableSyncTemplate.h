
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

		typedef enum
		{
			GET_AUTO
			, GET_TEMPORARY
			, GET_REGISTRY
		} GetSource;


		/** Table synchronizer template.
			@ingroup m10
		*/
		template <class K, class T>
		class SQLiteTableSyncTemplate : public util::FactorableTemplate<SQLiteTableSync, K>
		{
		public:
			typedef T							ObjectType;
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
			typedef T ObjectsClass;


			virtual const std::string& getTableName() const { return TABLE_NAME; }
			virtual std::string getTriggerOverrideClause() const { return TRIGGERS_ENABLED_CLAUSE; }


			static std::string GetFieldValue(uid id, const std::string& field);

		private:
			
			/** Object links loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param temporary Objects to link must be temporarily and recursively created from the database
				@author Hugues Romain
				@date 2007				
			*/
			static void _link(T* obj, const SQLiteResultSPtr& rows, GetSource temporary);



			/** Remove all the links to the object present in the environment.
				@author Hugues Romain
				@date 2007				
			*/
			static void _unlink(T* obj);
			

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

			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static T* _Get(uid key, bool linked)
			{
				SQLiteResultSPtr rows(_GetRow(key));
				T* object(new T);
				load(object, rows);
				if (linked)
					link(object, rows, GET_TEMPORARY);
				return object;
			}

			static void link(T* obj, const SQLiteResultSPtr& rows, GetSource temporary)
			{
				if (obj->getLinked())
				{
					unlink(obj);
					if (temporary)
						obj->clearChildTemporaryObjects();
				}
				_link(obj, rows, temporary);
				obj->setLinked(true);
			}

			static void unlink(T* obj)
			{
				_unlink(obj);
				obj->setLinked(false);
			}
			
			/** Object properties loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@author Hugues Romain
				@date 2007
				@warning To complete the load when building the RAM environment, follow the properties load by the link method
			*/
			static void load(T* obj, const SQLiteResultSPtr& rows);

			
			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007
				
				The object is recognized by its key :
					- if the object has already a key, then the corresponding record is replaced
					- if the object does not have any key, then the autoincrement function generates one for it.
			*/
			static void save(T* obj);



			/** Gets from the database a temporary object linked by another one.
				@param key UID of the object
				@param obj Parent object which link to the returned object
				@param linked recursive get
				@return Pointer to the created object
				@author Hugues Romain
				@date 2007

				The deletion of the object will be autoamtically done at the deletion of the parent object
			*/
			template<class C>
			static const T* Get(uid key, C* obj, bool linked)
			{
				const T* c(static_cast<const T*>(_Get(key, linked)));
				obj->addChildTemporaryObject(c);
				return c;
			}


						/** Gets from the database a temporary object linked by another one.
				@param key UID of the object
				@param obj Parent object which link to the returned object
				@param linked recursive get
				@return Pointer to the created object
				@author Hugues Romain
				@date 2007

				The deletion of the object will be autoamtically done at the deletion of the parent object
			*/
			template<class C>
			static T* GetUpdateable(uid key, C* obj)
			{
				T* c(_Get(key, true));
				obj->addChildTemporaryObject(c);
				return c;
			}



			template<class C>
			static const T* Get(uid key, C* obj, bool linked, GetSource source)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::Get(key).get()
					: Get(key, obj, linked);
			}

			template<class C>
			static T* GetUpdateable(uid key, C* obj, GetSource source)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::GetUpdateable(key).get()
					: GetUpdateable(key, obj);
			}

			static boost::shared_ptr<const T> Get(uid key, GetSource source = GET_AUTO, bool linked = false)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::Get(key)
					: boost::shared_ptr<const T>(_Get(key, linked));
			}

			static boost::shared_ptr<T> GetUpdateable(uid key, GetSource source = GET_TEMPORARY)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::GetUpdateable(key)
					: boost::shared_ptr<T>(_Get(key, true));
			}

			static boost::shared_ptr<T> createEmpty();
			static void remove(uid key);

		};


		template <class K, class T>
		std::string SQLiteTableSyncTemplate<K, T>::GetFieldValue(uid id, const std::string& field)
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

		template <class K, class T>
			void SQLiteTableSyncTemplate<K,T>::remove(uid key)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			std::stringstream query;
			query
				<< "DELETE FROM " << TABLE_NAME
				<< " WHERE " << TABLE_COL_ID << "=" << util::Conversion::ToString(key);
			sqlite->execUpdate(query.str());
		}

		template <class K, class T>
		boost::shared_ptr<T> SQLiteTableSyncTemplate<K,T>::createEmpty()
		{
			boost::shared_ptr<T> object(new T);
			save(object.get());
			return object;
		}

		template <class K, class T>
			boost::shared_ptr<boost::mutex> SQLiteTableSyncTemplate<K,T>::_idMutex(new boost::mutex); 

		template <class K, class T>
			int SQLiteTableSyncTemplate<K,T>::_autoIncrementValue(1); 

		template <class K, class T>
			uid SQLiteTableSyncTemplate<K,T>::getId()
		{			
			boost::mutex::scoped_lock mutex(*_idMutex);

			//	if (it == _autoIncrementValues.end())
			//		throw Exception("Autoincrement not initialized for table "+ getTableName());

			//	int retval = it->second++;
			int retval = _autoIncrementValue++;

			return encodeUId(retval);
		}


		template <class K, class T>
			uid SQLiteTableSyncTemplate<K,T>::encodeUId (long objectId)
		{
		    // TODO : plenty of functions should be at SQLiteTableSync level directly.
		    // default value is 1 for compatibility
			static int nodeId = util::Conversion::ToInt (DbModuleClass::GetParameter ("dbring_node_id", "1"));
			return util::encodeUId (TABLE_ID, 
							  1, // TODO : remove grid id, deprecated with ring nodes
							  nodeId, 
							  objectId);
		}



		template <class K, class T>
			void SQLiteTableSyncTemplate<K,T>::initAutoIncrement()
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

