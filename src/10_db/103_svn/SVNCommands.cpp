
/** SVNCommands class implementation.
	@file SVNCommands.cpp

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

#include "SVNCommands.hpp"

#include "Log.h"

#include <sstream>
#include <stdio.h>
#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	
	namespace db
	{
		namespace svn
		{
			namespace SVNCommands
			{
				CommandOutput _runSVN(
					const std::string& user,
					const std::string& password,
					const std::string& command,
					const std::string& parameters
				){
					CommandOutput result(make_pair(1, string()));
					stringstream cmd;
					cmd << "svn " << command;
					cmd << " --no-auth-cache";
					cmd << " --non-interactive";
					if(!user.empty())
					{
						cmd << " --username " << user;
					}
					if(!password.empty())
					{
						cmd << " --password " << password;
					}
					cmd << " " << parameters << " 2>&1";
					string cmdStr(cmd.str());

					Log::GetInstance().trace(cmdStr);

					stringstream response;
					FILE* pipe = popen(cmdStr.c_str(), "r");
					if(!pipe)
					{
						Log::GetInstance().debug("popen failure");
						return result;
					}
					char buffer[128];
					while(!feof(pipe))
					{
						if(fgets(buffer, 128, pipe) != NULL)
						{
							response << buffer;
						}
					}
					result.first = pclose(pipe);
					result.second = response.str();

					return result;
				}



				void _svnAdd(
					const boost::filesystem::path& file
				){

					_runSVN(
						string(),
						string(),
						"add",
						"\"" + file.string() + "\""
					);
				}



				void _svnDelete(
					const boost::filesystem::path& file
				){
					_runSVN(
						string(),
						string(),
						"delete",
						"--force \""+ file.string() + "\""
					);
				}



				void _svnMove(
					const boost::filesystem::path& oldFile,
					const boost::filesystem::path& newFile
				){
					_runSVN(
						string(),
						string(),
						"move",
						"\"" + oldFile.string() +"\" \""+ newFile.string() + "\""
					);
				}



				void _svnMkdir(
					const std::string& user,
					const std::string& password,
					const std::string& url
				){
					CommandOutput result(
						_runSVN(
							user,
							password,
							"mkdir",
							"-m \"Object creation\" "+ url
					)	);
					if(result.first)
					{
						throw Exception(result);
					}
				}



				LsResult _svnLs(
					const std::string& user,
					const std::string& password,
					const std::string& url
				){
					CommandOutput ls(_runSVN(user, password, "ls", url));
					if(ls.first)
					{
						throw synthese::Exception("Bad URL");
					}
					LsResult files;
					if(!ls.second.empty())
					{
						split(files, ls.second, is_any_of("\n\r"), token_compress_on);
					}
					return files;
				}



				void _svnCheckout(
					const std::string& user,
					const std::string& password,
					const std::string& url,
					const boost::filesystem::path& localPath
				){
					CommandOutput result(
						_runSVN(
							user,
							password,
							"co",
							url + " \"" + localPath.string() + "\""
					)	);
					if(result.first)
					{
						throw Exception(result);
					}
				}



				bool _svnUpdate(
					const std::string& user,
					const std::string& password,
					const boost::filesystem::path& localPath
				){
					_runSVN(
						user,
						password,
						"up",
						"\"" + localPath.string() + "\""
					);

					return true;
				}



				bool _svnCommit(
					const std::string& message,
					const std::string& user,
					const std::string& password,
					const boost::filesystem::path& localPath
				){
					CommandOutput result(
						_runSVN(
							user,
							password,
							"ci",
							" -m \""+ message +"\" \""+ localPath.string() + "\""
					)	);
					
					return true;
				}
}	}	}	}
