
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
		/// SQLite data types (see http://www.sqlite.org/datatype3.html and http://www.gaia-gis.it/spatialite/spatialite-sql-2.3.1.html#p16)
		typedef enum
		{
			SQL_INTEGER,
			SQL_DOUBLE,
			SQL_TEXT,
			SQL_REAL,
			SQL_BOOLEAN,
			SQL_TIME,
			SQL_DATE,
			SQL_TIMESTAMP,
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

		typedef unsigned int SRID;
	}
}

#endif // SYNTHESE_db_Types_h__



