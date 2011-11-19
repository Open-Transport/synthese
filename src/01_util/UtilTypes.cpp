
/** UtilTypes class implementation.
	@file UtilTypes.cpp

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

#include "UtilTypes.h"

namespace synthese
{
	namespace util
	{




		synthese::util::RegistryKeyType encodeUId( RegistryTableType tableId, RegistryNodeType gridNodeId, RegistryObjectType objectId )
		{
			RegistryKeyType id (objectId);
			RegistryKeyType tmp = gridNodeId;
			id |= (tmp << 32);
			tmp = tableId;
			id |= (tmp << 48);
			return id;
		}



		synthese::util::RegistryTableType decodeTableId( RegistryKeyType id )
		{
			return static_cast<RegistryTableType>((id & 0xFFFF000000000000LL) >> 48);
		}



		synthese::util::RegistryNodeType decodeGridNodeId( RegistryKeyType id )
		{
			return static_cast<RegistryNodeType>((id & 0x0000FFFF00000000LL) >> 32);
		}



		synthese::util::RegistryObjectType decodeObjectId( RegistryKeyType id )
		{
			return static_cast<RegistryObjectType>(id & 0x00000000FFFFFFFFLL);
		}
	}
}
