
/** VirtualShapeVirtualTable class implementation.
	@file VirtualShapeVirtualTable.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "VirtualShapeVirtualTable.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;
using namespace boost::algorithm;

namespace synthese
{
	namespace db
	{
		VirtualShapeVirtualTable::VirtualShapeVirtualTable(
			const boost::filesystem::path& path,
			const std::string& codePage,
			CoordinatesSystem::SRID srid
		):	SQLiteVirtualTable(
			GetTableName(path),
			"VirtualShape(\"" + replace_all_copy(path.file_string(), "\\", "\\\\") + "\"," + codePage + "," + lexical_cast<string>(srid) + ")"
		)
		{}



		std::string VirtualShapeVirtualTable::GetTableName( const boost::filesystem::path& path )
		{
			string out(path.file_string());
			replace_if(out.begin(), out.end(), is_any_of(" \\:/"), '_');
			return "shapefile_" + out;
		}
	}
}
