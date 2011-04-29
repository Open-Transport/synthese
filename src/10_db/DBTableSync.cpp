////////////////////////////////////////////////////////////////////////////////
/// DBTableSync class implementation.
///	@file DBTableSync.cpp
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
#include "DB.hpp"
#include "DBConstants.h"
#include "DBTableSync.hpp"
#include "DBException.hpp"
#include "Conversion.h"

#include <sstream>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <stdio.h> // va_start
#include <stdarg.h> // va_start

using namespace std;
using namespace boost;

namespace synthese
{
    using namespace util;

    namespace db
    {
		DBTableSync::DBTableSync(
		):	FactoryBase<DBTableSync>()
		{

		}


		DBTableSync::~DBTableSync ()
		{

		}



		DBTableSync::Field::Field(
			const std::string nameA, // std::string()
			const FieldType typeA // SQL_TEXT
		):	name(nameA),
			type(typeA)
		{
		}



		bool DBTableSync::Field::empty(
		) const {
			return name.empty();
		}



		bool DBTableSync::Field::isGeometry() const
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



		DBTableSync::Index::Index(
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



		DBTableSync::Index::Index(
		){
		}



		bool DBTableSync::Index::empty(
		) const {
			return fields.empty() || fields[0].empty();
		}

		DBTableSync::Format::Format(
			const std::string& name,
			const bool ignoreCallbacksOnFirstSync, // false
			const bool hasAutoIncrement // true
		):
			NAME(name),
			HAS_AUTO_INCREMENT(hasAutoIncrement),
			IGNORE_CALLBACKS_ON_FIRST_SYNC(ignoreCallbacksOnFirstSync)
		{
			if(name.size() < 6)
			{
				throw DBException("Inconsistent table name in parse table id");
			}

			ID = lexical_cast<TableId>(name.substr(1, 3));

			if (ID == 0)
			{
				throw DBException("Inconsistent table name in parse table id");
			}
		}
	}
}

