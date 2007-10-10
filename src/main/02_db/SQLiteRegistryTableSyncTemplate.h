
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

#include "SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace db
	{
		/** SQLiteRegistryTableSyncTemplate class.
			@ingroup m02
		*/
		template<class K, class T>
		class SQLiteRegistryTableSyncTemplate : public SQLiteTableSyncTemplate<K,T>
		{
		public:
			SQLiteRegistryTableSyncTemplate() : SQLiteTableSyncTemplate<K,T>() {}

		protected:

			/** Action to do on DisplayType creation.
			This method loads a new object in ram.
			*/
			void rowsAdded(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
				, bool isFirstSync = false
			){
				while (rows->next ())
				{
					if (T::Contains(rows->getLongLong (TABLE_COL_ID)))
					{
						boost::shared_ptr<T> address(T::GetUpdateable(rows->getLongLong (TABLE_COL_ID)));
						SQLiteTableSyncTemplate<K,T>::unlink(address.get());
						load (address.get(), rows);
						    SQLiteTableSyncTemplate<K,T>::link(address.get(), rows, GET_REGISTRY);
					}
					else
					{
						T* object(new T);
						load(object, rows);
						    SQLiteTableSyncTemplate<K,T>::link(object, rows, GET_REGISTRY);
						object->store();
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
				while (rows->next ())
				{
					uid id = rows->getLongLong (TABLE_COL_ID);
					if (T::Contains(id))
					{
						boost::shared_ptr<T> address(T::GetUpdateable(id));
						    SQLiteTableSyncTemplate<K,T>::unlink(address.get());
						load (address.get(), rows);
						    SQLiteTableSyncTemplate<K,T>::link(address.get(), rows, GET_REGISTRY);
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
				while (rows->next ())
				{
					uid id = rows->getLongLong (TABLE_COL_ID);
					if (T::Contains(id))
					{
					    SQLiteTableSyncTemplate<K,T>::unlink(T::GetUpdateable(id).get());
						T::Remove(id);
					}
				}
			}

		};
	}
}

#endif // SYNTHESE_db_SQLiteRegistryTableSyncTemplate_h__
