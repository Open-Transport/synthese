
/** ComplexObjectFieldDefinition class header.
	@file ComplexObjectFieldDefinition.hpp

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

#ifndef SYNTHESE__ComplexObjectFieldDefinition_hpp__
#define SYNTHESE__ComplexObjectFieldDefinition_hpp__

#include "Field.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Template to use to define a complex field set of a schema.
	///	@ingroup m00
	template<class C>
	class ComplexObjectFieldDefinition
	{
	public:
		static const Field FIELDS[];
		static void AddFields(FieldsList& l)
		{
			for(size_t i(0); !FIELDS[i].empty(); ++i)
			{
				l.push_back(FIELDS[i]);
			}
		}

		static const std::string& GetFieldKey()
		{
			return FIELDS[0].name;
		}
	};
}

#endif // SYNTHESE__ComplexObjectFieldDefinition_hpp__

