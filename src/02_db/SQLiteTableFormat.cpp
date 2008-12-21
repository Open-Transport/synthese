
/** SQLiteTableFormat class implementation.
	@file SQLiteTableFormat.cpp

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

#include <boost/foreach.hpp>
#include <stdarg.h>
#include <sstream>

#include "SQLiteTableFormat.h"
#include "SQLiteException.h"
#include "Conversion.h"

using namespace std;

namespace synthese
{
	using namespace util;
	
	namespace db
	{
		SQLiteTableFormat::SQLiteTableFormat(
			const std::string& name,
			bool hasAutoIncrement,
			const std::string& triggerOverrideClause,
			bool ignoreCallbacksOnFirstSync,
			bool enableTriggers,
			const Fields& fields,
			const Indexes& indexes
		):	NAME(name),
			ID(_ParseTableId(name)),
			HAS_AUTO_INCREMENT(hasAutoIncrement),
			TRIGGER_OVERRIDE_CLAUSE(triggerOverrideClause),
			IGNORE_CALLBACKS_ON_FIRST_SYNC(ignoreCallbacksOnFirstSync),
			ENABLE_TRIGGERS(enableTriggers),
			FIELDS(fields),
			INDEXES(indexes)
		{
			if (FIELDS.empty())
			{
				throw SQLiteException("Table "+ NAME +" has no field.");
			}
			
			BOOST_FOREACH(const Field& field, FIELDS)
			{
				if (field.name.empty())
				{
					throw SQLiteException("Table "+ NAME +" has at least a field without name.");
					break;
				}
			}
		}



		SQLiteTableFormat::~SQLiteTableFormat ()
		{

		}



		bool SQLiteTableFormat::hasTableColumn(
			const std::string& name
		) const	{
			BOOST_FOREACH(const Field& field, FIELDS)
			{
				if(field.name == name) return true;
			}
			return false;
		}



		bool 
		SQLiteTableFormat::hasNonUpdatableColumn () const
		{
			BOOST_FOREACH(const Field& field, FIELDS)
			{
				if (!field.updatable)
					return true;
			}
			return false;
		}



		SQLiteTableFormat::Fields SQLiteTableFormat::CreateFields(
			Field first, ...
		){
			Fields result;
			Field col(first);
			va_list marker;
			va_start(marker, first);
			while(!col.name.empty())
			{
				result.push_back(col);
				col = va_arg( marker, Field);
			}
			va_end( marker );
			return result;
		}



		SQLiteTableFormat::Indexes SQLiteTableFormat::CreateIndexes(
			Index first, ...
		) {
			Indexes result;
			Index col(first);
			va_list marker;
			va_start(marker, first);
			while(!col.name.empty())
			{
				result.push_back(col);
				col = va_arg( marker, Index);
			}
			va_end( marker );
			return result;
		}



		SQLiteTableFormat::Index::Index(
			std::string nameA,
			std::vector<std::string> fieldsA
		):	name(nameA),
			fields(fieldsA.empty() ? CreateFieldsList(nameA, string()) : fieldsA)
		{

		}



		SQLiteTableFormat::Field::Field(
			std::string nameA,
			FieldType typeA,
			bool updatableA
		):	name(nameA),
			type(typeA),
			updatable(updatableA)
		{

		}



		SQLiteTableFormat::TableId SQLiteTableFormat::_ParseTableId(
			const std::string& tableName
		){
			if(tableName.size() < 6)
				throw SQLiteException("Inconsistent table name in parse table id");
			
			TableId id(Conversion::ToInt(tableName.substr (1, 4)));
			
			if (id <= 0)
				throw SQLiteException("Inconsistent table name in parse table id");

			return id;
		}



		std::string SQLiteTableFormat::getSQLFieldsSchema(
		) const {
			stringstream sql;
			sql << "CREATE TABLE " << NAME << " ("
				<< FIELDS[0].name << " " << _GetSQLType(FIELDS[0].type)
				<< " UNIQUE PRIMARY KEY ON CONFLICT ROLLBACK";

			for (size_t i=1; i<FIELDS.size(); ++i)
			{
				sql << ", " << FIELDS[i].name << " " << _GetSQLType(FIELDS[i].type);
			}
			sql << ")";
			return sql.str();
		}



		std::string SQLiteTableFormat::_GetSQLType(
			FieldType type
		){
			switch(type)
			{
			case INTEGER:
				return "INTEGER";
			case DOUBLE:
				return "DOUBLE";
			case REAL:
				return "REAL";
			case TEXT:
				return "TEXT";
			case BOOLEAN:
				return "BOOLEAN";
			case TIME:
				return "TIME";
			case DATE:
				return "DATE";
			case TIMESTAMP:
				return "TIMESTAMP";
			}
			return string();
		}



		std::string SQLiteTableFormat::getSQLTriggerNoUpdate(
		) const {
			std::vector<std::string> nonUpdatableColumns;
			BOOST_FOREACH(const Field& field, FIELDS)
			{
				if(field.updatable) continue;

				nonUpdatableColumns.push_back(field.name);
			}

			// If no non updatable field, return empty trigger
			if (nonUpdatableColumns.empty()) return string();

			stringstream columnList;
			BOOST_FOREACH(const string& field, nonUpdatableColumns)
			{
				if (!columnList.str().empty())
					columnList << ", ";
				columnList << field;
			}

			stringstream sql;
			sql << "CREATE TRIGGER "
				<< NAME << "_no_update"
				<< " BEFORE UPDATE OF "
				<< columnList.str() << " ON " << NAME
				<< " BEGIN SELECT RAISE (ABORT, 'Update of "
				<< columnList.str() << " in " << NAME << " is forbidden.') WHERE "
				<< TRIGGER_OVERRIDE_CLAUSE << "; END";

			return sql.str();
		}



		std::string SQLiteTableFormat::getSQLIndexSchema(
			const Index& index
		) const {
			// Creation of the statement
			stringstream s;
			s	<< "CREATE INDEX " << getIndexDBName(index)
				<< " ON " << NAME << "(";
			for (Index::Fields::const_iterator it = index.fields.begin(); it != index.fields.end(); ++it)
			{
				if (it != index.fields.begin())
					s << ",";
				s << *it;
			}
			s << ")";

			return s.str();
		}



		std::string SQLiteTableFormat::getIndexDBName(
			const Index& index
		) const {
			stringstream s;
			s << NAME << "_" << index.name;
			return s.str();
		}



		SQLiteTableFormat::Index::Fields SQLiteTableFormat::Index::CreateFieldsList(
			std::string first, ...
		){
			Fields result;
			string col(first);
			va_list marker;
			va_start(marker, first);
			while(!col.empty())
			{
				result.push_back(col);
				col = va_arg( marker, string);
			}
			va_end( marker );
			return result;
		}
	}
}
