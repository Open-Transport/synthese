
/** DbModuleConfigTableSync class implementation.
	@file DbModuleConfigTableSync.cpp

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


#include "DbModuleConfigTableSync.h"

#include "ModuleClass.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	
	template<> const string util::FactorableTemplate<DBTableSync,DbModuleConfigTableSync>::FACTORY_KEY(
		"999 db config"
	);

	namespace server
	{
		const string DbModuleConfigTableSync::COL_PARAMNAME("param_name");
		const string DbModuleConfigTableSync::COL_PARAMVALUE("param_value");
	}

	namespace db
	{
	    template<> const DBTableSync::Format DBTableSyncTemplate<DbModuleConfigTableSync>::TABLE(
			"t999_config", false, false
			);

		template<> const DBTableSync::Field DBTableSyncTemplate<DbModuleConfigTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(DbModuleConfigTableSync::COL_PARAMNAME, SQL_TEXT_FIXED),
			DBTableSync::Field(DbModuleConfigTableSync::COL_PARAMVALUE, SQL_DATETIME),
			DBTableSync::Field()
		};
		
		template<> const DBTableSync::Index DBTableSyncTemplate<DbModuleConfigTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};
	}

	namespace server
	{
		void 
		DbModuleConfigTableSync::rowsAdded(
			DB* db, 
			const DBResultSPtr& rows
		){
			while (rows->next ())
			{
				ModuleClass::SetParameter (rows->getText (COL_PARAMNAME), 
						     rows->getText (COL_PARAMVALUE));
			}
		}



		void DbModuleConfigTableSync::rowsUpdated(
			DB* db, 
			const DBResultSPtr& rows
		){
		    rowsAdded (db, rows);
		}



		void 
		DbModuleConfigTableSync::rowsRemoved(
			DB* db,
			const RowIdList& rowIds
		){
		}



		const std::string& DbModuleConfigTableSync::getTableName() const
		{
			return TABLE.NAME;
		}
	}
}

