/** GTFSStopsFileFormat class header.
	@file GTFSStopsFileFormat.hpp

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

#ifndef SYNTHESE_GTFSStopsFileFormat_H__
#define SYNTHESE_GTFSStopsFileFormat_H__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

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
		class StopPoint;

		//////////////////////////////////////////////////////////////////////////
		/// GTFS file format for stops integration.
		/// See http://code.google.com/intl/fr/transit/spec/transit_feed_specification.html
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		/// @since 3.2.1
		/// @date 2011
		class GTFSStopsFileFormat:
			public impex::FileFormatTemplate<GTFSStopsFileFormat>
		{
		public:

			class Importer_:
				public impex::MultipleFileTypesImporter<GTFSStopsFileFormat>
			{
			public:
				static const std::string FILE_STOPS;
				static const std::string FILE_TRANSFERS;

				static const std::string PARAMETER_IMPORT_STOP_AREA;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_CITY;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;

			private:
				static const std::string SEP;

				bool _importStopArea;
				bool _interactive;
				boost::shared_ptr<const geography::City> _defaultCity;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;

				mutable impex::ImportableTableSync::ObjectBySource<StopPointTableSync> _stopPoints;

				typedef std::map<std::string, std::size_t> FieldsMap;
				mutable FieldsMap _fieldsMap;
				mutable std::vector<std::string> _line;

				void _loadFieldsMap(const std::string& line) const;
				std::string _getValue(const std::string& field) const;
				void _loadLine(const std::string& line) const;

			protected:

				virtual bool _controlPathsMap() const;
				
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
				/// @since 3.2.1
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual server::ParametersMap _getParametersMap() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const server::ParametersMap& map);


				virtual db::SQLiteTransaction _save() const;
			};

			typedef impex::NoExportPolicy<GTFSStopsFileFormat> Exporter_;
		};
}	}

#endif
