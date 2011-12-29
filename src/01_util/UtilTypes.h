////////////////////////////////////////////////////////////////////////////////
/// UtilTypes class header.
///	@file UtilTypes.h
///	@author Hugues Romain
///	@date 2008-12-26 17:49
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_util_UtilTypes_h__
#define SYNTHESE_util_UtilTypes_h__

namespace synthese
{
	/// @defgroup m01 01 Utilities
	/// @ingroup m0
	/// @{
	namespace util
	{
		typedef unsigned long long int RegistryKeyType;
		typedef unsigned long int RegistryObjectType;
		typedef unsigned int RegistryTableType;
		typedef unsigned int RegistryNodeType;

		typedef enum
		{
			UNKNOWN_LOAD_LEVEL = 0,
			FIELDS_ONLY_LOAD_LEVEL = 1,
			UP_LINKS_LOAD_LEVEL = 5,
			DOWN_LINKS_LOAD_LEVEL = 6,
			UP_DOWN_LINKS_LOAD_LEVEL = 10,
			RECURSIVE_LINKS_LOAD_LEVEL = 20,
			ALGORITHMS_OPTIMIZATION_LOAD_LEVEL = 30
		} LinkLevel;


		////////////////////////////////////////////////////////////////////
		///	Encodes a universal id.
		///	@param tableId (2 bytes)
		///	@param gridNodeId (2 bytes)
		///	@param objectId (4 bytes)
		///	@author Marc Jambert
		////////////////////////////////////////////////////////////////////
		RegistryKeyType encodeUId(
			RegistryTableType tableId,
			RegistryNodeType gridNodeId,
			RegistryObjectType objectId
		);



		////////////////////////////////////////////////////////////////////
		/// Reads the table ID in an object id.
		///	@param id the object id
		///	@return the table id
		///	@author Marc Jambert
		///	@date 2006
		////////////////////////////////////////////////////////////////////
		RegistryTableType decodeTableId (RegistryKeyType id);

		RegistryNodeType decodeGridNodeId (RegistryKeyType id);

		RegistryObjectType decodeObjectId (RegistryKeyType id);
	}
	/// @}
}

#endif // SYNTHESE_util_UtilTypes_h__
