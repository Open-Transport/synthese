
/** SQLiteInheritedTableSyncTemplate class header.
	@file SQLiteInheritedTableSyncTemplate.h

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

#ifndef SYNTHESE_db_SQLiteInheritedTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteInheritedTableSyncTemplate_h__

#include "02_db/Types.h"
#include "SQLiteResult.h"
#include "SQLiteTableSyncTemplate.h"
#include "SQLiteException.h"
#include "DBEmptyResultException.h"
#include "DBModule.h"
#include "FactorableTemplate.h"
#include "Env.h"

namespace synthese
{
	namespace db
	{
		/** SQLiteInheritedTableSyncTemplate class.
			@ingroup m10

			@warning The implementations of rowAdded, rowUpdated, and rowRemoved must handle only one row at each execution.
		*/
		template<class ParentTableSyncClass, class TableSyncClass, class ObjectClass>
		class SQLiteInheritedTableSyncTemplate
			: public util::FactorableTemplate<ParentTableSyncClass,TableSyncClass>
		{
		protected:
			/** Object links loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param temporary Objects to link must be temporarily and recursively created from the database
				@author Hugues Romain
				@date 2007

				This method must be implemented by each template instantiation
			*/
			static void Load(
				ObjectClass* obj,
				const SQLiteResultSPtr& rows,
				util::Env* env,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			);



			static void Save(ObjectClass* obj);



			/** Remove all the links to the object present in the environment.
				@param obj Pointer to the object to unlink
				@author Hugues Romain
				@date 2007				

				This method must be implemented by each template instantiation
			*/
			static void Unlink(ObjectClass* obj, util::Env* env);


		public:


			static boost::shared_ptr<ObjectClass> GetEditable(
				util::RegistryKeyType key,
				util::Env* env = NULL,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				if(env != NULL)
				{
					util::Registry<ObjectClass>& registry(env->template getEditableRegistry<ObjectClass>());
					if (registry.contains(key))
						return registry.getEditable(key);
				}

				boost::shared_ptr<ObjectClass> object;
				try
				{
					SQLiteResultSPtr rows(SQLiteTableSyncTemplate<ParentTableSyncClass>::_GetRow(key));
					object.reset(new ObjectClass(rows->getKey()));
					Load(object.get(), rows, env, linkLevel);
				}
				catch (typename db::DBEmptyResultException<TableSyncClass>&)
				{
					if (autoCreate == NEVER_CREATE)
						throw util::ObjectNotFoundException<ObjectClass>(key, "Object not found in "+ ParentTableSyncClass::TABLE_NAME);
					object.reset(new ObjectClass(key));
				}

				if (env != NULL)
				{
					env->template getEditableRegistry<ObjectClass>().add(object);
				}
				return object;
			}


			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static boost::shared_ptr<const ObjectClass> Get(
				util::RegistryKeyType key,
				util::Env* env = NULL,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				return boost::const_pointer_cast<const ObjectClass>(GetEditable(key,env,linkLevel,autoCreate));
			}



			/** Load obects into an environment, from a SQL query.
				@param query SQL query
				@param env Environment to write
				@param linkLevel Link level
				@throws Exception if the load failed
			*/
			static void LoadFromQuery(
				const std::string& query,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				try
				{
					util::Registry<ObjectClass>& registry(env.template getEditableRegistry<ObjectClass>());
					SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query);
					while (rows->next ())
					{
						boost::shared_ptr<ObjectClass> object(new ObjectClass(rows->getKey()));
						Load(object.get(), rows, &env, linkLevel);
						registry.add(object);
					}
				}
				catch(SQLiteException& e)
				{
					throw util::Exception(e.getMessage());
				}
			}



			SQLiteInheritedTableSyncTemplate()
				: util::FactorableTemplate<ParentTableSyncClass,TableSyncClass>()
			{
			}

		protected:
			virtual typename ParentTableSyncClass::ObjectType* _create(util::RegistryKeyType key)
			{
				return new ObjectClass(key);
			}

			virtual boost::shared_ptr<typename ParentTableSyncClass::ObjectType> _getEditable(
				util::RegistryKeyType key,
				util::Env* env = NULL,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				try
				{
					return boost::static_pointer_cast<typename ParentTableSyncClass::ObjectType, ObjectClass>(GetEditable(key, env,linkLevel, autoCreate));
				}
				catch(util::ObjectNotFoundException<ObjectClass>& e)
				{
					throw util::ObjectNotFoundException<typename ParentTableSyncClass::ObjectType>(e.getKey(), e.getMessage());
				}
			}


			virtual boost::shared_ptr<const typename ParentTableSyncClass::ObjectType> _get(
				util::RegistryKeyType key,
				util::Env* env = NULL,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				try
				{
					return boost::static_pointer_cast<const typename ParentTableSyncClass::ObjectType, const ObjectClass>(Get(key, env,linkLevel, autoCreate));
				}
				catch(util::ObjectNotFoundException<ObjectClass>& e)
				{
					throw util::ObjectNotFoundException<typename ParentTableSyncClass::ObjectType>(e.getKey(), e.getMessage());
				}
			}

			virtual void _load(
				typename ParentTableSyncClass::ObjectType* obj,
				const SQLiteResultSPtr& rows,
				util::Env* env,
				util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
			){
				Load(static_cast<ObjectClass*>(obj), rows, env, linkLevel);
			}

			
			virtual void _save(
				typename ParentTableSyncClass::ObjectType* obj
			){
				Save(static_cast<ObjectClass*>(obj));
			}
		};
	}
}

#endif // SYNTHESE_db_SQLiteInheritedTableSyncTemplate_h__
