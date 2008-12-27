
////////////////////////////////////////////////////////////////////////////////
/// UId type and functions header.
///	@file UId.h
///	@author Hugues Romain
///	@date 2008-12-26 17:49
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_UTIL_UID_H
#define SYNTHESE_UTIL_UID_H

////////////////////////////////////////////////////////////////////
/// Unique ID for SYNTHESE objects.
///	@ingroup m01
///	The ID is the binary concatenation of 4 sub-keys :
///		- a table ID (2 bytes)
///		- a grid ID (4 bits) determnating the gris which belongs the creator SYNTHESE instance
///		- a node ID (12 bits) determinating the creator SYNTHESE instance whitin the grid
///		- an autoincrement object ID, unique by instance and by table
////////////////////////////////////////////////////////////////////
typedef signed long long int uid;

namespace synthese
{
    namespace util
    {
		/// @addtogroup m01
		//@{
	
		////////////////////////////////////////////////////////////////////
		///	Encodes a universal id.
		///	@param tableId (2 bytes)
		///	@param gridId (4 bits)
		/// TO BE REMOVED : deprecated!!!
		///	@param gridNodeId (12 bits)
		///	@param objectId (4 bytes)
		////////////////////////////////////////////////////////////////////
		uid encodeUId (int tableId, int gridId, int gridNodeId, long objectId);



		////////////////////////////////////////////////////////////////////
		/// Reads the table ID in an object uid.
		///	@param id the object uid
		///	@return int the table id
		///	@author Marc Jambert
		///	@date 2006
		////////////////////////////////////////////////////////////////////
		int decodeTableId (const uid& id);

		int decodeGridId (const uid& id);

		int decodeGridNodeId (const uid& id);

		long decodeObjectId (const uid& id);
		    
		//@}
    }
}

#endif
