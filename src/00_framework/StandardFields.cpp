
/** StandardFieldNames class implementation.
	@file StandardFieldNames.cpp

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

#include "StandardFields.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	// Simple types
	FIELD_DEFINITION_OF_TYPE(Date, "date", SQL_DATE)
	FIELD_DEFINITION_OF_TYPE(EndTime, "end_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(EndDate, "end_date", SQL_DATE)
	FIELD_DEFINITION_OF_TYPE(Key, "id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Name, "name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Rank, "rank", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(StartTime, "start_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(StartDate, "start_date", SQL_DATE)
	FIELD_DEFINITION_OF_TYPE(Title, "title", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(PointGeometry, "geometry", SQL_GEOM_POINT)
	FIELD_DEFINITION_OF_TYPE(LineStringGeometry, "geometry", SQL_GEOM_LINESTRING)
}

