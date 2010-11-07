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
#include "SQLiteTransaction.h"
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
	using namespace db;
	using namespace admin;
	

	template<> const string util::FactorableTemplate<Function,impex::ImportFunction>::FACTORY_KEY("ImportFunction");
	
	namespace impex
	{
		/// Parameter names declarations
		const string ImportFunction::PARAMETER_DATA_SOURCE("ds");
		const string ImportFunction::PARAMETER_DO_IMPORT("di");



		ImportFunction::ImportFunction():
			FactorableTemplate<Function, ImportFunction>(),
			_doImport(false)
		{
		}



		ParametersMap ImportFunction::_getParametersMap() const
		{
			ParametersMap map(_importer.get() ? _importer->getParametersMap() : ParametersMap());
			if(_importer.get())
			{
				map.insert(PARAMETER_DATA_SOURCE, _importer->getDataSource().getKey());
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
				shared_ptr<const DataSource> dataSource(Env::GetOfficialEnv().get<DataSource>(dataSourceId));
				_importer = dataSource->getImporter();
				_importer->setFromParametersMap(map, true);
				
				stringstream output;
				_doImport = _importer->parseFiles(output, optional<const AdminRequest&>()) && map.getDefault<bool>(PARAMETER_DO_IMPORT, false);
				_output = output.str();
			}
			catch(ObjectNotFoundException<DataSource> e)
			{
				throw RequestException("Datasource not found");
			}
			catch(Exception e)
			{
				throw RequestException("Load failed : " + e.getMessage());
			}
		}
		


		void ImportFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(_doImport)
			{
				_importer->save().run();
			}

			stream << _output;
		}
		
		
		
		bool ImportFunction::isAuthorized(const Session* session
		) const {
			return true;
		}



		std::string ImportFunction::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
