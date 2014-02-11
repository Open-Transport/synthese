
/** HeuresFileFormat class header.
	@file HeuresFileFormat.hpp

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

#ifndef SYNTHESE_HeuresFileFormat_H__
#define SYNTHESE_HeuresFileFormat_H__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "OneFileExporter.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTOperationFileFormat.hpp"
#include "PTFileFormat.hpp"

#include "DepotTableSync.hpp"
#include "DriverService.hpp"
#include "ScheduledService.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace util
	{
		class Registrable;
	}

	namespace geography
	{
		class City;
	}

	namespace tree
	{
		class TreeFolder;
	}

	namespace pt
	{
		class Destination;
		class JourneyPattern;
		class CommercialLine;
		class ScheduledService;
		class TransportNetwork;
	}

	namespace pt_operation
	{
		class Depot;
		class DeadRun;
		class VehicleService;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Heures file format.
		/// @ingroup m61File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The Heures schedules import uses 3 files :
		///	<ul>
		///		<li>itinerai.tmp : Routes</li>
		///		<li>troncons.tmp : Schedules</li>
		///		<li>services.tmp : Calendars</li>
		///	</ul>
		///
		class HeuresFileFormat:
			public impex::FileFormatTemplate<HeuresFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<HeuresFileFormat>,
				public PTDataCleanerFileFormat,
				public PTFileFormat,
				public PTOperationFileFormat
			{

			public:
				static const std::string FILE_POINTSARRETS;
				static const std::string FILE_ITINERAI;
				static const std::string FILE_TRONCONS;
				static const std::string FILE_SERVICES;

				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_DAY7_CALENDAR_ID;
				static const std::string PARAMETER_STOPS_DATASOURCE_ID;

			private:

				bool _displayLinkedStops;
				boost::shared_ptr<pt::TransportNetwork> _network;
				boost::shared_ptr<const calendar::CalendarTemplate> _day7CalendarTemplate;
				boost::shared_ptr<const impex::DataSource> _stopsDataSource;

				mutable ImportableStopPoints _linkedStopPoints;
				mutable ImportableStopPoints _nonLinkedStopPoints;

				typedef std::map<std::pair<int, std::string>, pt::JourneyPattern*> RoutesMap;
				mutable RoutesMap _routes;

				typedef std::map<std::pair<int, int>, std::vector<pt::SchedulesBasedService*> > ServicesMap;
				mutable ServicesMap _services;

				typedef std::map<int, pt::Destination*> DestinationsMap;
				mutable DestinationsMap _destinations;

				mutable impex::ImportableTableSync::ObjectBySource<pt_operation::DepotTableSync> _depots;

				struct ScheduleMapElement
				{
					typedef std::vector<std::pair<int, int> > TechnicalLink;
					TechnicalLink technicalLink; // technical line, elementary service number
					pt::ScheduledService::Schedules departure;
					pt::ScheduledService::Schedules arrival;
					std::vector<pt_operation::VehicleService*> vehicleServices;
					typedef std::vector<
						std::pair<
							boost::shared_ptr<pt_operation::DriverService::Chunk>,
							std::size_t // Rank in the chunk
						>
					> DriverServices;
					DriverServices driverServices;
				};

				struct DeadRunRoute
				{
					bool depotToStop;
					pt_operation::Depot* depot;
					pt::StopPoint* stop;
					graph::MetricOffset length;

					DeadRunRoute() :
						depot(NULL),
						stop(NULL)
					{}
				};

				typedef std::map<
					std::pair<int, std::string>,
					DeadRunRoute
				> DeadRunRoutes;
				mutable DeadRunRoutes _deadRunRoutes;

				typedef std::map<
					std::string,
					boost::shared_ptr<pt_operation::DriverService::Chunk>
				> Troncons;
				mutable Troncons _troncons;

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



				virtual db::DBTransaction _save() const;



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
			};

			class Exporter_:
				public impex::OneFileExporter<HeuresFileFormat>
			{
			private:
				boost::shared_ptr<const pt::TransportNetwork> _network;
				boost::shared_ptr<tree::TreeFolder> _folder;
				boost::shared_ptr<const impex::DataSource> _dataSource;
				bool _generateRouteCode;
				typedef std::map<const pt::JourneyPattern*, size_t> RouteCodes;
				mutable RouteCodes _routeCodes;
				size_t _serviceNumberPosition;

			public:
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_DATASOURCE_ID;
				static const std::string PARAMETER_GENERATE_ROUTE_CODE;
				static const std::string PARAMETER_SERVICE_NUMBER_POSITION;

				Exporter_();

				virtual util::ParametersMap getParametersMap() const;

				virtual void setFromParametersMap(const util::ParametersMap& map);

				virtual void build(std::ostream& os) const;

				static void _writeHour(std::ostream& os, const boost::posix_time::time_duration& duration);
				static void _writeTextAndSpaces(
					std::ostream& os,
					const std::string& value,
					size_t width,
					bool spacesAtRight = true,
					char spaceChar = ' '
				);

				virtual std::string getOutputMimeType() const { return "application/zip"; }
				virtual std::string getFileName() const { return "Heures.zip"; }
			};
		};
}	}

#endif
