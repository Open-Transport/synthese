
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

#include "Importer.hpp"

#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "RuleUser.h"
#include "SchedulesBasedService.h"
#include "StopPoint.hpp"

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

	namespace data_exchange
	{
		/** PTFileFormat class.
			@ingroup m61
		*/
		class PTFileFormat:
			public virtual impex::Importer
		{
		protected:
			static const std::string ATTR_DISTANCE;
			static const std::string ATTR_SOURCE_CODE;
			static const std::string ATTR_SOURCE_CITY_NAME;
			static const std::string ATTR_SOURCE_NAME;
			static const std::string ATTR_SOURCE_X;
			static const std::string ATTR_SOURCE_Y;
			static const std::string ATTR_SOURCE_SYNTHESE_X;
			static const std::string ATTR_SOURCE_SYNTHESE_Y;
			static const std::string TAG_LINKED_STOP_AREA;
			static const std::string TAG_LINKED_STOP_POINT;
			static const std::string TAG_STOP_POINT;
			static const std::string TAG_STOP_AREA;
			static const std::string TAG_SOURCE_LINE;



			PTFileFormat(
				util::Env& env,
				const impex::Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);



			boost::shared_ptr<pt::JourneyPattern> _createJourneyPattern(
				const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
				pt::CommercialLine& line,
				const impex::DataSource& source
			) const;



			pt::TransportNetwork* _createOrUpdateNetwork(
				impex::ImportableTableSync::ObjectBySource<pt::TransportNetworkTableSync>& networks,
				const std::string& id,
				const std::string& name,
				const impex::DataSource& source,
				const std::string& url = std::string(),
				const std::string& timezone = std::string(),
				const std::string& phone = std::string(),
				const std::string& lang = std::string(),
				const std::string& fareUrl = std::string()
			) const;



			pt::Destination* _createOrUpdateDestination(
				impex::ImportableTableSync::ObjectBySource<pt::DestinationTableSync>& destinations,
				const std::string& id,
				const std::string& displayText,
				const std::string& ttsText,
				const impex::DataSource& source
			) const;



			pt::CommercialLine* _createOrUpdateLine(
				impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync>& lines,
				const std::string& id,
				boost::optional<const std::string&> name,
				boost::optional<const std::string&> shortName,
				boost::optional<util::RGBColor> color,
				pt::TransportNetwork& defaultNetwork,
				const impex::DataSource& source,
				bool restrictInDefaultNetwork = false
			) const;



			pt::CommercialLine* _getLine(
				impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync>& lines,
				const std::string& id,
				const impex::DataSource& source,
				boost::optional<pt::TransportNetwork&> network = boost::optional<pt::TransportNetwork&>()
			) const;



			std::set<pt::StopArea*> _createOrUpdateStopAreas(
				impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync>& stopAreas,
				const std::string& id,
				const std::string& name,
				const geography::City* city,
				bool updateCityIfExists,
				boost::posix_time::time_duration defaultTransferDuration,
				const impex::DataSource& source
			) const;



			pt::StopArea* _createStopArea(
				impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync>& stopAreas,
				const std::string& id,
				const std::string& name,
				geography::City& city,
				boost::posix_time::time_duration defaultTransferDuration,
				bool mainStopArea,
				const impex::DataSource& source
			) const;



			std::set<pt::StopArea*> _getStopAreas(
				const impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync>& stopAreas,
				const std::string& id,
				boost::optional<const std::string&> name,
				bool errorIfNotFound = true
			) const;



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
				std::string cityName;
				std::string name;
				boost::shared_ptr<geos::geom::Point> coords;
				const pt::StopArea* stopArea;
				std::set<pt::StopPoint*> linkedStopPoints;
				std::set<std::string> lineCodes;

				ImportableStopPoint(): stopArea(NULL) {}
			};
			typedef std::map<std::string, ImportableStopPoint> ImportableStopPoints;


			void _exportStopAreas(
				const ImportableStopAreas& objects
			) const;



			void _exportStopPoints(
				const ImportableStopPoints& objects
			) const;



			std::set<pt::StopPoint*> _createOrUpdateStop(
				impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stops,
				const std::string& code,
				boost::optional<const std::string&> name,
				boost::optional<const graph::RuleUser::Rules&> useRules,
				boost::optional<const pt::StopArea*> stopArea,
				boost::optional<const pt::StopPoint::Geometry*> geometry,
				const impex::DataSource& source,
				bool doNotUpdate = false
			) const;



			pt::StopPoint* _createStop(
				impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stops,
				const std::string& code,
				boost::optional<const std::string&> name,
				const pt::StopArea& stopArea,
				const impex::DataSource& source
			) const;



			std::set<pt::StopPoint*> _createOrUpdateStopWithStopAreaAutocreation(
				impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stops,
				const std::string& code,
				const std::string& name,
				boost::optional<const pt::StopPoint::Geometry*> geometry,
				const geography::City& cityForStopAreaAutoGeneration,
				boost::optional<boost::posix_time::time_duration> defaultTransferDuration,
				const impex::DataSource& source,
				boost::optional<const graph::RuleUser::Rules&> rules
			) const;



			std::set<pt::StopPoint*> _getStopPoints(
				const impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync>& stopPoints,
				const std::string& id,
				boost::optional<const std::string&> name,
				bool errorIfNotFound = true
			) const;



			vehicle::RollingStock* _getTransportMode(
				const impex::ImportableTableSync::ObjectBySource<vehicle::RollingStockTableSync>& transportModes,
				const std::string& id
			) const;



			pt::JourneyPattern* _createOrUpdateRoute(
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
				bool removeOldCodes,
				bool updateMetricOffsetOnUpdate,
				bool attemptToCopyExistingGeometries = true,
				bool allowDifferentStopPointsInSameStopArea = false
			) const;


			std::set<pt::JourneyPattern*> _getRoutes(
				pt::CommercialLine& line,
				const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization& servedStops,
				const impex::DataSource& source
			) const;



			pt::ScheduledService* _createOrUpdateService(
				pt::JourneyPattern& route,
				const pt::SchedulesBasedService::Schedules& departureSchedules,
				const pt::SchedulesBasedService::Schedules& arrivalSchedules,
				const std::string& number,
				const impex::DataSource& source,
				boost::optional<const std::string&> team = boost::optional<const std::string&>(),
				boost::optional<const graph::RuleUser::Rules&> rules = boost::optional<const graph::RuleUser::Rules&>(),
				boost::optional<const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization&> servedVertices = boost::optional<const pt::JourneyPattern::StopsWithDepartureArrivalAuthorization&>(),
				boost::optional<const std::string&> id = boost::optional<const std::string&>(),
				boost::optional<const pt::SchedulesBasedService::Comments&> arrivalComments = boost::optional<const pt::SchedulesBasedService::Comments&>(),
				boost::optional<const pt::SchedulesBasedService::Comments&> departureComments = boost::optional<const pt::SchedulesBasedService::Comments&>()
			) const;



			pt::ContinuousService* _createOrUpdateContinuousService(
				pt::JourneyPattern& route,
				const pt::SchedulesBasedService::Schedules& departureSchedules,
				const pt::SchedulesBasedService::Schedules& arrivalSchedules,
				const std::string& number,
				const boost::posix_time::time_duration& range,
				const boost::posix_time::time_duration& waitingTime,
				const impex::DataSource& source
			) const;
		};
}	}

#endif // SYNTHESE_pt_PTFileFormat_hpp__
