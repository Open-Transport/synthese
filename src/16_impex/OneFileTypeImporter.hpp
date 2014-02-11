
/** OneFileTypeImporter class header.
	@file OneFileTypeImporter.hpp

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

#ifndef SYNTHESE_impex_OneFileTypeImporter_hpp__
#define SYNTHESE_impex_OneFileTypeImporter_hpp__

#include "Importer.hpp"

#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/operations.hpp>

#ifdef _MSC_VER
#pragma warning( disable : 4250 )
#endif

namespace synthese
{
	namespace impex
	{
		/** OneFileTypeImporter class.
			@ingroup m16
		*/
		template<class FF>
		class OneFileTypeImporter:
			virtual public Importer
		{
		public:
			static const bool IMPORTABLE;
			static const std::string PARAMETER_PATH;	//!< Path of the files to import
			static const std::string PARAMETER_DIRECTORY;

			typedef std::set<boost::filesystem::path> FilePathsSet;

		protected:
			virtual bool _parse(
				const boost::filesystem::path& filePath
			) const = 0;

			FilePathsSet _pathsSet;
			boost::filesystem::path _dirPath;

			virtual void _setFromParametersMap(const util::ParametersMap& map) {}

			virtual util::ParametersMap _getParametersMap() const { return util::ParametersMap(); }

		public:
			OneFileTypeImporter(
				util::Env& env,
				const Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
			{}



			void setFromParametersMap(
				const util::ParametersMap& map,
				bool doImport
			){
				if(map.getOptional<std::string>(PARAMETER_PATH))
				{
					std::vector<std::string> pathsVector;
					std::string path(map.get<std::string>(PARAMETER_PATH));
					boost::algorithm::split(pathsVector, path, boost::is_any_of(","));
					BOOST_FOREACH(const std::string& token, pathsVector)
					{
						if(token.empty()) continue;
						_pathsSet.insert(token);
					}
				}
				else if(map.getOptional<std::string>(PARAMETER_DIRECTORY))
				{
					_dirPath = map.get<std::string>(PARAMETER_DIRECTORY);
					if(boost::filesystem::is_directory(_dirPath))
					{
						for(boost::filesystem::directory_iterator it(_dirPath); it != boost::filesystem::directory_iterator(); ++it)
						{
							boost::filesystem::path filePath(*it);
							if(!boost::filesystem::is_directory(filePath))
							{
								_pathsSet.insert(filePath);
							}
						}
					}
				}
				_setFromParametersMap(map);
			}


			util::ParametersMap getParametersMap() const
			{
				std::stringstream s;
				bool first(true);
				BOOST_FOREACH(const FilePathsSet::value_type& path, _pathsSet)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						s << ",";
					}
					s << path;
				}
				util::ParametersMap result(_getParametersMap());
				result.insert(PARAMETER_PATH, s.str());
				return result;
			}


			bool parseFiles(
			) const {

				bool result(true);
				BOOST_FOREACH(const FilePathsSet::value_type& path, _pathsSet)
				{
					result &= _parse(path);
				}
				return result;
			}
		};

		template<class FF>
		const bool OneFileTypeImporter<FF>::IMPORTABLE(true);

		template<class FF>
		const std::string OneFileTypeImporter<FF>::PARAMETER_PATH("pa");

		template<class FF>
		const std::string OneFileTypeImporter<FF>::PARAMETER_DIRECTORY("dr");
}	}

#endif // SYNTHESE_impex_OneFileTypeImporter_hpp__
