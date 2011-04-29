
/** Types class header.
	@file DBTypes.h

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

#ifndef SYNTHESE_db_Types_h__
#define SYNTHESE_db_Types_h__

namespace synthese
{
	namespace db
	{
		////////////////////////////////////////////////////////////////////
		/// Database column types.
		typedef enum
		{
			SQL_INTEGER,
			SQL_DOUBLE,
			SQL_TEXT,
			// Text with a fixed length. It has to be used instead of SQL_TEXT on the first column.
			// That's because MySQL creates a primary key on the first column, and needs a fixed
			// length if the type is text.
			// TODO: remove this workaround once we use VARCHAR with a fixed length on MySQL.
			SQL_TEXT_FIXED,
			SQL_BOOLEAN,
			SQL_DATETIME,
			SQL_DATE,
			SQL_TIME,
			SQL_GEOM_POINT,
			SQL_GEOM_LINESTRING,
			SQL_GEOM_POLYGON,
			SQL_GEOM_MULTIPOINT,
			SQL_GEOM_MULTILINESTRING,
			SQL_GEOM_MULTIPOLYGON,
			SQL_GEOM_GEOMETRYCOLLECTION
		} FieldType;

		typedef enum
		{
			AUTO_CREATE,
			NEVER_CREATE
		} AutoCreation;
}	}

#endif // SYNTHESE_db_Types_h__



