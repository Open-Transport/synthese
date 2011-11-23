
/** IneoFileFormat class header.
	@file IneoFileFormat.hpp

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

#ifndef SYNTHESE_IneoFileFormat_H__
#define SYNTHESE_IneoFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "Calendar.h"
#include "DestinationTableSync.hpp"
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
		class TransportNetwork;
		class JourneyPattern;
		class CommercialLine;
		class ScheduledService;
		class RollingStock;

		//////////////////////////////////////////////////////////////////////////
		/// Ineo file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class IneoFileFormat:
			public impex::FileFormatTemplate<IneoFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<IneoFileFormat>,
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string FILE_PNT; // Stops
				static const std::string FILE_DIS; // Distances
				static const std::string FILE_DST; // Destinations
				static const std::string FILE_LIG; // Lines
				static const std::string FILE_CJV; // Validity dates
				static const std::string FILE_HOR; // Schedules
				static const std::string FILE_CAL; // Calendars

				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_TRANSPORT_MODE_TYPE_LG_MASK;

				static const std::string PARAMETER_AUTO_IMPORT_STOPS;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_CITY;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;
				static const std::string PARAMETER_LINE_READ_METHOD;
				static const std::string VALUE_CIDX;
				static const std::string VALUE_SV;
				static const std::string PARAMETER_LINE_SHORT_NAME_FIELD;
				static const std::string VALUE_NLGIV;
				static const std::string VALUE_MNLC;
				static const std::string PARAMETER_ADD_WAYBACK_TO_JOURNEYPATTERN_CODE;
				static const std::string PARAMETER_STOP_AREA_ID_FIELD;
				static const std::string VALUE_MNCP;
				static const std::string PARAMETER_STOP_ID_FIELD;
				static const std::string VALUE_MNLP;
				static const std::string VALUE_IDENTSMS;
				static const std::string PARAMETER_STOP_CITY_CODE_FIELD;
				static const std::string VALUE_CODE_COMMUNE;
				static const std::string PARAMETER_STOP_NAME_FIELD;
				static const std::string VALUE_LIBP;
				static const std::string VALUE_LIBCOM;
				static const std::string PARAMETER_STOP_HANDICAPPED_ACCESSIBILITY_FIELD;
				static const std::string VALUE_UFR;
				
			private:
				//! @name Parameters
				//@{
					boost::shared_ptr<TransportNetwork> _network;
					
// TODO				std::map<std::string, boost::shared_ptr<RollingStock> > _transportModeTypeLgMap;

					bool _importStopAreas;

					bool _autoImportStops;
					bool _displayLinkedStops;
					boost::shared_ptr<const geography::City> _defaultCity;
					boost::posix_time::time_duration _stopAreaDefaultTransferDuration;
					std::string _lineReadMethod;
					std::string _lineShortNameField;
					std::string _stopAreaIdField;
					std::string _stopIdField;
					std::string _stopNameField;
					std::string _stopCityCodeField;
					std::string _stopHandicappedAccessibilityField;
				//@}

				static const std::string SEP;

				typedef std::map<std::string, std::vector<std::string> > FieldMaps;
				mutable FieldMaps _fieldsMap;

				bool _interactive;

				mutable std::map<std::string, std::string> _line;
				mutable std::string _section;

				void _clearFieldsMap() const;
				std::string _getValue(const std::string& field) const;
				bool _readLine(std::ifstream& file) const;
				void _loadLine(const std::string& line) const;

				//! @name Parameters
				//@{
					boost::gregorian::date _startDate;
					boost::gregorian::date _endDate;
					bool _addWaybackToJourneyPatternCode;
				//@}

				mutable impex::ImportableTableSync::ObjectBySource<DestinationTableSync> _destinations;
				mutable impex::ImportableTableSync::ObjectBySource<StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync> _lines;
				mutable std::map<std::pair<std::string, std::string>, pt::JourneyPattern*> _journeyPatterns;
				mutable std::map<std::pair<std::string, std::string>, graph::MetricOffset> _distances;
				mutable std::map<std::pair<int, int>, std::vector<boost::gregorian::date> > _dates;
				mutable std::map<std::string, std::vector<int> > _calendars;

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

			typedef impex::NoExportPolicy<IneoFileFormat> Exporter_;
		};
	}
}

#endif
