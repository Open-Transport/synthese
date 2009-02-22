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

#include <boost/foreach.hpp>
#include <sstream>
#include <fstream>

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
			_doImport(false),
			_dataSource(NULL)
		{}



		void FileFormat::setDoImport(bool value)
		{
			_doImport = value;
		}

		void FileFormat::setDataSource(const DataSource* value)
		{
			_dataSource = value;
		}
		
		void FileFormat::parseFiles(
			const std::set<std::string>& paths
		){
			_preImport();
			
			BOOST_FOREACH(const std::string& path, paths)
			{
				ifstream ifs(path.c_str());
				if (!ifs)
				{
					throw Exception("Could no open the file" + path);
				}
			
				// Read the whole file into a string
				stringstream ss;
				ss << ifs.rdbuf();
				ifs.close();
				
				_parse(ss.str());
			}
			
			if(_doImport)
			{
				_postImport();
			}
		}
	}
}

