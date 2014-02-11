
/** GTFSFileFormat class header.
	@file GTFSFileFormat.hpp

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

#ifndef SYNTHESE_GTFSFileFormat_H__
#define SYNTHESE_GTFSFileFormat_H__

#include "Calendar.h"
#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "OneFileExporter.hpp"
#include "PTFileFormat.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "Calendar.h"
#include "PTDataCleanerFileFormat.hpp"
#include "LineStopTableSync.h"
#include "Path.h"
#include "SchedulesBasedService.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <list>
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
		class TransportNetwork;
		class PTUseRule;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// GTFS file format.
		/// See http://code.google.com/intl/fr/transit/spec/transit_feed_specification.html
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class GTFSFileFormat:
			public impex::FileFormatTemplate<GTFSFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<GTFSFileFormat>,
				public PTDataCleanerFileFormat,
				public PTFileFormat
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
				static const std::string PARAMETER_USE_RULE_BLOCK_ID_MASK;

			private:
				static const std::string SEP;

				bool _importStopArea;
				bool _interactive;
				bool _displayLinkedStops;
				boost::shared_ptr<const geography::City> _defaultCity;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;

				typedef std::map<std::string, std::size_t> FieldsMap;
				mutable FieldsMap _fieldsMap;

				typedef std::map<std::string, const pt::PTUseRule*> PTUseRuleBlockMasks;
				PTUseRuleBlockMasks _ptUseRuleBlockMasks;
				static std::string _serializePTUseRuleBlockMasks(const PTUseRuleBlockMasks& object);

				mutable std::vector<std::string> _line;

				void _loadFieldsMap(const std::string& line) const;
				std::string _getValue(const std::string& field) const;
				void _loadLine(const std::string& line) const;

				typedef std::map<std::string, calendar::Calendar> Calendars;
				mutable Calendars _calendars;

				mutable impex::ImportableTableSync::ObjectBySource<pt::TransportNetworkTableSync> _networks;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;

				struct Trip
				{
					const pt::PTUseRule* useRule;
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
					const std::string& key
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

			class Exporter_:
				public impex::OneFileExporter<GTFSFileFormat>
			{
			private:
				boost::shared_ptr<const pt::TransportNetwork> _network;

				util::RegistryKeyType _key(util::RegistryKeyType key,
					util::RegistryKeyType suffix = 0
				) const;

				std::string _Str(std::string str) const;

				std::string _SubLine(std::string str) const;

				void _addShapes(const graph::Path* path,
					util::RegistryKeyType shapeIdKey,
					std::stringstream& shapesTxt,
					std::stringstream& tripTxt,
					std::string tripName
				) const;

				void _addTrips(std::stringstream& trip_txt,
					util::RegistryKeyType trip,
					util::RegistryKeyType service,
					util::RegistryKeyType route,
					std::string tripHeadSign,
					bool tripDirection
				) const;

				void _addFrequencies(std::stringstream& frequencies,
					util::RegistryKeyType tripId,
					const pt::ContinuousService* service
				) const;

				void _filesProvider(const pt::SchedulesBasedService* service,
					std::stringstream& stopTimesTxt,
					std::stringstream& tripsTxt,
					std::stringstream& shapesTxt,
					std::stringstream& calendarTxt,
					std::stringstream& calendarDatesTxt,
					std::stringstream& frequenciesTxt,
					std::list< std::pair< const calendar::Calendar * , util::RegistryKeyType > > & calendarMap,
					bool isContinious
				) const;

				void _addStopTimes(std::stringstream& stopTimes,
					const pt::LineStopTableSync::SearchResult linestops,
					const pt::SchedulesBasedService* service,
					bool& stopTimesExist,
					bool isContinious,
					bool isReservationMandandatory
				) const;

				void _addCalendars(std::stringstream& calendar,
					std::stringstream& calendarDates,
					const pt::SchedulesBasedService* service,
					util::RegistryKeyType serviceId,
					bool isContinious
				) const;

				static const std::string LABEL_TAD;
				static const std::string LABEL_NO_EXPORT_GTFS;
				static const int WGS84_SRID;

				static std::map<std::string,util::RegistryKeyType> shapeId;

			public:
				static const std::string PARAMETER_NETWORK_ID;

				Exporter_(){}

				virtual util::ParametersMap getParametersMap() const;

				virtual void setFromParametersMap(const util::ParametersMap& map);

				virtual void build(std::ostream& os) const;

				virtual std::string getOutputMimeType() const { return "application/zip"; }
				virtual std::string getFileName() const { return "GTFS.zip"; }

			};
		};
	}
}

#endif
