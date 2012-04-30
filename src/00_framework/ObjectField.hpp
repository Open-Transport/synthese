
/** ObjectField class header.
	@file ObjectField.hpp

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

#ifndef SYNTHESE__ObjectField_hpp__
#define SYNTHESE__ObjectField_hpp__

#include "SimpleObjectField.hpp"

#include "ParametersMap.h"
#include "Record.hpp"
#include "ObjectBase.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Field of standard object using various database fields.
	/// ComplexObjectField are part of schemes of Object classes.
	/// Partial and total template specializations are present in StandardFields.hpp
	///	@ingroup m00
	/// @author Hugues Romain
	/// @date 2012
	template<class C, class T>
	class ObjectField:
		public SimpleObjectField<C, T>
	{
	public:
		typedef T Type;

		static void UnSerialize(
			T& fieldObject,
			const std::string& text,
			const util::Env& env
		);



		static std::string Serialize(
			const T& fieldObject,
			util::ParametersMap::SerializationFormat format
		);



		static void GetLinkedObjectsIdsFromText(
			LinkedObjectsIds& list,
			const std::string& text
		);
	};
}

#endif
