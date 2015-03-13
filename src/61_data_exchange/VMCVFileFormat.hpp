
//////////////////////////////////////////////////////////////////////////
/// Ineo BDSI file format class header.
///	@file VMCVFileFormat.hpp
///	@author Thomas Puigt
///	@date 2015
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_VMCVFileFormat_H__
#define SYNTHESE_VMCVFileFormat_H__

#include "DatabaseReadImporter.hpp"
#include "FactorableTemplate.h"
#include "FileFormatTemplate.h"
#include "PTFileFormat.hpp"
#include "NoExportPolicy.hpp"

#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "SchedulesBasedService.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace departure_boards
	{
		class DisplayScreen;
	}

	namespace pt_operation
	{
		class Depot;
	}

	namespace pt
	{
		class StopPoint;
		class StopArea;
		class CommercialLine;
		class JourneyPattern;
		class ScheduledService;
		class TransportNetwork;
	}

	namespace geography
	{
		class City;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// VMCV file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class VMCVFileFormat:
			public impex::FileFormatTemplate<VMCVFileFormat>
		{
		public:

			class Importer_:
				public impex::DatabaseReadImporter<VMCVFileFormat>,
				public PTFileFormat
			{
			public:
				static const std::string PARAMETER_DB_CONN_STRING;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_CITY;
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_PLANNED_DATASOURCE_ID;
				static const std::string PARAMETER_HYSTERESIS;
				static const std::string PARAMETER_DELAY_BUS_STOP;
				static const std::string PARAMETER_DAY_BREAK_TIME;

			private:
				// Vector to avoid reentrance and mutex to protect this vector
				static boost::recursive_mutex _tabRunningVmcvDBMutex;
				static std::set<util::RegistryKeyType> _runningVmcvDB;

				boost::optional<std::string> _dbConnString;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;
				boost::shared_ptr<const impex::DataSource> _plannedDataSource;
				boost::posix_time::time_duration _hysteresis;
				boost::posix_time::time_duration _delay_bus_stop;
				boost::posix_time::time_duration _dayBreakTime;
				boost::shared_ptr<const geography::City> _defaultCity;
				boost::shared_ptr<pt::TransportNetwork> _network;

				mutable std::set<pt::ScheduledService*> _servicesToSave;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync> _stopAreas;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;

				struct VMCVPoint
				{
					int id; /* PNT_Id */
					boost::shared_ptr<geos::geom::Point> geometry;
					const pt::StopPoint* syntheseStop;
//					pt_operation::Depot* syntheseDepot;
				};
				typedef std::map<int, VMCVPoint> VMCVPoints;

				struct VMCVLineStop
				{
					int id; /* PNT_Id */
					boost::shared_ptr<geos::geom::Point> geometry;
					pt::StopPoint* syntheseStop;
					graph::MetricOffset offsetFromPreviousStop;
					std::string getStopName() const;
				};

				/*
				 * A link is a couple of physical stops bounded by
				 * a sequence of waypoints (i.e. points with no stop associated).
				 * Only the physical stops (VMCVStops) are registered in
				 * this temporary object.
				 */
				struct Link
				{
					int id; /* LNK_Id */
					typedef std::vector<VMCVLineStop> VMCVLineStops;
					VMCVLineStops stops;
					boost::shared_ptr<geos::geom::LineString> lineString;
				};
				typedef std::map<int /* LNK_Id */, Link> LinksMap;

				void _selectAndLoadLink(
						LinksMap& links,
						int id,
						const Link::VMCVLineStops& stops,
						boost::shared_ptr<geos::geom::LineString> lineString
				) const;

				struct Route
				{
					int id; /* ROU_Id */
					std::string name;
					pt::CommercialLine* line;
					bool direction;
					// Destination dest;
					typedef std::vector<Link> Links;
					Links links;
				};

				typedef std::map<int /* ROU_Id */, Route> RoutesMap;
				mutable RoutesMap _routes;


				void _selectAndLoadRoute(
						RoutesMap& routes,
						const Route::Links& links,
						pt::CommercialLine* line,
						const std::string& name,
						bool direction,
						int id
				) const;

				struct VMCVSchedule
				{
					boost::posix_time::time_duration dept; /* departure time */
				};
				typedef std::vector<VMCVSchedule> VMCVSchedules;
				typedef std::map<int /* ROU_Id */, VMCVSchedules> VMCVSchedulesMap;
				mutable VMCVSchedulesMap _vmcvSchedules;

				typedef std::map<int /* HTY_Id */, calendar::Calendar> Calendars;
				mutable Calendars _calendars;

				struct Run
				{
					int id; /* Run_Id */
					Route* route;
					std::string service_number;
					calendar::Calendar calendar;
					VMCVSchedules schedules;
				};
				typedef std::map<int /* RUN_Id */, Run> RunsMap;
				mutable RunsMap _runs;

				void _selectAndLoadRun(
						RunsMap& runs,
						int runId,
						Route& route,
						const calendar::Calendar& calendar,
						const VMCVSchedules& schedules,
						const std::string& service_number
				) const;



				void _logLoadDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logWarningDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logDebugDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logTraceDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;
				const boost::posix_time::time_duration& getHysteresis() const { return _hysteresis; }


			protected:
				//////////////////////////////////////////////////////////////////////////
				/// Generates a generic parameters map from the action parameters.
				/// @return The generated parameters map
				util::ParametersMap getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads the parameters of the action on a generic parameters map.
				/// @param map Parameters map to interpret
				/// @exception ActionException Occurs when some parameters are missing or incorrect.
				void _setFromParametersMap(const util::ParametersMap& map);

			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);


				db::DBTransaction saveNow() const;
				virtual db::DBTransaction _save() const;


				//////////////////////////////////////////////////////////////////////////
				/// The action execution code.
				virtual bool _read(
				) const;
			};


			typedef impex::NoExportPolicy<VMCVFileFormat> Exporter_;
		};
}	}

#endif // SYNTHESE_VMCVFileFormat_H__

