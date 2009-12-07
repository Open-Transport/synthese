////////////////////////////////////////////////////////////////////////////////
///	Import class implementation.
///	@file Import.cpp
///	@author Hugues Romain
///	@date 2009
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

#include "FileFormat.h"
#include <stdarg.h>
#include <boost/foreach.hpp>
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace impex
	{
		FileFormat::FileFormat(
			Env* env
		):	FactoryBase<FileFormat>(),
			_env(env),
			_dataSource(NULL)
		{}


		FileFormat::Files::Files( const char* value, ... )
		{
			const char* col(value);
			va_list marker;
			for(va_start(marker, value); col[0]; col = va_arg(marker, const char*))
			{
				_files.push_back(string(col));
			}
			va_end(marker);
		}

		const FileFormat::Files::FilesVector& FileFormat::Files::getFiles() const
		{
			return _files;
		}


		void FileFormat::setEnv(Env* value)
		{
			_env = value;
		}

		void FileFormat::setDataSource(const DataSource* value)
		{
			_dataSource = value;
		}


		bool FileFormat::_controlPathsMap(
			const FileFormat::FilePathsMap& paths
		){
			return true;
		}


		void FileFormat::parseFiles(
			const FilePathsSet& paths,
			std::ostream& os
		){
			BOOST_FOREACH(const FilePathsSet::value_type& path, paths)
			{
				_parse(path, os);
			}
		}



		void FileFormat::parseFiles(
			const FilePathsMap& paths,
			std::ostream& os
		){
			if (!_controlPathsMap(paths))
			{
				throw Exception("At least a bad or missing file name");
			}
			const Files::FilesVector& files(getFiles());
			BOOST_FOREACH(const string& key, files)
			{
				FilePathsMap::const_iterator it(paths.find(key));
				if(it == paths.end())
					continue;
				const FilePathsMap::mapped_type& path(it->second);

				_parse(path, os, key);
			}
		}
	}
}

