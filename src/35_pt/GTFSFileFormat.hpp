
/** GTFSFileFormat class header.
	@file GTFSFileFormat.h

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

#ifndef SYNTHESE_GTFSFileFormat_H__
#define SYNTHESE_GTFSFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "Calendar.h"
#include "PTDataCleanerFileFormat.hpp"

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
		class JourneyPattern;
		class CommercialLine;
		class ScheduledService;

		//////////////////////////////////////////////////////////////////////////
		/// GTFS file format.
		/// See http://code.google.com/intl/fr/transit/spec/transit_feed_specification.html
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class GTFSFileFormat:
			public impex::FileFormatTemplate<GTFSFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<GTFSFileFormat>,
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string FILE_STOPS;
				static const std::string FILE_TRANSFERS;
				static const std::string FILE_AGENCY;
				static const std::string FILE_ROUTES;
				static const std::string FILE_CALENDAR;
				static const std::string FILE_CALENDAR_DATES;
				static const std::string FILE_TRIPS;
				static const std::string FILE_STOP_TIMES;
				static const std::string FILE_FARE_ATTRIBUTES;
				static const std::string FILE_FARE_RULES;
				static const std::string FILE_SHAPES;
				static const std::string FILE_FREQUENCIES;

				static const std::string PARAMETER_IMPORT_STOP_AREA;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_CITY;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;

			private:
				static const std::string SEP;

				bool _importStopArea;
				bool _interactive;
				bool _displayLinkedStops;
				boost::shared_ptr<const geography::City> _defaultCity;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;

				typedef std::map<std::string, std::size_t> FieldsMap;
				mutable FieldsMap _fieldsMap;

				mutable std::vector<std::string> _line;

				void _loadFieldsMap(const std::string& line) const;
				std::string _getValue(const std::string& field) const;
				void _loadLine(const std::string& line) const;

				typedef std::map<std::string, calendar::Calendar> Calendars;
				mutable Calendars _calendars;

				mutable impex::ImportableTableSync::ObjectBySource<TransportNetworkTableSync> _networks;
				mutable impex::ImportableTableSync::ObjectBySource<StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync> _lines;

				struct Trip
				{
					pt::CommercialLine* line;
					calendar::Calendar calendar;
					std::string destination;
					bool direction;
				};
				typedef std::map<std::string, Trip> TripsMap;
				mutable TripsMap _trips;

				struct TripDetail
				{
					boost::posix_time::time_duration arrivalTime;
					boost::posix_time::time_duration departureTime;
					std::set<pt::StopPoint*> stop;
					graph::MetricOffset offsetFromLast;
				};
				typedef std::vector<TripDetail> TripDetailVector;


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
				/// @since 3.2.0
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
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<GTFSFileFormat> Exporter_;
		};
	}
}

#endif
