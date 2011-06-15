
/** PTFileFormat class header.
	@file PTFileFormat.hpp

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

#ifndef SYNTHESE_pt_PTFileFormat_hpp__
#define SYNTHESE_pt_PTFileFormat_hpp__

#include "JourneyPattern.hpp"
#include "ImportableTableSync.hpp"
#include "StopPoint.hpp"
#include "ScheduledService.h"
#include "AdminInterfaceElement.h"

namespace synthese
{
	namespace util
	{
		class Env;
		struct RGBColor;
	}

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class TransportNetwork;
		class TransportNetworkTableSync;
		class StopArea;
		class StopAreaTableSync;
		class CommercialLine;
		class CommercialLineTableSync;
		class StopPointTableSync;
		class RollingStock;
		class Destination;
		class DestinationTableSync;


		/** PTFileFormat class.
			@ingroup m35
		*/
		class PTFileFormat
		{
		public:
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static boost::shared_ptr<JourneyPattern> CreateJourneyPattern(
				const JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
				CommercialLine& line,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);


			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static TransportNetwork* CreateOrUpdateNetwork(
				impex::ImportableTableSync::ObjectBySource<TransportNetworkTableSync>& networks,
				const std::string& id,
				const std::string& name,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static Destination* CreateOrUpdateDestination(
				impex::ImportableTableSync::ObjectBySource<DestinationTableSync>& destinations,
				const std::string& id,
				const std::string& displayText,
				const std::string& ttsText,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static CommercialLine* CreateOrUpdateLine(
				impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
				const std::string& id,
				const std::string& name,
				const std::string& shortName,
				boost::optional<util::RGBColor> color,
				const TransportNetwork& network,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			static CommercialLine* GetLine(
				impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
				const std::string& id,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);


			static std::set<StopArea*> CreateOrUpdateStopAreas(
				impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
				const std::string& id,
				const std::string& name,
				const geography::City& city,
				boost::posix_time::time_duration defaultTransferDuration,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			static std::set<StopArea*> GetStopAreas(
				const impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
				const std::string& id,
				boost::optional<const std::string&> name,
				std::ostream& logStream,
				bool errorIfNotFound = true
			);



			struct ImportableStopArea
			{
				std::string operatorCode;
				std::string cityName;
				std::string name;
				boost::shared_ptr<geos::geom::Point> coords;
				std::set<StopArea*> linkedStopAreas;
			};
			typedef std::vector<ImportableStopArea> ImportableStopAreas;


			struct ImportableStopPoint
			{
				std::string operatorCode;
				std::string cityName;
				std::string name;
				boost::shared_ptr<geos::geom::Point> coords;
				const StopArea* stopArea;
				std::set<StopPoint*> linkedStopPoints;

				ImportableStopPoint(): stopArea(NULL) {}
			};
			typedef std::vector<ImportableStopPoint> ImportableStopPoints;


			static void DisplayStopAreaImportScreen(
				const ImportableStopAreas& objects,
				const admin::AdminRequest& request,
				bool createCityIfNecessary,
				bool createPhysicalStop,
				boost::shared_ptr<const geography::City> defaultCity,
				util::Env& env,
				const impex::DataSource& source,
				std::ostream& stream
			);

			static void DisplayStopPointImportScreen(
				const ImportableStopPoints& objects,
				const admin::AdminRequest& request,
				util::Env& env,
				const impex::DataSource& source,
				std::ostream& stream
			);





			//////////////////////////////////////////////////////////////////////////
			///	Gets a stop point.
			///		- search stop points with link to the datasource whit the correct id
			///		- if not found and if stop area is specified creates a new stop point in the stop area
			/// @param cityForStopAreaAutoGeneration if NULL no stop area is generated if stopArea is null
			static std::set<StopPoint*> CreateOrUpdateStopPoints(
				impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
				const std::string& id,
				const std::string& name,
				const StopArea* stopArea,
				const StopPoint::Geometry* geometry,
				const geography::City* cityForStopAreaAutoGeneration,
				boost::optional<boost::posix_time::time_duration> defaultTransferDuration,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);



			static std::set<StopPoint*> GetStopPoints(
				const impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
				const std::string& id,
				boost::optional<const std::string&> name,
				std::ostream& logStream,
				bool errorIfNotFound = true
			);



			static JourneyPattern* CreateOrUpdateRoute(
				pt::CommercialLine& line,
				boost::optional<const std::string&> id,
				boost::optional<const std::string&> name,
				boost::optional<const std::string&> destination,
				boost::optional<Destination*> destinationObj,
				bool direction,
				pt::RollingStock* rollingStock,
				const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);


			static std::set<JourneyPattern*> GetRoutes(
				pt::CommercialLine& line,
				const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
				const impex::DataSource& source
			);



			static ScheduledService* CreateOrUpdateService(
				JourneyPattern& route,
				const ScheduledService::Schedules& departureSchedules,
				const ScheduledService::Schedules& arrivalSchedules,
				const std::string& number,
				const impex::DataSource& source,
				util::Env& env,
				std::ostream& logStream
			);
		};
	}
}

#endif // SYNTHESE_pt_PTFileFormat_hpp__
