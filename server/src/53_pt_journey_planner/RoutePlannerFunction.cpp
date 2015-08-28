
/** RoutePlannerFunction class implementation.
	@file RoutePlannerFunction.cpp

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

#include "RoutePlannerFunction.h"

#include "AccessParameters.h"
#include "AlgorithmLogger.hpp"
#include "Edge.h"
#include "HourPeriod.h"
#include "ObjectNotFoundException.h"
#include "ReservationRuleInterfacePage.h"
#include "SentScenario.h"
#include "Session.h"
#include "JourneyPattern.hpp"
#include "PlacesListService.hpp"
#include "PTModule.h"
#include "PTRoutePlannerResult.h"
#include "PTServiceConfigTableSync.hpp"
#include "PTTimeSlotRoutePlanner.h"
#include "Request.h"
#include "RequestException.h"
#include "UserFavoriteJourney.h"
#include "UserFavoriteJourneyTableSync.h"
#include "Road.h"
#include "RoadPath.hpp"
#include "RoadPlace.h"
#include "Hub.h"
#include "Service.h"
#include "CommercialLine.h"
#include "TransportNetwork.h"
#include "RollingStock.hpp"
#include "RGBColor.h"
#include "Crossing.h"
#include "StopPoint.hpp"
#include "OnlineReservationRule.h"
#include "ReservationContact.h"
#include "RollingStockFilter.h"
#include "Address.h"
#include "ContinuousService.h"
#include "Webpage.h"
#include "CoordinatesSystem.hpp"
#include "User.h"
#include "City.h"
#include "Place.h"
#include "PTConstants.h"
#include "DateTimeInterfacePage.h"
#include "NamedPlace.h"
#include "Webpage.h"
#include "StopArea.hpp"
#include "StaticFunctionRequest.h"
#include "RoutePlannerFunction.h"
#include "LineMarkerInterfacePage.h"
#include "Alarm.h"
#include "PTModule.h"
#include "Destination.hpp"
#include "Junction.hpp"
#include "Fare.hpp"
#include "FareTicket.hpp"
#include "HTMLForm.h"
#include "CMSModule.hpp"
#include "PTUseRule.h"
#include "GetMessagesFunction.hpp"
#include "CustomBroadcastPoint.hpp"

#include <geos/io/WKTWriter.h>
#include <geos/geom/LineString.h>
#include <geos/geom/MultiLineString.h>
#include <geos/operation/distance/DistanceOp.h>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace algorithm;
	using namespace db;
	using namespace fare;
	using namespace pt;
	using namespace pt_website;
	using namespace server;
	using namespace util;
	using namespace vehicle;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace pt_journey_planner;
	using namespace resa;
	using namespace html;
	using namespace cms;
	using namespace messages;

	template<>
	const string FactorableTemplate<server::Function, RoutePlannerFunction>::FACTORY_KEY= "rp";

	namespace pt_journey_planner
	{
		const string RoutePlannerFunction::PARAMETER_CONFIG_ID = "config_id";
		const string RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER = "msn";
		const string RoutePlannerFunction::PARAMETER_MAX_DEPTH = "md";
		const string RoutePlannerFunction::PARAMETER_APPROACH_SPEED = "apsp";
		const string RoutePlannerFunction::PARAMETER_MAX_APPROACH_DISTANCE = "mad";
		const string RoutePlannerFunction::PARAMETER_DAY = "dy";
		const string RoutePlannerFunction::PARAMETER_PERIOD_ID = "pi";
		const string RoutePlannerFunction::PARAMETER_ACCESSIBILITY = "ac";
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT = "dct";
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT = "act";
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT = "dpt";
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT = "apt";
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_CLASS_FILTER = "departure_class_filter";
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_CLASS_FILTER = "arrival_class_filter";
		const string RoutePlannerFunction::PARAMETER_FAVORITE_ID = "fid";
		const string RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME = "da";
		const string RoutePlannerFunction::PARAMETER_LOWEST_ARRIVAL_TIME = "ii";
		const string RoutePlannerFunction::PARAMETER_HIGHEST_DEPARTURE_TIME = "ha";
		const string RoutePlannerFunction::PARAMETER_HIGHEST_ARRIVAL_TIME = "ia";
		const string RoutePlannerFunction::PARAMETER_NETWORK_LIST = "nwl";
		const string RoutePlannerFunction::PARAMETER_ROLLING_STOCK_LIST = "tml";
		const string RoutePlannerFunction::PARAMETER_ROLLING_STOCK_FILTER_ID = "tm";
		const string RoutePlannerFunction::PARAMETER_MIN_MAX_DURATION_RATIO_FILTER = "min_max_duration_ratio_filter";
		const string RoutePlannerFunction::PARAMETER_MIN_WAITING_TIME_FILTER = "min_waiting_time_filter";
		const string RoutePlannerFunction::PARAMETER_FARE_CALCULATION = "fc";
		const string RoutePlannerFunction::PARAMETER_MAX_TRANSFER_DURATION = "max_transfer_duration";
		const string RoutePlannerFunction::PARAMETER_LOG_PATH = "log_path";
		const string RoutePlannerFunction::PARAMETER_SRID = "srid";
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_XY("departure_place_XY");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_XY("arrival_place_XY");
		const string RoutePlannerFunction::PARAMETER_INVERT_XY("invert_XY");
		const string RoutePlannerFunction::PARAMETER_BROADCAST_POINT_ID = "broadcast_point";

		const string RoutePlannerFunction::PARAMETER_OUTPUT_FORMAT = "output_format";
		const string RoutePlannerFunction::VALUE_ADMIN_HTML = "admin";

		const string RoutePlannerFunction::PARAMETER_PAGE = "page";
		const string RoutePlannerFunction::PARAMETER_SCHEDULES_ROW_PAGE("schedules_row_page");
		const string RoutePlannerFunction::PARAMETER_SCHEDULES_CELL_PAGE("schedule_cell_page");
		const string RoutePlannerFunction::PARAMETER_LINES_ROW_PAGE("lines_row_page");
		const string RoutePlannerFunction::PARAMETER_LINE_MARKER_PAGE("line_marker_page");
		const string RoutePlannerFunction::PARAMETER_BOARD_PAGE("board_page");
		const string RoutePlannerFunction::PARAMETER_WARNING_PAGE("warning_page");
		const string RoutePlannerFunction::PARAMETER_WARNING_CHECK_PAGE = "warning_check_page";
		const string RoutePlannerFunction::PARAMETER_RESERVATION_PAGE("reservation_page");
		const string RoutePlannerFunction::PARAMETER_DURATION_PAGE("duration_page");
		const string RoutePlannerFunction::PARAMETER_TEXT_DURATION_PAGE("text_duration_page");
		const string RoutePlannerFunction::PARAMETER_MAP_PAGE("map_page");
		const string RoutePlannerFunction::PARAMETER_MAP_LINE_PAGE("map_line_page");
		const string RoutePlannerFunction::PARAMETER_DATE_TIME_PAGE("date_time_page");
		const string RoutePlannerFunction::PARAMETER_STOP_CELL_PAGE("stop_cell_page");
		const string RoutePlannerFunction::PARAMETER_SERVICE_CELL_PAGE("service_cell_page");
		const string RoutePlannerFunction::PARAMETER_ROAD_CELL_PAGE("junction_cell_page");
		const string RoutePlannerFunction::PARAMETER_JUNCTION_CELL_PAGE("external_junction_cell_page");
		const string RoutePlannerFunction::PARAMETER_TICKET_CELL_PAGE("ticket_cell_page");
		const string RoutePlannerFunction::PARAMETER_MAP_STOP_PAGE("map_stop_page");
		const string RoutePlannerFunction::PARAMETER_MAP_SERVICE_PAGE("map_service_page");
		const string RoutePlannerFunction::PARAMETER_MAP_ROAD_PAGE("map_junction_page");
		const string RoutePlannerFunction::PARAMETER_MAP_JUNCTION_PAGE("map_external_junction_page");
		const string RoutePlannerFunction::PARAMETER_RESULT_ROW_PAGE("result_row_page");
		const string RoutePlannerFunction::PARAMETER_IGNORE_RESERVATION_RULES("irr");
		const string RoutePlannerFunction::PARAMETER_RESERVATION_DELAY_TYPE("delay_type");

		//XML output only:
		const string RoutePlannerFunction::PARAMETER_SHOW_RESULT_TABLE("showResTab");
		const string RoutePlannerFunction::PARAMETER_SHOW_COORDINATES("showCoords");

		const string RoutePlannerFunction::DATA_SOLUTIONS_NUMBER("solutions_number");
		const string RoutePlannerFunction::DATA_LINES("lines");
		const string RoutePlannerFunction::DATA_SCHEDULES("schedules");
		const string RoutePlannerFunction::DATA_WARNINGS("warnings");
		const string RoutePlannerFunction::DATA_DURATIONS("durations");
		const string RoutePlannerFunction::DATA_RESERVATIONS("reservations");
		const string RoutePlannerFunction::DATA_BOARDS("boards");
		const string RoutePlannerFunction::DATA_MAPS_LINES("maps_lines");
		const string RoutePlannerFunction::DATA_MAPS = "maps";
		const string RoutePlannerFunction::DATA_FILTERED_JOURNEYS = "filtered_journeys";
		const string RoutePlannerFunction::DATA_MAX_WARNING_LEVEL_ON_STOP = "max_warning_level_on_stop";
		const string RoutePlannerFunction::DATA_MAX_WARNING_LEVEL_ON_LINE = "max_warning_level_on_line";
		const string RoutePlannerFunction::DATA_HAS_RESERVATION = "has_reservation";
		const string RoutePlannerFunction::DATA_RESULT_ROWS = "result_rows";

		const string RoutePlannerFunction::DATA_INTERNAL_DATE("internal_date");
		const string RoutePlannerFunction::DATA_ORIGIN_CITY_TEXT("origin_city_text");
		const string RoutePlannerFunction::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string RoutePlannerFunction::DATA_ORIGIN_PLACE_TEXT("origin_place_text");
		const string RoutePlannerFunction::DATA_ORIGIN_PLACE_LONGITUDE("origin_place_longitude");
		const string RoutePlannerFunction::DATA_ORIGIN_PLACE_LATITUDE("origin_place_latitude");
		const string RoutePlannerFunction::DATA_BIKE_FILTER("bike_filter");
		const string RoutePlannerFunction::DATA_DESTINATION_CITY_TEXT("destination_city_text");
		const string RoutePlannerFunction::DATA_DESTINATION_PLACE_ID("destination_place_id");
		const string RoutePlannerFunction::DATA_DESTINATION_PLACE_TEXT("destination_place_text");
		const string RoutePlannerFunction::DATA_DESTINATION_PLACE_LONGITUDE("destination_place_longitude");
		const string RoutePlannerFunction::DATA_DESTINATION_PLACE_LATITUDE("destination_place_latitude");
		const string RoutePlannerFunction::DATA_PERIOD_ID("period_id");
		const string RoutePlannerFunction::DATA_DATE("date");
		const string RoutePlannerFunction::DATA_PERIOD("period");
		const string RoutePlannerFunction::DATA_ACCESSIBILITY_CODE("accessibility_code");
		const string RoutePlannerFunction::DATA_SITE_ID("site_id");
		const string RoutePlannerFunction::DATA_IS_SAME_PLACES("is_same_places");
		const string RoutePlannerFunction::DATA_USER_FULL_NAME("user_full_name");
		const string RoutePlannerFunction::DATA_USER_PHONE("user_phone");
		const string RoutePlannerFunction::DATA_USER_ID("user_id");

		const string RoutePlannerFunction::DATA_CELLS("cells");
		const string RoutePlannerFunction::DATA_IS_ODD_ROW("is_odd_row");
		const string RoutePlannerFunction::DATA_IS_ORIGIN_ROW("is_origin_row");
		const string RoutePlannerFunction::DATA_IS_DESTINATION_ROW("is_destination_row");
		const string RoutePlannerFunction::DATA_PLACE_NAME("place_name");

		const string RoutePlannerFunction::DATA_LINE_ID = "line_id";
		const string RoutePlannerFunction::DATA_STOP_ID = "stop_id";
		const string RoutePlannerFunction::DATA_IS_FIRST_ROW = "is_first_row";
		const string RoutePlannerFunction::DATA_IS_LAST_ROW = "is_last_row";
		const string RoutePlannerFunction::DATA_COLUMN_NUMBER("column_number");
		const string RoutePlannerFunction::DATA_ROW_NUMBER("row_number");
		const string RoutePlannerFunction::DATA_IS_FOOT("is_foot");
		const string RoutePlannerFunction::DATA_FIRST_TIME("first_time");
		const string RoutePlannerFunction::DATA_LAST_TIME("last_time");
		const string RoutePlannerFunction::DATA_IS_CONTINUOUS_SERVICE("is_continuous_service");
		const string RoutePlannerFunction::DATA_IS_FIRST_WRITING("is_first_writing");
		const string RoutePlannerFunction::DATA_IS_LAST_WRITING("is_last_writing");
		const string RoutePlannerFunction::DATA_IS_FIRST_FOOT("is_first_foot");

		const string RoutePlannerFunction::DATA_CONTENT("content");
		const string RoutePlannerFunction::DATA_RANK("rank");
		const string RoutePlannerFunction::DATA_JUNCTIONS_NUMBER("junctions_number");

		const string RoutePlannerFunction::DATA_START_DATE = "start_date";
		const string RoutePlannerFunction::DATA_END_DATE = "end_date";
		const string RoutePlannerFunction::DATA_IS_THE_LAST_JOURNEY_BOARD("is_the_last_journey_board");
		const string RoutePlannerFunction::DATA_DEPARTURE_TIME("departure_time");
		const string RoutePlannerFunction::DATA_DEPARTURE_DATE("departure_date");
		const string RoutePlannerFunction::DATA_DEPARTURE_TIME_INTERNAL_FORMAT("internal_departure_time");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME("continuous_service_last_departure_time");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_DATE("continuous_service_last_departure_date");
		const string RoutePlannerFunction::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string RoutePlannerFunction::DATA_DEPARTURE_PLACE_LONGITUDE("departure_longitude");
		const string RoutePlannerFunction::DATA_DEPARTURE_PLACE_LATITUDE("departure_latitude");
		const string RoutePlannerFunction::DATA_ARRIVAL_TIME("arrival_time");
		const string RoutePlannerFunction::DATA_ARRIVAL_DATE("arrival_date");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME("continuous_service_last_arrival_time");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_DATE("continuous_service_last_arrival_date");
		const string RoutePlannerFunction::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string RoutePlannerFunction::DATA_ARRIVAL_PLACE_LONGITUDE("arrival_longitude");
		const string RoutePlannerFunction::DATA_ARRIVAL_PLACE_LATITUDE("arrival_latitude");
		const string RoutePlannerFunction::DATA_DURATION("duration");
		const string RoutePlannerFunction::DATA_RESERVATION_AVAILABLE("reservation_available");
		const string RoutePlannerFunction::DATA_RESERVATION_COMPULSORY("reservation_compulsory");
		const string RoutePlannerFunction::DATA_RESERVATION_DELAY("reservation_delay");
		const string RoutePlannerFunction::DATA_RESERVATION_DEADLINE("reservation_deadline");
		const string RoutePlannerFunction::DATA_RESERVATION_PHONE_NUMBER("reservation_phone_number");
		const string RoutePlannerFunction::DATA_ONLINE_RESERVATION("online_reservation");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_WAITING("continuous_service_waiting");
		const string RoutePlannerFunction::DATA_CO2_EMISSIONS("co2_emissions");
		const string RoutePlannerFunction::DATA_ENERGY_CONSUMPTION("energy_consumption");
		const string RoutePlannerFunction::DATA_TICKETS("tickets");
		const string RoutePlannerFunction::DATA_TICKET_NAME("ticket_name");
		const string RoutePlannerFunction::DATA_TICKET_PRICE("ticket_price");
		const string RoutePlannerFunction::DATA_TICKET_CURRENCY("ticket_currency");
		const string RoutePlannerFunction::DATA_DISTANCE = "distance";

		// Cells
		const string RoutePlannerFunction::DATA_ODD_ROW("is_odd_row");

		// Stop cells
		const string RoutePlannerFunction::DATA_IS_ARRIVAL("is_arrival");
		const string RoutePlannerFunction::DATA_IS_TERMINUS("is_terminus");
		const string RoutePlannerFunction::DATA_STOP_AREA_NAME("stop_name");
		const string RoutePlannerFunction::DATA_LONGITUDE("longitude");
		const string RoutePlannerFunction::DATA_LATITUDE("latitude");
		const string RoutePlannerFunction::DATA_DEPARTURE_STOP_NAME("departure_stop_name");
		const string RoutePlannerFunction::DATA_DEPARTURE_LONGITUDE("departure_longitude");
		const string RoutePlannerFunction::DATA_DEPARTURE_LATITUDE("departure_latitude");
		const string RoutePlannerFunction::DATA_ARRIVAL_STOP_NAME("arrival_stop_name");
		const string RoutePlannerFunction::DATA_ARRIVAL_LONGITUDE("arrival_longitude");
		const string RoutePlannerFunction::DATA_ARRIVAL_LATITUDE("arrival_latitude");
		const string RoutePlannerFunction::DATA_IS_LAST_LEG("is_last_leg");
		const string RoutePlannerFunction::DATA_IS_FIRST_LEG("is_first_leg");
		const string RoutePlannerFunction::DATA_USER_CLASS_CODE("ac");

		// Road cells
		const string RoutePlannerFunction::DATA_REACHED_PLACE_IS_NAMED("reached_place_is_named");
		const string RoutePlannerFunction::DATA_ROAD_NAME("road_name");
		const string RoutePlannerFunction::DATA_LENGTH("length");

		// Junction cells
		const string RoutePlannerFunction::DATA_START_STOP_NAME("startStop");
		const string RoutePlannerFunction::DATA_END_STOP_NAME("endStop");

		// Service cells
		const string RoutePlannerFunction::DATA_FIRST_DEPARTURE_TIME("first_departure_time");
		const string RoutePlannerFunction::DATA_LAST_DEPARTURE_TIME("last_departure_time");
		const string RoutePlannerFunction::DATA_FIRST_ARRIVAL_TIME("first_arrival_time");
		const string RoutePlannerFunction::DATA_LAST_ARRIVAL_TIME("last_arrival_time");
		const string RoutePlannerFunction::DATA_ROLLINGSTOCK_ID("rolling_stock_id");
		const string RoutePlannerFunction::DATA_ROLLINGSTOCK_NAME("rolling_stock_name");
		const string RoutePlannerFunction::DATA_ROLLINGSTOCK_ARTICLE("rolling_stock_article");
		const string RoutePlannerFunction::DATA_DESTINATION_NAME("destination_name");
		const string RoutePlannerFunction::DATA_HANDICAPPED_FILTER_STATUS("handicapped_filter_status");
		const string RoutePlannerFunction::DATA_HANDICAPPED_PLACES_NUMBER("handicapped_places_number");
		const string RoutePlannerFunction::DATA_BIKE_FILTER_STATUS("bike_filter_status");
		const string RoutePlannerFunction::DATA_BIKE_PLACES_NUMBER("bike_places_number");
		const string RoutePlannerFunction::DATA_WKT("wkt");
		const string RoutePlannerFunction::DATA_LINE_MARKERS("line_markers");
		const string RoutePlannerFunction::DATA_NETWORK("network");
		const string RoutePlannerFunction::DATA_SERVICE_NUMBER("service_number");

		RoutePlannerFunction::RoutePlannerFunction(
		):	_startDate(not_a_date_time),
			_endDate(not_a_date_time),
			_startArrivalDate(not_a_date_time),
			_endArrivalDate(not_a_date_time),
			_period(NULL),
			_logger(new AlgorithmLogger()),
			_config(NULL),
			_broadcastPoint(NULL)
		{}



		ParametersMap RoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map;

			// Config
			if(_config)
			{
				map.insert(PARAMETER_CONFIG_ID, _config->getKey());
			}

			// Max transfer duration
			if(_maxTransferDuration)
			{
				map.insert(PARAMETER_MAX_TRANSFER_DURATION, _maxTransferDuration->total_seconds() / 60);
			}

			// Min max duration ratio filter
			if(_minMaxDurationRatioFilter)
			{
				map.insert(PARAMETER_MIN_MAX_DURATION_RATIO_FILTER, *_minMaxDurationRatioFilter);
			}

			// Warning check page
			if(_warningCheckPage.get())
			{
				map.insert(PARAMETER_WARNING_CHECK_PAGE, _warningCheckPage->getKey());
			}

			// Log path
			if(_logger && !_logger->getDirectory().empty())
			{
				map.insert(PARAMETER_LOG_PATH, _logger->getDirectory().string());
			}

			// Departure place
			if(_departure_place.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_departure_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PLACE_TEXT,
						dynamic_cast<NamedPlace*>(_departure_place.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_departure_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PLACE_TEXT,
						dynamic_cast<City*>(_departure_place.placeResult.value.get())->getName()
					);
				}
			}
			else
			{
				map.insert(PARAMETER_DEPARTURE_CITY_TEXT, _originCityText);
				map.insert(PARAMETER_DEPARTURE_PLACE_TEXT, _originPlaceText);
			}

			// Arrival place
			if(_arrival_place.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_arrival_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PLACE_TEXT,
						dynamic_cast<NamedPlace*>(_arrival_place.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_arrival_place.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PLACE_TEXT,
						dynamic_cast<City*>(_arrival_place.placeResult.value.get())->getName()
					);
				}
			}
			else
			{
				map.insert(PARAMETER_ARRIVAL_CITY_TEXT, _destinationCityText);
				map.insert(PARAMETER_ARRIVAL_PLACE_TEXT, _destinationPlaceText);
			}

			// Start Date
			if(!_startDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_LOWEST_DEPARTURE_TIME, _startDate);
			}

			// End date
			if(!_endDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_HIGHEST_DEPARTURE_TIME, _endDate);
			}

			// Start Arrival Date
			if(!_startArrivalDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_LOWEST_ARRIVAL_TIME, _startArrivalDate);
			}

			// End Arrival date
			if(!_endArrivalDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_HIGHEST_ARRIVAL_TIME, _endArrivalDate);
			}

			// Max solutions number
			if(	_maxSolutionsNumber)
			{
				map.insert(PARAMETER_MAX_SOLUTIONS_NUMBER, *_maxSolutionsNumber);
			}

			// Output messages
			if(_broadcastPoint)
			{
				map.insert(PARAMETER_BROADCAST_POINT_ID, _broadcastPoint->getKey());
			}

			return map;
		}



		void RoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Config
			RegistryKeyType configId(map.getDefault<RegistryKeyType>(PARAMETER_CONFIG_ID, 0));
			if(configId) try
			{
				_config = PTServiceConfigTableSync::Get(configId, *_env).get();
			}
			catch (ObjectNotFoundException<PTServiceConfig>&)
			{
				throw RequestException("No such config");
			}

			_outputRoadApproachDetail =
				_config ?
				_config->get<DisplayRoadApproachDetails>() :
				true
			;

			// Log path
			if(!map.getDefault<string>(PARAMETER_LOG_PATH).empty())
			{
				_logger.reset(
					new AlgorithmLogger(map.get<string>(PARAMETER_LOG_PATH))
				);
			}

			// Set coordinate system if provided else 4326 (WGS84)
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, 4326)
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Max transfer duration
			if(map.isDefined(PARAMETER_MAX_TRANSFER_DURATION))
			{
				_maxTransferDuration = minutes(map.get<int>(PARAMETER_MAX_TRANSFER_DURATION));
			}

			// Min max duration filter
			if(map.getDefault<double>(PARAMETER_MIN_MAX_DURATION_RATIO_FILTER, 0) > 0)
			{
				_minMaxDurationRatioFilter = map.get<double>(PARAMETER_MIN_MAX_DURATION_RATIO_FILTER);
			}

			AccessParameters::AllowedPathClasses allowedPathClasses;
			try
			{
				// Rolling stock filter
				if(map.getOptional<RegistryKeyType>(PARAMETER_ROLLING_STOCK_FILTER_ID))
				{
					_rollingStockFilter = Env::GetOfficialEnv().get<RollingStockFilter>(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_FILTER_ID));
					allowedPathClasses = _rollingStockFilter.get() ? _rollingStockFilter->getAllowedPathClasses() : AccessParameters::AllowedPathClasses();
				}
			}
			catch(ObjectNotFoundException<RollingStockFilter>&)
			{
			}

			string rsStr(map.getDefault<string>(PARAMETER_ROLLING_STOCK_LIST));
			try
			{
				if(!rsStr.empty())
							{
					vector<string> rsVect;
					split(rsVect, rsStr, is_any_of(",; "));
					allowedPathClasses.insert(0);
					BOOST_FOREACH(string& rsItem, rsVect)
					{
						allowedPathClasses.insert(lexical_cast<RegistryKeyType>(rsItem));
					}
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Rolling Stock List is unreadable");
			}

			AccessParameters::AllowedNetworks allowedNetworks;
			string nwlStr(map.getDefault<string>(PARAMETER_NETWORK_LIST));
			try
			{
				if(!nwlStr.empty())
							{
					vector<string> nwVect;
					split(nwVect, nwlStr, is_any_of(",; "));
					allowedNetworks.insert(0);
					BOOST_FOREACH(string& nwItem, nwVect)
					{
						allowedNetworks.insert(lexical_cast<RegistryKeyType>(nwItem));
					}
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Network List is unreadable");
			}

			// Accessibility
			optional<unsigned int> acint(map.getOptional<unsigned int>(PARAMETER_ACCESSIBILITY));
			if(_config)
			{
				_accessParameters = _config->getAccessParameters(
					acint ? static_cast<UserClassCode>(*acint) : USER_PEDESTRIAN,
					allowedPathClasses,
					allowedNetworks
				);
			}
			else
			{
				if(acint && *acint == USER_HANDICAPPED)
				{
					_accessParameters = AccessParameters(
						*acint, false, false, 300, posix_time::hours(24), 0.556, boost::optional<size_t>(), allowedPathClasses, allowedNetworks
					);
				}
				else if(acint && *acint == USER_BIKE)
				{
					_accessParameters = AccessParameters(
						*acint, false, false, 3000, posix_time::hours(24), 4.167, boost::optional<size_t>(), allowedPathClasses, allowedNetworks
					);
				}
				else
				{
					_accessParameters = AccessParameters(
						USER_PEDESTRIAN, false, false, 1000, posix_time::hours(24), 1.111, boost::optional<size_t>(), allowedPathClasses, allowedNetworks
					);
				}
			}

			// Max depth
			if(map.getOptional<size_t>(PARAMETER_MAX_DEPTH))
			{
				optional<size_t> maxDepth = map.getOptional<size_t>(PARAMETER_MAX_DEPTH);
				if (*maxDepth > 0)
				{
					*maxDepth -= 1; // max depth (number of journeys) is number of connections + 1
				}
				_accessParameters.setMaxtransportConnectionsCount(maxDepth);
			}

			// Filter on waiting time
			if(map.getDefault<long>(PARAMETER_MIN_WAITING_TIME_FILTER, 0))
			{
				_minWaitingTimeFilter = minutes(map.get<long>(PARAMETER_MIN_WAITING_TIME_FILTER));
			}

			// Approach speed
			if(map.getOptional<double>(PARAMETER_APPROACH_SPEED))
			{
				_accessParameters.setApproachSpeed(*(map.getOptional<double>(PARAMETER_APPROACH_SPEED)));
			}

			if(map.getOptional<int>(PARAMETER_MAX_APPROACH_DISTANCE))
			{
				_accessParameters.setMaxApproachDistance(*(map.getOptional<int>(PARAMETER_MAX_APPROACH_DISTANCE)));
			}

			// Origin and destination places
			optional<RegistryKeyType> favoriteId(map.getOptional<RegistryKeyType>(PARAMETER_FAVORITE_ID));
			if (favoriteId) // Favorite places
			{
				/// TODO implement it
			}
			else
			{
				string originPlaceText = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_TEXT);
				string destinationPlaceText = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_TEXT);

				string originPlaceXY = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_XY);
				string destinationPlaceXY = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_XY);
				bool invertXY = map.getDefault<bool>(PARAMETER_INVERT_XY);

				// Get departure place

				if( // Two fields input
					map.isDefined(PARAMETER_DEPARTURE_CITY_TEXT) &&
					map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT)
				){
					_originCityText = map.getDefault<string>(PARAMETER_DEPARTURE_CITY_TEXT);
					_originPlaceText = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_TEXT);
					if(!_originCityText.empty() || !_originPlaceText.empty())
					{
						if(_originCityText.empty())
						{
							RoadModule::ExtendedFetchPlacesResult results(PTModule::ExtendedFetchPlaces(_originPlaceText, 1));
							if(!results.empty())
							{
								_departure_place = *results.begin();
						}	}
						else
						{
							_departure_place = _config ?
								_config->extendedFetchPlace(_originCityText, _originPlaceText) :
								RoadModule::ExtendedFetchPlace(_originCityText, _originPlaceText)
							;
						}
					}
				}
				// One field input
				else if(!originPlaceText.empty())
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					// Departure
					placesListService.setClassFilter(map.getDefault<string>(PARAMETER_DEPARTURE_CLASS_FILTER));
					placesListService.setCitiesWithAtLeastAStop(false);
					placesListService.setText(originPlaceText);
					_departure_place.placeResult = placesListService.getPlaceFromBestResult(placesListService.runWithoutOutput());
				}
				// XY input
				else if(!originPlaceXY.empty())
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);
					placesListService.setCitiesWithAtLeastAStop(false);
					placesListService.addRequiredUserClass(_accessParameters.getUserClass());
					placesListService.setCoordinatesXY(originPlaceXY, invertXY);
					_departure_place.placeResult = placesListService.getPlaceFromBestResult(placesListService.runWithoutOutput());
				}

				// Get arrival place

				if( // Two fields input
					map.isDefined(PARAMETER_ARRIVAL_CITY_TEXT) &&
					map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT)
				){
					_destinationCityText = map.getDefault<string>(PARAMETER_ARRIVAL_CITY_TEXT);
					_destinationPlaceText = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_TEXT);
					if(!_destinationCityText.empty() || !_destinationPlaceText.empty())
					{
						if(_destinationCityText.empty())
						{
							RoadModule::ExtendedFetchPlacesResult results(PTModule::ExtendedFetchPlaces(_destinationPlaceText, 1));
							if(!results.empty())
							{
								_arrival_place = *results.begin();
						}	}
						else
						{
							_arrival_place = _config ?
								_config->extendedFetchPlace(_destinationCityText, _destinationPlaceText) :
								RoadModule::ExtendedFetchPlace(_destinationCityText, _destinationPlaceText)
							;
						}
					}
				}
				// One field input
				else if(!destinationPlaceText.empty())
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					// Arrival
					placesListService.setClassFilter(map.getDefault<string>(PARAMETER_ARRIVAL_CLASS_FILTER));
					placesListService.setCitiesWithAtLeastAStop(false);
					placesListService.setText(destinationPlaceText);
					_arrival_place.placeResult = placesListService.getPlaceFromBestResult(placesListService.runWithoutOutput());
				}
				// XY input
				else if(!destinationPlaceXY.empty())
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);
					placesListService.setCitiesWithAtLeastAStop(false);
					placesListService.addRequiredUserClass(_accessParameters.getUserClass());
					placesListService.setCoordinatesXY(destinationPlaceXY, invertXY);
					_arrival_place.placeResult = placesListService.getPlaceFromBestResult(placesListService.runWithoutOutput());
				}
			}

			// Date parameters
			try
			{
				// 1a : by day and time period
				if(!map.getDefault<string>(PARAMETER_DAY).empty())
				{
					// Config check
					if(!_config)
					{
						throw RequestException("A config must be defined to use this date specification method.");
					}

					// Day
					_day = from_string(map.get<string>(PARAMETER_DAY));

					// Time period
					_periodId = map.get<size_t>(PARAMETER_PERIOD_ID);
					if (_periodId >= _config->get<HourPeriods>().size())
					{
						throw RequestException("Bad value for period id");
					}
					_period = &_config->get<HourPeriods>().at(_periodId);
				}
				// 1abcde : optional bounds specification
				else
				{
					// Lowest departure time
					if(!map.getDefault<string>(PARAMETER_LOWEST_DEPARTURE_TIME).empty())
					{
						_startDate = time_from_string(map.get<string>(PARAMETER_LOWEST_DEPARTURE_TIME));
					}

					// Highest departure time
					if(!map.getDefault<string>(PARAMETER_HIGHEST_DEPARTURE_TIME).empty())
					{
						_endDate = time_from_string(map.get<string>(PARAMETER_HIGHEST_DEPARTURE_TIME));
					}

					// Lowest arrival time
					if(!map.getDefault<string>(PARAMETER_LOWEST_ARRIVAL_TIME).empty())
					{
						_startArrivalDate = time_from_string(map.get<string>(PARAMETER_LOWEST_ARRIVAL_TIME));
					}

					// Highest departure time
					if(!map.getDefault<string>(PARAMETER_HIGHEST_ARRIVAL_TIME).empty())
					{
						_endArrivalDate = time_from_string(map.get<string>(PARAMETER_HIGHEST_ARRIVAL_TIME));
					}
				}
			}
			catch(PTServiceConfig::ForbiddenDateException)
			{
				throw RequestException("Date in the past is forbidden");
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw RequestException(e.what());
			}

			// Max solutions number
			_maxSolutionsNumber = map.getOptional<size_t>(PARAMETER_MAX_SOLUTIONS_NUMBER);

			// Fare Calculation
			_fareCalculation = map.getDefault<bool>(PARAMETER_FARE_CALCULATION,false);

			// Ignore Reservation Rules
			_ignoreReservationRules = map.getDefault<bool>(PARAMETER_IGNORE_RESERVATION_RULES, false);

			// Reservation Rules Delay type
			if(map.getDefault<int>(PARAMETER_RESERVATION_DELAY_TYPE, 0))
			{
				_reservationRulesDelayType = UseRule::RESERVATION_EXTERNAL_DELAY;
			}
			else
			{
				_reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY;
			}

			if(	!_departure_place.placeResult.value || !_arrival_place.placeResult.value
			){
				return;
			}

			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);

			//XML output options
			_showResTab = true;
			optional<string> showResTabLabel(map.getOptional<string>(PARAMETER_SHOW_RESULT_TABLE));
			if((showResTabLabel)
				&&(*showResTabLabel=="no"))
			{
				_showResTab = false;
			}
			_showCoords = true;
			optional<string> showCoordsLabel(map.getOptional<string>(PARAMETER_SHOW_COORDINATES));
			if((showCoordsLabel)
				&&(*showCoordsLabel=="no"))
			{
				_showCoords = false;
			}

			// Output messages
			RegistryKeyType broadcastPointId(
				map.getDefault<RegistryKeyType>(PARAMETER_BROADCAST_POINT_ID, 0)
			);
			if(broadcastPointId)
			{
				try
				{
					_broadcastPoint = Env::GetOfficialEnv().get<CustomBroadcastPoint>(broadcastPointId).get();
				}
				catch(ObjectNotFoundException<CustomBroadcastPoint>&)
				{
					throw RequestException("No such broadcast point");
				}
			}

			// Pages
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE));
				if(id)
				{
					_page = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such main page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_SCHEDULES_ROW_PAGE));
				if(id)
				{
					_schedulesRowPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such schedules row page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_SCHEDULES_CELL_PAGE));
				if(id)
				{
					_schedulesCellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such schedules cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_LINES_ROW_PAGE));
				if(id)
				{
					_linesRowPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such lines row page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_LINE_MARKER_PAGE));
				if(id)
				{
					_lineMarkerPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such line marker page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_BOARD_PAGE));
				if(id)
				{
					_boardPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such board page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_WARNING_PAGE));
				if(id)
				{
					_warningPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such warning page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_RESERVATION_PAGE));
				if(id)
				{
					_reservationPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such reservation page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_DURATION_PAGE));
				if(id)
				{
					_durationPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such duration page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_TEXT_DURATION_PAGE));
				if(id)
				{
					_textDurationPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such text duration page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAP_PAGE));
				if(id)
				{
					_mapPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAP_LINE_PAGE));
				if(id)
				{
					_mapLinePage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map line page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_DATE_TIME_PAGE));
				if(id)
				{
					_dateTimePage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such date time page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_STOP_CELL_PAGE));
				if(id)
				{
					_stopCellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such stop cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_SERVICE_CELL_PAGE));
				if(id)
				{
					_serviceCellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such service cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_ROAD_CELL_PAGE));
				if(id)
				{
					_roadPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such road cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_JUNCTION_CELL_PAGE));
				if(id)
				{
					_junctionPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such junction (external) cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_TICKET_CELL_PAGE));
				if(id)
				{
					_ticketCellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such ticket cell page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAP_STOP_PAGE));
				if(id)
				{
					_mapStopCellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map stop page : "+ e.getMessage());
			}
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAP_SERVICE_PAGE));
				if(id)
				{
					_mapServiceCellPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map service page : "+ e.getMessage());
			}

			// Map road page
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAP_ROAD_PAGE));
				if(id)
				{
					_mapRoadPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map road page : "+ e.getMessage());
			}

			// Map junction page
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAP_JUNCTION_PAGE));
				if(id)
				{
					_mapJunctionPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such map junction (external) page : "+ e.getMessage());
			}

			// Warning check page
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_WARNING_CHECK_PAGE));
				if(id)
				{
					_warningCheckPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such warning check page : "+ e.getMessage());
			}

			// Result row page
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_RESULT_ROW_PAGE));
				if(id)
				{
					_resultRowPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such result row page : "+ e.getMessage());
			}
		}



		util::ParametersMap RoutePlannerFunction::run(
			ostream& stream,
			const Request& request
		) const	{

			// Checks if there is something to plan
			if (!_departure_place.placeResult.value || !_arrival_place.placeResult.value)
			{
				return ParametersMap();
			}



			//////////////////////////////////////////////////////////////////////////
			// Time bounds generation

			// Declarations
			ptime startDate;
			ptime endDate;
			ptime startArrivalDate;
			ptime endArrivalDate;
			PlanningOrder planningOrder(DEPARTURE_FIRST);

			// Max run time for bounds extension
			time_duration maxRunTime(minutes(0));
			if(	_departure_place.placeResult.value &&
				_arrival_place.placeResult.value &&
				_departure_place.placeResult.value->getPoint().get() &&
				_arrival_place.placeResult.value->getPoint().get() &&
				!_departure_place.placeResult.value->getPoint()->isEmpty() &&
				!_arrival_place.placeResult.value->getPoint()->isEmpty()
			){
				maxRunTime =
					minutes(120) +
					minutes(
					6 * static_cast<int>(
						_departure_place.placeResult.value->getPoint()->distance(
							_arrival_place.placeResult.value->getPoint().get()
						) / 1000
				)	);
			}

			// 1a Day and period
			if(!_day.is_not_a_date())
			{
				startDate = ptime(_day, time_duration(0, 0, 0));
				endDate = startDate;
				_config->applyPeriod(*_period, startDate, endDate);

				startArrivalDate = startDate;
				endArrivalDate = endDate + maxRunTime;
			}
			// 1d : full bounds specification
			else if(
				!_startDate.is_not_a_date_time() &&
				!_endArrivalDate.is_not_a_date_time()
			){
				startDate = _startDate;
				endDate = _endDate.is_not_a_date_time() ?
					_endArrivalDate - maxRunTime :
					_endDate;
				endArrivalDate = _endArrivalDate;
				startArrivalDate = _startArrivalDate.is_not_a_date_time() ?
					_startDate :
					_startArrivalDate;
			}
			// 1b : departure date(s) only
			else if(
				!_startDate.is_not_a_date_time()
			){
				startDate = _startDate;
				endDate = _endDate.is_not_a_date_time() ?
					_startDate + hours(24) :
					_endDate;
				endArrivalDate = endDate + maxRunTime;
				startArrivalDate = _startDate;
			}
			// 1c : arrival date(s) only
			else if(
				!_endArrivalDate.is_not_a_date_time()
			){
				endArrivalDate = _endArrivalDate;
				startArrivalDate = _startArrivalDate.is_not_a_date_time() ?
					_endArrivalDate - hours(24) :
					_startArrivalDate;
				endDate = _endArrivalDate;
				startDate = startArrivalDate - maxRunTime;
				planningOrder = ARRIVAL_FIRST;
			}
			// 1e : full default values
			else
			{
				startDate = second_clock::local_time();
				endDate = startDate + hours(24);
				startArrivalDate = startDate;
				endArrivalDate = endDate + maxRunTime;
			}


			//////////////////////////////////////////////////////////////////////////
			// Journey planning

			// Initialization
			PTTimeSlotRoutePlanner r(
				_departure_place.placeResult.value.get(),
				_arrival_place.placeResult.value.get(),
				startDate,
				endDate,
				startArrivalDate,
				endArrivalDate,
				_maxSolutionsNumber,
				_accessParameters,
				planningOrder,
				_ignoreReservationRules,
				*_logger,
				_maxTransferDuration,
				_minMaxDurationRatioFilter,
				PTModule::isTheoreticalAllowed(),
				PTModule::isRealTimeAllowed(),
				_reservationRulesDelayType
			);

			// Computing
			_result.reset(new PTRoutePlannerResult(r.run()));

			// Min waiting time filter
			if(_minWaitingTimeFilter)
			{
				_result->filterOnWaitingTime(*_minWaitingTimeFilter);
			}

			// Messages
			ParametersMap messagesOnBroadCastPoint;
			if(_broadcastPoint)
			{
				// Parameters map
				ParametersMap parameters;

				GetMessagesFunction f(
					_broadcastPoint,
					parameters
				);
				messagesOnBroadCastPoint = f.run(stream, request);
			}

			//////////////////////////////////////////////////////////////////////////
			// Display

			// CMS display
			if(_page.get())
			{
				display(
					stream,
					request,
					*_result,
					startDate.date(),
					_periodId,
					_departure_place.placeResult.value.get(),
					_arrival_place.placeResult.value.get(),
					_period,
					_accessParameters,
					messagesOnBroadCastPoint
				);
			}
			else if(_outputFormat == VALUE_ADMIN_HTML)
			{
				_result->displayHTMLTable(stream, optional<HTMLForm&>(), string(), false);
			}
			else
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<routePlannerResult xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/routeplanner_result.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" <<
					"<query"
				;
				if(_maxSolutionsNumber)
				{
					stream << " maxSolutions=\"" << *_maxSolutionsNumber << "\"";
				}
				stream << " userProfile=\"" << _accessParameters.getUserClass() << "\"";
				if(request.getSession())
				{
					stream << " sessionId=\"" << request.getSession()->getKey() << "\"";
				}
				if(_config)
				{
					stream << " siteId=\"" << _config->getKey() << "\"";
				}
				stream <<
					"><timeBounds" <<
						" minDepartureHour=\"" << posix_time::to_iso_extended_string(r.getLowestDepartureTime()) << "\"" <<
						" minArrivalHour=\"" << posix_time::to_iso_extended_string(r.getLowestArrivalTime()) << "\"" <<
						" maxArrivalHour=\"" << posix_time::to_iso_extended_string(r.getHighestArrivalTime()) << "\"" <<
						" maxDepartureHour=\"" << posix_time::to_iso_extended_string(r.getHighestDepartureTime()) << "\"" <<
					" />"
				;
				if(_period)
				{
					stream <<
						"<timePeriod id=\"" << _periodId << "\" date=\"" <<
						to_iso_extended_string(startDate.date()) << "\" name=\"" <<
						_period->getCaption() << "\" />"
					;
				}
	
				stream << "<places";
 
				if(_departure_place.cityResult.value.get())
				{
					stream << " departureCity=\"" << _departure_place.placeResult.key.getSource() << "\" departureCityNameTrust=\"" << _departure_place.cityResult.score.phoneticScore << "\"";
				}
				else
				{
					string cityName;
					if(dynamic_cast<const NamedPlace*>(_departure_place.placeResult.value.get()))
					{
						cityName = dynamic_cast<const NamedPlace*>(_departure_place.placeResult.value.get())->getCity()->getName();
					}
					else
					{
						cityName = dynamic_cast<const City*>(_departure_place.placeResult.value.get())->getName();
					}
					stream << " departureCity=\"" << cityName << "\" departureCityNameTrust=\"1\"";
				}

				if(_arrival_place.cityResult.value.get())
				{
					stream << " arrivalCity=\"" << _arrival_place.cityResult.key.getSource() << "\" arrivalCityNameTrust=\"" << _arrival_place.cityResult.score.phoneticScore << "\"";
				}
				else
				{
					string cityName;
					if(dynamic_cast<const NamedPlace*>(_arrival_place.placeResult.value.get()))
					{
						cityName = dynamic_cast<const NamedPlace*>(_arrival_place.placeResult.value.get())->getCity()->getName();
					}
					else
					{
						cityName = dynamic_cast<const City*>(_arrival_place.placeResult.value.get())->getName();
					}
					stream << " arrivalCity=\"" << cityName << "\" arrivalCityNameTrust=\"1\"";
				}


				if(dynamic_cast<Place*>(_departure_place.cityResult.value.get()) != dynamic_cast<Place*>(_departure_place.placeResult.value.get()))
				{
					stream <<
						" departureStop=\"" << _departure_place.placeResult.key.getSource() << "\" departureStopNameTrust=\"" << _departure_place.placeResult.score.phoneticScore << "\""
					;
				}
				if(dynamic_cast<Place*>(_arrival_place.cityResult.value.get()) != dynamic_cast<Place*>(_arrival_place.placeResult.value.get()))
				{
					stream <<
						" arrivalStop=\"" << _arrival_place.placeResult.key.getSource() << "\" arrivalStopNameTrust=\"" << _arrival_place.placeResult.score.phoneticScore << "\""
					;
				}
				stream << " />";
//					if(_favorite.get())
//					{
//						stream <<
//							"<favorite id=\"" << _favorite->getKey() << "\" />"
//						;
//					}

				if(_rollingStockFilter.get())
				{
					stream << "<transportModeFilter id=\"" << _rollingStockFilter->getKey() << "\" name=\"" << _rollingStockFilter->getName() << "\"/>";
				}

				stream <<
					"</query>" <<
					"<journeys>"
				;
				const PTRoutePlannerResult::PlacesListConfiguration::List& placesList(
					_result->getOrderedPlaces().getResult()
				);
				PlacesContentVector sheetRows(placesList.size());
				BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
				{
					stream.reset(new ostringstream);
				}

				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, _result->getJourneys())
				{
					bool hasALineAlert(false); // Interactive
					bool hasAStopAlert(false); // Interactive
					bool pedestrianMode = false;

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itPlaces(placesList.begin());

					stream <<
						"<journey hasALineAlert=\"" << (hasALineAlert ? "true" : "false") << "\" hasAStopAlert=\"" << (hasAStopAlert ? "true" : "false") << "\""
					;
					if(journey.getContinuousServiceRange().total_seconds() > 0)
					{
						stream << " continuousServiceDuration=\"" << journey.getContinuousServiceRange() << "\"";
					}

					// CO2 Emissions, Energy consumption and total distance computation
					double co2Emissions = 0;
					double energyConsumption = 0;
					double totalDistance = 0;
					BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
					{
						const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
						if((line == NULL)||(!line->getRollingStock())) continue;

						double distance = su.getDistance();

						// if the route has no distance data
						if(distance == 0)
						{
							if(su.getGeometry())
							{
								distance = CoordinatesSystem::GetCoordinatesSystem(27572).convertGeometry(
									*su.getGeometry())->getLength();
									distance *= 1.10;
							}
						}
						if(distance > 0)
						{
							co2Emissions += distance * line->getRollingStock()->get<CO2Emissions>() / RollingStock::CO2_EMISSIONS_DISTANCE_UNIT_IN_METERS;
							energyConsumption += distance * line->getRollingStock()->get<EnergyConsumption>() / RollingStock::ENERGY_CONSUMPTION_DISTANCE_UNIT_IN_METERS;
							totalDistance += distance;
						}
					}
					stream << " " << DATA_CO2_EMISSIONS << "=\"" << co2Emissions << "\"";
					stream << " " << DATA_ENERGY_CONSUMPTION << "=\"" << energyConsumption << "\"";
					stream << " " << DATA_DISTANCE << "=\"" << totalDistance << "\"";
					stream << ">";

					if(journey.getReservationCompliance(false, _reservationRulesDelayType) != false)
					{
						set<const ReservationContact*> resaRules;
						BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
						{
							const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
							if(line == NULL) continue;

							if(	line->getCommercialLine()->getReservationContact() &&
								UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su, false, _reservationRulesDelayType))
							){
								resaRules.insert(line->getCommercialLine()->getReservationContact());
							}
						}
						stringstream sPhones;
						stringstream sOpeningHours;
						bool onlineBooking(!resaRules.empty());
						BOOST_FOREACH(const ReservationContact* rc, resaRules)
						{
							sPhones << rc->get<PhoneExchangeNumber>() << " ";
							sOpeningHours << rc->get<PhoneExchangeOpeningHours>() << " ";
							if (!OnlineReservationRule::GetOnlineReservationRule(rc))
							{
								onlineBooking = false;
							}
						}

						stream << "<reservation" <<
							" online=\"" << (onlineBooking ? "true" : "false") << "\"" <<
							" type=\"" << (journey.getReservationCompliance(false, _reservationRulesDelayType) == true ? "compulsory" : "optional") << "\""
						;
						if(!sOpeningHours.str().empty())
						{
							stream << " openingHours=\"" << sOpeningHours.str() << "\"";
						}
						if(!sPhones.str().empty())
						{
							stream << " phoneNumber=\"" << sPhones.str() << "\"";
						}
						stream << " deadLine=\"" << posix_time::to_iso_extended_string(journey.getReservationDeadLine(_reservationRulesDelayType)) << "\" />";
					}
					stream << "<chunks>";

					// Loop on each leg
					const Journey::ServiceUses& jl(journey.getServiceUses());
					double partialDistance(0);
					bool firstApproach(true);
					Journey::ServiceUses::const_iterator lastApproachBeginning(jl.end());
					Journey::ServiceUses::const_iterator lastTransportEnding(jl.end());

					for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
					{
						const ServicePointer& leg(*itl);

						if(	PTRoutePlannerResult::HaveToDisplayDepartureStopOnGrid(itl, jl, false)
						){
							const NamedPlace* placeToSearch(
								PTRoutePlannerResult::GetNamedPlaceForDeparture(
									leg.getService()->getPath()->isPedestrianMode(),
									itl == jl.begin() ? NULL : &(*(itl - 1)),
									leg,
									*placesList.begin()->place
							)	);

							for (; itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow)
							{
								**itSheetRow << "<cell />";
							}

							pedestrianMode = leg.getService()->getPath()->isPedestrianMode();

							// Saving of the columns on each lines
							if(itl == jl.begin())
							{
								**itSheetRow << "<cell";
							}
							**itSheetRow <<
								" departureDateTime=\"" <<
								posix_time::to_iso_extended_string(leg.getDepartureDateTime()) << "\"";
							if(journey.getContinuousServiceRange().total_seconds() > 0)
							{
								posix_time::ptime edTime(leg.getDepartureDateTime());
								edTime += journey.getContinuousServiceRange();
								**itSheetRow << " endDepartureDateTime=\"" <<
									posix_time::to_iso_extended_string(edTime) << "\"";
							}
							if(pedestrianMode)
							{
								**itSheetRow << " pedestrian=\"departure\"";
							}
							**itSheetRow << " />";

							++itPlaces; ++itSheetRow;
						}

						if(	PTRoutePlannerResult::HaveToDisplayArrivalStopOnGrid(itl, jl)
						){
							const NamedPlace* placeToSearch(
								PTRoutePlannerResult::GetNamedPlaceForArrival(
									leg.getService()->getPath()->isPedestrianMode(),
									leg,
									itl == jl.end()-1 ? NULL : &(*(itl+1)),
									*placesList.rbegin()->place
							)	);

							for (; itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow )
							{
								**itSheetRow << "<cell";
								if(pedestrianMode)
								{
									**itSheetRow << " pedestrian=\"traversal\"";
								}
								**itSheetRow << " />";
							}
							**itSheetRow << "<cell arrivalDateTime=\"" <<
								posix_time::to_iso_extended_string(leg.getArrivalDateTime()) << "\"";
							if(journey.getContinuousServiceRange().total_seconds() > 0)
							{
								posix_time::ptime eaTime(leg.getArrivalDateTime());
								eaTime += journey.getContinuousServiceRange();
								**itSheetRow << " endArrivalDateTime=\"" <<
									posix_time::to_iso_extended_string(eaTime) << "\"";
							}
							if(pedestrianMode)
							{
								**itSheetRow << " pedestrian=\"arrival\"";
							}
							if(	itl == jl.end() - 1)
							{
								**itSheetRow << " />";
							}
						}


						const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(leg.getService()->getPath()));
						const Junction* junction(dynamic_cast<const Junction*>(leg.getService()->getPath()));
						if(line != NULL)
						{
							// Insertion of fake leg if site does not output road approach detail
							if(!_outputRoadApproachDetail)
							{
								if(firstApproach)
								{
									if(line->isPedestrianMode())
									{
										partialDistance += leg.getDistance();
									}
									else
									{
										if(itl != jl.begin())
										{
											const RoadPath* road(dynamic_cast<const RoadPath*> (jl.begin()->getService()->getPath()));
											const ptime& departureTime(jl.begin()->getDepartureDateTime());
											const ptime& arrivalTime((itl-1)->getArrivalDateTime());
											stream <<
												"<street" <<
												" length=\"" << ceil(partialDistance) << "\"" <<
												" city=\"Departure approach\"" <<
												" name=\"Departure approach\"" <<
												" departureTime=\"" << posix_time::to_iso_extended_string(departureTime) << "\"" <<
												" arrivalTime=\"" << posix_time::to_iso_extended_string(arrivalTime) << "\"";
											if(journey.getContinuousServiceRange().total_seconds() > 0)
											{
												posix_time::ptime edTime(departureTime);
												edTime += journey.getContinuousServiceRange();
												posix_time::ptime eaTime(arrivalTime);
												eaTime += journey.getContinuousServiceRange();
												stream <<
													" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
													" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
											}
											stream << ">" <<
												"<startAddress>";


											if(dynamic_cast<const NamedPlace*>(_result->getDeparturePlace()))
											{
												_xmlDisplayAddress(stream, dynamic_cast<const NamedPlace&>(*_result->getDeparturePlace()),_showCoords);
											}
											else if(dynamic_cast<const Crossing*>(jl.begin()->getDepartureEdge()->getFromVertex()))
											{
												if(dynamic_cast<const RoadPlace*>(_departure_place.placeResult.value.get()))
												{
													_xmlDisplayRoadPlace(
														stream,
														dynamic_cast<const RoadPlace&>(*_departure_place.placeResult.value.get()),
														_showCoords
													);
												}
												else
												{
													_xmlDisplayAddress(
														stream,
														*dynamic_cast<const Crossing*>(jl.begin()->getDepartureEdge()->getFromVertex()),
														*road->getRoad()->get<RoadPlace>(),
														_showCoords
													);
												}
											}
											stream <<
												"</startAddress>" <<
												"<endAddress>";
											if(dynamic_cast<const NamedPlace*>(leg.getDepartureEdge()->getHub()))
											{
												_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*leg.getDepartureEdge()->getHub()),_showCoords);
											}
											else if(dynamic_cast<const Crossing*>((itl-1)->getArrivalEdge()->getFromVertex()))
											{
												_xmlDisplayAddress(
													stream,
													*dynamic_cast<const Crossing*>((itl-1)->getArrivalEdge()->getFromVertex()),
													*road->getRoad()->get<RoadPlace>(),
													_showCoords
												);
											}
											stream <<
												"</endAddress>" <<
												"</street>"
											;
										}

										// Transport state
										firstApproach = false;
										partialDistance = 0;
									}
								}
							}

							stream <<
								"<" << (line->isPedestrianMode() ? "connection" : "transport") <<
									" length=\"" << ceil(leg.getDistance()) << "\"" <<
									" departureTime=\"" << posix_time::to_iso_extended_string(leg.getDepartureDateTime()) << "\"" <<
									" arrivalTime=\"" << posix_time::to_iso_extended_string(leg.getArrivalDateTime()) << "\"";
							if(journey.getContinuousServiceRange().total_seconds() > 0)
							{
								posix_time::ptime edTime(leg.getDepartureDateTime());
								edTime += journey.getContinuousServiceRange();
								posix_time::ptime eaTime(leg.getArrivalDateTime());
								eaTime += journey.getContinuousServiceRange();
								stream <<
									" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
									" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
							}
							const ContinuousService* cserv(dynamic_cast<const ContinuousService*>(leg.getService()));
							if(cserv && cserv->getMaxWaitingTime().total_seconds() > 0)
							{
								stream << " possibleWaitingTime=\"" << (cserv->getMaxWaitingTime().total_seconds() / 60) << "\"";
							}
							stream <<
									" startStopIsTerminus=\"" << (leg.getDepartureEdge()->getRankInPath() == 0 ? "true" : "false") << "\"" <<
									" endStopIsTerminus=\"" << (leg.getArrivalEdge()->getRankInPath()+1 == leg.getArrivalEdge()->getParentPath()->getEdges().size() ? "true" : "false") << "\"";
							if(!line->getDirection().empty())
							{
								stream << " destinationText=\"" << line->getDirection() << "\"";
							}
							else if(line->getDirectionObj())
							{
								stream << " destinationText=\"" << line->getDirectionObj()->get<DisplayedText>() << "\"";
							}
							stream <<
								">";
							_xmlDisplayPhysicalStop(stream, DATA_START_STOP_NAME, dynamic_cast<const StopPoint&>(*leg.getDepartureEdge()->getFromVertex()),_showCoords);
							_xmlDisplayPhysicalStop(stream, DATA_END_STOP_NAME, dynamic_cast<const StopPoint&>(*leg.getArrivalEdge()->getFromVertex()),_showCoords);
							_xmlDisplayPhysicalStop(stream, "destinationStop", dynamic_cast<const StopPoint&>(*(*line->getEdges().rbegin())->getFromVertex()),_showCoords);
							if(!line->isPedestrianMode())
							{
								stream <<
									"<line" <<
									" id=\"" << line->getCommercialLine()->getKey() << "\"" <<
									" service_id=\"" << line->getKey() << "\"";
								if(line->getCommercialLine()->getColor())
								{
									stream << " color=\"" << line->getCommercialLine()->getColor()->toXMLColor() << "\"";
								}
								if(!line->getCommercialLine()->getStyle().empty())
								{
									stream << " cssClass=\"" << line->getCommercialLine()->getStyle() << "\"";
								}
								if(!line->getCommercialLine()->getImage().empty())
								{
									stream << " imgURL=\"" << line->getCommercialLine()->getImage() << "\"";
								}
								if(!line->getCommercialLine()->getLongName().empty())
								{
									stream << " longName=\"" << line->getCommercialLine()->getLongName() << "\"";
								}
								if(!line->getCommercialLine()->getShortName().empty())
								{
									stream << " shortName=\"" << line->getCommercialLine()->getShortName() << "\"";
								}
								stream << ">";
								if(false) // Transform into interactive
								{
								}
								stream <<
									"</line>";
								if(line->getRollingStock())
								{
									stream <<
										"<vehicleType" <<
										" id=\"" << line->getRollingStock()->getKey() << "\"" <<
										" article=\"" << line->getRollingStock()->get<Article>() << "\"" <<
										" name=\"" << line->getRollingStock()->getName() << "\"" <<
										" />";
								}

								boost::shared_ptr<ParametersMap> pmMessages(new ParametersMap);
								BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, messagesOnBroadCastPoint.getSubMaps("message"))
								{
									bool displayMessage(false);
									if(pmMessage->hasSubMaps(Alarm::TAG_RECIPIENTS))
									{
										BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmRecipient, pmMessage->getSubMaps(Alarm::TAG_RECIPIENTS))
										{
											if (pmRecipient->hasSubMaps("line"))
											{
												BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pmRecipient->getSubMaps("line"))
												{
													if (pmLine->getValue(Registrable::ATTR_ID) == lexical_cast<string>(line->getCommercialLine()->getKey()))
													{
														displayMessage = true;
														break;
													}
												}
											}
											if (displayMessage)
												break;
										}
									}
									if (displayMessage)
									{
										pmMessages->insert(string("message"), pmMessage);
									}
								}
								pmMessages->outputXML(stream, "messages");
							}
							stream << "</transport>";
						}
						else if (junction != NULL)
						{
							stream <<
								"<junction" <<
									" length=\"" << ceil(leg.getDistance()) << "\"" <<
									" departureTime=\"" << posix_time::to_iso_extended_string(leg.getDepartureDateTime()) << "\"" <<
									" arrivalTime=\"" << posix_time::to_iso_extended_string(leg.getArrivalDateTime()) << "\">";
							_xmlDisplayPhysicalStop(stream, DATA_START_STOP_NAME, dynamic_cast<const StopPoint&>(*leg.getDepartureEdge()->getFromVertex()),_showCoords);
							_xmlDisplayPhysicalStop(stream, DATA_END_STOP_NAME, dynamic_cast<const StopPoint&>(*leg.getArrivalEdge()->getFromVertex()),_showCoords);
							stream << "</junction>";
						}

						const RoadPath* road(dynamic_cast<const RoadPath*> (leg.getService()->getPath ()));
						if(road != NULL)
						{
							if(_outputRoadApproachDetail)
							{
								stream <<
									"<street" <<
									" length=\"" << ceil(leg.getDistance()) << "\"" <<
									" city=\"" << road->getRoad()->get<RoadPlace>()->getCity()->getName() << "\"" <<
									" name=\"" << road->getRoad()->get<RoadPlace>()->getName() << "\"" <<
									" departureTime=\"" << posix_time::to_iso_extended_string(leg.getDepartureDateTime()) << "\"" <<
									" arrivalTime=\"" << posix_time::to_iso_extended_string(leg.getArrivalDateTime()) << "\"";
								if(journey.getContinuousServiceRange().total_seconds() > 0)
								{
									posix_time::ptime edTime(leg.getDepartureDateTime());
									edTime += journey.getContinuousServiceRange();
									posix_time::ptime eaTime(leg.getArrivalDateTime());
									eaTime += journey.getContinuousServiceRange();
									stream <<
										" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
										" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
								}
								stream <<
									">" <<
									"<startAddress>";
								if(dynamic_cast<const NamedPlace*>(leg.getDepartureEdge()->getHub()))
								{
									_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*leg.getDepartureEdge()->getHub()),_showCoords);
								}
								else if(dynamic_cast<const Crossing*>(leg.getDepartureEdge()->getFromVertex()))
								{
									if(itl == jl.begin() && dynamic_cast<const RoadPlace*>(_departure_place.placeResult.value.get()))
									{
										_xmlDisplayRoadPlace(
											stream,
											dynamic_cast<const RoadPlace&>(*_departure_place.placeResult.value),
											_showCoords
										);
									}
									else
									{
										_xmlDisplayAddress(
											stream,
											*dynamic_cast<const Crossing*>(leg.getDepartureEdge()->getFromVertex()),
											*road->getRoad()->get<RoadPlace>(),
											_showCoords
											);
									}
								}
								stream <<
									"</startAddress>" <<
									"<endAddress>";
								if(dynamic_cast<const NamedPlace*>(leg.getArrivalEdge()->getHub()))
								{
									_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*leg.getArrivalEdge()->getHub()),_showCoords);
								}
								else if(dynamic_cast<const Crossing*>(leg.getArrivalEdge()->getFromVertex()))
								{
									if(itl == jl.end() - 1 && dynamic_cast<const RoadPlace*>(_arrival_place.placeResult.value.get()))
									{
										_xmlDisplayRoadPlace(
											stream,
											dynamic_cast<const RoadPlace&>(*_arrival_place.placeResult.value),
											_showCoords
										);
									}
									else
									{
										_xmlDisplayAddress(
											stream,
											*dynamic_cast<const Crossing*>(leg.getArrivalEdge()->getFromVertex()),
											*road->getRoad()->get<RoadPlace>(),
											_showCoords
										);
									}
								}
								stream <<
									"</endAddress>" <<
									"</street>"
								;
							}
							else
							{
								partialDistance += ceil(leg.getDistance());
								if(!firstApproach && lastApproachBeginning == jl.end())
								{
									lastApproachBeginning = itl;
								}
							}
						}
						else
						{
							lastTransportEnding = itl;
						}

						// Junction :

					}

					// Bug 7694 : fulfill unused arrival rows
					for(++itSheetRow; itSheetRow != sheetRows.end(); ++itSheetRow)
					{
						**itSheetRow << "<cell />";
					}

					if(!_outputRoadApproachDetail && lastApproachBeginning != jl.end() && lastTransportEnding != jl.end())
					{
						const RoadPath* road(dynamic_cast<const RoadPath*> ((jl.end()-1)->getService()->getPath ()));
						const ptime& departureTime(lastApproachBeginning->getDepartureDateTime());
						const ptime& arrivalTime((jl.end()-1)->getArrivalDateTime());
						stream <<
							"<street" <<
							" length=\"" << ceil(partialDistance) << "\"" <<
							" city=\"Arrival approach\"" <<
							" name=\"Arrival approach\"" <<
							" departureTime=\"" << posix_time::to_iso_extended_string(departureTime) << "\"" <<
							" arrivalTime=\"" << posix_time::to_iso_extended_string(arrivalTime) << "\"";
						if(journey.getContinuousServiceRange().total_seconds() > 0)
						{
							posix_time::ptime edTime(departureTime);
							edTime += journey.getContinuousServiceRange();
							posix_time::ptime eaTime(arrivalTime);
							eaTime += journey.getContinuousServiceRange();
							stream <<
								" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
								" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
						}
						stream << ">" <<
							"<startAddress>";
						if(dynamic_cast<const NamedPlace*>(lastTransportEnding->getArrivalEdge()->getHub()))
						{
							_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*lastTransportEnding->getArrivalEdge()->getHub()),_showCoords);
						}
						else if(dynamic_cast<const Crossing*>(lastApproachBeginning->getDepartureEdge()->getFromVertex()))
						{
							_xmlDisplayAddress(
								stream,
								*dynamic_cast<const Crossing*>(lastApproachBeginning->getDepartureEdge()->getFromVertex()),
								*road->getRoad()->get<RoadPlace>(),
								_showCoords
							);
						}
						stream <<
							"</startAddress>" <<
							"<endAddress>";

						if(dynamic_cast<const NamedPlace*>(_result->getArrivalPlace()))
						{
							_xmlDisplayAddress(stream, dynamic_cast<const NamedPlace&>(*_result->getArrivalPlace()),_showCoords);
						}
						else if(dynamic_cast<const Crossing*>((jl.end()-1)->getArrivalEdge()->getFromVertex()))
						{
							if(dynamic_cast<const RoadPlace*>(_arrival_place.placeResult.value.get()))
							{
								_xmlDisplayRoadPlace(
									stream,
									dynamic_cast<const RoadPlace&>(*_arrival_place.placeResult.value),
									_showCoords
								);
							}
							else
							{
								_xmlDisplayAddress(
									stream,
									*dynamic_cast<const Crossing*>((jl.end()-1)->getArrivalEdge()->getFromVertex()),
									*road->getRoad()->get<RoadPlace>(),
									_showCoords
								);
							}
						}
						stream <<
							"</endAddress>" <<
							"</street>"
							;

					}

					stream << "</chunks></journey>";
				}
				stream <<
					"</journeys>";

				if(_showResTab)
				{
					stream << "<resultTable>";

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					BOOST_FOREACH(const PTRoutePlannerResult::PlacesListConfiguration::List::value_type& row, _result->getOrderedPlaces().getResult())
					{
						assert(dynamic_cast<const NamedPlace*>(row.place));
						//						const NamedPlace* np(dynamic_cast<const NamedPlace*>(row.place));

						stream <<
							"<row type=\"" << (row.isOrigin ? "departure" : row.isDestination ? "arrival" : "connection") << "\">" <<
							"<cells>" <<
							(*itSheetRow)->str() <<
							"</cells>" <<
							"<place>";
						if(dynamic_cast<const RoadPlace*>(row.place))
						{
							_xmlDisplayRoadPlace(stream, dynamic_cast<const RoadPlace&>(*row.place),_showCoords);
						}
						else
						{
							_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*row.place),_showCoords);
						}

						stream <<
							"</place>" <<
							"</row>";
						++itSheetRow;
					}
					stream << "</resultTable>";
				}
				stream << "</routePlannerResult>"
				;
			}

			return util::ParametersMap();
		}



		bool RoutePlannerFunction::isAuthorized(
			const Session* session
		) const {
			if(	_favorite.get() &&
				(	!session ||
					!session->getUser()
//					|| _favorite->getUser()->getKey() != session->getUser()->getKey()
			)	){
				return false;
			}

			return true;
		}



		std::string RoutePlannerFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}



		void RoutePlannerFunction::_xmlDisplayConnectionPlace(
			std::ostream& stream,
			const NamedPlace& np,
			bool showCoords
		) const {
			boost::shared_ptr<Point> gp;

			showCoords &= (np.getPoint().get() != NULL && !np.getPoint()->isEmpty());

			if(showCoords)
			{
				gp = _coordinatesSystem->convertPoint(
					*np.getPoint()
				);

				stream <<
					"<connectionPlace" <<
					" latitude=\"" << gp->getY() << "\"" <<
					" longitude=\"" << gp->getX() << "\"" <<
					" id=\"" << np.getKey() << "\"" <<
					" city=\"" << np.getCity()->getName() << "\"" <<
					" x=\"" << static_cast<int>(np.getPoint()->getX()) << "\"" <<
					" y=\"" << static_cast<int>(np.getPoint()->getY()) << "\""
					" name=\"" << np.getName() << "\"" <<
					">";
			}
			else //Don't show coords
			{
				stream <<
					"<connectionPlace" <<
					" id=\"" << np.getKey() << "\"" <<
					" city=\"" << np.getCity()->getName() << "\"" <<
					" name=\"" << np.getName() << "\"" <<
					">";
			}
			if(false) // Test if alarm on place
			{
				stream <<
					"<alert" <<
						" id=\"9834\"" <<
						" level=\"interruption\"" <<
						" startValidity=\"2000-01-01T00:00:07.0Z\"" <<
						" endValidity=\"2099-12-31T23:59:00.0Z\"" <<
					">SNCF en greve</alert>"
				;
			}
			stream << "</connectionPlace>";
		}



		void RoutePlannerFunction::_xmlDisplayPhysicalStop(
			std::ostream& stream,
			const std::string& tag,
			const pt::StopPoint& stop,
			bool showCoords
		) const {
			boost::shared_ptr<Point> gp;
			if(stop.getGeometry().get() && !stop.getGeometry()->isEmpty())
			{
				gp = _coordinatesSystem->convertPoint(
					*stop.getGeometry()
				);
			}
			else if(stop.getConnectionPlace()->getPoint().get() && !stop.getConnectionPlace()->getPoint()->isEmpty())
			{
				gp = _coordinatesSystem->convertPoint(
					*stop.getConnectionPlace()->getPoint()
				);
			}

			if(showCoords && gp.get())
			{
				boost::shared_ptr<Point> pt(stop.getGeometry().get() ? stop.getGeometry() : stop.getConnectionPlace()->getPoint());

				stream <<
					"<" << tag <<
					" latitude=\"" << gp->getY() << "\"" <<
					" longitude=\"" << gp->getX() << "\"" <<
					" id=\"" << stop.getKey() << "\"" <<
					" x=\"" << static_cast<int>(pt->getX()) << "\"" <<
					" y=\"" << static_cast<int>(pt->getY()) << "\"" <<
					" name=\"" << stop.getName() << "\"" <<
					">";
			}
			else//Don't show coords
			{
				stream <<
					"<" << tag <<
					" id=\"" << stop.getKey() << "\"" <<
					" name=\"" << stop.getName() << "\"" <<
					">";
			}
			_xmlDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*stop.getHub()),showCoords);
			stream << "</" << tag << ">";
		}



		void RoutePlannerFunction::_xmlDisplayAddress(
					std::ostream& stream,
					const NamedPlace& np,
					bool showCoords
		) const {
			boost::shared_ptr<Point> gp;

			if(	np.getPoint().get() &&
				!np.getPoint()->isEmpty()
			){
				gp = _coordinatesSystem->convertPoint(
					*np.getPoint()
				);
			}

			if(showCoords && gp.get())
			{
				stream <<
					"<address" <<
					" latitude=\"" << gp->getY() << "\"" <<
					" longitude=\"" << gp->getX() << "\"" <<
					" id=\"" << np.getKey() << "\"" <<
					" x=\"" << static_cast<int>(np.getPoint()->getX()) << "\"" <<
					" y=\"" << static_cast<int>(np.getPoint()->getY()) << "\"" <<
					" city=\"" << np.getCity()->getName() << "\"" <<
					" streetName=\"" << np.getName() << "\"" <<
					" />";
			}
			else
			{
				stream <<
					"<address" <<
					" id=\"" << np.getKey() << "\"" <<
					" city=\"" << np.getCity()->getName() << "\"" <<
					" streetName=\"" << np.getName() << "\"" <<
					" />";
			}
		}



		void RoutePlannerFunction::_xmlDisplayAddress(
			std::ostream& stream,
			const road::Crossing& address,
			const road::RoadPlace& roadPlace,
			bool showCoords
		) const {
			boost::shared_ptr<Point> gp;

			if(	address.getGeometry().get() &&
				!address.getGeometry()->isEmpty()
			){
				gp = _coordinatesSystem->convertPoint(
					*address.getGeometry()
				);
			}

			if(showCoords && gp.get())
			{
				stream <<
					"<address" <<
					" latitude=\"" << gp->getY() << "\"" <<
					" longitude=\"" << gp->getX() << "\"" <<
					" id=\"" << address.getKey() << "\"" <<
					" x=\"" << static_cast<int>(address.getGeometry()->getX()) << "\"" <<
					" y=\"" << static_cast<int>(address.getGeometry()->getY()) << "\"" <<
					" city=\"" << roadPlace.getCity()->getName() << "\"" <<
					" streetName=\"" << roadPlace.getName() << "\"" <<
					" />";
			}
			else
			{
				stream <<
					"<address" <<
					" id=\"" << address.getKey() << "\"" <<
					" city=\"" << roadPlace.getCity()->getName() << "\"" <<
					" streetName=\"" << roadPlace.getName() << "\"" <<
					" />";
			}
		}



		void RoutePlannerFunction::_xmlDisplayRoadPlace(
			std::ostream& stream,
			const road::RoadPlace& roadPlace,
			bool showCoords
		) const {
			boost::shared_ptr<Point> gp;

			if(	roadPlace.getPoint().get() &&
				!roadPlace.getPoint()->isEmpty()
			){
				gp = _coordinatesSystem->convertPoint(
					*roadPlace.getPoint()
				);
			}

			if(showCoords && gp.get())
			{
				stream <<
					"<address" <<
					" latitude=\"" << gp->getY() << "\"" <<
					" longitude=\"" << gp->getX() << "\"" <<
					" id=\"" << roadPlace.getKey() << "\"" <<
					" x=\"" << static_cast<int>(roadPlace.getPoint()->getX()) << "\"" <<
					" y=\"" << static_cast<int>(roadPlace.getPoint()->getY()) << "\"" <<
					" city=\"" << roadPlace.getCity()->getName() << "\"" <<
					" streetName=\"" << roadPlace.getName() << "\"" <<
					" />";
			}
			else
			{
				stream <<
					"<address" <<
					" id=\"" << roadPlace.getKey() << "\"" <<
					" city=\"" << roadPlace.getCity()->getName() << "\"" <<
					" streetName=\"" << roadPlace.getName() << "\"" <<
					" />";
			}
		}



		void RoutePlannerFunction::display(
			std::ostream& stream,
			const server::Request& request,
			const pt_journey_planner::PTRoutePlannerResult& object,
			const boost::gregorian::date& date,
			size_t periodId,
			const geography::Place* originPlace,
			const geography::Place* destinationPlace,
			const pt_website::HourPeriod* period,
			const graph::AccessParameters& accessParameters,
			util::ParametersMap& messagesOnBroadCastPoint
		) const	{
			ParametersMap pm(getTemplateParameters());

			const City* originCity(dynamic_cast<const City*>(originPlace));
			string originPlaceName;
			if (originCity == NULL)
			{
				originCity = dynamic_cast<const NamedPlace*>(originPlace)->getCity();
				originPlaceName = dynamic_cast<const NamedPlace*>(originPlace)->getName();
			}
			const City* destinationCity(dynamic_cast<const City*>(destinationPlace));
			string destinationPlaceName;
			if (destinationCity == NULL)
			{
				destinationCity = dynamic_cast<const NamedPlace*>(destinationPlace)->getCity();
				destinationPlaceName = dynamic_cast<const NamedPlace*>(destinationPlace)->getName();
			}

			pm.insert(DATA_INTERNAL_DATE, to_iso_extended_string(date));
			pm.insert(DATA_ORIGIN_CITY_TEXT, originCity->getName());
			pm.insert(DATA_HANDICAPPED_FILTER, accessParameters.getUserClass() == USER_HANDICAPPED);
			pm.insert(DATA_ORIGIN_PLACE_TEXT, originPlaceName);
			if(originPlace->getPoint())
			{
				boost::shared_ptr<Point> originPoint(_coordinatesSystem->convertPoint(*(originPlace->getPoint())));
				pm.insert(DATA_ORIGIN_PLACE_LONGITUDE, originPoint->getX());
				pm.insert(DATA_ORIGIN_PLACE_LATITUDE, originPoint->getY());
			}
			pm.insert(DATA_BIKE_FILTER, accessParameters.getUserClass() == USER_BIKE);
			pm.insert(DATA_DESTINATION_CITY_TEXT, destinationCity->getName());
			//pm.insert("" /*lexical_cast<string>(destinationPlace->getKey())*/);
			pm.insert(DATA_DESTINATION_PLACE_TEXT, destinationPlaceName);
			if(destinationPlace->getPoint())
			{
				boost::shared_ptr<Point> destinationPoint(_coordinatesSystem->convertPoint(*(destinationPlace->getPoint())));
				pm.insert(DATA_DESTINATION_PLACE_LONGITUDE, destinationPoint->getX());
				pm.insert(DATA_DESTINATION_PLACE_LATITUDE, destinationPoint->getY());
			}
			pm.insert(DATA_FILTERED_JOURNEYS, object.getFiltered());

			// Text formatted date
			if(_dateTimePage.get())
			{
				stringstream sDate;
				DateTimeInterfacePage::Display(sDate, _dateTimePage, request, date);
				pm.insert(DATA_DATE, sDate.str());
			}

			if(period)
			{
				pm.insert(DATA_PERIOD_ID, periodId);
				pm.insert(DATA_PERIOD, period->getCaption());
			}
			pm.insert(DATA_SOLUTIONS_NUMBER, object.getJourneys().size());
			pm.insert(DATA_ACCESSIBILITY_CODE, static_cast<int>(accessParameters.getUserClass()));
			pm.insert(DATA_SITE_ID, _page->getRoot()->getKey());
			pm.insert(DATA_IS_SAME_PLACES, object.getSamePlaces());
			if(request.getUser().get())
			{
				pm.insert(DATA_USER_FULL_NAME, request.getUser()->getFullName());
				pm.insert(DATA_USER_PHONE, request.getUser()->getPhone());
				pm.insert(DATA_USER_ID, request.getUser()->getKey());
			}



			// Schedule rows
			if(_schedulesRowPage.get() && _schedulesCellPage.get())
			{
				stringstream rows;
				const PTRoutePlannerResult::PlacesListConfiguration::List& placesList(
					object.getOrderedPlaces().getResult()
				);
				typedef vector<boost::shared_ptr<ostringstream> > PlacesContentVector;
				PlacesContentVector sheetRows(placesList.size());
				BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
				{
					stream.reset(new ostringstream);
				}

				// Cells

				// Loop on each journey
				int i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(object.getJourneys().begin());
					it != object.getJourneys().end();
					++it, ++i
				){
					bool pedestrianMode = false;
					bool lastPedestrianMode = false;

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itPlaces(placesList.begin());

					// Loop on each leg
					const Journey::ServiceUses& jl(it->getServiceUses());
					for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
					{
						const ServicePointer& leg(*itl);

						if(	PTRoutePlannerResult::HaveToDisplayDepartureStopOnGrid(itl, jl, false)
						){
							const NamedPlace* placeToSearch(
								PTRoutePlannerResult::GetNamedPlaceForDeparture(
									leg.getService()->getPath()->isPedestrianMode(),
									itl == jl.begin() ? NULL : &(*(itl-1)),
									leg,
									*placesList.begin()->place
							)	);

							ptime lastDateTime(leg.getDepartureDateTime());
							lastDateTime += it->getContinuousServiceRange();

							_displayEmptyCells(request, placesList, itSheetRow, itPlaces, *placeToSearch, i, pedestrianMode);

							pedestrianMode = leg.getService()->getPath()->isPedestrianMode();

							// Saving of the columns on each lines
							if (itPlaces != placesList.end())
							{
								_displayScheduleCell(
									**itSheetRow,
									request,
									i,
									pedestrianMode,
									leg.getDepartureDateTime().time_of_day(),
									lastDateTime.time_of_day(),
									it->getContinuousServiceRange().total_seconds() > 0,
									itPlaces->isOrigin && itl == jl.begin(),
									true,
									pedestrianMode && !lastPedestrianMode,
									itPlaces->isOrigin,
									itPlaces->isDestination,
									leg.getService()->getServiceNumber()
								);
								++itPlaces; ++itSheetRow;
							}
							lastPedestrianMode = pedestrianMode;
						}

						if(	PTRoutePlannerResult::HaveToDisplayArrivalStopOnGrid(itl, jl)
						){
							const NamedPlace* placeToSearch(
								PTRoutePlannerResult::GetNamedPlaceForArrival(
									leg.getService()->getPath()->isPedestrianMode(),
									leg,
									itl+1 == jl.end() ? NULL : &(*(itl+1)),
									*placesList.rbegin()->place
							)	);

							_displayEmptyCells(request, placesList, itSheetRow, itPlaces, *placeToSearch, i, pedestrianMode);

							ptime lastDateTime(leg.getArrivalDateTime());
							lastDateTime += it->getContinuousServiceRange();

							if (itPlaces != placesList.end())
							{
								_displayScheduleCell(
									**itSheetRow,
									request,
									i,
									pedestrianMode,
									leg.getArrivalDateTime().time_of_day(),
									lastDateTime.time_of_day(),
									it->getContinuousServiceRange().total_seconds() > 0,
									true,
									itPlaces->isDestination && itl+1 == jl.end(),
									false,
									itPlaces->isOrigin,
									itPlaces->isDestination,
									leg.getService()->getServiceNumber()
								);
							}
						}
					}

					// Fill in the last cells
					if (itPlaces != placesList.end())
					{
						for (++itPlaces, ++itSheetRow; itPlaces != placesList.end(); ++itPlaces, ++itSheetRow)
						{
							_displayScheduleCell(
								**itSheetRow,
								request,
								i,
								false,
								time_duration(not_a_date_time),
								time_duration(not_a_date_time),
								false,
								true,
								true,
								false,
								itPlaces->isOrigin,
								itPlaces->isDestination
							);
						}
					}
				}

				// Initialization of text lines
				bool color(false);
				PlacesContentVector::const_iterator it(sheetRows.begin());
				BOOST_FOREACH(const PTRoutePlannerResult::PlacesListConfiguration::List::value_type& pi, placesList)
				{
					_displayRow(
						rows,
						request,
						*pi.place,
						(*it)->str(),
						color,
						pi.isOrigin,
						pi.isDestination
					);
					color = !color;
					++it;
				}

				pm.insert(DATA_SCHEDULES, rows.str());
			}

			// Lines row
			if(_linesRowPage.get())
			{
				stringstream linesRow;
				size_t n(1);
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					_displayLinesCell(
						linesRow,
						request,
						n,
						journey
					);
					++n;
				}
				pm.insert(DATA_LINES, linesRow.str());
			}


			// Boards
			if(_boardPage.get())
			{
				stringstream boards;

				logic::tribool hFilter(
					false
					//Conversion::ToTribool(_handicappedFilter->getValue(parameters, variables, object, request))
				);
				logic::tribool bFilter(
					false
					//Conversion::ToTribool(_bikeFilter->getValue(parameters, variables, object, request))
				);

				size_t i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(object.getJourneys().begin());
					it != object.getJourneys().end();
					++it, ++i
				){
					_displayJourney(
						boards,
						_boardPage,
						_stopCellPage,
						_serviceCellPage,
						_roadPage,
						_junctionPage,
						request,
						i,
						*it,
						*object.getDeparturePlace(),
						*object.getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 == object.getJourneys().end(),
						messagesOnBroadCastPoint
					);
				}

				pm.insert(DATA_BOARDS, boards.str());
			}

			// Warnings row
			if(_warningPage.get())
			{
				stringstream warnings;

				size_t n(1);
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					_displayWarningCell(
						warnings,
						request,
						n,
						journey
					);
					++n;
				}

				pm.insert(DATA_WARNINGS, warnings.str());
			}

			// Route Planner fisrt arrival is the arrival of the first service
			if(!object.getJourneys().empty() && !object.getJourneys().begin()->getServiceUses().empty())
			{
				ptime firstDateArrivalTime = (object.getJourneys().begin()->getServiceUses().end() - 1)->getArrivalDateTime();
				displayFullDate(
					DATA_FIRST_ARRIVAL_TIME,
					firstDateArrivalTime,
					pm
				);
			}

			// Route Planner last departure is the departure of the last service
			if(!object.getJourneys().empty() && !(object.getJourneys().end() - 1)->getServiceUses().empty())
			{
				ptime lastDateDepartureTime = (object.getJourneys().end() - 1)->getServiceUses().begin()->getDepartureDateTime();
				displayFullDate(
					DATA_LAST_DEPARTURE_TIME,
					lastDateDepartureTime + (object.getJourneys().end() - 1)->getContinuousServiceRange(), // If last service is continious, add range
					pm
				);
			}

			// Result row
			if(_resultRowPage.get())
			{
				stringstream resultRows;

				size_t n(1);
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					// Loop on each leg
					const Journey::ServiceUses& jl(journey.getServiceUses());

					_displayResultRow(
						resultRows,
						request,
						n,
						journey,
						(jl.end() - 1)->getArrivalDateTime(),
						jl.begin()->getDepartureDateTime(),
						journey.getContinuousServiceRange()
					);
					++n;
				}

				pm.insert(DATA_RESULT_ROWS, resultRows.str());
			}

			// Warning levels
			PTRoutePlannerResult::MaxAlarmLevels alarmLevels(object.getMaxAlarmLevels());
			pm.insert(DATA_MAX_WARNING_LEVEL_ON_LINE, alarmLevels.lineLevel);
			pm.insert(DATA_MAX_WARNING_LEVEL_ON_STOP, alarmLevels.stopLevel);

			// Durations row
			if(_durationPage.get())
			{
				stringstream durations;

				size_t n(1);
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					ParametersMap pm(request.getFunction()->getTemplateParameters());
					DateTimeInterfacePage::fillParametersMap(
						pm,
						journey.getDuration()
					);
					pm.insert(DATA_COLUMN_NUMBER, n);
					++n;
					_durationPage->display(durations, request, pm);
				}
				pm.insert(DATA_DURATIONS, durations.str());
			}


			// Reservations row
			stringstream reservations;
			bool hasReservation(false);
			BOOST_FOREACH(PTRoutePlannerResult::Journeys::value_type journey, object.getJourneys())
			{
				// Display of the reservation row cell
				if(_reservationPage.get())
				{
					ReservationRuleInterfacePage::Display(
						reservations,
						_reservationPage,
						_dateTimePage,
						request,
						getTemplateParameters(),
						journey
					);
				}

				// Register the reservation availability
				hasReservation |= bool(journey.getReservationCompliance(false, _reservationRulesDelayType) != false);
			}
			pm.insert(DATA_RESERVATIONS, reservations.str());
			pm.insert(DATA_HAS_RESERVATION, hasReservation);


			// Maps lines
			if(_mapLinePage.get())
			{
				stringstream mapsLines;

				// Selection of the lines to display
				set<const CommercialLine*> lines;
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					BOOST_FOREACH(const ServicePointer& service, journey.getServiceUses())
					{
						if(dynamic_cast<const JourneyPattern*>(service.getService()->getPath()))
						{
							lines.insert(static_cast<const JourneyPattern*>(service.getService()->getPath())->getCommercialLine());
						}
					}
				}

				// Display of each line
				BOOST_FOREACH(const CommercialLine* line, lines)
				{
					LineMarkerInterfacePage::Display(
						mapsLines,
						_mapLinePage,
						request,
						*line
					);
				}
				pm.insert(DATA_MAPS_LINES, mapsLines.str());
			}


			// Maps
			if(_mapPage.get())
			{
				stringstream maps;

				logic::tribool hFilter(
					false
					//Conversion::ToTribool(_handicappedFilter->getValue(parameters, variables, object, request))
				);
				logic::tribool bFilter(
					false
					//Conversion::ToTribool(_bikeFilter->getValue(parameters, variables, object, request))
				);

				size_t i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(object.getJourneys().begin());
					it != object.getJourneys().end();
					++it, ++i
				){
					_displayJourney(
						maps,
						_mapPage,
						_mapStopCellPage,
						_mapServiceCellPage,
						_mapRoadPage,
						_mapJunctionPage,
						request,
						i,
						*it,
						*object.getDeparturePlace(),
						*object.getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 != object.getJourneys().end(),
						messagesOnBroadCastPoint
					);
				}

				pm.insert(DATA_MAPS, maps.str());
			}

			_page->display(stream, request, pm);
		}



		void RoutePlannerFunction::_displayEmptyCells(
			const server::Request& request,
			const PTRoutePlannerResult::PlacesListConfiguration::List& placesList,
			PlacesContentVector::iterator& itSheetRow,
			PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator& itPlaces,
			const NamedPlace& placeToSearch,
			size_t columnNumber,
			bool displayFoot
		) const {
			for (; itPlaces != placesList.end() && itPlaces->place != &placeToSearch; ++itPlaces, ++itSheetRow)
			{
				_displayScheduleCell(
					**itSheetRow,
					request,
					columnNumber,
					displayFoot,
					time_duration(not_a_date_time),
					time_duration(not_a_date_time),
					false,
					true,
					true,
					false,
					itPlaces->isOrigin,
					itPlaces->isDestination
				);
			}
		}



		void RoutePlannerFunction::_displayRow(
			std::ostream& stream,
			const server::Request& request,
			const geography::NamedPlace& place,
			const std::string& cells,
			bool alternateColor,
			bool isOrigin,
			bool isDestination
		) const	{
			ParametersMap pm(getTemplateParameters());

			pm.insert(Request::PARAMETER_OBJECT_ID, place.getKey());
			pm.insert(DATA_CELLS, cells);
			pm.insert(DATA_IS_DESTINATION_ROW, isDestination);
			pm.insert(DATA_IS_ODD_ROW, alternateColor);
			pm.insert(DATA_IS_ORIGIN_ROW, isOrigin);
			pm.insert(DATA_PLACE_NAME, place.getFullName());

			_schedulesRowPage->display(stream, request, pm);
		}



		void RoutePlannerFunction::_displayScheduleCell(
			std::ostream& stream,
			const server::Request& request,
			size_t columnNumber,
			bool isItFootLine,
			const boost::posix_time::time_duration& firstTime,
			const boost::posix_time::time_duration& lastTime,
			bool isItContinuousService,
			bool isFirstWriting,
			bool isLastWriting,
			bool isFirstFoot,
			bool isOriginRow,
			bool isDestinationRow,
			string serviceNumber
		) const {
			ParametersMap pm(getTemplateParameters());

			pm.insert(DATA_IS_DESTINATION_ROW, isDestinationRow);
			pm.insert(DATA_IS_ORIGIN_ROW, isOriginRow);
			pm.insert(DATA_COLUMN_NUMBER, columnNumber);
			pm.insert(DATA_IS_FOOT, isItFootLine);
			{
				stringstream s;
				if(!firstTime.is_not_a_date_time())
				{
					s << setfill('0') << setw(2) << firstTime.hours() << ":" << setfill('0') << setw(2) << firstTime.minutes();
				}
				pm.insert(DATA_FIRST_TIME, s.str());
			}{
				stringstream s;
				if(!lastTime.is_not_a_date_time())
				{
					s << setfill('0') << setw(2) << lastTime.hours() << ":" << setfill('0') << setw(2) << lastTime.minutes();
				}
				pm.insert(DATA_LAST_TIME, s.str());
			}
			pm.insert(DATA_IS_CONTINUOUS_SERVICE, isItContinuousService);
			pm.insert(DATA_IS_FIRST_WRITING, isFirstWriting);
			pm.insert(DATA_IS_LAST_WRITING, isLastWriting);
			pm.insert(DATA_IS_FIRST_FOOT, isFirstFoot);
			pm.insert(DATA_SERVICE_NUMBER, serviceNumber);

			_schedulesCellPage->display(stream ,request, pm);
		}



		void RoutePlannerFunction::_displayLinesCell(
			std::ostream& stream,
			const server::Request& request,
			std::size_t columnNumber,
			const graph::Journey& journey
		) const	{
			ParametersMap pm(getTemplateParameters());

			pm.insert(DATA_COLUMN_NUMBER, columnNumber);

			// Content
			if(_lineMarkerPage.get())
			{
				// Declarations
				stringstream content;
				size_t number(0);

				// Loop on lines
				BOOST_FOREACH(const ServicePointer& leg, journey.getServiceUses())
				{
					if(	dynamic_cast<const JourneyPattern*>(leg.getService()->getPath())
					){
						++number;
						LineMarkerInterfacePage::Display(
							content,
							_lineMarkerPage,
							request,
							*static_cast<const JourneyPattern*>(leg.getService()->getPath ())->getCommercialLine()
						);
					}
				}

				// Output
				pm.insert(DATA_CONTENT, content.str());
				pm.insert("number", number);
			}

			_linesRowPage->display(stream ,request, pm);
		}


		void RoutePlannerFunction::_displayResultRow(
			std::ostream& stream,
			const Request& request,
			std::size_t rowNumber,
			const Journey& journey,
			const ptime& firstArrivalTime,
			const ptime& firstDepartureTime,
			const time_duration& rangeDuration
		) const {

			// Precondition check
			assert(_resultRowPage.get());

			// Declarations
			ParametersMap pm(getTemplateParameters());

			// Display Lines used
			int junctionsNumber = -1; // junctionsNumber is legNumber -1
			if(_lineMarkerPage.get())
			{
				stringstream lineMarkers;

				BOOST_FOREACH(const ServicePointer& leg, journey.getServiceUses())
				{
					if(dynamic_cast<const JourneyPattern*>(leg.getService()->getPath())
					){
						ParametersMap pmLine;
						RollingStock* rs = static_cast<const JourneyPattern*>(leg.getService()->getPath())->getRollingStock();
						pmLine.insert(DATA_ROLLINGSTOCK_ID, (rs ? rs->getKey() : RegistryKeyType(0)));

						LineMarkerInterfacePage::Display(
							lineMarkers,
							_lineMarkerPage,
							request,
							*static_cast<const JourneyPattern*>(leg.getService()->getPath ())->getCommercialLine(),
							pmLine
						);
						junctionsNumber++;
					}
				}
				pm.insert(DATA_LINE_MARKERS, lineMarkers.str());
			}

			const ContinuousService* continuousService(dynamic_cast<const ContinuousService*>((*journey.getServiceUses().begin()).getService()));
			if(continuousService)
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_WAITING, continuousService->getMaxWaitingTime().total_seconds() / 60);
			}

			fillTimeParameters(
				firstArrivalTime,
				firstDepartureTime,
				rangeDuration,
				pm
			);

			pm.insert(DATA_JUNCTIONS_NUMBER,junctionsNumber);
			pm.insert(DATA_IS_CONTINUOUS_SERVICE, rangeDuration.total_seconds() > 0);
			pm.insert(DATA_ROW_NUMBER, rowNumber);

			// Register the reservation availability
			bool hasReservation = bool(journey.getReservationCompliance(false, _reservationRulesDelayType) != false);
			pm.insert(DATA_HAS_RESERVATION, hasReservation);

			// Insert HOURS and MINUTES duration
			DateTimeInterfacePage::fillParametersMap(
						pm,
						journey.getDuration()
			);

			_resultRowPage->display(stream ,request, pm);
		}

		void RoutePlannerFunction::_displayWarningCell(
			std::ostream& stream,
			const server::Request& request,
			std::size_t columnNumber,
			const graph::Journey& journey
		) const	{

			// Precondition check
			assert(_warningPage.get());

			// Declarations
			ParametersMap pm(getTemplateParameters());

			pm.insert(DATA_COLUMN_NUMBER, columnNumber);

			// Content
			if(_warningCheckPage.get())
			{
				// Declarations
				stringstream content;
				typedef map<CommercialLine*, pair<ptime, ptime> > Lines;
				Lines lines;
				typedef map<const StopArea*, pair<ptime, ptime> > Stops;
				Stops stops;

				// Reading of all parts of the journey
				BOOST_FOREACH(const ServicePointer& leg, journey.getServiceUses())
				{
					// Line
					const JourneyPattern* journeyPattern(
						dynamic_cast<const JourneyPattern*>(leg.getService()->getPath())
					);
					if(	journeyPattern
					){
						lines.insert(
							make_pair(
								journeyPattern->getCommercialLine(),
								make_pair(leg.getDepartureDateTime(), leg.getArrivalDateTime())
						)	);

						// Departure stop
						const StopArea* departureStopArea(
							dynamic_cast<const StopArea*>(
								leg.getDepartureEdge()->getFromVertex()->getHub()
						)	);
						if(departureStopArea)
						{
							Stops::iterator it(
								stops.find(departureStopArea)
							);
							if(it == stops.end())
							{
								stops.insert(
									make_pair(
										departureStopArea,
										make_pair(leg.getDepartureDateTime(), leg.getDepartureDateTime())
								)	);
							}
							else
							{
								it->second.second = leg.getDepartureDateTime();
							}
						}

						// Arrival stop
						const StopArea* arrivalStopArea(
							dynamic_cast<const StopArea*>(
								leg.getArrivalEdge()->getFromVertex()->getHub()
						)	);
						if(arrivalStopArea)
						{
							stops.insert(
								make_pair(
									arrivalStopArea,
									make_pair(leg.getArrivalDateTime(), leg.getArrivalDateTime())
							)	);
						}
					}
				}

				// Loop on lines
				BOOST_FOREACH(const Lines::value_type& line, lines)
				{
					ParametersMap pm2(getTemplateParameters());
					pm2.insert(DATA_LINE_ID, line.first->getKey());
					pm2.insert(DATA_START_DATE, line.second.first);
					pm2.insert(DATA_END_DATE, line.second.second);
					_warningCheckPage->display(content, request, pm2);
				}

				// Loop on stops
				BOOST_FOREACH(const Stops::value_type& stop, stops)
				{
					ParametersMap pm2(getTemplateParameters());
					pm2.insert(DATA_STOP_ID, stop.first->getKey());
					pm2.insert(DATA_START_DATE, stop.second.first);
					pm2.insert(DATA_END_DATE, stop.second.second);
					_warningCheckPage->display(content, request, pm2);
				}

				pm.insert(DATA_CONTENT, content.str());
			}

			_warningPage->display(stream ,request, pm);
		}



		void RoutePlannerFunction::_displayJourney(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> stopCellPage,
			boost::shared_ptr<const cms::Webpage> serviceCellPage,
			boost::shared_ptr<const cms::Webpage> roadPage,
			boost::shared_ptr<const cms::Webpage> junctionPage,
			const server::Request& request,
			std::size_t n,
			const graph::Journey& journey,
			const geography::Place& departurePlace,
			const geography::Place& arrivalPlace,
			boost::logic::tribool handicappedFilter,
			boost::logic::tribool bikeFilter,
			bool isTheLast,
			util::ParametersMap messagesOnBroadCastPoint
		) const	{
			ParametersMap pm(getTemplateParameters());

			// Rank
			pm.insert(DATA_RANK, n);
			pm.insert(DATA_IS_THE_LAST_JOURNEY_BOARD, isTheLast);

			// Filters
			pm.insert(DATA_HANDICAPPED_FILTER, handicappedFilter);
			pm.insert(DATA_BIKE_FILTER, bikeFilter);

			// CO2 Emissions, Energy consumption and total distance computation
			double co2Emissions = 0;
			double energyConsumption = 0;
			double totalDistance = 0;
			BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
			{
				const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
				if((line == NULL)||(!line->getRollingStock())) continue;

				double distance = su.getDistance();

				// if the route has no distance data
				if(distance == 0)
				{
					if(su.getGeometry())
					{
						distance = CoordinatesSystem::GetCoordinatesSystem(27572).convertGeometry(
							*su.getGeometry())->getLength();
							distance *= 1.10;
					}
				}
				if(distance > 0)
				{
					co2Emissions += distance * line->getRollingStock()->get<CO2Emissions>() / RollingStock::CO2_EMISSIONS_DISTANCE_UNIT_IN_METERS;
					energyConsumption += distance * line->getRollingStock()->get<EnergyConsumption>() / RollingStock::ENERGY_CONSUMPTION_DISTANCE_UNIT_IN_METERS;
					totalDistance += distance;
				}
			}
			// TODO : set precision outside of RoutePlannerFunction
			stringstream sCO2Emissions, sEnergyConsumption;
			sCO2Emissions << std::fixed << setprecision(2) << co2Emissions;
			sEnergyConsumption << std::fixed << setprecision(2) << energyConsumption;
			cout.unsetf(ios::fixed);
			pm.insert(DATA_CO2_EMISSIONS, sCO2Emissions.str());
			pm.insert(DATA_ENERGY_CONSUMPTION, sEnergyConsumption.str());
			pm.insert(DATA_DISTANCE, totalDistance);

			// Fare calculation
			if((_fareCalculation) && (_ticketCellPage.get()))
			{
				typedef pair<vector<FareTicket>,double> Solution;
				typedef vector<Solution> Solutions;
				Solutions solutions;
				Solution firstSolution;
				firstSolution.second = 0;
				solutions.push_back(firstSolution);

				const ServicePointer* lastService = NULL;

				BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
				{
					const UseRule& useRule = su.getService()->getUseRule(_accessParameters.getUserClassRank());
					if(!dynamic_cast<const PTUseRule*>(&useRule))
					{
						lastService = &su;
						continue;
					}
					const PTUseRule* ptUseRule = dynamic_cast<const PTUseRule*>(&useRule);

					BOOST_FOREACH(Solutions::value_type& solution, solutions)
					{
						// TODO for each Fare of this service
						const Fare* fare = ptUseRule->getDefaultFare();
						if(fare == NULL)
						{
							continue;
						}

						bool ticketAvailable = false;
						// test if an old ticket is available
						BOOST_FOREACH(FareTicket& ticket, solution.first)
						{
							if((ticket.getFare() == NULL) || (ticket.getFare() != fare))
								continue;

							if(ticket.isAvailable(&su,lastService))
							{
								ticketAvailable = true;
								ticket.useService(&su);
							}
						}
						// if there is no ticket available, add new one
						if(!ticketAvailable)
						{
							FareTicket ticket(&su,fare);

							solution.first.push_back(ticket);
							solution.second += ticket.getPrice();
						}
					}
					lastService = &su;
				}

				// Select the best solution according to the total price
				vector<FareTicket> ticketsList;
				double minPrice = std::numeric_limits<int>::max();
				BOOST_FOREACH(Solutions::value_type& solution, solutions)
				{
					if(solution.second < minPrice)
					{
						ticketsList = solution.first;
						minPrice = solution.second;
					}
				}
				stringstream sTickets;
				BOOST_FOREACH(const FareTicket& ticket, ticketsList)
				{
					ParametersMap pmTicket(getTemplateParameters());
					pmTicket.insert(DATA_TICKET_PRICE, ticket.getPrice());
					pmTicket.insert(DATA_TICKET_NAME, ticket.getFare() ? ticket.getFare()->getName() : string());
					pmTicket.insert(DATA_TICKET_CURRENCY, ticket.getFare() ? ticket.getFare()->getCurrency() : string());
					_ticketCellPage->display(sTickets, request, pmTicket);
				}
				pm.insert(DATA_TICKETS, sTickets.str());
				ticketsList.clear();
				solutions.clear();
			}

			// Departure time
			{
				stringstream s;
				s << setw(2) << setfill('0') << journey.getFirstDepartureTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << journey.getFirstDepartureTime().time_of_day().minutes();
				pm.insert(DATA_DEPARTURE_TIME, s.str());
			}
			if(_dateTimePage.get())
			{
				stringstream sDate;
				DateTimeInterfacePage::Display(sDate, _dateTimePage, request, journey.getFirstDepartureTime());
				pm.insert(DATA_DEPARTURE_DATE, sDate.str());
			}
			else
			{
				pm.insert(DATA_DEPARTURE_DATE, journey.getFirstDepartureTime());
			}
			pm.insert(DATA_DEPARTURE_TIME_INTERNAL_FORMAT, to_iso_extended_string(journey.getFirstDepartureTime()));

			if(journey.getContinuousServiceRange().total_seconds())
			{
				BOOST_FOREACH(const Journey::ServiceUses::value_type& service, journey.getServiceUses())
				{
					if(dynamic_cast<const ContinuousService*>(service.getService()))
					{
						pm.insert(DATA_CONTINUOUS_SERVICE_WAITING, static_cast<const ContinuousService*>(service.getService())->getMaxWaitingTime().total_seconds() / 60);
						break;
					}
				}

				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_DATE, to_simple_string(journey.getLastDepartureTime()));
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME, to_simple_string(journey.getLastDepartureTime().time_of_day()));
			}

			// Departure place
			if(dynamic_cast<const Crossing*>(journey.getOrigin()->getHub()))
			{
				if(dynamic_cast<const NamedPlace*>(&departurePlace))
				{
					pm.insert(DATA_DEPARTURE_PLACE_NAME, dynamic_cast<const NamedPlace&>(departurePlace).getFullName());
				}
				else
				{
					pm.insert(DATA_DEPARTURE_PLACE_NAME, dynamic_cast<const City&>(departurePlace).getName());
				}
			}
			else
			{
				pm.insert(DATA_DEPARTURE_PLACE_NAME, dynamic_cast<const NamedPlace&>(*journey.getOrigin()->getHub()).getFullName());
			}

			if(departurePlace.getPoint())
			{
				boost::shared_ptr<Point> departurePoint(
					_coordinatesSystem->convertPoint(
						*departurePlace.getPoint()
				)	);
				pm.insert(DATA_DEPARTURE_PLACE_LONGITUDE, departurePoint->getX());
				pm.insert(DATA_DEPARTURE_PLACE_LATITUDE, departurePoint->getY());
			}

			// Arrival time
			{
				stringstream s;
				s << setw(2) << setfill('0') << journey.getFirstArrivalTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << journey.getFirstArrivalTime().time_of_day().minutes();
				pm.insert(DATA_ARRIVAL_TIME, s.str());
			}
			if(_dateTimePage.get())
			{
				stringstream sDate;
				DateTimeInterfacePage::Display(sDate, _dateTimePage, request, journey.getFirstArrivalTime());
				pm.insert(DATA_ARRIVAL_DATE, sDate.str());
			}
			else
			{
				pm.insert(DATA_ARRIVAL_DATE, journey.getFirstArrivalTime());
			}

			if(journey.getContinuousServiceRange().total_seconds())
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_DATE, to_simple_string(journey.getLastArrivalTime()));
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME, to_simple_string(journey.getLastArrivalTime().time_of_day()));
			}

			// Arrival place
			if(dynamic_cast<const Crossing*>(journey.getDestination()->getHub()))
			{
				if(dynamic_cast<const NamedPlace*>(&arrivalPlace))
				{
					pm.insert(DATA_ARRIVAL_PLACE_NAME, dynamic_cast<const NamedPlace&>(arrivalPlace).getFullName());
				}
				else
				{
					pm.insert(DATA_ARRIVAL_PLACE_NAME, dynamic_cast<const City&>(arrivalPlace).getName());
				}
			}
			else
			{
				pm.insert(DATA_ARRIVAL_PLACE_NAME, dynamic_cast<const NamedPlace&>(*journey.getDestination()->getHub()).getFullName());
			}

			if(arrivalPlace.getPoint())
			{
				boost::shared_ptr<Point> arrivalPoint(
					_coordinatesSystem->convertPoint(
						*arrivalPlace.getPoint()
				)	);
				pm.insert(DATA_ARRIVAL_PLACE_LONGITUDE, arrivalPoint->getX());
				pm.insert(DATA_ARRIVAL_PLACE_LATITUDE, arrivalPoint->getY());
			}
			// Duration
			if(_textDurationPage.get())
			{
				stringstream sDuration;
				DateTimeInterfacePage::Display(sDuration, _textDurationPage, request, journey.getDuration());
				pm.insert(DATA_DURATION, sDuration.str());
			}
			else
			{
				pm.insert(DATA_DURATION, journey.getDuration());
			}

			// Reservation
			ptime now(second_clock::local_time());
			ptime resaDeadLine(journey.getReservationDeadLine(_reservationRulesDelayType));
			logic::tribool resaCompliance(journey.getReservationCompliance(false, _reservationRulesDelayType));
			pm.insert(DATA_RESERVATION_AVAILABLE, resaCompliance && resaDeadLine > now);
			pm.insert(DATA_RESERVATION_COMPULSORY, resaCompliance == true);
			pm.insert(DATA_RESERVATION_DELAY, resaDeadLine.is_not_a_date_time() ? 0 : (resaDeadLine - now).total_seconds() / 60);

			if(!journey.getReservationDeadLine(_reservationRulesDelayType).is_not_a_date_time())
			{
				if(_dateTimePage.get())
				{
					stringstream sResa;
					DateTimeInterfacePage::Display(sResa, _dateTimePage, request, journey.getReservationDeadLine(_reservationRulesDelayType));
					pm.insert(DATA_RESERVATION_DEADLINE, sResa.str());
				}
				else
				{
					pm.insert(DATA_RESERVATION_DEADLINE, journey.getReservationDeadLine(_reservationRulesDelayType));
				}
			}

			// Reservation contact
			set<const ReservationContact*> resaRules;
			BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
			{
				const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
				if(line == NULL) continue;

				if(	line->getCommercialLine()->getReservationContact() &&
					UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su, false, _reservationRulesDelayType))
				){
					resaRules.insert(line->getCommercialLine()->getReservationContact());
				}
			}
			stringstream sPhones;
			bool onlineBooking(!resaRules.empty());
			BOOST_FOREACH(const ReservationContact* rc, resaRules)
			{
				sPhones <<
					rc->get<PhoneExchangeNumber>() <<
					" (" << rc->get<PhoneExchangeOpeningHours>() << ") "
				;
				if (!OnlineReservationRule::GetOnlineReservationRule(rc))
				{
					onlineBooking = false;
				}
			}
			pm.insert(DATA_RESERVATION_PHONE_NUMBER, sPhones.str());
			pm.insert(DATA_ONLINE_RESERVATION, onlineBooking);

			// Content
			if(stopCellPage.get() && serviceCellPage.get() && roadPage.get())
			{
				stringstream content;

				// Loop on lines of the board
				bool __Couleur = false;
				bool isFirstFoot(true);
				vector<Journey::ServiceUses::const_iterator> roadServiceUses;

				const Hub* lastPlace(NULL);

				const Journey::ServiceUses& services(journey.getServiceUses());
				for (Journey::ServiceUses::const_iterator it = services.begin(); it != services.end(); ++it)
				{
					const ServicePointer& leg(*it);

					const RoadPath* road(dynamic_cast<const RoadPath*> (leg.getService()->getPath()));
					const Junction* junction(dynamic_cast<const Junction*> (leg.getService()->getPath()));
					if(	road == NULL &&
						junction == NULL
					){
						isFirstFoot = true;

						// LIGNE ARRET MONTEE Si premier point d'arret et si alerte
						if (leg.getDepartureEdge()->getHub() != lastPlace)
						{
							_displayStopCell(
								content,
								stopCellPage,
								request,
								false,
								false,
								NULL,
								dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()),
								__Couleur,
								leg.getDepartureDateTime(),
								journey.getContinuousServiceRange(),
								false,
								it == services.begin()
							);

							lastPlace = leg.getDepartureEdge()->getHub();
							__Couleur = !__Couleur;
						}

						_displayServiceCell(
							content,
							serviceCellPage,
							request,
							leg,
							journey.getContinuousServiceRange(),
							handicappedFilter,
							bikeFilter,
							__Couleur,
							it+1 == services.end(),
							it == services.begin(),
							messagesOnBroadCastPoint
						);

						__Couleur = !__Couleur;

						_displayStopCell(
							content,
							stopCellPage,
							request,
							true,
							leg.getArrivalEdge()->getHub() == leg.getService()->getPath()->getEdges().back()->getHub(),
							static_cast<const StopPoint*>(leg.getArrivalEdge()->getFromVertex()),
							it+1 != services.end()	? dynamic_cast<const StopPoint*>((it+1)->getDepartureEdge()->getFromVertex()) : NULL,
							__Couleur,
							leg.getArrivalDateTime(),
							journey.getContinuousServiceRange(),
							it+1 == services.end(),
							false
						);

						lastPlace = leg.getArrivalEdge()->getHub();
						__Couleur = !__Couleur;

					}
					else
					{
						roadServiceUses.push_back(it);

						if (road && it + 1 != services.end())
						{
							const ServicePointer& nextLeg(*(it+1));
							const RoadPath* nextRoad(dynamic_cast<const RoadPath*>(nextLeg.getService()->getPath()));

							if(	nextRoad &&
								(	&*nextRoad->getRoad()->get<RoadPlace>() == &*road->getRoad()->get<RoadPlace>() ||
									nextRoad->getRoad()->get<RoadPlace>()->getName() == road->getRoad()->get<RoadPlace>()->getName()
							)	){
								continue;
							}
						}

						// Distance and geometry
						double distance(0);
						vector<Geometry*> geometries;
						vector<boost::shared_ptr<Geometry> > geometriesSPtr;
						BOOST_FOREACH(Journey::ServiceUses::const_iterator itLeg, roadServiceUses)
						{
							distance += itLeg->getDistance();
							boost::shared_ptr<LineString> geometry(itLeg->getGeometry());
							if(geometry.get())
							{
								boost::shared_ptr<Geometry> geometryProjected(
									_coordinatesSystem->convertGeometry(
										*static_cast<Geometry*>(geometry.get())
								)	);
								geometriesSPtr.push_back(geometryProjected);
								geometries.push_back(geometryProjected.get());
							}
						}

						boost::shared_ptr<MultiLineString> multiLineString(
							_coordinatesSystem->getGeometryFactory().createMultiLineString(
								geometries
						)	);

						if(dynamic_cast<const RoadPath*>(leg.getService()->getPath()))
						{
							_displayRoadCell(
								content,
								roadPage,
								request,
								__Couleur,
								distance,
								multiLineString.get(),
								dynamic_cast<const RoadPath*>(leg.getService()->getPath())->getRoad(),
								*(*roadServiceUses.begin())->getDepartureEdge()->getFromVertex(),
								*(*roadServiceUses.rbegin())->getArrivalEdge()->getFromVertex(),
								it+1 == services.end(),
								*(roadServiceUses.begin()) == services.begin(),
								isFirstFoot,
								(*roadServiceUses.begin())->getDepartureDateTime(),
								(*roadServiceUses.rbegin())->getArrivalDateTime(),
								(*roadServiceUses.begin())->getUserClassRank()
							);
						}
						else if (dynamic_cast<const Junction*>(leg.getService()->getPath()))
						{
							if(junctionPage.get())
							{
								string startStopName = (dynamic_cast<const StopPoint&>(*leg.getDepartureEdge()->getFromVertex())).getName();
								string endStopName =  (dynamic_cast<const StopPoint&>(*leg.getArrivalEdge()->getFromVertex())).getName();
								_displayJunctionCell(
									content,
									roadPage,
									request,
									__Couleur,
									distance,
									multiLineString.get(),
									dynamic_cast<const Junction*>(leg.getService()->getPath()),
									*(*roadServiceUses.begin())->getDepartureEdge()->getFromVertex(),
									*(*roadServiceUses.rbegin())->getArrivalEdge()->getFromVertex(),
									it+1 == services.end(),
									*(roadServiceUses.begin()) == services.begin(),
									isFirstFoot,
									(*roadServiceUses.begin())->getDepartureDateTime(),
									(*roadServiceUses.rbegin())->getArrivalDateTime(),
									(*roadServiceUses.begin())->getUserClassRank(),
									startStopName,
									endStopName
								);
							}
						}
						else
						{
							throw Exception("Leg is not a Road neither a Junction");
						}

						roadServiceUses.clear();
						__Couleur = !__Couleur;
						isFirstFoot = false;
					}
				}
				pm.insert(DATA_CONTENT, content.str());
			}

			page->display(stream, request, pm);
		}



		void RoutePlannerFunction::_displayStopCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			bool isItArrival,
			bool isItTerminus,
			const pt::StopPoint* arrivalPhysicalStop,
			const pt::StopPoint* departurePhysicalStop,
			bool color,
			const boost::posix_time::ptime& time,
			boost::posix_time::time_duration continuousServiceRange,
			bool isLastLeg,
			bool isFirstLeg
		) const {
			ParametersMap pm(getTemplateParameters());

			ptime endRangeTime(time);
			if (continuousServiceRange.total_seconds() > 0)
			{
				endRangeTime += continuousServiceRange;
			}

			pm.insert(DATA_IS_ARRIVAL, isItArrival);

			pm.insert(DATA_IS_TERMINUS, isItTerminus);

			// Place
			const NamedPlace& place(
				dynamic_cast<const NamedPlace&>(
					*(departurePhysicalStop ? departurePhysicalStop : arrivalPhysicalStop)->getHub()
			)	);
			pm.insert(DATA_STOP_AREA_NAME, place.getFullName());
			if(	place.getPoint().get() &&
				!place.getPoint()->isEmpty()
			){
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(
						*place.getPoint()
				)	);
				pm.insert(DATA_LONGITUDE, point->getX());
				pm.insert(DATA_LATITUDE, point->getY());
			}
			if(dynamic_cast<const StopArea*>(&place))
			{
				dynamic_cast<const StopArea&>(place).toParametersMap(pm, _coordinatesSystem, "STOP_AREA_");
			}

			// Arrival stop
			if(arrivalPhysicalStop)
			{
				pm.insert(DATA_ARRIVAL_STOP_NAME, arrivalPhysicalStop->getName());
				// Point
				if(	arrivalPhysicalStop->getGeometry().get() &&
					!arrivalPhysicalStop->getGeometry()->isEmpty()
				){
					boost::shared_ptr<Point> point(
						_coordinatesSystem->convertPoint(
							*arrivalPhysicalStop->getGeometry()
					)	);
					pm.insert(DATA_ARRIVAL_LONGITUDE, point->getX());
					pm.insert(DATA_ARRIVAL_LATITUDE, point->getY());
				}
			}

			// Departure stop
			if(departurePhysicalStop)
			{
				pm.insert(DATA_DEPARTURE_STOP_NAME, departurePhysicalStop->getName());
				// Point
				if(	departurePhysicalStop->getGeometry().get() &&
					!departurePhysicalStop->getGeometry()->isEmpty()
				){
					boost::shared_ptr<Point> point(
						_coordinatesSystem->convertPoint(
							*departurePhysicalStop->getGeometry()
					)	);
					pm.insert(DATA_DEPARTURE_LONGITUDE, point->getX());
					pm.insert(DATA_DEPARTURE_LATITUDE, point->getY());
				}
			}

			pm.insert(DATA_ODD_ROW, color);
			{
				stringstream s;
				if(!time.is_not_a_date_time())
				{
					s << setw(2) << setfill('0') << time.time_of_day().hours() << ":" << setw(2) << setfill('0') << time.time_of_day().minutes();
				}
				pm.insert(DATA_FIRST_TIME, s.str()); // 6
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << endRangeTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << endRangeTime.time_of_day().minutes();
				}
				pm.insert(DATA_LAST_TIME, s.str()); // 7
			}

			pm.insert(DATA_IS_LAST_LEG, isLastLeg);
			pm.insert(DATA_IS_FIRST_LEG, isFirstLeg);

			page->display(stream, request, pm);
		}



		void RoutePlannerFunction::_displayRoadCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			bool color,
			double distance,
			const geos::geom::Geometry* geometry,
			const road::Road* road,
			const graph::Vertex& departureVertex,
			const graph::Vertex& arrivalVertex,
			bool isLastLeg,
			bool isFirstLeg,
			bool isFirstFoot,
			const posix_time::ptime departureTime,
			const posix_time::ptime arrivalTime,
			size_t userClassRank
		) const {
			ParametersMap pm(getTemplateParameters());

			// Departure point
			if(isFirstLeg)
			{
				Place* place = _departure_place.placeResult.value.get();
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(*(place->getPoint()))
				);

				pm.insert(DATA_DEPARTURE_LONGITUDE, point->getX());
				pm.insert(DATA_DEPARTURE_LATITUDE, point->getY());
			}
			else if(departureVertex.getGeometry().get() &&
					!departureVertex.getGeometry()->isEmpty()
			){
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(
						*departureVertex.getGeometry()
				)	);
				pm.insert(DATA_DEPARTURE_LONGITUDE, point->getX());
				pm.insert(DATA_DEPARTURE_LATITUDE, point->getY());
			}

			// Arrival point
			if(isLastLeg)
			{
				Place* place = _arrival_place.placeResult.value.get();
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(*(place->getPoint()))
				);

				pm.insert(DATA_ARRIVAL_LONGITUDE, point->getX());
				pm.insert(DATA_ARRIVAL_LATITUDE, point->getY());
			}
			else if(arrivalVertex.getGeometry().get() &&
					!arrivalVertex.getGeometry()->isEmpty()
			){
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(
						*arrivalVertex.getGeometry()
				)	);
				pm.insert(DATA_ARRIVAL_LONGITUDE, point->getX());
				pm.insert(DATA_ARRIVAL_LATITUDE, point->getY());
			}

			pm.insert(DATA_REACHED_PLACE_IS_NAMED, dynamic_cast<const NamedPlace*>(arrivalVertex.getHub()) != NULL);

			pm.insert(DATA_ODD_ROW, color);
			if(road && road->get<RoadPlace>())
			{
				pm.insert(DATA_ROAD_NAME, road->get<RoadPlace>()->getName());
			}
			pm.insert(DATA_LENGTH, static_cast<int>(floor(distance)));
			pm.insert(DATA_IS_FIRST_LEG, isFirstLeg);
			pm.insert(DATA_IS_LAST_LEG, isLastLeg);
			pm.insert(DATA_IS_FIRST_FOOT, isFirstFoot);

			pm.insert(DATA_DEPARTURE_TIME, departureTime.time_of_day());
			pm.insert(DATA_ARRIVAL_TIME, arrivalTime.time_of_day());
			pm.insert(DATA_DURATION, arrivalTime - departureTime);
			pm.insert(DATA_USER_CLASS_CODE, USER_CLASS_CODE_OFFSET + userClassRank);

			// WKT
			if(geometry)
			{
				boost::shared_ptr<Geometry> geometryProjected(
					_coordinatesSystem->convertGeometry(
						*geometry
				)	);

				boost::shared_ptr<WKTWriter> wktWriter(new WKTWriter);
				if(geometryProjected.get() && !geometryProjected->isEmpty())
				{
					pm.insert(DATA_WKT, wktWriter->write(geometryProjected.get()));
				}
			}

			page->display(stream, request, pm);
		}

		void RoutePlannerFunction::_displayJunctionCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			bool color,
			double distance,
			const geos::geom::Geometry* geometry,
			const pt::Junction* junction,
			const graph::Vertex& departureVertex,
			const graph::Vertex& arrivalVertex,
			bool isLastLeg,
			bool isFirstLeg,
			bool isFirstFoot,
			const posix_time::ptime departureTime,
			const posix_time::ptime arrivalTime,
			size_t userClassRank,
			const string startStopName,
			const string endStopName
		) const {
			ParametersMap pm(getTemplateParameters());

			// Departure point
			if(	departureVertex.getGeometry().get() &&
				!departureVertex.getGeometry()->isEmpty()
			){
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(
						*departureVertex.getGeometry()
				)	);
				pm.insert(DATA_DEPARTURE_LONGITUDE, point->getX());
				pm.insert(DATA_DEPARTURE_LATITUDE, point->getY());
			}
			// Arrival point
			if(	arrivalVertex.getGeometry().get() &&
				!arrivalVertex.getGeometry()->isEmpty()
			){
				boost::shared_ptr<Point> point(
					_coordinatesSystem->convertPoint(
						*arrivalVertex.getGeometry()
				)	);
				pm.insert(DATA_ARRIVAL_LONGITUDE, point->getX());
				pm.insert(DATA_ARRIVAL_LATITUDE, point->getY());
			}
			
			pm.insert(DATA_ODD_ROW, color);
			pm.insert(DATA_LENGTH, static_cast<int>(floor(distance)));
			pm.insert(DATA_IS_FIRST_FOOT, isFirstFoot);
			
			pm.insert(DATA_DEPARTURE_TIME, departureTime.time_of_day());
			pm.insert(DATA_ARRIVAL_TIME, arrivalTime.time_of_day());
			pm.insert(DATA_DURATION, arrivalTime - departureTime);
			pm.insert(DATA_USER_CLASS_CODE, USER_CLASS_CODE_OFFSET + userClassRank);
			
			pm.insert(DATA_START_STOP_NAME, startStopName);
			pm.insert(DATA_END_STOP_NAME, endStopName);
			
			// WKT
			if(geometry)
			{
				boost::shared_ptr<Geometry> geometryProjected(
					_coordinatesSystem->convertGeometry(
						*geometry
				)	);
				boost::shared_ptr<WKTWriter> wktWriter(new WKTWriter);
				if(geometryProjected.get() && !geometryProjected->isEmpty())
				{
					pm.insert(DATA_WKT, wktWriter->write(geometryProjected.get()));
				}
			}
			
			pm.insert(DATA_IS_FIRST_LEG, isFirstLeg);
			pm.insert(DATA_IS_LAST_LEG, isLastLeg);
			
			page->display(stream, request, pm);
		}

		void RoutePlannerFunction::displayTime(
			const string & datafieldName,
			const ptime & time,
			ParametersMap & pm,
			bool displayBlankField
		) const {
			stringstream s;
			if(!displayBlankField)
				s << setw(2) << setfill('0') << time.time_of_day().hours() << ":" << setw(2) << setfill('0') << time.time_of_day().minutes();
			pm.insert(datafieldName, s.str());
		}

		void RoutePlannerFunction::displayFullDate(
			const string & datafieldName,
			const ptime & time,
			ParametersMap & pm
		) const {
			stringstream s;
			s << to_iso_extended_string(time.date()) << ' ';
			s << setw(2) << setfill('0') << time.time_of_day().hours() << ":" << setw(2) << setfill('0') << time.time_of_day().minutes();
			pm.insert(datafieldName, s.str());
		}


		void RoutePlannerFunction::fillTimeParameters(
			const ptime & firstArrivalDateTime,
			const ptime & firstDepartureDateTime,
			const time_duration & rangeDuration,
			ParametersMap & pm
		) const {
			ptime lastArrivalDateTime = firstArrivalDateTime;
			ptime lastDepartureDateTime = firstDepartureDateTime;
			if(rangeDuration.total_seconds() > 0)
			{
				lastDepartureDateTime += rangeDuration;
				lastArrivalDateTime += rangeDuration;
			}

			displayTime(DATA_FIRST_DEPARTURE_TIME, firstDepartureDateTime, pm);
			displayTime(DATA_LAST_DEPARTURE_TIME, lastDepartureDateTime, pm,(rangeDuration.total_seconds() == 0));
			displayTime(DATA_FIRST_ARRIVAL_TIME, firstArrivalDateTime, pm);
			displayTime(DATA_LAST_ARRIVAL_TIME, lastArrivalDateTime, pm,(rangeDuration.total_seconds() == 0));
		}

		void RoutePlannerFunction::_displayServiceCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const graph::ServicePointer& serviceUse,
			boost::posix_time::time_duration continuousServiceRange,
			boost::logic::tribool handicappedFilterStatus,
			boost::logic::tribool bikeFilterStatus,
			bool color,
			bool isLastLeg,
			bool isFirstLeg,
			util::ParametersMap& messagesOnBroadCastPoint
		) const {
			ParametersMap pm(getTemplateParameters());

			// Continuous service
			ptime firstDepartureDateTime(serviceUse.getDepartureDateTime());
			ptime firstArrivalDateTime(serviceUse.getArrivalDateTime());

			// JourneyPattern extraction
			const JourneyPattern* line(static_cast<const JourneyPattern*>(serviceUse.getService()->getPath()));
			const CommercialLine* commercialLine(line->getCommercialLine());
			const ContinuousService* continuousService(dynamic_cast<const ContinuousService*>(serviceUse.getService()));

			// Build of the parameters vector
			fillTimeParameters(
				firstArrivalDateTime,
				firstDepartureDateTime,
				continuousServiceRange,
				pm
			);

			pm.insert(DATA_DURATION, firstArrivalDateTime - firstDepartureDateTime);

			if(line->getRollingStock())
			{
				pm.insert(DATA_ROLLINGSTOCK_ID, line->getRollingStock()->getKey()); // 4
				pm.insert(DATA_ROLLINGSTOCK_NAME, line->getRollingStock()->getName()); // 5
				pm.insert(DATA_ROLLINGSTOCK_ARTICLE, line->getRollingStock()->get<Article>()); // 6
			}
			pm.insert(DATA_NETWORK, line->getNetwork()->getName());
			string lineDirection(
				line->getDirection().empty() && line->getDirectionObj() ?
				line->getDirectionObj()->get<DisplayedText>() :
				line->getDirection()
			);
			pm.insert(
				DATA_DESTINATION_NAME,
				lineDirection.empty() ? line->getDestination()->getConnectionPlace()->getFullName() : lineDirection
			);
			pm.insert(DATA_HANDICAPPED_FILTER_STATUS, handicappedFilterStatus);
			pm.insert(
				DATA_HANDICAPPED_PLACES_NUMBER,
				serviceUse.getUseRule().getAccessCapacity () ?
					lexical_cast<string>(*serviceUse.getUseRule().getAccessCapacity ()) :
					"9999"
			);
			pm.insert(DATA_BIKE_FILTER_STATUS, bikeFilterStatus);
			pm.insert(
				DATA_BIKE_PLACES_NUMBER,
				serviceUse.getUseRule().getAccessCapacity () ?
					lexical_cast<string>(*serviceUse.getUseRule().getAccessCapacity ()) :
				"9999"
			); // 11
			commercialLine->toParametersMap(pm, false);
			serviceUse.getService()->toParametersMap(pm, false);
			if(continuousService)
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_WAITING, continuousService->getMaxWaitingTime().total_seconds() / 60);
			}

			pm.insert(DATA_ODD_ROW, color);
			pm.insert(DATA_IS_FIRST_LEG, isFirstLeg);
			pm.insert(DATA_IS_LAST_LEG, isLastLeg);

			boost::shared_ptr<LineString> geometry(serviceUse.getGeometry());
			if(geometry.get())
			{
				boost::shared_ptr<Geometry> geometryProjected(
					_coordinatesSystem->convertGeometry(
						*static_cast<Geometry*>(geometry.get())
				)	);

				boost::shared_ptr<WKTWriter> wktWriter(new WKTWriter);
				if(geometryProjected.get() && !geometryProjected->isEmpty())
				{
					pm.insert(DATA_WKT, wktWriter->write(geometryProjected.get()));
				}
			}

			boost::shared_ptr<ParametersMap> pmMessages(new ParametersMap);
			// Messages output
			BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, messagesOnBroadCastPoint.getSubMaps("message"))
			{
				bool displayMessage(false);
				if(pmMessage->hasSubMaps(Alarm::TAG_RECIPIENTS))
				{
					BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmRecipient, pmMessage->getSubMaps(Alarm::TAG_RECIPIENTS))
					{
						if (pmRecipient->hasSubMaps("line"))
						{
							BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pmRecipient->getSubMaps("line"))
							{
								if (pmLine->getValue(Registrable::ATTR_ID) == lexical_cast<string>(line->getCommercialLine()->getKey()))
								{
									displayMessage = true;
									break;
								}
							}
						}
						if (displayMessage)
							break;
					}
				}
				if (displayMessage)
				{
					pmMessages->insert(string("message"), pmMessage);
				}
			}

			pm.insert(string("messages"), pmMessages);

			page->display(stream, request, pm);
		}



		void RoutePlannerFunction::setDeparturePlace(
			const boost::shared_ptr<geography::Place>& value
		){
			road::RoadModule::ExtendedFetchPlaceResult item;
			if(dynamic_cast<const NamedPlace*>(value.get()))
			{
				item.placeResult.value = dynamic_pointer_cast<NamedPlace, Place>(value);
			}
			else
			{
				item.cityResult.value = dynamic_pointer_cast<City, Place>(value);
			}
			_departure_place = item;
		}



		void RoutePlannerFunction::setArrivalPlace(
			const boost::shared_ptr<geography::Place>& value
		){
			road::RoadModule::ExtendedFetchPlaceResult item;
			if(dynamic_cast<const NamedPlace*>(value.get()))
			{
				item.placeResult.value = dynamic_pointer_cast<NamedPlace, Place>(value);
			}
			else
			{
				item.cityResult.value = dynamic_pointer_cast<City, Place>(value);
			}
			_arrival_place = item;
		}
}	}
