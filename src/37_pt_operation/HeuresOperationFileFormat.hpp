
/** HeuresOperationFileFormat class header.
	@file HeuresOperationFileFormat.hpp

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

#ifndef SYNTHESE_HeuresOperationFileFormat_H__
#define SYNTHESE_HeuresOperationFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "GraphTypes.h"
#include "DepotTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "DriverService.hpp"
#include "ScheduledService.h"

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

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class TransportNetwork;
		class StopPoint;
		class JourneyPattern;
	}

	namespace pt_operation
	{
		class Depot;
		class DeadRun;
		class VehicleService;

		//////////////////////////////////////////////////////////////////////////
		/// Heures file format.
		/// @ingroup m35File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The Heures operation import uses 2 files :
		///	<ul>
		///		<li>troncons.tmp : Schedules</li>
		///		<li>services.tmp : Calendars</li>
		///	</ul>
		class HeuresOperationFileFormat:
			public impex::FileFormatTemplate<HeuresOperationFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<HeuresOperationFileFormat>
			{

			public:
				static const std::string FILE_TRONCONS;
				static const std::string FILE_POINTSARRETS;
				static const std::string FILE_ITINERAI;
				static const std::string FILE_SERVICES;

				static const std::string PARAMETER_START_DATE;
				static const std::string PARAMETER_END_DATE;
				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_PT_DATASOURCE_ID;
				static const std::string PARAMETER_STOPS_DATASOURCE_ID;
				static const std::string PARAMETER_CLEAN_OLD_DATA;
				static const std::string PARAMETER_DAY7_CALENDAR_ID;

			private:

				bool _displayLinkedStops;
				boost::shared_ptr<const pt::TransportNetwork> _network;
				boost::shared_ptr<const impex::DataSource> _ptDataSource;
				boost::shared_ptr<const impex::DataSource> _stopsDataSource;
				boost::optional<boost::gregorian::date> _startDate;
				boost::optional<boost::gregorian::date> _endDate;
				boost::shared_ptr<const calendar::CalendarTemplate> _day7CalendarTemplate;

				mutable impex::ImportableTableSync::ObjectBySource<DepotTableSync> _depots;

				struct ScheduleMapElement
				{
					pt::ScheduledService::Schedules departure;
					pt::ScheduledService::Schedules arrival;
					std::vector<VehicleService*> vehicleServices;
					typedef std::vector<
						std::pair<
							boost::shared_ptr<DriverService::Chunk>,
							std::size_t // Rank in the chunk
						>
					> DriverServices;
					DriverServices driverServices;
				};

				typedef std::map<std::pair<int, std::string>, pt::JourneyPattern*> RoutesMap;
				mutable RoutesMap _routes;

				typedef std::map<std::string, std::vector<DeadRun*> > ServicesMap;
				mutable ServicesMap _services;

				struct DeadRunRoute
				{
					bool depotToStop;
					Depot* depot;
					pt::StopPoint* stop;
					graph::MetricOffset length;
				};

				typedef std::map<
					std::pair<int, std::string>,
					DeadRunRoute
				> DeadRunRoutes;
				mutable DeadRunRoutes _deadRunRoutes;

				typedef std::map<
					std::string,
					boost::shared_ptr<DriverService::Chunk>
				> Troncons;
				mutable Troncons _troncons;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					const std::string& key,
					boost::optional<const server::Request&> adminRequest
				) const;



			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					const impex::ImportLogger& logger
				);
				


				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const server::Request& request
				) const;

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

			typedef impex::NoExportPolicy<HeuresOperationFileFormat> Exporter_;
		};
	}
}

#endif
