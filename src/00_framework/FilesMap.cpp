
/** FilesMap class implementation.
	@file FilesMap.cpp

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

#include "FilesMap.hpp"

#include "Exception.h"

namespace synthese
{
	FilesMap::FilesMap()
	{
	}



	void FilesMap::insert( const std::string& key, Map::mapped_type& value )
	{
		_map[key] = value;
	}



	const FilesMap::Map::mapped_type& FilesMap::get( const std::string& key ) const
	{
		Map::const_iterator it(_map.find(key));
		if(it == _map.end())
		{
			throw Exception("No such item");
		}
		return it->second;
	}

}

