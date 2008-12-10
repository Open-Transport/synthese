
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

#include "SQLiteTableSyncTemplate.h"
#include "02_db/Types.h"

namespace synthese
{
	namespace util
	{
		class Env;
	}

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
			//! \name Static methods to implement by each derived class
			//@{
				static std::string	_GetSubClassKey(const SQLiteResultSPtr& row);
				static std::string	_GetSubClassKey(const T* obj);
			
				/** Fields load operations common to all subclasses.
					@param obj Pointer to the object to load from the database
					@param rows Row to read
					@param env Environment to read
					@param linkLevel link level
					@author Hugues Romain
					@date 2008
				*/
				static void _CommonLoad(
					T* obj,
					const SQLiteResultSPtr& rows,
					util::Env* env,
					util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
				);
			//@}

			//! \name Virtual template instance launchers
			//@{
				virtual T* _create()
				{
					assert(false);
					return NULL;
				}


				virtual void _load(
					T* obj,
					const SQLiteResultSPtr& rows,
					util::Env* env,
					util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
					){
						assert(false);
				}


				virtual void _save(
					T* obj
				){
						assert(false);
				}



				virtual boost::shared_ptr<const T> _get(
					util::RegistryKeyType key,
					util::Env* env = NULL,
					util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
					AutoCreation autoCreate = NEVER_CREATE
					){
						assert(false);
						return boost::shared_ptr<const T>();
				}




				virtual boost::shared_ptr<T> _getEditable(
					util::RegistryKeyType key,
					util::Env* env = NULL,
					util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
					AutoCreation autoCreate = NEVER_CREATE
					){
						assert(false);
						return boost::shared_ptr<T>();
				}
			//@}

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
						DBModule::AddSubClass(rows->getKey(), subClassKey);
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
						tablesync->rowsRemoved(sqlite, sync, rows);
					}
				}
			}

			SQLiteInheritanceTableSyncTemplate() : SQLiteTableSyncTemplate<K>()
			{

			}



			static void Load(
				T* obj,
				const SQLiteResultSPtr& rows,
				util::Env* env,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			){
				boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(rows)));
				tablesync->_load(obj, rows, env, linkLevel);
			}


			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007

				The object is recognized by its key :
				- if the object has already a key, then the corresponding record is replaced
				- if the object does not have any key, then the autoincrement function generates one for it.
			*/
			static void Save(T* obj)
			{
				boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(obj)));
				tablesync->_save(obj);
			}


			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static boost::shared_ptr<T> GetEditable(
				util::RegistryKeyType key,
				util::Env* env = NULL,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				SQLiteResultSPtr rows(_GetRow(key));
				boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(rows)));
				return tablesync->_getEditable(key, env, linkLevel, autoCreate);
			}


			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static boost::shared_ptr<const T> Get(
				util::RegistryKeyType key,
				util::Env* env = NULL,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
					SQLiteResultSPtr rows(_GetRow(key));
					boost::shared_ptr<K> tablesync(util::Factory<K>::create(_GetSubClassKey(rows)));
					return tablesync->_get(key, env, linkLevel, autoCreate);
			}

		};
	}
}

#endif // SYNTHESE_db_SQLiteInheritanceTableSyncTemplate_h__
