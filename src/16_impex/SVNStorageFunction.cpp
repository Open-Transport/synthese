
/** SVNStorageFunction class implementation.
	@file SVNStorageFunction.cpp
	@author Sylvain Pasche
	@date 2012

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

#include "SVNStorageFunction.hpp"

#include "DumpFileFormat.hpp"
#include "ImportFunction.h"
#include "ExportFunction.hpp"
#include "RequestException.h"
#include "Request.h"
#include "SecurityRight.h"
#include "ServerModule.h"

#include "boost/algorithm/string/replace.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <stdio.h>
#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif

using namespace std;
namespace fs = boost::filesystem;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,impex::SVNStorageFunction>::FACTORY_KEY("SVNStorageFunction");

	namespace
	{
		string escapeCommandLineParameter(const string& input)
		{
			string escapedString(input);
			boost::algorithm::replace_all(escapedString, "\\", "\\\\");
			boost::algorithm::replace_all(escapedString, "\"", "\\\"");
			return "\"" + escapedString + "\"";
		}
	}

	namespace impex
	{
		class FileFormat;
	}

	namespace impex
	{
		const string SVNStorageFunction::PARAMETER_DATASOURCE_ID("datasource_id");
		const string SVNStorageFunction::PARAMETER_OBJECT_ID("object_id");
		const string SVNStorageFunction::PARAMETER_EXPORT_PATH("export_path");
		const string SVNStorageFunction::PARAMETER_CHECKOUT_PATH("checkout_path");
		const string SVNStorageFunction::PARAMETER_URL("url");
		const string SVNStorageFunction::PARAMETER_USERNAME("username");
		const string SVNStorageFunction::PARAMETER_PASSWORD("password");
		const string SVNStorageFunction::PARAMETER_UPDATE("update");
		const string SVNStorageFunction::PARAMETER_COMMIT("commit");
		const string SVNStorageFunction::PARAMETER_COMMIT_MESSAGE("commit_message");
		const string SVNStorageFunction::PARAMETER_VERBOSE("verbose");

		const string SVNStorageFunction::VALUE_SUCCESS("success");



		ParametersMap SVNStorageFunction::_getParametersMap() const
		{
			ParametersMap map;

			map.insert(PARAMETER_DATASOURCE_ID, _datasourceId);
			map.insert(PARAMETER_OBJECT_ID, _objectId);
			map.insert(PARAMETER_EXPORT_PATH, _exportPath);
			map.insert(PARAMETER_CHECKOUT_PATH, _checkoutPath);
			map.insert(PARAMETER_URL, _url);
			map.insert(PARAMETER_USERNAME, _username);
			map.insert(PARAMETER_PASSWORD, _password);
			map.insert(PARAMETER_COMMIT, _commit);
			map.insert(PARAMETER_COMMIT_MESSAGE, _commitMessage);
			map.insert(PARAMETER_VERBOSE, _verbose);

			return map;
		}



		void SVNStorageFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_datasourceId = map.getDefault<string>(PARAMETER_DATASOURCE_ID);
			_objectId = map.getDefault<string>(PARAMETER_OBJECT_ID);
			_exportPath = map.getDefault<string>(PARAMETER_EXPORT_PATH);
			_checkoutPath = map.getDefault<string>(PARAMETER_CHECKOUT_PATH);
			_url = map.getDefault<string>(PARAMETER_URL);
			_username = map.getDefault<string>(PARAMETER_USERNAME);
			_password = map.getDefault<string>(PARAMETER_PASSWORD);
			_update = map.getDefault<bool>(PARAMETER_UPDATE);
			_commit = map.getDefault<bool>(PARAMETER_COMMIT);
			_commitMessage = map.getDefault<string>(PARAMETER_COMMIT_MESSAGE);
			_verbose = map.getDefault<bool>(PARAMETER_VERBOSE);
		}



		ParametersMap SVNStorageFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			if(!_update && !_commit)
			{
				Log::GetInstance().debug("Nothing to do");
				return map;
			}

			// Data export

			fs::remove_all(_exportPath);
			fs::create_directory(_exportPath);

			stream << "Running export.\n\n";
			boost::shared_ptr<Function> exportFunction = boost::shared_ptr<Function>(
				util::Factory<Function>::create("ExportFunction")
			);
			ParametersMap exportMap;
			exportMap.insert(
				ExportFunction::PARAMETER_FILE_FORMAT,
				FactorableTemplate<impex::FileFormat, impex::DumpFileFormat>::FACTORY_KEY
			);
			exportMap.insert(
				DumpFileFormat::Exporter_::PARAMETER_OBJECT_ID,
				_objectId
			);
			exportMap.insert(
				DumpFileFormat::Exporter_::PARAMETER_PATH,
				_exportPath
			);

			try
			{
				exportFunction->_setFromParametersMap(exportMap);
				exportFunction->run(stream, request);
			}
			catch(exception& e)
			{
				stream << "Exception during export: " << e.what() << "\n";
				map.insert(VALUE_SUCCESS, false);
				return map;
			}

			// Run svn script.

			stringstream commandLine;
			commandLine << ServerModule::GetPythonPath();

			fs::path daemonDirectory(fs::initial_path<fs::path>());
			if(getenv("SYNTHESE_SVN_SCRIPT_DIR"))
			{
				daemonDirectory = getenv("SYNTHESE_SVN_SCRIPT_DIR");
			}
			fs::path svnScriptPath = daemonDirectory / "synthese_svn.py";

			commandLine << " " << escapeCommandLineParameter(svnScriptPath.native_file_string());

			commandLine << " --export-path " << escapeCommandLineParameter(_exportPath);
			commandLine << " --checkout-path " << escapeCommandLineParameter(_checkoutPath);
			commandLine << " --url " << escapeCommandLineParameter(_url);
			commandLine << " --username " << escapeCommandLineParameter(_username);
			commandLine << " --password " << escapeCommandLineParameter(_password);
			if(_commit)
			{
				commandLine << " --commit";
			}
			commandLine << " --commit-message " << escapeCommandLineParameter(_commitMessage);
			if(_verbose)
			{
				commandLine << " -v";
			}

			commandLine << " 2>&1";

			Log::GetInstance().debug("Script command line: " + commandLine.str());

			stream << "Running commit script.\n\n";
			FILE* pipe = popen(commandLine.str().c_str(), "r");
			if(!pipe)
			{
				map.insert(VALUE_SUCCESS, false);
				stream << "popen failure";
				return map;
			}
			char buffer[128];
			while(!feof(pipe))
			{
				if(fgets(buffer, 128, pipe) != NULL)
				{
					stream << buffer;
				}
			}
			stream << "\n\n";
			int status = pclose(pipe);

			map.insert(VALUE_SUCCESS, status == 0);
			if(status != 0)
			{
				return map;
			}

			// Data import

			stream << "Running import.\n\n";

			boost::shared_ptr<Function> importFunction = boost::shared_ptr<Function>(
				util::Factory<Function>::create("ImportFunction")
			);
			ParametersMap importMap;
			importMap.insert(
				ImportFunction::PARAMETER_DO_IMPORT,
				true
			);
			importMap.insert(
				ImportFunction::PARAMETER_DATA_SOURCE,
				_datasourceId
			);

			fs::path importPath = fs::path(_exportPath) / (_objectId + ".dump");

			importMap.insert(
				DumpFileFormat::Importer_::PARAMETER_PATH,
				importPath.native_file_string()
			);
			try
			{
				importFunction->_setFromParametersMap(importMap);
				importFunction->run(stream, request);
			}
			catch(exception& e)
			{
				stream << "Exception during import: " << e.what() << "\n";
				map.insert(VALUE_SUCCESS, false);
				return map;
			}

			fs::remove_all(_exportPath);

			return map;
		}



		bool SVNStorageFunction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(WRITE);
		}



		std::string SVNStorageFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}

