
/** Field class header.
	@file Field.hpp

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

#ifndef SYNTHESE__Field_hpp__
#define SYNTHESE__Field_hpp__

#include <string>
#include <vector>

namespace synthese
{
	////////////////////////////////////////////////////////////////////
	/// Field types.
	typedef enum
	{
		SQL_INTEGER = 0,
		SQL_DOUBLE = 1,
		SQL_TEXT = 2,
		// Text with a fixed length. It has to be used instead of SQL_TEXT on the first column.
		// That's because MySQL creates a primary key on the first column, and needs a fixed
		// length if the type is text.
		// TODO: remove this workaround once we use VARCHAR with a fixed length on MySQL.
		SQL_TEXT_FIXED = 3,
		SQL_BOOLEAN = 4,
		SQL_DATETIME = 5,
		SQL_DATE = 6,
		SQL_TIME = 7,
		SQL_GEOM_POINT = 8,
		SQL_GEOM_LINESTRING = 9,
		SQL_GEOM_POLYGON = 10,
		SQL_GEOM_MULTIPOINT = 11,
		SQL_GEOM_MULTILINESTRING = 12,
		SQL_GEOM_MULTIPOLYGON = 13,
		SQL_GEOM_GEOMETRYCOLLECTION = 14,
		SQL_BLOB = 15
	} FieldType;


	/** Field class.
		@ingroup m
	*/
	class Field
	{
	public:
		std::string	name;
		FieldType	type;
		bool		exportOnFile;


		////////////////////////////////////////////////////////////////////
		///	Field constructor.
		///	@param nameA name of the field
		///	@param typeA type of the field
		///	@author Hugues Romain
		///	@date 2008
		Field(
			const std::string nameA = std::string(),
			const FieldType typeA = SQL_TEXT,
			bool exportOnFile = false
		);



		////////////////////////////////////////////////////////////////////
		///	Tests if the field is empty.
		///	@return bool true if the field is empty
		///	@author Hugues Romain
		///	@date 2008
		bool empty() const;



		//////////////////////////////////////////////////////////////////////////
		/// Tests if the field type corresponds to a geometry.
		/// @return true if the field corresponds to a geometry
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.2.0
		bool isGeometry() const;
	};



	typedef std::vector<Field> FieldsList;
}

#endif // SYNTHESE__Field_hpp__

