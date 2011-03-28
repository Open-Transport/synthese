
/** DBAutoRegisterTableSyncTemplate class header.
@file DBRegistryTableSyncTemplate.hpp

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

#ifndef SYNTHESE_db_DBAutoRegisterTableSyncTemplate_hpp__
#define SYNTHESE_db_DBAutoRegisterTableSyncTemplate_hpp__

#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace db
	{
		/** Table sync template class, providing auto-registration of elements at first get.
			@ingroup m10

			The elements are loaded into the registry at the first use of Get with the source parameter on GET_AUTO or GET_REGISTRY.
		*/
		template<class K, class T>
		class DBAutoRegisterTableSyncTemplate : public DBDirectTableSyncTemplate<K,T>
		{
		public:
			DBAutoRegisterTableSyncTemplate()
			:	DBDirectTableSyncTemplate<K,T>()
			{}

			virtual bool getRegisterInSubClassMap() const
			{
				return false;
			}


			/** Action to do on object creation or replacement.
				Only if the event is a replacement, and if the concerned object is already loaded into the registry, then it is updated.
			*/
			void rowsAdded(
				DB* db,
				const DBResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				while (rows->next ())
				{
					try
					{
						if (registry.contains(rows->getKey()))
						{
							boost::shared_ptr<T> address(registry.getEditable(rows->getKey()));
							DBDirectTableSyncTemplate<K,T>::Unlink(address.get());
							Load (address.get(), rows, env, util::FIELDS_ONLY_LOAD_LEVEL);
						}
					}
					catch(Exception& e)
					{
						util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : ", e);
					}
				}
			}


			/** Action to do on object update.
				The object is updated only if it is already loaded in the corresponding registry.
			*/
			void rowsUpdated(
				DB* db,
				const DBResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				while (rows->next ())
				{
					try
					{
						util::RegistryKeyType id = rows->getKey();
						if (registry.contains(id))
						{
							boost::shared_ptr<T> address(registry.getEditable(id));
							DBDirectTableSyncTemplate<K,T>::Unlink(address.get());
							Load (address.get(), rows, env, util::FIELDS_ONLY_LOAD_LEVEL);
						}
					}
					catch (Exception& e)
					{
						util::Log::GetInstance().warn("Error on load after row update : ", e);
					}
				}

			}


			/** Action to do on object deletion.
				If it is registered, this method deletes the corresponding object in its registry, and runs all necessary cleaning actions.
			*/
			void rowsRemoved(
				DB* db,
				const RowIdList& rowIds
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				BOOST_FOREACH(const util::RegistryKeyType& rowId, rowIds)
				{
					try
					{
						if (registry.contains(rowId))
						{
							DBDirectTableSyncTemplate<K,T>::Unlink(registry.getEditable(rowId).get());
							registry.remove(rowId);
						}
					}
					catch (Exception& e)
					{
						util::Log::GetInstance().warn("Error on unload after row deletion : ", e);
					}
				}
			}
		};
	}
}

#endif // SYNTHESE_db_DBAutoRegisterTableSyncTemplate_hpp__
