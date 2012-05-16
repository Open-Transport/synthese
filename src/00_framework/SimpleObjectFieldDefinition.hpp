
/** SimpleObjectFieldDefinition class header.
	@file SimpleObjectFieldDefinition.hpp

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

#ifndef SYNTHESE__ObjectFieldDefinition_hpp__
#define SYNTHESE__ObjectFieldDefinition_hpp__

#include "Field.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Template to use to define a simple field of a schema.
	///	@ingroup m00
	template<class C>
	class SimpleObjectFieldDefinition
	{
	public:
		static const bool EXPORT_CONTENT_AS_FILE;
		static const Field FIELD;
		static void AddFields(FieldsList& l)
		{
			l.push_back(FIELD);
		}
	};
}

#endif // SYNTHESE__ObjectFieldDefinition_hpp__

