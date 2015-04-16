
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

#include "Env.h"
#include "FrameworkTypes.hpp"
#include "SVNRepository.hpp"
#include "SimpleObjectFieldDefinition.hpp"

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
				public SimpleObjectFieldDefinition<SVNWorkingCopy>
			{
			public:
				typedef SVNWorkingCopy Type;


			private:
				boost::filesystem::path _path; //<! The working copy path
				SVNRepository _repo; // The repository
				ObjectBase* _object; //<! The corresponding object in SYNTHESE

				mutable util::Env _env;


				//////////////////////////////////////////////////////////////////////////
				/// Removes an object from the database and its sub objects (recursion).
				/// @param object the object to remove
				/// @param transaction the transaction to populate
				void _clean(
					const ObjectBase& object,
					db::DBTransaction& transaction
				) const;


				
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
					const util::Registrable& object,
					const boost::filesystem::path& dirPath,
					const bool noCommit
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Exports the current object to the working copy.
				/// The export process maintains the working copy at a compliant status :
				///  - new files are declared as added
				///  - files that no longer exists are declared as removed
				void _exportToWC(const bool noCommit) const;



				//////////////////////////////////////////////////////////////////////////
				/// Imports the content of the working copy into the current object.
				/// @warning this method should erase all data already contained in SYNTHESE
				/// @param save false for simulation mode
				void _importWC(
					bool save = false
				);


			public:
				SVNWorkingCopy();

				/// @name Getters
				//@{
					ObjectBase* getObject() const { return _object; }
					const boost::filesystem::path& getPath() const { return _path; }
					const SVNRepository& getRepo() const { return _repo; }
				//@}

				/// @name Setters
				//@{
					void setObject(ObjectBase* value);
					void setPath(const boost::filesystem::path& value){ _path = value; }
					void setRepo(const SVNRepository& value){ _repo = value; }
				//@}

				/// @name Services
				//@{
					//////////////////////////////////////////////////////////////////////////
					/// Initialize the repository URL with the data of the working copy
					/// @pre the working copy must exist on the filesystem
					/// @pre the repository URL must point to an inexistent directory
					void create(
						const std::string& user,
						const std::string& password,
						const bool noCommit
					) const;



					//////////////////////////////////////////////////////////////////////////
					/// Write the working copy on the filesystem through the svn co command.
					/// @pre the working copy must not exist on the filesystem
					/// @pre the repository URL must point to a valid SYNTHESE directory
					void checkout(
						const std::string& user,
						const std::string& password
					);



					void commit(const std::string& message,
						const std::string& user,
						const std::string& password,
						const bool noCommit,
						const bool noUpdate
					);



					//////////////////////////////////////////////////////////////////////////
					/// Performs an update of the current data from the server.
					/// Two behaviors : 
					///  - if a working copy already exists on the filesystem, it is updated
					///  - if no working copy exists on the filesystem, the data from the server
					///    is entirely imported in the database. In this case, local modifications
					///    are lost.
					/// @param user valid login on the server
					/// @param password password corresponding to the login
					/// @param noUpdate do not perform a svn update
					/// @param noWCSave do not save the current site to the Working Copy
					/// thus loosing the current changes.
					void update(const std::string& user,
						const std::string& password,
						bool noUpdate,
						bool noWCSave);
				//@}

				bool from_string(
					const std::string& text
				);

				std::string to_string(
				) const;

				static bool LoadFromRecord(
					Type& fieldObject,
					ObjectBase& object,
					const Record& record,
					const util::Env& env
				);

				static void SaveToFilesMap(
					const Type& fieldObject,
					const ObjectBase& object,
					FilesMap& map
				);

				static void SaveToParametersMap(
					const Type& fieldObject,
					const ObjectBase& object,
					util::ParametersMap& map,
					const std::string& prefix,
					boost::logic::tribool withFiles
				);

				static void SaveToDBContent(
					const Type& fieldObject,
					const ObjectBase& object,
					DBContent& content
				);

				static void GetLinkedObjectsIds(
					LinkedObjectsIds& list, 
					const Record& record
				);
			};
}	}	}

#endif
