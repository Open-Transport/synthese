/** Import/export file format class header.
	@file FileFormat.h

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

#ifndef SYNTHESE_impex_FileFormat_h__
#define SYNTHESE_impex_FileFormat_h__

#include "FactoryBase.h"
#include "Env.h"

#include <boost/filesystem/path.hpp>
#include <set>
#include <map>

namespace synthese
{
	namespace impex
	{
		class DataSource;
		
		/** Import/export file format factorable base class.
			
			Each import subclass defines a format which can be imported.
			
			@ingroup m16

			Import

			There is two ways to launch the data import upon file paths :
				- run parseFiles with a paths set argument : there is only one file type to read, 
					but the data can be provided in more than one file. The set contains
					all the paths.
				- run parseFiles with a paths map argument : there is more than one file type, each
					type contains a part of the fields to import and is identified by a key.
					The key is also the parameter code Request field containing the file path.
					The files are loaded respecting the key alphabetical order.
		*/
		class FileFormat : public util::FactoryBase<FileFormat>
		{
		public:
			typedef std::string FileKey;
			typedef std::set<boost::filesystem::path> FilePathsSet;
			typedef std::map<FileKey, boost::filesystem::path> FilePathsMap;

			class Files
			{
			public:
				typedef std::vector<FileKey> FilesVector;

			private:
				FilesVector _files;

			public:
				Files(
					const char* value,
					...
				);

				const FilesVector& getFiles() const;
			};

		protected:
			FileFormat(
				util::Env* env = NULL
			);
			
			util::Env*				_env;
			const DataSource*		_dataSource;
			
			virtual bool _controlPathsMap(
				const FilePathsMap& paths
			);

			virtual void _parse(
				const boost::filesystem::path& filePath,
				std::ostream& os,
				std::string key = std::string()
			) = 0;
			
			
		public:
			virtual const Files::FilesVector& getFiles() const = 0;

			/** Generic export method.
				The generic export consists in the export of each registered exportable class
				@author Hugues Romain
				@date 2008
			*/
			virtual void build(
				std::ostream& os
			) = 0;
			
			void parseFiles(
				const FilePathsSet& paths,
				std::ostream& os
			);

			void parseFiles(
				const FilePathsMap& paths,
				std::ostream& os
			);
			
			virtual void save(
				std::ostream& os
			) const = 0;
			
			void setDataSource(const DataSource* value);
			void setEnv(util::Env* value);
		};
	}
}

#endif // SYNTHESE_impex_Import_h__
