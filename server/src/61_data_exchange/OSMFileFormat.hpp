
/** OSMFileFormat class header.
	@file OSMFileFormat.hpp

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

#ifndef SYNTHESE_dataexchange_OSMFileFormat_hpp__
#define SYNTHESE_dataexchange_OSMFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"

#include <string>


namespace synthese
{

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// OSM CityBoundaries file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m34
		class OSMFileFormat:
			public impex::FileFormatTemplate<OSMFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_: public impex::OneFileTypeImporter<OSMFileFormat>
			{
			private:

				static const std::string PARAMETER_COUNTRY_CODE;

				static const std::string PARAMETER_PROJECT_HOUSES;

				boost::optional<std::string> _countryCode;

				bool _projectHouses;


			protected:

				virtual bool _parse(
					const boost::filesystem::path& filePath
				) const;


			public:

				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);


				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.3.0
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.3.0
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				//////////////////////////////////////////////////////////////////////////
				/// Saves all imported data.
				/// @return SQL transaction to run
				/// @author Sylvain Pasche
				/// @since 3.3.0
				/// @date 2011
				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<OSMFileFormat> Exporter_;
		};
	}
}

#endif // SYNTHESE_dataexchange_OSMFileFormat_hpp__
