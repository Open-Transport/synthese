
/** Field class implementation.
	@file Field.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "Field.hpp"

namespace synthese
{
	Field::Field(
		const std::string nameA /*= std::string()*/,
		const FieldType typeA /*= SQL_TEXT */,
		bool exportOnFilesA
	):	name(nameA),
		type(typeA),
		exportOnFile(exportOnFilesA)
	{}



	bool Field::empty() const
	{
		return name.empty();
	}



	bool Field::isGeometry() const
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
}	}
