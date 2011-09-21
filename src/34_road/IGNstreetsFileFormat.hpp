
/** IGNstreetsFileFormat class header.
	@file IGNstreetsFileFormat.hpp
	@author Gaël Sauvanet

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

#ifndef SYNTHESE_road_IGNstreetFileFormat_hpp__
#define SYNTHESE_road_IGNstreetFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "MainRoadChunk.hpp"

#include <map>
#include <ostream>
#include <string>

namespace geos
{
	namespace geom
	{
		class LineString;
	}
}

namespace synthese
{
	namespace road
	{
		//////////////////////////////////////////////////////////////////////////
		/// 34.16 : IGNstreets file format class.
		///	@ingroup m34
		/// @author Gaël Sauvanet
		/// @since 3.2.0
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class IGNstreetsFileFormat:
			public impex::FileFormatTemplate<IGNstreetsFileFormat>
		{
		protected:
			//! @name Fields of address table
			//@{
				static const std::string _FIELD_NOM_VOIE;
				static const std::string _FIELD_NUMERO;
				static const std::string _FIELD_GEOMETRY;
				static const std::string _FIELD_INSEE_COMM;
			//@}

		public:

			class Importer_:
				public impex::MultipleFileTypesImporter<IGNstreetsFileFormat>
			{
				friend class impex::MultipleFileTypesImporter<IGNstreetsFileFormat>;

			private:
				//! @name Tables
				//@{
					static const std::string FILE_ADDRESS;
				//@}

				//! @name Parameters
				//@{
				//@}


			protected:

				//////////////////////////////////////////////////////////////////////////
				/// Checks that all necessary input files are available.
				/// @result true if all necessary files are present
				virtual bool _checkPathsMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads an input files and load its content to the memory.
				/// @param filePath file to read
				/// @param os stream to write log messages on
				/// @param key type of the file
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const admin::AdminRequest&> request
				) const;


			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				):	impex::MultipleFileTypesImporter<IGNstreetsFileFormat>(env, dataSource),
					impex::Importer(env, dataSource)
				{}

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Gaël Sauvanet
				/// @date 2010
				/// @since 3.1.16
				virtual util::ParametersMap _getParametersMap() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Gaël Sauvanet
				/// @date 2010
				/// @since 3.1.16
				virtual void _setFromParametersMap(const util::ParametersMap& map);

				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Saves all imported data.
				/// @return SQL transaction to run
				/// @author Gaël Sauvanet
				/// @since 3.2.0
				/// @date 2010
				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<IGNstreetsFileFormat> Exporter_;

			friend class Importer_;
		};
}	}

#endif // SYNTHESE_road_IGNstreetFileFormat_hpp__
