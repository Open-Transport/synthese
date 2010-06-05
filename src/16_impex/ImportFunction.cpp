/** ImportFunction class implementation.
	@file ImportFunction.cpp
	@author Hugues Romain
	@date 2009

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

#include "RequestException.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "FileFormat.h"

#include "ImportFunction.h"

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,impex::ImportFunction>::FACTORY_KEY("ImportFunction");
	
	namespace impex
	{
		/// Parameter names declarations
		const string ImportFunction::PARAMETER_PATH("pa");
		const string ImportFunction::PARAMETER_DATA_SOURCE("ds");
		const string ImportFunction::PARAMETER_DO_IMPORT("di");



		ImportFunction::ImportFunction()
			: FactorableTemplate<Function, ImportFunction>(),
			_doImport(false)
		{
			setEnv(shared_ptr<Env>(new Env));
		}



		ParametersMap ImportFunction::_getParametersMap() const
		{
			ParametersMap map(_fileFormat.get() ? _fileFormat->_getParametersMap(true) : ParametersMap());
			if(_dataSource.get())
			{
				map.insert(PARAMETER_DATA_SOURCE, _dataSource->getKey());
			}
			map.insert(PARAMETER_DO_IMPORT, _doImport);
			return map;
		}

		void ImportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Datasource
			RegistryKeyType dataSourceId(map.get<RegistryKeyType>(PARAMETER_DATA_SOURCE));
			try
			{
				_dataSource = DataSourceTableSync::Get(dataSourceId, *_env);
			}
			catch(ObjectNotFoundException<DataSource> e)
			{
				throw RequestException("Datasource not found");
			}
			
			// Do import ?
			_doImport = map.getDefault<bool>(PARAMETER_DO_IMPORT, false);
			
			// Input parsing
			try
			{
				stringstream output;
				_fileFormat.reset(Factory<FileFormat>::create(_dataSource->getFormat()));

				// Paths
				FileFormat::Files::FilesVector files(_fileFormat->getFiles());
				_fileFormat->setEnv(_env.get());
				_fileFormat->setDataSource(_dataSource.get());
				_fileFormat->_setFromParametersMap(map, true);

				if(files.empty())
				{
					FileFormat::FilePathsSet paths;
					string text(map.get<string>(PARAMETER_PATH));
					tokenizer<char_separator<char> > pathsTokens(text, char_separator<char>(","));
					BOOST_FOREACH(const string& token, pathsTokens)
					{
						if(token.empty()) continue;
						paths.insert(token);
					}
					_fileFormat->parseFiles(paths, output);
				}
				else
				{
					FileFormat::FilePathsMap paths;
					BOOST_FOREACH(const string& key, files)
					{
						paths.insert(make_pair(key, map.getDefault<string>(PARAMETER_PATH + key)));
					}
					_fileFormat->parseFiles(paths, output);
				}

				_output = output.str();
			}
			catch(Exception e)
			{
				throw RequestException("Load failed : " + e.getMessage());
			}
		}
		


		void ImportFunction::run( std::ostream& stream, const Request& request ) const
		{
			stream << _output;
			if(_doImport)
			{
				_fileFormat->save(stream);
			}
		}
		
		
		
		bool ImportFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string ImportFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
