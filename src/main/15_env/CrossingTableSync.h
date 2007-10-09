
/** CrossingTableSync class header.
	@file CrossingTableSync.h

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


#ifndef SYNTHESE_ENVLSSQL_CROSSINGTABLESYNC_H
#define SYNTHESE_ENVLSSQL_CROSSINGTABLESYNC_H

#include "Crossing.h"

#include "02_db/SQLiteRegistryTableSyncTemplate.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace env
	{
		/** Crossing SQLite table synchronizer.
		    The memory registry for this table is connection places registry.
		    Crossing are just a special kind of connection place (road-road) that we want 
		    to keep in a separate table.

		    @ingroup m15LS refLS
		    
		    Connection places table :
		    - on insert : 
		    - on update : 
		    - on delete : X
		*/
		class CrossingTableSync : public db::SQLiteRegistryTableSyncTemplate<CrossingTableSync,Crossing>
		{
		public:
			static const std::string TABLE_COL_CITYID;

			CrossingTableSync ();
			~CrossingTableSync ();
		};
	}
}
#endif
