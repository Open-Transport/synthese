
/** ShapeFileFormat class header.
	@file ShapeFileFormat.hpp

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

#ifndef SYNTHESE_ShapeFileFormat_H__
#define SYNTHESE_ShapeFileFormat_H__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Shapefile file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class ShapeFileFormat:
			public impex::FileFormatTemplate<ShapeFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<ShapeFileFormat>,
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string FILE_SHAPE; // Shapefile

				static const std::string PARAMETER_STOP_AREA_DEFAULT_CITY;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;

				static const std::string PARAMETER_FIELD_STOP_NAME1;
				static const std::string PARAMETER_FIELD_STOP_NAME2;
				static const std::string PARAMETER_FIELD_STOP_OPERATOR_CODE;
				static const std::string PARAMETER_FIELD_CITY_NAME;
				static const std::string PARAMETER_FIELD_CITY_CODE;

				static const std::string _FIELD_GEOMETRY;

			private:
				//! @name Parameters
				//@{
					bool _displayLinkedStops;
					boost::shared_ptr<const geography::City> _defaultCity;
					boost::posix_time::time_duration _stopAreaDefaultTransferDuration;

					boost::optional<std::string> _stopName1;
					boost::optional<std::string> _stopName2;
					boost::optional<std::string> _stopOperatorCode;
					boost::optional<std::string> _cityName;
					boost::optional<std::string> _cityCode;
				//@}

				mutable impex::ImportableTableSync::ObjectBySource<StopPointTableSync> _stopPoints;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;


			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);

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
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @date 2011
				/// @since 3.2.1
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<ShapeFileFormat> Exporter_;
		};
	}
}

#endif
