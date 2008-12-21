////////////////////////////////////////////////////////////////////////////////
/// SQLiteTableSync class implementation.
///	@file SQLiteTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-14 0:29
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

#include "02_db/Constants.h"
#include "SQLite.h"
#include "SQLiteTableSync.h"
#include "SQLiteException.h"
#include "Conversion.h"

#include <sstream>
#include <boost/thread/mutex.hpp>

using namespace std;

namespace synthese
{
    using namespace util;

    namespace db
    {
		SQLiteTableSync::SQLiteTableSync(
		){

		}


		SQLiteTableSync::~SQLiteTableSync ()
		{

		}


	
		std::vector<std::string> 
		SQLiteTableSync::GetTableColumnsDb (synthese::db::SQLite* sqlite,
							const std::string& tableName)
		{
			std::vector<std::string> cols;
			std::string sql = "PRAGMA TABLE_INFO (" + tableName + ")";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			while (result->next ())
			{
			    cols.push_back (result->getText (1));
			}
			return cols;
		}
		




		std::string 
		SQLiteTableSync::GetSQLSchemaDb (synthese::db::SQLite* sqlite,
						 const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='table' and name='" +
			tableName + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (result->next() == false) return "";
			return result->getText (0);
		}



		std::string 
		SQLiteTableSync::GetTriggerNoUpdateDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_update" + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (result->next () == false) return "";
			return result->getText (0);
		}
    }
}

