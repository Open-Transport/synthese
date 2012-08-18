
/** SVNWorkingCopy class header.
	@file SVNWorkingCopy.hpp

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


#ifndef SYNTHESE_svn_SVNWorkingCopy_H
#define SYNTHESE_svn_SVNWorkingCopy_H

#include "SimpleObjectField.hpp"

#include "Env.h"

#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	class ObjectBase;

	namespace db
	{
		class DBTransaction;

		namespace svn
		{
			//////////////////////////////////////////////////////////////////////////
			/// SVN Working copy maintainer.
			/// @ingroup m103
			///
			///  - Extraction and import of data at a generic SQL dump format.
			///  - SVN backend
			///	 - Inter-SYNTHESE service
			class SVNWorkingCopy:
				public ObjectField<SVNWorkingCopy, SVNWorkingCopy>
			{
			public:
				typedef SVNWorkingCopy Type;

				typedef std::pair<int, std::string> CommandOutput;

				class CommandException:
					public synthese::Exception
				{
					CommandOutput _commandOutput;

				public:
					CommandException(
						const CommandOutput& commandOutput
					):	synthese::Exception("Error on SVN command : " + commandOutput.second),
						_commandOutput(commandOutput)
					{}



					const CommandOutput& getCommandOutput() const { return _commandOutput; }
				};

			private:
				boost::filesystem::path _path; //<! The working copy path
				std::string _repoURL; //<! The URL of the repository
				ObjectBase* _object; //<! The corresponding object in SYNTHESE

				mutable util::Env _env;
				
				typedef std::set<util::RegistryKeyType> ImportedObjects;

				//////////////////////////////////////////////////////////////////////////
				/// File importer (for use in recursive mode)
				/// @param dirPath the path of the directory containing files to import
				/// @return the imported objects (only from the directory itself)
				ImportedObjects _import(
					const boost::filesystem::path& dirPath,
					db::DBTransaction& transaction,
					bool first = false
				);



				//////////////////////////////////////////////////////////////////////////
				/// File exporter (for use in recursive mode)
				/// @param object the object to export
				/// @param dirPath the directory where to export the files
				void _export(
					const ObjectBase& object,
					const boost::filesystem::path& dirPath
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Runs a SVN command through the svn binary.
				static CommandOutput _runSVN(
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
				static bool _svnUpdate(
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
				static bool _svnCommit(
					const std::string& message,
					const std::string& user,
					const std::string& password,
					const boost::filesystem::path& localPath
				);



				//////////////////////////////////////////////////////////////////////////
				/// Creates a directory in the repository.
				/// @throw CommandException if the directory creation failed
				static void _svnMkdir(
					const std::string& user,
					const std::string& password,
					const std::string& url
				);



				//////////////////////////////////////////////////////////////////////////
				/// Checks out a repository into a working copy.
				/// @throw CommandException if the checkout failed
				static void _svnCheckout(
					const std::string& user,
					const std::string& password,
					const std::string& url,
					const boost::filesystem::path& localPath
				);



				typedef std::vector<std::string> LsResult;

				static LsResult _svnLs(
					const std::string& user,
					const std::string& password,
					const std::string& url
				);



				static void _svnCreate(
					const std::string& user,
					const std::string& password
				);



				static void _svnAdd(
					const boost::filesystem::path& file
				);



				static void _svnDelete(
					const boost::filesystem::path& file
				);



				static void _svnMove(
					const boost::filesystem::path& oldFile,
					const boost::filesystem::path& newFile
				);



				//////////////////////////////////////////////////////////////////////////
				/// Exports the current object to the working copy.
				/// The export process maintains the working copy at a compliant status :
				///  - new files are declared as added
				///  - files that non longer exists are declared as removed
				void _exportToWC() const;



				//////////////////////////////////////////////////////////////////////////
				/// Imports the content of the working copy into the current object.
				/// @warning this method should erase all data already contained in SYNTHESE
				/// @param save false for simulation mode
				void _importWC(
					bool save = false
				);


			public:
				SVNWorkingCopy(
					ObjectBase* object
				);

				/// @name Getters
				//@{
					ObjectBase* getObject() const { return _object; }
					const boost::filesystem::path& getPath() const { return _path; }
					const std::string& getRepoURL() const { return _repoURL; }
				//@}

				/// @name Setters
				//@{
					void setObject(ObjectBase* value){ _object = value; }
					void setPath(const boost::filesystem::path& value){ _path = value; }
					void setRepoURL(const std::string& value){ _repoURL = value; }
				//@}

				/// @name Services
				//@{
					//////////////////////////////////////////////////////////////////////////
					/// Initialize the repository URL with the data of the working copy
					/// @pre the working copy must exist on the filesystem
					/// @pre the repository URL must point to an inexistent directory
					void create(
						const std::string& user,
						const std::string& password
					) const;



					//////////////////////////////////////////////////////////////////////////
					/// Write the working copy on the filesystem through the svn co command.
					/// @pre the working copy must not exist on the filesystem
					/// @pre the repository URL must point to a valid SYNTHESE directory
					void checkout(
						const std::string& user,
						const std::string& password
					);



					void commit(
						const std::string& message,
						const std::string& user,
						const std::string& password
					);



					void update(
						const std::string& user,
						const std::string& password
					);
				//@}
			};
}	}	}

#endif
