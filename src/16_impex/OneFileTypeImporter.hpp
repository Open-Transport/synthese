
/** OneFileTypeImporter class header.
	@file OneFileTypeImporter.hpp

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

#ifndef SYNTHESE_impex_OneFileTypeImporter_hpp__
#define SYNTHESE_impex_OneFileTypeImporter_hpp__

#include "Importer.hpp"

namespace synthese
{
	namespace impex
	{
		/** OneFileTypeImporter class.
			@ingroup m16
		*/
		template<class FF>
		class OneFileTypeImporter:
			public Importer
		{
		public:
			static const bool IMPORTABLE;
			static const std::string PARAMETER_PATH;	//!< Path of the files to import

			typedef std::set<boost::filesystem::path> FilePathsSet;

		protected:
			virtual bool _parse(
				const boost::filesystem::path& filePath,
				std::ostream& os,
				boost::optional<const admin::AdminRequest&> request
			) const = 0;

			FilePathsSet _pathsSet;

			virtual void _setFromParametersMap(const server::ParametersMap& map) {}

			virtual server::ParametersMap _getParametersMap() const { return server::ParametersMap(); }

		public:
			OneFileTypeImporter(const DataSource& dataSource):
				Importer(dataSource)
			{}
			

			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doImport
			){
				tokenizer<char_separator<char> > pathsTokens(
					map.get<string>(PARAMETER_PATH),
					char_separator<char>(",")
				);
				BOOST_FOREACH(const string& token, pathsTokens)
				{
					if(token.empty()) continue;
					_pathsSet.insert(token);
				}
				_setFromParametersMap(map);
			}


			server::ParametersMap getParametersMap() const
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
				server::ParametersMap result(_getParametersMap());
				result.insert(PARAMETER_PATH, s.str());
				return result;
			}


			bool parseFiles(
				std::ostream& os,
				boost::optional<const admin::AdminRequest&> request
			) const {
				bool result(true);
				BOOST_FOREACH(const FilePathsSet::value_type& path, _pathsSet)
				{
					result &= _parse(path, os, request);
				}
				return result;
			}
		};

		template<class FF>
		const bool OneFileTypeImporter<FF>::IMPORTABLE(true);

		template<class FF>
		const std::string OneFileTypeImporter<FF>::PARAMETER_PATH("pa");
}	}

#endif // SYNTHESE_impex_OneFileTypeImporter_hpp__
