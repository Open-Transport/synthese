
/** PubliBikeCSVFileFormat class header.
	@file PubliBikeCSVFileFormat.hpp

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

#ifndef SYNTHESE_PubliBikeCSVFileFormat_H__
#define SYNTHESE_PubliBikeCSVFileFormat_H__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"

#include "ImportableTableSync.hpp"
#include "PublicBikeStationTableSync.hpp"
#include "PublicBikeNetworkTableSync.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// PubliBike CSV file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class PubliBikeCSVFileFormat:
			public impex::FileFormatTemplate<PubliBikeCSVFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::OneFileTypeImporter<PubliBikeCSVFileFormat>
			{
			public:
				static const std::string FILE_PUBLIBIKE;
				static const std::string PARAMETER_PROJECT_STATIONS;



			private:
				static const std::string SEP;

				mutable std::vector<std::string> _line;
				std::string _getValue(std::size_t rank) const;
				void _loadLine(const std::string& line) const;
				bool _projectStations;

				mutable impex::ImportableTableSync::ObjectBySource<public_biking::PublicBikeStationTableSync> _stations;
				mutable impex::ImportableTableSync::ObjectBySource<public_biking::PublicBikeNetworkTableSync> _networks;

				public_biking::PublicBikeNetwork* _createOrUpdatePublicBikeNetwork(
					impex::ImportableTableSync::ObjectBySource<public_biking::PublicBikeNetworkTableSync>& publicBikeNetworks,
					const std::string& networkNumber,
					const std::string& networkName
				) const;

				public_biking::PublicBikeStation* _createOrUpdatePublicBikeStation(
					impex::ImportableTableSync::ObjectBySource<public_biking::PublicBikeStationTableSync>& publicBikeStations,
					const public_biking::PublicBikeNetwork& network,
					const std::string& stationNumber,
					const std::string& stationName,
					boost::optional<boost::shared_ptr<geos::geom::Point> > geometry,
					const geography::City& city
				) const;



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
				/// @author Thomas Puigt
				/// @date 2014
				/// @since 3.8
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Thomas Puigt
				/// @date 2014
				/// @since 3.8
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<PubliBikeCSVFileFormat> Exporter_;
		};
	}
}

#endif
