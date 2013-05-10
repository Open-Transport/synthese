
/** PTFileFormat class header.
	@file PTFileFormat.hpp

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

#ifndef SYNTHESE_pt_PTFileFormat_hpp__
#define SYNTHESE_pt_PTFileFormat_hpp__

#include "JourneyPattern.hpp"
#include "ImportableTableSync.hpp"
#include "StopPoint.hpp"
#include "AdminInterfaceElement.h"
#include "RuleUser.h"
#include "SchedulesBasedService.h"

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

	namespace vehicle
	{
		class RollingStock;
		class RollingStockTableSync;
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
		class Destination;
		class DestinationTableSync;
		class ScheduledService;
		class ContinuousService;
	}

	namespace impex
	{
		class ImportLogger;
	}

	namespace data_exchange
	{
		/** PTFileFormat class.
			@ingroup m61
		*/
		class PTFileFormat
		{
		public:
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static boost::shared_ptr<pt::JourneyPattern> CreateJourneyPattern(
				const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
				pt::CommercialLine& line,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);


			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static pt::TransportNetwork* CreateOrUpdateNetwork(
				impex::ImportableTableSync::ObjectBySource<pt::TransportNetworkTableSync>& networks,
				const std::string& id,
				const std::string& name,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);



			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static pt::Destination* CreateOrUpdateDestination(
				impex::ImportableTableSync::ObjectBySource<pt::DestinationTableSync>& destinations,
				const std::string& id,
				const std::string& displayText,
				const std::string& ttsText,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);



			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			/// The network of the line is never changed if an existing line is returned.
			static pt::CommercialLine* CreateOrUpdateLine(
				impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync>& lines,
				const std::string& id,
				boost::optional<const std::string&> name,
				boost::optional<const std::string&> shortName,
				boost::optional<util::RGBColor> color,
				pt::TransportNetwork& defaultNetwork,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger,
				bool restrictInDefaultNetwork = false
			);



			static pt::CommercialLine* GetLine(
				impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync>& lines,
				const std::string& id,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger,
				boost::optional<pt::TransportNetwork&> network = boost::optional<pt::TransportNetwork&>()
			);



			static std::set<pt::StopArea*> CreateOrUpdateStopAreas(
				impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync>& stopAreas,
				const std::string& id,
				const std::string& name,
				const geography::City* city,
				bool updateCityIfExists,
				boost::posix_time::time_duration defaultTransferDuration,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);



			static pt::StopArea* CreateStopArea(
				impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync>& stopAreas,
				const std::string& id,
				const std::string& name,
				geography::City& city,
				boost::posix_time::time_duration defaultTransferDuration,
				bool mainStopArea,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);



			static std::set<pt::StopArea*> GetStopAreas(
				const impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync>& stopAreas,
				const std::string& id,
				boost::optional<const std::string&> name,
				const impex::ImportLogger& importLogger,
				bool errorIfNotFound = true
			);



			struct ImportableStopArea
			{
				std::string operatorCode;
				std::string cityName;
				std::string name;
				boost::shared_ptr<geos::geom::Point> coords;
				std::set<pt::StopArea*> linkedStopAreas;
			};
			typedef std::vector<ImportableStopArea> ImportableStopAreas;


			struct ImportableStopPoint
			{
				std::string operatorCode;
				std::string cityName;
				std::string name;
				boost::shared_ptr<geos::geom::Point> coords;
				const pt::StopArea* stopArea;
				std::set<pt::StopPoint*> linkedStopPoints;

				ImportableStopPoint(): stopArea(NULL) {}
			};
			typedef std::vector<ImportableStopPoint> ImportableStopPoints;


			static void DisplayStopAreaImportScreen(
				const ImportableStopAreas& objects,
				const server::Request& request,
				bool createCityIfNecessary,
				bool createPhysicalStop,
				boost::shared_ptr<const geography::City> defaultCity,
				util::Env& env,
				const impex::DataSource& source,
				const impex::ImportLogger& importLogger
			);

			static void DisplayStopPointImportScreen(
				const ImportableStopPoints& objects,
				const server::Request& request,
				util::Env& env,
				const impex::DataSource& source,
				const impex::ImportLogger& importLogger
			);



			//////////////////////////////////////////////////////////////////////////
			///	Stop creation or update.
			/// The stop is identified by the specified datasource and the code.
			/// If the stop is not found, a stop is created in the specified stop area, if defined in
			/// the corresponding parameter. If not, a error message is written on the log stream.
			/// @param stops the stops which are registered to the data source
			/// @param code the code of the stop to update or create, as known by the datasource
			/// @param name the name of the stop (updated only if defined)
			/// @param stopArea the stop area which the stop belongs to (updated only if defined)
			/// @param geometry the location of the stop (updated only if defined)
			/// @param source the data source
			/// @param env the environment to read and populate
			/// @param logStream the stream to write the logs on
			/// @return the updated or created stops. Empty is no stop was neither found nor created in case
			/// of the stop area parameter is not defined
			static std::set<pt::StopPoint*> CreateOrUpdateStop(
				impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stops,
				const std::string& code,
				boost::optional<const std::string&> name,
				boost::optional<const graph::RuleUser::Rules&> useRules,
				boost::optional<const pt::StopArea*> stopArea,
				boost::optional<const pt::StopPoint::Geometry*> geometry,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger,
				bool doNotUpdate = false
			);



			static pt::StopPoint* CreateStop(
				impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stops,
				const std::string& code,
				boost::optional<const std::string&> name,
				const pt::StopArea& stopArea,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);



			//////////////////////////////////////////////////////////////////////////
			/// Stop creation or update with creation of a stop area based on the name if necessary.
			static std::set<pt::StopPoint*> CreateOrUpdateStopWithStopAreaAutocreation(
				impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stops,
				const std::string& code,
				const std::string& name,
				boost::optional<const pt::StopPoint::Geometry*> geometry,
				const geography::City& cityForStopAreaAutoGeneration,
				boost::optional<boost::posix_time::time_duration> defaultTransferDuration,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger,
				boost::optional<const graph::RuleUser::Rules&> rules
			);



			static std::set<pt::StopPoint*> GetStopPoints(
				const impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stopPoints,
				const std::string& id,
				boost::optional<const std::string&> name,
				const impex::ImportLogger& importLogger,
				bool errorIfNotFound = true
			);



			static vehicle::RollingStock* GetTransportMode(
				const impex::ImportableTableSync::ObjectBySource<vehicle::RollingStockTableSync>& transportModes,
				const std::string& id,
				const impex::ImportLogger& importLogger
			);



			//////////////////////////////////////////////////////////////////////////
			/// Search for an existing route which matches with the defined parameters, or create a new one if no existing route is compliant.
			/// @param line The line
			/// @param removeOldCodes Removes codes on similar routes with the same code for the data source (routes with different stops are not cleaned)
			/// @pre The line object must link to all existing routes (use JourneyPatternTableSync::Search to populate the object)
			/// @author Hugues Romain
			static pt::JourneyPattern* CreateOrUpdateRoute(
				pt::CommercialLine& line,
				boost::optional<const std::string&> id,
				boost::optional<const std::string&> name,
				boost::optional<const std::string&> destination,
				boost::optional<pt::Destination*> destinationObj,
				boost::optional<const graph::RuleUser::Rules&> useRule,
				boost::optional<bool> wayBack,
				vehicle::RollingStock* rollingStock,
				const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger,
				bool removeOldCodes,
				bool updateMetricOffsetOnUpdate,
				bool attemptToCopyExistingGeometries = true,
				bool allowDifferentStopPointsInSameStopArea = false
			);


			static std::set<pt::JourneyPattern*> GetRoutes(
				pt::CommercialLine& line,
				const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
				const impex::DataSource& source,
				const impex::ImportLogger& importLogger
			);



			static pt::ScheduledService* CreateOrUpdateService(
				pt::JourneyPattern& route,
				const pt::SchedulesBasedService::Schedules& departureSchedules,
				const pt::SchedulesBasedService::Schedules& arrivalSchedules,
				const std::string& number,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger,
				boost::optional<const std::string&> team = boost::optional<const std::string&>(),
				boost::optional<const graph::RuleUser::Rules&> rules = boost::optional<const graph::RuleUser::Rules&>(),
				boost::optional<const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization&> servedVertices = boost::optional<const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization&>()
			);



			static pt::ContinuousService* CreateOrUpdateContinuousService(
				pt::JourneyPattern& route,
				const pt::SchedulesBasedService::Schedules& departureSchedules,
				const pt::SchedulesBasedService::Schedules& arrivalSchedules,
				const std::string& number,
				const boost::posix_time::time_duration& range,
				const boost::posix_time::time_duration& waitingTime,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& importLogger
			);
		};
}	}

#endif // SYNTHESE_pt_PTFileFormat_hpp__
