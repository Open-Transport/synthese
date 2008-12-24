
/** SQLiteRegistryTableSyncTemplate class header.
	@file SQLiteRegistryTableSyncTemplate.h

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

#ifndef SYNTHESE_db_SQLiteRegistryTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteRegistryTableSyncTemplate_h__

#include "SQLiteDirectTableSyncTemplate.h"

#include "01_util/Exception.h"
#include "01_util/Log.h"

namespace synthese
{
	namespace db
	{
		/** SQLiteRegistryTableSyncTemplate class.
				- class K : Class of the table sync
				- class T : Class of the corresponding objects
			@ingroup m10
		*/
		template<class K, class T>
		class SQLiteRegistryTableSyncTemplate : public SQLiteDirectTableSyncTemplate<K,T>
		{
		public:
			SQLiteRegistryTableSyncTemplate() : SQLiteDirectTableSyncTemplate<K,T>() {}

			static SQLiteTableFormat CreateFormat(
				std::string name,
				SQLiteTableFormat::Fields fields,
				SQLiteTableFormat::Indexes indexes = SQLiteTableFormat::Indexes()
			){
				return SQLiteDirectTableSyncTemplate<K,T>::CreateFormat(
					name,
					fields,
					indexes,
					false
				);
			}

			/** Action to do on DisplayType creation.
			This method loads a new object in ram.
			*/
			void rowsAdded(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
				, bool isFirstSync = false
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				while (rows->next ())
				{
					try
					{
						if (registry.contains(rows->getLongLong (TABLE_COL_ID)))
						{
							boost::shared_ptr<T> address(registry.getEditable(rows->getKey()));
							SQLiteDirectTableSyncTemplate<K,T>::Unlink(address.get());
							Load (address.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
						}
						else
						{
							boost::shared_ptr<T> object(K::GetNewObject(rows));
							Load(object.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
							registry.add(object);
						}
					}
					catch(util::Exception& e)
					{
						util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : ", e);
					}
				}
			}


			/** Action to do on DisplayType creation.
			This method updates the corresponding object in ram.
			*/
			void rowsUpdated(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
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
							Unlink(address.get());
							Load(address.get(), rows, env, util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
						}
					}
					catch (util::Exception& e)
					{
						util::Log::GetInstance().warn("Error on load after row update : ", e);
					}
				}
			}


			/** Action to do on DisplayType deletion.
			This method deletes the corresponding object in ram and runs 
			all necessary cleaning actions.
			*/
			void rowsRemoved(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
			){
				util::Env& env(util::Env::GetOfficialEnv());
				util::Registry<T>& registry(env.getEditableRegistry<T>());
				while (rows->next ())
				{
					try
					{
						util::RegistryKeyType id(rows->getKey());
						if (registry.contains(id))
						{
							SQLiteDirectTableSyncTemplate<K,T>::Unlink(registry.getEditable(id).get());
							registry.remove(id);
						}
					}
					catch (util::Exception& e)
					{
						util::Log::GetInstance().warn("Error on unload after row deletion : ", e);
					}
				}
			}

		};
	}
}

#endif // SYNTHESE_db_SQLiteRegistryTableSyncTemplate_h__
