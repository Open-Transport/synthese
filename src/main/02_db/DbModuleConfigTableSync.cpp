
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
#include "02_db/SQLite.h"


#include "DbModuleClass.h"
#include "Constants.h"



namespace synthese
{
	template<> const std::string util::FactorableTemplate<db::SQLiteTableSync, db::DbModuleConfigTableSync>::FACTORY_KEY("999 db config");

	namespace db
	{

	    const std::string DbModuleConfigTableSync::TABLE_NAME ("t999_config");
	    const std::string DbModuleConfigTableSync::COL_PARAMNAME ("param_name");
	    const std::string DbModuleConfigTableSync::COL_PARAMVALUE ("param_value");


		DbModuleConfigTableSync::DbModuleConfigTableSync ()
			: util::FactorableTemplate<synthese::db::SQLiteTableSync,DbModuleConfigTableSync>()// (TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (COL_PARAMNAME, "TEXT", false);
			addTableColumn (COL_PARAMVALUE, "TIMESTAMP", true);
		}



		DbModuleConfigTableSync::~DbModuleConfigTableSync ()
		{

		}

		    


		void 
		DbModuleConfigTableSync::rowsAdded (SQLite* sqlite, 
						    SQLiteSync* sync,
						    const SQLiteResultSPtr& rows, bool isFirstSync)
		{
		    while (rows->next ())
		    {
			DbModuleClass::SetParameter (rows->getText (COL_PARAMNAME), 
						     rows->getText (COL_PARAMVALUE));
		    }
		    
		}



		void 
		DbModuleConfigTableSync::rowsUpdated (SQLite* sqlite, 
							SQLiteSync* sync,
							const SQLiteResultSPtr& rows)
		{
		    rowsAdded (sqlite, sync, rows);
		}



		void 
		DbModuleConfigTableSync::rowsRemoved (SQLite* sqlite, 
							SQLiteSync* sync,
							const SQLiteResultSPtr& rows)
		{

		}

		const std::string& DbModuleConfigTableSync::getTableName() const
		{
			return TABLE_NAME;
		}



	}
}

