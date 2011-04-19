
/** ManyFileTypesImporter class header.
	@file ManyFileTypesImporter.hpp

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

#ifndef SYNTHESE_impex_ManyFileTypesImporter_hpp__
#define SYNTHESE_impex_ManyFileTypesImporter_hpp__

#include "Exception.h"
#include "Importer.hpp"

#include <vector>
#include <stdarg.h>

namespace synthese
{
	namespace impex
	{
		/** ManyFileTypesImporter class.
			@ingroup m16
		*/
		template<class FF>
		class MultipleFileTypesImporter:
			public Importer
		{
		private:
			static const std::string PARAMETER_PATH;	//!< Path of the files to import

		public:
			static const bool IMPORTABLE;
			
			typedef std::string FileKey;
			typedef std::map<FileKey, boost::filesystem::path> FilePathsMap;

		protected:
			mutable FilePathsMap			_pathsMap;

			static std::string _getFileParameterName(const FileKey& file){ return PARAMETER_PATH + file; }

		public:
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
				){
					const char* col(value);
					va_list marker;
					for(va_start(marker, value); col[0]; col = va_arg(marker, const char*))
					{
						_files.push_back(std::string(col));
					}
					va_end(marker);
				}


				const FilesVector& getFiles() const { return _files; }
			};

			class MissingFileException:
				public Exception
			{
			public:
				MissingFileException() : Exception("At least a bad or missing file name") {}
			};

			static const Files FILES;

			MultipleFileTypesImporter(
				util::Env& env,
				const DataSource& dataSource
			):	Importer(env, dataSource)
			{}

			virtual bool _parse(
				const boost::filesystem::path& filePath,
				std::ostream& os,
				const FileKey& fileKey,
				boost::optional<const admin::AdminRequest&> request
			) const = 0;

			virtual bool _controlPathsMap() const = 0;

			virtual void _setFromParametersMap(const server::ParametersMap& map) {}

			virtual server::ParametersMap _getParametersMap() const { return server::ParametersMap(); }

			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doImport
			){
				BOOST_FOREACH(const std::string& key, FILES.getFiles())
				{
					std::string filePath(map.getDefault<std::string>(_getFileParameterName(key)));
					if(filePath.empty())
					{
						continue;
					}
					_pathsMap.insert(std::make_pair(key, filePath));
				}
				if (doImport && !_controlPathsMap())
				{
					throw MissingFileException();
				}
				_setFromParametersMap(map);
			}


			server::ParametersMap getParametersMap() const
			{
				server::ParametersMap result(_getParametersMap());
				BOOST_FOREACH(const FilePathsMap::value_type& it, _pathsMap)
				{
					result.insert(_getFileParameterName(it.first), it.second.file_string());
				}
				return result;
			}


			bool parseFiles(
				std::ostream& os,
				boost::optional<const admin::AdminRequest&> request
			) const {
				BOOST_FOREACH(const std::string& key, FILES.getFiles())
				{
					FilePathsMap::const_iterator it(_pathsMap.find(key));
					if(it == _pathsMap.end() || it->second.file_string().empty())
					{
						continue;
					}
					const FilePathsMap::mapped_type& path(it->second);

					if(!_parse(path, os, key, request))
					{
						return false;
					}
				}
				return true;
			}
		};

		template<class FF>
		const bool MultipleFileTypesImporter<FF>::IMPORTABLE(true);

		template<class FF>
		const std::string MultipleFileTypesImporter<FF>::PARAMETER_PATH("pa");
}	}

#endif // SYNTHESE_impex_ManyFileTypesImporter_hpp__
