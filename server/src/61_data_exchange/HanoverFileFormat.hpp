
/** HanoverFileFormat class header.
	@file HanoverFileFormat.hpp

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

#ifndef SYNTHESE_HanoverFileFormat_hpp__
#define SYNTHESE_HanoverFileFormat_hpp__

#include "Importer.hpp"

#include "StopPoint.hpp"

namespace synthese
{
	namespace data_exchange
	{
		/** HanoverFileFormat class.
			@ingroup m61
		*/
		class HanoverFileFormat:
			public virtual impex::Importer
		{
		protected:
			HanoverFileFormat(
				util::Env& env,
				const impex::Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);


			struct HanoverLineStop
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
			 * Only the physical stops (HanoverTheoricalStops) are registered in
			 * this temporary object.
			 */
			struct Link
			{
				int id; /* LNK_Id */
				typedef std::vector<HanoverLineStop> HanoverLineStops;
				HanoverLineStops stops;
				boost::shared_ptr<geos::geom::LineString> lineString;
			};
			typedef std::map<int /* LNK_Id */, Link> LinksMap;

			struct Route
			{
				int id; /* ROU_Id */
				std::string name;
				pt::CommercialLine* line;
				bool direction;
				typedef std::vector<Link> Links;
				Links links;
			};

			typedef std::map<int /* ROU_Id */, Route> RoutesMap;

			struct HanoverSchedule
			{
				boost::posix_time::time_duration dept; /* departure time */
				boost::posix_time::time_duration deptRT; /* RT departure time */
			};
			typedef std::vector<HanoverSchedule> HanoverSchedules;
			typedef std::map<int /* ROU_Id */, HanoverSchedules> HanoverSchedulesMap;

			typedef std::map<int /* HTY_Id */, calendar::Calendar> Calendars;

			struct Run
			{
				int id; /* Run_Id */
				Route* route;
				std::string service_number;
				calendar::Calendar calendar;
				HanoverSchedules schedules;
			};
			typedef std::map<int /* RUN_Id */, Run> RunsMap;

			void _selectAndLoadLink(
				LinksMap& links,
				int id,
				const Link::HanoverLineStops& stops,
				boost::shared_ptr<geos::geom::LineString> lineString
			) const;

			struct HanoverPoint
			{
				int id; /* PNT_Id */
				boost::shared_ptr<geos::geom::Point> geometry;
				const pt::StopPoint* syntheseStop;
			};
			typedef std::map<int, HanoverPoint> HanoverPoints;

			void _selectAndLoadRoute(
				RoutesMap& routes,
				const Route::Links& links,
				pt::CommercialLine* line,
				const std::string& name,
				bool direction,
				int id
			) const;

			void _selectAndLoadRun(
				RunsMap& runs,
				int runId,
				Route& route,
				const calendar::Calendar& calendar,
				const HanoverSchedules& schedules,
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

		};
}	}

#endif // SYNTHESE_HanoverFileFormat_hpp__
