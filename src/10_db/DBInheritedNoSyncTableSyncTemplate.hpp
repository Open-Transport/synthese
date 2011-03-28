
/** DBInheritedNoSyncTableSyncTemplate class header.
	@file DBInheritedNoSyncTableSyncTemplate.hpp

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

#ifndef SYNTHESE_db_DBInheritedNoSyncTableSyncTemplate_hpp__
#define SYNTHESE_db_DBInheritedNoSyncTableSyncTemplate_hpp__

#include "DBInheritedTableSyncTemplate.hpp"

#include "01_util/ConstantReturner.h"

namespace synthese
{
	namespace db
	{
		class DB;

		/** DBInheritedNoSyncTableSyncTemplate class.
			@ingroup m10
		*/
		template<class ParentTableSyncClass, class TableSyncClass, class ObjectClass>
		class DBInheritedNoSyncTableSyncTemplate
			: public DBInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>
		{
		public:
			typedef util::ConstantReturnerFalse RegisterInSubClassMap;

			DBInheritedNoSyncTableSyncTemplate()
				: DBInheritedTableSyncTemplate<ParentTableSyncClass,TableSyncClass,ObjectClass>()
			{

			}

			void rowsAdded (
				DB* db, 
				const DBResultSPtr& rows
			){

			}

			void rowsUpdated (
				DB* db, 
				const DBResultSPtr& rows
			){

			}

			void rowsRemoved (
				DB* db, 
				const RowIdList& rowIds
			){

			}

		};
	}
}

#endif // SYNTHESE_db_DBInheritedNoSyncTableSyncTemplate_hpp__
