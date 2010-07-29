	
/** SQLiteInheritedRegistryTableSync class header.
	@file SQLiteInheritedRegistryTableSync.h

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

#ifndef SYNTHESE_db_SQLiteInheritedRegistryTableSync_h__
#define SYNTHESE_db_SQLiteInheritedRegistryTableSync_h__

#include "SQLiteInheritedTableSyncTemplate.h"

#include "DBConstants.h"

#include "ConstantReturner.h"

namespace synthese
{
	namespace db
	{
		class SQLite;
		class SQLiteSync;

		/** SQLiteInheritedRegistryTableSync class.
			@ingroup m10
		*/
		template<class ParentTableSyncClass, class TableSyncClass, class ObjectClass>
		class SQLiteInheritedRegistryTableSync
			: public SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>
		{
		public:
			virtual bool getRegisterInSubClassMap() const
			{
				return true;
			}

			SQLiteInheritedRegistryTableSync()
				: SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>()
			{

			}



			void rowsAdded (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				typename ObjectClass::Registry& registry(env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>());
				util::RegistryKeyType key(rows->getKey());
				try
				{
					if (registry.contains(key))
					{
						ObjectClass* object(static_cast<ObjectClass*>(registry.getEditable(key).get()));
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Unlink(
							object
						);
						Load(object, rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
					}
					else
					{
						boost::shared_ptr<ObjectClass> object(new ObjectClass(key));
						registry.add(
							boost::static_pointer_cast<typename ObjectClass::Registry::ObjectsClass,ObjectClass>(object)
						);
						Load(object.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
					}
				}
				catch(util::Exception& e)
				{
					if(registry.contains(key))
					{
						registry.remove(key);
					}
					util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : ", e);
				}
			}



			void rowsUpdated(
				SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				typename ObjectClass::Registry& registry(env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>());
				try
				{
					util::RegistryKeyType id(rows->getKey());
					if (registry.contains(id))
					{
						ObjectClass* object(static_cast<ObjectClass*>(registry.getEditable(rows->getKey()).get()));
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Unlink(
							object
						);
						Load(object, rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
					}
				}
				catch (util::Exception& e)
				{
					util::Log::GetInstance().warn("Error on load after row update : ", e);
				}
			}



			void rowsRemoved(
				SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				typename ObjectClass::Registry& registry(env.getEditableRegistry<typename ObjectClass::Registry::ObjectsClass>());
				try
				{
					util::RegistryKeyType id = rows->getKey();
					if (registry.contains(id))
					{
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Unlink(
							static_cast<ObjectClass*>(registry.getEditable(id).get())
						);
						registry.remove(id);
					}
				}
				catch (util::Exception& e)
				{
					util::Log::GetInstance().warn("Error on unload after row deletion : ", e);
				}
			}


		};
	}
}

#endif // SYNTHESE_db_SQLiteInheritedRegistryTableSync_h__
