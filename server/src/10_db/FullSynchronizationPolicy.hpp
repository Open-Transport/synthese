
/** FullSynchronizationPolicy class header.
	@file FullSynchronizationPolicy.hpp

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

#ifndef SYNTHESE_db_FullSynchronizationPolicy_hpp__
#define SYNTHESE_db_FullSynchronizationPolicy_hpp__

#include "Env.h"
#include "DB.hpp"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// DB-memory Synchronization policy : each object is loaded in memory.
		///	@ingroup m10
		/// @author Hugues Romain
		/// @date 2013
		template<
			class K,
			class T
		>
		class FullSynchronizationPolicy
		{
		public:
			FullSynchronizationPolicy() {}



			//////////////////////////////////////////////////////////////////////////
			/// Action to do on row insertion / replacement.
			///	This method loads the object in physical memory.
			static void RowsAdded(
				DB* db,
				const DBResultSPtr& rows
			);



			//////////////////////////////////////////////////////////////////////////
			/// Synchronization on row update.
			/// This method updates the corresponding object in ram only if it has
			/// already be loaded.
			/// @throws nothing
			static void RowsUpdated(
				DB* db,
				const DBResultSPtr& rows
			);



			//////////////////////////////////////////////////////////////////////////
			/// Synchronization on row deletion.
			/// This method deletes the corresponding object in ram and runs
			/// all necessary cleaning actions.
			static void RowsRemoved(
				DB* db,
				const RowIdList& rowIds
			);
		};



		template<class K, class T>
		void FullSynchronizationPolicy<K, T>::RowsRemoved(
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
						K::Unlink(registry.getEditable(rowId).get());
						registry.remove(rowId);
					}
				}
				catch (Exception& e)
				{
					util::Log::GetInstance().warn("Error on unload after row deletion : ", e);
				}
			}
		}


		template<class K, class T>
		void FullSynchronizationPolicy<K, T>::RowsUpdated(
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
						K::Unlink(address.get());
						K::Load(address.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
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



		template<class K, class T>
		void FullSynchronizationPolicy<K, T>::RowsAdded(
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
						K::Unlink(address.get());
						K::Load (address.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
					}
					else
					{
						boost::shared_ptr<T> object(K::GetNewObject(*rows));
						registry.add(object);
						K::Load(object.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
					}
				}
				catch(Exception& e)
				{
					if(registry.contains(key))
					{
						registry.remove(key);
					}
					util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : " + boost::lexical_cast<std::string>(key) + " : ", e);
				}
			}
		}
	}
}

#endif // SYNTHESE_db_FullSynchronizationPolicy_hpp__

