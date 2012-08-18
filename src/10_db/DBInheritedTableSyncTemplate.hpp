
/** DBInheritedTableSyncTemplate class header.
	@file DBInheritedTableSyncTemplate.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_db_DBInheritedTableSyncTemplate_hpp__
#define SYNTHESE_db_DBInheritedTableSyncTemplate_hpp__

#include "DBTypes.h"
#include "DBResult.hpp"
#include "DBTableSyncTemplate.hpp"
#include "DBException.hpp"
#include "DBEmptyResultException.h"
#include "DBModule.h"
#include "FactorableTemplate.h"
#include "Env.h"

namespace synthese
{
	namespace db
	{
		/** DBInheritedTableSyncTemplate class.
			@ingroup m10

			@warning The implementations of rowAdded, rowUpdated, and rowRemoved must handle only one row at each execution.
		*/
		template<class ParentTableSyncClass, class TableSyncClass, class ObjectClass>
		class DBInheritedTableSyncTemplate
			: public util::FactorableTemplate<ParentTableSyncClass,TableSyncClass>
		{
		public:
			typedef std::vector<boost::shared_ptr<ObjectClass> > SearchResult;
			typedef ObjectClass ObjectType;

			/** Object links loader from the database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param temporary Objects to link must be temporarily and recursively created from the database
				@author Hugues Romain
				@date 2007

				This method must be implemented by each template instantiation
			*/
			static void Load(
				ObjectClass* obj,
				const DBResultSPtr& rows,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static void Save(
				ObjectClass* obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);



			/** Remove all the links to the object present in the environment.
				@param obj Pointer to the object to unlink
				@author Hugues Romain
				@date 2007

				This method must be implemented by each template instantiation
			*/
			static void Unlink(ObjectClass* obj);


		public:


			static boost::shared_ptr<ObjectClass> GetEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				typename ObjectClass::Registry& registry(
					env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>()
				);
				if (registry.contains(key))
				{
					return boost::static_pointer_cast<ObjectClass,typename ObjectClass::Registry::ObjectsClass>(registry.getEditable(key));
				}

				boost::shared_ptr<ObjectClass> object;
				try
				{
					DBResultSPtr rows(DBTableSyncTemplate<ParentTableSyncClass>::_GetRow(key));
					object.reset(new ObjectClass(rows->getKey()));
					env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>().add(
						boost::static_pointer_cast<typename ObjectClass::Registry::ObjectsClass, ObjectClass>(
							object
					)	);
					Load(object.get(), rows, env, linkLevel);
				}
				catch (typename db::DBEmptyResultException<TableSyncClass>&)
				{
					if (autoCreate == NEVER_CREATE)
						throw util::ObjectNotFoundException<ObjectClass>(key, "Object not found in "+ ParentTableSyncClass::TABLE.NAME);
					object.reset(new ObjectClass(key));
					env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>().add(
						boost::static_pointer_cast<typename ObjectClass::Registry::ObjectsClass, ObjectClass>(
							object
					)	);
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
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				return boost::const_pointer_cast<const ObjectClass>(GetEditable(key,env,linkLevel,autoCreate));
			}



			static boost::shared_ptr<typename ObjectClass::Registry::ObjectsClass> LoadFromRow(
				const DBResultSPtr& rows,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				typename ObjectClass::Registry& registry(env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>());
				util::RegistryKeyType key(rows->getKey());
				try
				{
					if(registry.contains(key))
					{
						return registry.getEditable(key);
					}
					else
					{
						boost::shared_ptr<ObjectClass> object(new ObjectClass(key));
						registry.add(
							boost::static_pointer_cast<typename ObjectClass::Registry::ObjectsClass,ObjectClass>(object)
						);
						Load(object.get(), rows, env, linkLevel);
						return boost::static_pointer_cast<typename ObjectClass::Registry::ObjectsClass,ObjectClass>(object);
				}	}
				catch(std::exception& e)
				{
					if(registry.contains(key))
					{
						registry.remove(key);
					}
					util::Log::GetInstance().warn("Skipped object in results load.", e);
					return boost::shared_ptr<typename ObjectClass::Registry::ObjectsClass>();
				}
			}



			virtual boost::shared_ptr<typename ParentTableSyncClass::ObjectType> _loadFromRow(
				const DBResultSPtr& rows,
				util::Env& env,
				util::LinkLevel linkLevel
			) const {
				return boost::static_pointer_cast<typename ParentTableSyncClass::ObjectType, typename ObjectClass::Registry::ObjectsClass>(
					LoadFromRow(rows, env, linkLevel)
				);
			}



			/** Load objects into an environment, from a SQL query.
				@param query SQL query
				@param env Environment to populate
				@param linkLevel Link level
				@throws Exception if the load failed
			*/
			static SearchResult LoadFromQuery(
				const std::string& query,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				SearchResult r;
				DBResultSPtr rows = DBModule::GetDB()->execQuery(query);
				while (rows->next ())
				{
					boost::shared_ptr<typename ObjectClass::Registry::ObjectsClass> o(
						LoadFromRow(
							rows,
							env,
							linkLevel
					)	);
					if(o.get())
					{
						r.push_back(o);
					}
				}
				return r;
			}



			DBInheritedTableSyncTemplate()
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
				util::Env& env = NULL,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
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
				util::Env& env = NULL,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
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
				const DBResultSPtr& rows,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			){
				Load(static_cast<ObjectClass*>(obj), rows, env, linkLevel);
			}


			virtual void _save(
				typename ParentTableSyncClass::ObjectType* obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			){
				Save(static_cast<ObjectClass*>(obj), transaction);
			}
		};
	}
}

#endif // SYNTHESE_db_DBInheritedTableSyncTemplate_hpp__
