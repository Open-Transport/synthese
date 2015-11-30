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
#include <boost/filesystem.hpp>

using namespace boost;
using namespace std;
using namespace boost::posix_time;

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

		const string ImportFunction::ATTR_IMPORT_END_TIME = "import_end_time";
		const string ImportFunction::ATTR_SUCCESS = "success";
		const string ImportFunction::ATTR_DONE = "done";
		const string ImportFunction::ATTR_LOGS = "logs";
		const string ImportFunction::TAG_LOG_ENTRY = "log_entry";



		ImportFunction::ImportFunction():
			FactorableTemplate<Function, ImportFunction>(),
			_doImport(false),
			_importDone(false)
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
				
				if (import->get<Forbidden>())
				{
					throw RequestException("The selected import is forbidden right now !");
				}

				// Log path
				ImportLogLevel minLogLevel(import->get<MinLogLevel>());
				bool outputLogs(false);
				if (minLogLevel < IMPORT_LOG_NOLOG)
				{
					outputLogs = true;
				}
				if(map.isDefined(PARAMETER_OUTPUT_LOGS))
				{
					outputLogs = map.getDefault<bool>(PARAMETER_OUTPUT_LOGS, false);
				}

				// Min log force
				if(map.isDefined(PARAMETER_MIN_LOG_LEVEL))
				{
					minLogLevel = static_cast<ImportLogLevel>(map.get<int>(PARAMETER_MIN_LOG_LEVEL));
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
					// Importer generation
					_importer = import->getImporter(
						*_env,
						minLogLevel,
						logPath,
						_output,
						_result
					);
					try
					{
						_importer->openLogFile();
					}
					catch(const boost::filesystem::filesystem_error& e)
					{
						throw RequestException("Failed to access log directory '" +
											logPath + "': " + e.code().message());
					}
				}
				else
				{
					// Importer generation
					_importer = import->getImporter(
						*_env,
						minLogLevel,
						logPath,
						optional<ostream&>(),
						_result
					);
				}

				// Use the parameters of the impor, except if they are overridden by request
				ParametersMap fullMap;
				BOOST_FOREACH(const ParametersMap::Map::value_type& element, map.getMap())
				{
					fullMap.insert(element.first, map.get<string>(element.first));
				}

				BOOST_FOREACH(const ParametersMap::Map::value_type& element, (import->get<Parameters>()).getMap())
				{
					if (!fullMap.isDefined(element.first))
					{
						fullMap.insert(element.first, element.second);
					}
				}
				
				_importer->setFromParametersMap(fullMap, true);

				_doImport = map.isTrue(PARAMETER_DO_IMPORT);
				_importDone = _importer->beforeParsing();
				_importDone &= _importer->parseFiles();
				_importDone &=_importer->afterParsing();
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

			if(_doImport && _importDone)
			{
				DBModule::DeactivateConditionalTablesUpdate();
				try
				{
					_importer->save().run();
				}
				catch(std::exception& e)
				{
					throw RequestException("Error while saving import : " + std::string(e.what()));
				}
				DBModule::ActivateConditionalTablesUpdate();

				// Result
				_result.insert(ATTR_DONE, true);
				_result.insert(ATTR_SUCCESS, true);
			}
			else
			{
				// Result
				_result.insert(ATTR_DONE, false);
				_result.insert(ATTR_SUCCESS, _importDone);
			}

			stringstream dateStr;
			ptime now(second_clock::local_time());
			dateStr <<
				now.date().year() << "-" <<
				setw(2) << setfill('0') << int(now.date().month()) << "-" <<
				setw(2) << setfill('0') << now.date().day() << " " <<
				setw(2) << setfill('0') << now.time_of_day().hours() << ":" <<
				setw(2) << setfill('0') << now.time_of_day().minutes() << ":" <<
				setw(2) << setfill('0') << now.time_of_day().seconds();
			_result.insert(ATTR_IMPORT_END_TIME, dateStr.str());
			_result.insert(ATTR_LOGS, _output.str());

			return _result;
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
