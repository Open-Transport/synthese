
/**	Object  class implementation.
	@file Object.cpp

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

#include "Object.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	// Simple types
	FIELD_DEFINITION_OF_TYPE(Key, "id", SQL_INTEGER)

/*		void ObjectField<Key, util::RegistryKeyType>::UnSerialize(
		util::RegistryKeyType& fieldObject,
		const std::string& text,
		const util::Env& env
		){
			if(!text.empty())
			{
				fieldObject = boost::lexical_cast<util::RegistryKeyType>(text);
			}
			else
			{
				fieldObject = static_cast<util::RegistryKeyType>(0);
			}
	}
	std::string ObjectField<Key, util::RegistryKeyType>::Serialize(
		const util::RegistryKeyType& fieldObject,
		SerializationFormat format
		){
			return boost::lexical_cast<std::string>(fieldObject);
	}


	void ObjectField<Key, util::RegistryKeyType>::GetLinkedObjectsIdsFromText(
		LinkedObjectsIds& list,
		const std::string& text
		){}
*/
}
