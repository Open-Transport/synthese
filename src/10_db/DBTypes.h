
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
		/// SQLite data types (see http://www.sqlite.org/datatype3.html)
		typedef enum
		{
			SQL_INTEGER,
			SQL_DOUBLE,
			SQL_TEXT,
			SQL_REAL,
			SQL_BOOLEAN,
			SQL_TIME,
			SQL_DATE,
			SQL_TIMESTAMP
		} FieldType;

		typedef enum
		{
			AUTO_CREATE,
			NEVER_CREATE
		} AutoCreation;
	}
}

#endif // SYNTHESE_db_Types_h__



