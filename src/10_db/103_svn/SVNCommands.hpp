
/** SVNCommands class header.
	@file SVNCommands.hpp

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

#ifndef SYNTHESE_svn_SVNCommands_hpp__
#define SYNTHESE_svn_SVNCommands_hpp__

#include "Exception.h"

#include <vector>
#include <boost/filesystem/path.hpp>

namespace synthese
{
	namespace db
	{
		namespace svn
		{
			/** SVNCommands namespace.
				@ingroup m103
			*/
			namespace SVNCommands
			{
				typedef std::pair<int, std::string> CommandOutput;

				class Exception:
					public synthese::Exception
				{
					CommandOutput _commandOutput;

				public:
					Exception(
						const CommandOutput& commandOutput
					):	synthese::Exception("Error on SVN command : " + commandOutput.second),
						_commandOutput(commandOutput)
					{}
					
					
					
					~Exception() throw() {}



					const CommandOutput& getCommandOutput() const { return _commandOutput; }
				};


				//////////////////////////////////////////////////////////////////////////
				/// Runs a SVN command through the svn binary.
				CommandOutput _runSVN(
					const std::string& user,
					const std::string& password,
					const std::string& command,
					const std::string& parameters
				);

				


				//////////////////////////////////////////////////////////////////////////
				/// Updates the working copy from the repository.
				/// @return false if there is at least one conflict
				/// @warning this method does not check if the working copy is up to date
				/// @pre the working copy must exist on the filesystem
				/// @pre the repository URL must point to a valid SYNTHESE directory
				bool _svnUpdate(
					const std::string& user,
					const std::string& password,
					const boost::filesystem::path& localPath
				);



				//////////////////////////////////////////////////////////////////////////
				/// Commits the content of the working copy into the repository.
				/// @param message the the commit message
				/// @return false is an update is necessary
				/// @warning this method does not check if the working copy is up to date
				/// @pre the working copy must exist on the filesystem
				/// @pre the repository URL must point to a valid SYNTHESE directory
				bool _svnCommit(
					const std::string& message,
					const std::string& user,
					const std::string& password,
					const boost::filesystem::path& localPath
				);



				//////////////////////////////////////////////////////////////////////////
				/// Creates a directory in the repository.
				/// @throw CommandException if the directory creation failed
				void _svnMkdir(
					const std::string& user,
					const std::string& password,
					const std::string& url
				);



				//////////////////////////////////////////////////////////////////////////
				/// Checks out a repository into a working copy.
				/// @throw CommandException if the checkout failed
				void _svnCheckout(
					const std::string& user,
					const std::string& password,
					const std::string& url,
					const boost::filesystem::path& localPath
				);



				typedef std::vector<std::string> LsResult;

				LsResult _svnLs(
					const std::string& user,
					const std::string& password,
					const std::string& url
				);



				void _svnCreate(
					const std::string& user,
					const std::string& password
				);



				void _svnAdd(
					const boost::filesystem::path& file
				);



				void _svnDelete(
					const boost::filesystem::path& file
				);



				void _svnMove(
					const boost::filesystem::path& oldFile,
					const boost::filesystem::path& newFile
				);
}	}	}	}

#endif // SYNTHESE_svn_SVNCommands_hpp__

