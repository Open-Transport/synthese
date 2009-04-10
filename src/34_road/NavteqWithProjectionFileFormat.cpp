
/** NavteqWithProjectionFileFormat class implementation.
	@file NavteqWithProjectionFileFormat.cpp

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

#include "NavteqWithProjectionFileFormat.h"
#include "cdbfile.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace impex;
	using namespace road;
	

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,NavteqWithProjectionFileFormat>::FACTORY_KEY("NavteqWithProjection");
	}

	namespace road
	{
		const std::string NavteqWithProjectionFileFormat::FILE_NODES("nodes"); 
		const std::string NavteqWithProjectionFileFormat::FILE_STREETS("streets");
	}

	namespace impex
	{
		template<> const FileFormat::Files FileFormatTemplate<NavteqWithProjectionFileFormat>::FILES(
			NavteqWithProjectionFileFormat::FILE_NODES.c_str(),
			NavteqWithProjectionFileFormat::FILE_STREETS.c_str(),
		"");
	}

	namespace road
	{
		NavteqWithProjectionFileFormat::NavteqWithProjectionFileFormat( util::Env* env /* = NULL */)
		{
			_env = env;
		}

		bool NavteqWithProjectionFileFormat::_controlPathsMap( const FilePathsMap& paths )
		{
			FilePathsMap::const_iterator it(paths.find(FILE_NODES));
			if(it == paths.end() || it->second.empty()) return false;
			it = paths.find(FILE_STREETS);
			if(it == paths.end() || it->second.empty()) return false;
			return true;
		}
	}
}

