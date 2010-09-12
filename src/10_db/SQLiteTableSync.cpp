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

#include "UtilConstants.h"
#include "DBConstants.h"
#include "SQLite.h"
#include "SQLiteTableSync.h"
#include "SQLiteException.h"
#include "Conversion.h"

#include <sstream>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace util;

    namespace db
    {
		SQLiteTableSync::SQLiteTableSync(
		):	FactoryBase<SQLiteTableSync>()
		{

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
		




		std::string SQLiteTableSync::GetSQLSchemaDb(
			SQLite* sqlite,
			 const std::string& tableName
		){
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='table' and name='" +
			tableName + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (!result->next()) return "";
			return result->getText (0);
		}



		std::string 
		SQLiteTableSync::GetTriggerNoUpdateDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName)
		{
			std::string sql = "SELECT sql FROM SQLITE_MASTER where type='trigger' and name='" +
			tableName + "_no_update" + "'";
			SQLiteResultSPtr result = sqlite->execQuery (sql);
			if (!result->next()) return "";
			return result->getText (0);
		}



		SQLiteTableSync::Field::Field(
			const std::string nameA, // std::string()
			const FieldType typeA, // SQL_TEXT
			const bool updatableA // true 
		):	name(nameA),
			type(typeA),
			updatable(updatableA)
		{
		}



		std::string SQLiteTableSync::Field::getSQLType(
		) const {
			switch(type)
			{
			case SQL_INTEGER:
				return "INTEGER";
			case SQL_DOUBLE:
				return "DOUBLE";
			case SQL_REAL:
				return "REAL";
			case SQL_TEXT:
				return "TEXT";
			case SQL_BOOLEAN:
				return "BOOLEAN";
			case SQL_TIME:
				return "TIME";
			case SQL_DATE:
				return "DATE";
			case SQL_TIMESTAMP:
				return "TIMESTAMP";
			default:
				if(isGeometry())
				{
					return "BLOB";
				}
			}
			return string();
		}



		bool SQLiteTableSync::Field::empty(
		) const {
			return name.empty();
		}



		bool SQLiteTableSync::Field::isGeometry() const
		{
			switch(type)
			{
				case SQL_GEOM_GEOMETRYCOLLECTION:
				case SQL_GEOM_LINESTRING:
				case SQL_GEOM_MULTILINESTRING:
				case SQL_GEOM_MULTIPOINT:
				case SQL_GEOM_MULTIPOLYGON:
				case SQL_GEOM_POINT:
				case SQL_GEOM_POLYGON:
					return true;
				default:
					return false;
			}
		}



		std::string SQLiteTableSync::Field::getGeometryType() const
		{
			switch(type)
			{
			case SQL_GEOM_GEOMETRYCOLLECTION:
				return "GEOMETRYCOLLECTION";
			case SQL_GEOM_LINESTRING:
				return "LINESTRING";
			case SQL_GEOM_MULTILINESTRING:
				return "MULTILINESTRING";
			case SQL_GEOM_MULTIPOINT:
				return "MULTIPOINT";
			case SQL_GEOM_MULTIPOLYGON:
				return "MULTIPOLYGON";
			case SQL_GEOM_POINT:
				return "POINT";
			case SQL_GEOM_POLYGON:
				return "POLYGON";
			default:
				return string();
			}
		}



		SQLiteTableSync::Index::Index(
			const char* first,
			...
		){
			stringstream names;
			const char* col(first);
			va_list marker;
			for(va_start(marker, first); col[0]; col = va_arg(marker, const char*))
			{
				fields.push_back(string(col));
			}
			va_end(marker);
		}



		SQLiteTableSync::Index::Index(
		){
		}



		bool SQLiteTableSync::Index::empty(
		) const {
			return fields.empty() || fields[0].empty();
		}

		SQLiteTableSync::Format::Format(
			const std::string& name,
			const bool ignoreCallbacksOnFirstSync, // false
			const bool enableTriggers, // true
			const std::string& triggerOverrideClause, // TRIGGERS_ENABLED_CLAUSE
			const bool hasAutoIncrement // true
		):
			NAME(name),
			HAS_AUTO_INCREMENT(hasAutoIncrement),
			TRIGGER_OVERRIDE_CLAUSE(triggerOverrideClause),
			IGNORE_CALLBACKS_ON_FIRST_SYNC(ignoreCallbacksOnFirstSync),
			ENABLE_TRIGGERS(enableTriggers)
		{
			if(name.size() < 6)
			{
				throw SQLiteException("Inconsistent table name in parse table id");
			}

			ID = lexical_cast<TableId>(name.substr(1, 3));

			if (ID == 0)
			{
				throw SQLiteException("Inconsistent table name in parse table id");
			}
		}
	}
}

