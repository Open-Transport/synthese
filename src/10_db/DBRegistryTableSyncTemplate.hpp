////////////////////////////////////////////////////////////////////////////////
/// DBRegistryTableSyncTemplate class header.
///	@file DBRegistryTableSyncTemplate.hpp
///	@author Hugues Romain
///	@date 2008-12-26 18:56
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_db_DBRegistryTableSyncTemplate_hpp__
#define SYNTHESE_db_DBRegistryTableSyncTemplate_hpp__

#include "DBDirectTableSyncTemplate.hpp"

#include "Exception.h"
#include "Log.h"

namespace synthese
{
	namespace db
	{
		////////////////////////////////////////////////////////////////////
		/// Table sync class with  synchronization between
		/// database and physical memory.
		///		- class K : Class of the table sync
		///		- class T : Class of the corresponding objects
		///	@ingroup m10
		template<class K, class T>
		class DBRegistryTableSyncTemplate:
			public DBDirectTableSyncTemplate<K,T>
		{
		public:
			DBRegistryTableSyncTemplate() : DBDirectTableSyncTemplate<K,T>() {}


			//////////////////////////////////////////////////////////////////////////
			/// Action to do on row insertion / replacement.
			///	This method loads the object in physical memory.
			void rowsAdded(
				DB* db,
				const DBResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				while (rows->next ())
				{
					util::RegistryKeyType key(rows->getKey());
					try
					{
						if (registry.contains(key))
						{
							boost::shared_ptr<T> address(registry.getEditable(key));
							DBDirectTableSyncTemplate<K,T>::Unlink(address.get());
							Load (address.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
						}
						else
						{
							boost::shared_ptr<T> object(K::GetNewObject(rows));
							registry.add(object);
							Load(object.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
						}
					}
					catch(Exception& e)
					{
						if(registry.contains(key))
						{
							registry.remove(key);
						}
						util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : ", e);
					}
				}
			}



			//////////////////////////////////////////////////////////////////////////
			/// Synchronization on row update.
			/// This method updates the corresponding object in ram only if it has
			/// already be loaded.
			/// @throws nothing
			void rowsUpdated(
				DB* db,
				const DBResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				while (rows->next ())
				{
					util::RegistryKeyType key(rows->getKey());
					try
					{
						if (registry.contains(key))
						{
							boost::shared_ptr<T> address(registry.getEditable(rows->getKey()));
							Unlink(address.get());
							Load(address.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
						}
					}
					catch (Exception& e)
					{
						if(registry.contains(key))
						{
							registry.remove(key);
						}
						util::Log::GetInstance().warn("Error on load after row update : ", e);
					}
				}
			}



			//////////////////////////////////////////////////////////////////////////
			/// Synchronization on row deletion.
			/// This method deletes the corresponding object in ram and runs 
			/// all necessary cleaning actions.
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

#endif // SYNTHESE_db_DBRegistryTableSyncTemplate_hpp__
