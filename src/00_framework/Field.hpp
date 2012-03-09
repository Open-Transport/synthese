
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

#include <vector>

namespace synthese
{
	////////////////////////////////////////////////////////////////////
	/// Field types.
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


	/** Field class.
		@ingroup m
	*/
	class Field
	{
	public:
		std::string	name;
		FieldType	type;


		////////////////////////////////////////////////////////////////////
		///	Field constructor.
		///	@param nameA name of the field
		///	@param typeA type of the field
		///	@author Hugues Romain
		///	@date 2008
		Field(
			const std::string nameA = std::string(),
			const FieldType typeA = SQL_TEXT
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

