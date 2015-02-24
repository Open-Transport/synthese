
/** VirtualDBFVirtualTable class implementation.
	@file VirtualDBFVirtualTable.cpp

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

#include "VirtualDBFVirtualTable.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace boost;
using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;

namespace synthese
{
	namespace db
	{
		VirtualDBFVirtualTable::VirtualDBFVirtualTable(
			const boost::filesystem::path& path,
			const std::string& codePage
		):	SQLiteVirtualTable(
			GetTableName(path),
			"VirtualDBF(\"" + replace_all_copy(path.string(), "\\", "\\\\") + "\"," + codePage + ")"
		)
		{}



		std::string VirtualDBFVirtualTable::GetTableName( const boost::filesystem::path& path )
		{
			string out(change_extension(path, string()).string());
			replace_if(out.begin(), out.end(), is_any_of(" \\:/()-+"), '_');
			return "DBFfile_" + out;
		}
	}
}
