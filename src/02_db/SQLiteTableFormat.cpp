
/** SQLiteTableSync::Formatclass implementation.
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
	}
}
