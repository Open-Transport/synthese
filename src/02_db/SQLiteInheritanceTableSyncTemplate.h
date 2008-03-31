
/** SQLiteInheritanceTableSyncTemplate class header.
	@file SQLiteInheritanceTableSyncTemplate.h

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

#ifndef SYNTHESE_db_SQLiteInheritanceTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteInheritanceTableSyncTemplate_h__

#include "02_db/SQLiteTableSyncTemplate.h"
#include "01_util/Factorable.h"

namespace synthese
{
	namespace db
	{
		/** SQLiteInheritanceTableSyncTemplate class.
		- class K : Class of the table sync
		- class T : Class of the corresponding objects
		@ingroup m10
		*/
		template<class K, class T>
		class SQLiteInheritanceTableSyncTemplate
			: public SQLiteTableSyncTemplate<K>
		{
		public:
			typedef T		ObjectType;
			typedef K		FactoryClass;

		protected:
			static std::string	_GetSubClassKey(const SQLiteResultSPtr& row);
			
		public:
			virtual bool getRegisterInSubClassMap() const
			{
				return false;
			}

			virtual void rowsAdded (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows, bool isFirstSync = false)
			{
				while (rows->next ())
				{
					std::string subClassKey(_GetSubClassKey(rows));
					boost::shared_ptr<K> tablesync(util::Factory<K>::create(subClassKey));
					tablesync->rowsAdded(sqlite, sync, rows, isFirstSync);
					if (tablesync->getRegisterInSubClassMap())
						DBModule::AddSubClass(rows->getLongLong(TABLE_COL_ID), subClassKey);
				}
			}

			virtual void rowsUpdated (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows)
			{
				while (rows->next ())
				{
					boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(rows)));
					tablesync->rowsUpdated(sqlite, sync, rows);
				}
			}

			virtual void rowsRemoved (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows)
			{
				while (rows->next ())
				{
					uid id(rows->getLongLong(TABLE_COL_ID));
					std::string subClass(DBModule::GetSubClass(id));
					if (!subClass.empty())
					{
						boost::shared_ptr<K> tablesync(util::Factory<K>::create(subClass));
						tablesync->rowsRemoved(sqlite, sync, rows);					}
				}
			}

			SQLiteInheritanceTableSyncTemplate() : SQLiteTableSyncTemplate<K>()
			{

			}

			virtual T* create()
			{
				assert(false);
				return NULL;
			}

			virtual T* get(SQLiteResultSPtr& row, bool linked)
			{
				assert(false);
				return NULL;
			}

			/** Object properties loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@author Hugues Romain
				@date 2007
				@warning To complete the load when building the RAM environment, follow the properties load by the link method
			*/
			static void Load(T* obj, const SQLiteResultSPtr& rows);



			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007

				The object is recognized by its key :
				- if the object has already a key, then the corresponding record is replaced
				- if the object does not have any key, then the autoincrement function generates one for it.
			*/
			static void Save(T* obj);


			static T* GetUpdateable(const SQLiteResultSPtr& row, bool linked=false)
			{
				boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(row)));
				return static_cast<T*>(tablesync->get(row));
			}


			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static T* GetUpdateable(uid key, bool linked=false)
			{
				SQLiteResultSPtr rows(_GetRow(key));
				boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(rows)));
				return static_cast<T*>(tablesync->get(rows, linked));
			}


			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static const T* Get(uid key, bool linked=false)
			{
				SQLiteResultSPtr rows(_GetRow(key));
				boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(rows)));
				return static_cast<const T*>(tablesync->get(rows, linked));
			}

		};
	}
}

#endif // SYNTHESE_db_SQLiteInheritanceTableSyncTemplate_h__
