
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

#include "02_db/Constants.h"

#include "01_util/ConstantReturner.h"

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
				const SQLiteResultSPtr& rows, bool isFirstSync = false)
			{
				try
				{
					if (ObjectClass::Contains(rows->getLongLong (TABLE_COL_ID)))
					{
						ObjectClass* address(static_cast<ObjectClass*>(ObjectClass::GetUpdateable(rows->getLongLong (TABLE_COL_ID)).get()));
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Unlink(address);
						Load (address, rows);
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Link(address, rows, GET_REGISTRY);
					}
					else
					{
						ObjectClass* object(new ObjectClass);
						Load(object, rows);
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Link(object, rows, GET_REGISTRY);
						object->store();
					}
				}
				catch(util::Exception& e)
				{
					util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : ", e);
				}
			}

			void rowsUpdated (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows)
			{
				try
				{
					uid id = rows->getLongLong (TABLE_COL_ID);
					if (ObjectClass::Contains(id))
					{
						ObjectClass* address(static_cast<ObjectClass*>(ObjectClass::GetUpdateable(id).get()));
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Unlink(address);
						Load (address, rows);
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Link(address, rows, GET_REGISTRY);
					}
				}
				catch (util::Exception& e)
				{
					util::Log::GetInstance().warn("Error on load after row update : ", e);
				}
			}

			void rowsRemoved (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows)
			{
				try
				{
					uid id = rows->getLongLong (TABLE_COL_ID);
					if (ObjectClass::Contains(id))
					{
						SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>::Unlink(static_cast<ObjectClass*>(ObjectClass::GetUpdateable(id).get()));
						ObjectClass::Remove(id);
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
