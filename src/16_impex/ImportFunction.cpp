/** ImportFunction class implementation.
	@file ImportFunction.cpp
	@author Hugues Romain
	@date 2009

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

#include "ImportFunction.h"

#include "RequestException.h"
#include "ImportTableSync.hpp"
#include "FileFormat.h"
#include "DBTransaction.hpp"

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
		const string ImportFunction::PARAMETER_IMPORT_ID = "import_id";
		const string ImportFunction::PARAMETER_DO_IMPORT = "di";
		const string ImportFunction::PARAMETER_LOG_PATH = "lp";
		const string ImportFunction::PARAMETER_MIN_LOG_LEVEL = "min_log_level";
		const string ImportFunction::PARAMETER_OUTPUT_LOGS = "output_logs";



		ImportFunction::ImportFunction():
			FactorableTemplate<Function, ImportFunction>(),
			_doImport(false)
		{
			setEnv(boost::shared_ptr<Env>(new Env));
		}



		ParametersMap ImportFunction::_getParametersMap() const
		{
			ParametersMap map(_importer.get() ? _importer->getParametersMap() : ParametersMap());
			if(_importer.get())
			{
				map.insert(PARAMETER_IMPORT_ID, _importer->getImport().getKey());
			}
			map.insert(PARAMETER_DO_IMPORT, _doImport);
			return map;
		}



		void ImportFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Import
			RegistryKeyType importId(map.get<RegistryKeyType>(PARAMETER_IMPORT_ID));
			try
			{
				boost::shared_ptr<const Import> import(ImportTableSync::Get(importId, *_env));
				if(!import->get<DataSource>())
				{
					throw RequestException("The id system of the specified import is not defined.");
				}

				// Log path
				bool outputLogs(map.getDefault<bool>(PARAMETER_OUTPUT_LOGS, false));

				// Min log force
				ImportLogger::Level minLogLevel(import->get<MinLogLevel>());
				if(map.isDefined(PARAMETER_MIN_LOG_LEVEL))
				{
					minLogLevel = static_cast<ImportLogger::Level>(map.get<int>(PARAMETER_MIN_LOG_LEVEL));
				}

				// Log path force
				string logPath(import->get<LogPath>());
				if(map.isDefined(PARAMETER_LOG_PATH))
				{
					logPath = map.get<string>(PARAMETER_LOG_PATH);
				}
				
				// Logger generation
				if(outputLogs)
				{
					_importLogger.reset(
						new ImportLogger(
							minLogLevel,
							logPath,
							_output
					)	);
				}
				else
				{
					_importLogger.reset(
						new ImportLogger(
						minLogLevel,
						logPath,
						optional<ostream&>()
					)	);
				}

				// Importer generation
				_importer = import->getImporter(*_env, *_importLogger);
				_importer->setFromParametersMap(map, true);

				_doImport = _importer->beforeParsing();
				_doImport &= _importer->parseFiles(optional<const Request&>()) && map.isTrue(PARAMETER_DO_IMPORT);
				_doImport &=_importer->afterParsing();
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



		ParametersMap ImportFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{

			ParametersMap pm;

			if(_doImport)
			{
				_importer->save().run();

				// If no log output
				if(!_importLogger->getOutputStream())
				{
					stream << "0";
				}
			}
			else
			{
				if(!_importLogger->getOutputStream())
				{
					stream << "1";
				}
			}
			if(_importLogger->getOutputStream())
			{
				stream << _output.str();
			}

			return pm;
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
