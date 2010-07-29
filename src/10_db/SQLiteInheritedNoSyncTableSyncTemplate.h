
/** SQLiteInheritedNoSyncTableSyncTemplate class header.
	@file SQLiteInheritedNoSyncTableSyncTemplate.h

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

#ifndef SYNTHESE_db_SQLiteInheritedNoSyncTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteInheritedNoSyncTableSyncTemplate_h__

#include "SQLiteInheritedTableSyncTemplate.h"

#include "01_util/ConstantReturner.h"

namespace synthese
{
	namespace db
	{
		class SQLite;
		class SQLiteSync;

		/** SQLiteInheritedNoSyncTableSyncTemplate class.
			@ingroup m10
		*/
		template<class ParentTableSyncClass, class TableSyncClass, class ObjectClass>
		class SQLiteInheritedNoSyncTableSyncTemplate
			: public SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>
		{
		public:
			typedef util::ConstantReturnerFalse RegisterInSubClassMap;

			SQLiteInheritedNoSyncTableSyncTemplate()
				: SQLiteInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>()
			{

			}

			void rowsAdded (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows
			){
			}

			void rowsUpdated (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows)
			{

			}

			void rowsRemoved (SQLite* sqlite, 
				SQLiteSync* sync,
				const SQLiteResultSPtr& rows)
			{

			}

		};
	}
}

#endif // SYNTHESE_db_SQLiteInheritedNoSyncTableSyncTemplate_h__
