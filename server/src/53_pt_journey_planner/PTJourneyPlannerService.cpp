
/** JourneyPlannerService class implementation.
	@file PTJourneyPlannerService.cpp

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

#include "PTJourneyPlannerService.hpp"

#include "RoadPath.hpp"
#include "SentScenario.h"
#include "AccessParameters.h"
#include "AlgorithmLogger.hpp"
#include "Edge.h"
#include "HourPeriod.h"
#include "Session.h"
#include "MimeTypes.hpp"
#include "ObjectNotFoundException.h"
#include "PTServiceConfigTableSync.hpp"
#include "JourneyPattern.hpp"
#include "PlacesListService.hpp"
#include "PTRoutePlannerResult.h"
#include "PTTimeSlotRoutePlanner.h"
#include "Request.h"
#include "RequestException.h"
#include "UserFavoriteJourney.h"
#include "UserFavoriteJourneyTableSync.h"
#include "Road.h"
#include "RoadPlace.h"
#include "Hub.h"
#include "Service.h"
#include "CommercialLine.h"
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
#include "RoadPlaceTableSync.h"
#include "StopAreaTableSync.hpp"

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
	using namespace fare;
	using namespace server;
	using namespace util;
	using namespace vehicle;
	using namespace pt;
	using namespace pt_website;
	using namespace db;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace pt_journey_planner;
	using namespace resa;
	using namespace html;
	using namespace cms;
	using namespace messages;

	template<> const string util::FactorableTemplate<pt_journey_planner::PTJourneyPlannerService::_FunctionWithSite,pt_journey_planner::PTJourneyPlannerService>::FACTORY_KEY("journey_planner");

	namespace pt_journey_planner
	{
		const string PTJourneyPlannerService::PARAMETER_CONFIG_ID = "config_id";
		const string PTJourneyPlannerService::PARAMETER_MAX_SOLUTIONS_NUMBER = "msn";
		const string PTJourneyPlannerService::PARAMETER_MAX_DEPTH = "md";
		const string PTJourneyPlannerService::PARAMETER_APPROACH_SPEED = "apsp";
		const string PTJourneyPlannerService::PARAMETER_MAX_APPROACH_DISTANCE = "mad";
		const string PTJourneyPlannerService::PARAMETER_DAY = "dy";
		const string PTJourneyPlannerService::PARAMETER_PERIOD_ID = "pi";
		const string PTJourneyPlannerService::PARAMETER_ACCESSIBILITY = "ac";
		const string PTJourneyPlannerService::PARAMETER_DEPARTURE_CITY_TEXT = "dct";
		const string PTJourneyPlannerService::PARAMETER_ARRIVAL_CITY_TEXT = "act";
		const string PTJourneyPlannerService::PARAMETER_DEPARTURE_PLACE_TEXT = "dpt";
		const string PTJourneyPlannerService::PARAMETER_ARRIVAL_PLACE_TEXT = "apt";
		const string PTJourneyPlannerService::PARAMETER_DEPARTURE_CLASS_FILTER = "departure_class_filter";
		const string PTJourneyPlannerService::PARAMETER_ARRIVAL_CLASS_FILTER = "arrival_class_filter";
		const string PTJourneyPlannerService::PARAMETER_FAVORITE_ID = "fid";
		const string PTJourneyPlannerService::PARAMETER_LOWEST_DEPARTURE_TIME = "da";
		const string PTJourneyPlannerService::PARAMETER_LOWEST_ARRIVAL_TIME = "ii";
		const string PTJourneyPlannerService::PARAMETER_HIGHEST_DEPARTURE_TIME = "ha";
		const string PTJourneyPlannerService::PARAMETER_HIGHEST_ARRIVAL_TIME = "ia";
		const string PTJourneyPlannerService::PARAMETER_ROLLING_STOCK_FILTER_ID = "tm";
		const string PTJourneyPlannerService::PARAMETER_MIN_MAX_DURATION_RATIO_FILTER = "min_max_duration_ratio_filter";
		const string PTJourneyPlannerService::PARAMETER_MIN_WAITING_TIME_FILTER = "min_waiting_time_filter";
		const string PTJourneyPlannerService::PARAMETER_FARE_CALCULATION = "fc";
		const string PTJourneyPlannerService::PARAMETER_MAX_TRANSFER_DURATION = "max_transfer_duration";
		const string PTJourneyPlannerService::PARAMETER_LOG_PATH = "log_path";
		const string PTJourneyPlannerService::PARAMETER_SRID = "srid";
		const string PTJourneyPlannerService::PARAMETER_DEPARTURE_PLACE_XY = "departure_place_XY";
		const string PTJourneyPlannerService::PARAMETER_ARRIVAL_PLACE_XY = "arrival_place_XY";
		const string PTJourneyPlannerService::PARAMETER_INVERT_XY = "invert_XY";
		const string PTJourneyPlannerService::PARAMETER_CONCATENATE_CONTIGUOUS_FOOT_LEGS = "concatenate_contiguous_foot_legs";
		const string PTJourneyPlannerService::PARAMETER_BROADCAST_POINT_ID = "broadcast_point";
		const string PTJourneyPlannerService::PARAMETER_DEPARTURE_PARKING_TEXT = "departure_parking_text";
		const string PTJourneyPlannerService::PARAMETER_ARRIVAL_PARKING_TEXT = "arrival_parking_text";
		const string PTJourneyPlannerService::PARAMETER_DEPARTURE_PARKING_XY = "departure_parking_xy";
		const string PTJourneyPlannerService::PARAMETER_ARRIVAL_PARKING_XY = "arrival_parking_xy";
		const string PTJourneyPlannerService::PARAMETER_START_WITH_CAR = "start_with_car";
		const string PTJourneyPlannerService::PARAMETER_END_WITH_CAR = "end_with_car";

		const string PTJourneyPlannerService::PARAMETER_OUTPUT_FORMAT = "output_format";
		const string PTJourneyPlannerService::VALUE_ADMIN_HTML = "admin";

		const string PTJourneyPlannerService::PARAMETER_PAGE = "page";

		const string PTJourneyPlannerService::DATA_SOLUTIONS_NUMBER("solutions_number");
		const string PTJourneyPlannerService::ITEM_JOURNEY_PLANNER_RESULT = "journey_planner_result";
		const string PTJourneyPlannerService::ITEM_SCHEDULE = "schedule";
		const string PTJourneyPlannerService::DATA_FILTERED_JOURNEYS = "filtered_journeys";
		const string PTJourneyPlannerService::DATA_MAX_WARNING_LEVEL_ON_STOP = "max_warning_level_on_stop";
		const string PTJourneyPlannerService::DATA_MAX_WARNING_LEVEL_ON_LINE = "max_warning_level_on_line";
		const string PTJourneyPlannerService::DATA_HAS_RESERVATION = "has_reservation";
		const string PTJourneyPlannerService::DATA_SERVICE_NUMBER = "service_number";

		const string PTJourneyPlannerService::DATA_INTERNAL_DATE("internal_date");
		const string PTJourneyPlannerService::DATA_ORIGIN_CITY_TEXT("origin_city_text");
		const string PTJourneyPlannerService::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string PTJourneyPlannerService::DATA_ORIGIN_PLACE_TEXT("origin_place_text");
		const string PTJourneyPlannerService::DATA_BIKE_FILTER("bike_filter");
		const string PTJourneyPlannerService::DATA_DESTINATION_CITY_TEXT("destination_city_text");
		const string PTJourneyPlannerService::DATA_DESTINATION_PLACE_ID("destination_place_id");
		const string PTJourneyPlannerService::DATA_DESTINATION_PLACE_TEXT("destination_place_text");
		const string PTJourneyPlannerService::DATA_PERIOD_ID("period_id");
		const string PTJourneyPlannerService::ARRAY_DATE = "date";
		const string PTJourneyPlannerService::DATA_PERIOD("period");
		const string PTJourneyPlannerService::DATA_ACCESSIBILITY_CODE("accessibility_code");
		const string PTJourneyPlannerService::DATA_SITE_ID("site_id");
		const string PTJourneyPlannerService::DATA_IS_SAME_PLACES("is_same_places");
		const string PTJourneyPlannerService::DATA_USER_FULL_NAME("user_full_name");
		const string PTJourneyPlannerService::DATA_USER_PHONE("user_phone");
		const string PTJourneyPlannerService::DATA_USER_ID("user_id");

		const string PTJourneyPlannerService::ITEM_CELL = "cell";
		const string PTJourneyPlannerService::DATA_IS_ODD_ROW("is_odd_row");
		const string PTJourneyPlannerService::DATA_IS_ORIGIN_ROW("is_origin_row");
		const string PTJourneyPlannerService::DATA_IS_DESTINATION_ROW("is_destination_row");
		const string PTJourneyPlannerService::DATA_PLACE_NAME("place_name");

		const string PTJourneyPlannerService::DATA_LINE_ID = "line_id";
		const string PTJourneyPlannerService::DATA_STOP_ID = "stop_id";
		const string PTJourneyPlannerService::DATA_IS_FIRST_ROW = "is_first_row";
		const string PTJourneyPlannerService::DATA_IS_LAST_ROW = "is_last_row";
		const string PTJourneyPlannerService::DATA_COLUMN_NUMBER("column_number");
		const string PTJourneyPlannerService::DATA_ROW_NUMBER("row_number");
		const string PTJourneyPlannerService::DATA_IS_FOOT("is_foot");
		const string PTJourneyPlannerService::DATA_IS_CAR("is_car");
		const string PTJourneyPlannerService::DATA_FIRST_TIME("first_time");
		const string PTJourneyPlannerService::DATA_LAST_TIME("last_time");
		const string PTJourneyPlannerService::DATA_IS_CONTINUOUS_SERVICE("is_continuous_service");
		const string PTJourneyPlannerService::DATA_IS_FIRST_WRITING("is_first_writing");
		const string PTJourneyPlannerService::DATA_IS_LAST_WRITING("is_last_writing");
		const string PTJourneyPlannerService::DATA_IS_FIRST_FOOT("is_first_foot");
		const string PTJourneyPlannerService::TAG_LINE = "line";

		const string PTJourneyPlannerService::ITEM_LEG = "leg";
		const string PTJourneyPlannerService::PREFIX_ARRIVAL = "arrival_";
		const string PTJourneyPlannerService::PREFIX_DEPARTURE = "departure_";
		const string PTJourneyPlannerService::PREFIX_PLACE = "place_";
		const string PTJourneyPlannerService::DATA_RANK("rank");

		const string PTJourneyPlannerService::DATA_START_DATE = "start_date";
		const string PTJourneyPlannerService::DATA_END_DATE = "end_date";
		const string PTJourneyPlannerService::DATA_IS_THE_LAST_JOURNEY_BOARD("is_the_last_journey_board");
		const string PTJourneyPlannerService::ITEM_BOARD = "board";
		const string PTJourneyPlannerService::DATA_DEPARTURE_TIME("departure_time");
		const string PTJourneyPlannerService::ITEM_DEPARTURE_TIME_OBJ = "departure_time_obj";
		const string PTJourneyPlannerService::DATA_DEPARTURE_TIME_INTERNAL_FORMAT("internal_departure_time");
		const string PTJourneyPlannerService::DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME("continuous_service_last_departure_time");
		const string PTJourneyPlannerService::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string PTJourneyPlannerService::DATA_DEPARTURE_PLACE_LONGITUDE("departure_longitude");
		const string PTJourneyPlannerService::DATA_DEPARTURE_PLACE_LATITUDE("departure_latitude");
		const string PTJourneyPlannerService::DATA_ARRIVAL_TIME("arrival_time");
		const string PTJourneyPlannerService::ITEM_ARRIVAL_TIME_OBJ = "arrival_time_obj";
		const string PTJourneyPlannerService::DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME("continuous_service_last_arrival_time");
		const string PTJourneyPlannerService::DATA_ARRIVAL_PLACE_NAME("arrival_place_name");
		const string PTJourneyPlannerService::DATA_ARRIVAL_PLACE_LONGITUDE("arrival_longitude");
		const string PTJourneyPlannerService::DATA_ARRIVAL_PLACE_LATITUDE("arrival_latitude");
		const string PTJourneyPlannerService::DATA_DURATION("duration");
		const string PTJourneyPlannerService::ITEM_DURATION_OBJ = "duration_obj";
		const string PTJourneyPlannerService::DATA_RESERVATION_AVAILABLE("reservation_available");
		const string PTJourneyPlannerService::DATA_RESERVATION_COMPULSORY("reservation_compulsory");
		const string PTJourneyPlannerService::DATA_RESERVATION_DELAY("reservation_delay");
		const string PTJourneyPlannerService::DATA_RESERVATION_DEADLINE("reservation_deadline");
		const string PTJourneyPlannerService::ITEM_RESERVATION_DEADLINE_OBJ = "reservation_deadline_obj";
		const string PTJourneyPlannerService::DATA_RESERVATION_PHONE_NUMBER("reservation_phone_number");
		const string PTJourneyPlannerService::DATA_ONLINE_RESERVATION("online_reservation");
		const string PTJourneyPlannerService::DATA_CONTINUOUS_SERVICE_WAITING("continuous_service_waiting");
		const string PTJourneyPlannerService::DATA_CO2_EMISSIONS("co2_emissions");
		const string PTJourneyPlannerService::DATA_ENERGY_CONSUMPTION("energy_consumption");
		const string PTJourneyPlannerService::ITEM_TICKET = "ticket";
		const string PTJourneyPlannerService::DATA_TICKET_NAME("ticket_name");
		const string PTJourneyPlannerService::DATA_TICKET_PRICE("ticket_price");
		const string PTJourneyPlannerService::DATA_TICKET_CURRENCY("ticket_currency");
		const string PTJourneyPlannerService::DATA_DISTANCE = "distance";

		// Cells
		const string PTJourneyPlannerService::DATA_ODD_ROW("is_odd_row");

		// Stop cells
		const string PTJourneyPlannerService::DATA_ARRIVAL_IS_TERMINUS = "arrival_is_terminus";
		const string PTJourneyPlannerService::DATA_STOP_AREA_NAME("stop_name");
		const string PTJourneyPlannerService::DATA_LONGITUDE("longitude");
		const string PTJourneyPlannerService::DATA_LATITUDE("latitude");
		const string PTJourneyPlannerService::DATA_DEPARTURE_STOP_NAME("departure_stop_name");
		const string PTJourneyPlannerService::DATA_DEPARTURE_LONGITUDE("departure_longitude");
		const string PTJourneyPlannerService::DATA_DEPARTURE_LATITUDE("departure_latitude");
		const string PTJourneyPlannerService::DATA_ARRIVAL_STOP_NAME("arrival_stop_name");
		const string PTJourneyPlannerService::DATA_ARRIVAL_LONGITUDE("arrival_longitude");
		const string PTJourneyPlannerService::DATA_ARRIVAL_LATITUDE("arrival_latitude");
		const string PTJourneyPlannerService::DATA_IS_LAST_LEG("is_last_leg");
		const string PTJourneyPlannerService::DATA_IS_FIRST_LEG("is_first_leg");
		const string PTJourneyPlannerService::DATA_IS_SAME_THAN_LAST_ARRIVAL_PLACE("is_same_than_last_arrival_place");
		const string PTJourneyPlannerService::DATA_IS_ENTERING_PARKING("is_entering_parking");
		const string PTJourneyPlannerService::DATA_IS_LEAVING_PARKING("is_leaving_parking");

		// Junction cells
		const string PTJourneyPlannerService::DATA_REACHED_PLACE_IS_NAMED("reached_place_is_named");
		const string PTJourneyPlannerService::DATA_ROAD_NAME("road_name");
		const string PTJourneyPlannerService::DATA_LENGTH("length");
		const string PTJourneyPlannerService::DATA_USER_CLASS_RANK("user_class_rank");

		// Service cells
		const string PTJourneyPlannerService::DATA_FIRST_DEPARTURE_TIME("first_departure_time");
		const string PTJourneyPlannerService::DATA_LAST_DEPARTURE_TIME("last_departure_time");
		const string PTJourneyPlannerService::DATA_FIRST_ARRIVAL_TIME("first_arrival_time");
		const string PTJourneyPlannerService::DATA_LAST_ARRIVAL_TIME("last_arrival_time");
		const string PTJourneyPlannerService::DATA_ROLLINGSTOCK_ID("rolling_stock_id");
		const string PTJourneyPlannerService::DATA_ROLLINGSTOCK_NAME("rolling_stock_name");
		const string PTJourneyPlannerService::DATA_ROLLINGSTOCK_ARTICLE("rolling_stock_article");
		const string PTJourneyPlannerService::DATA_DESTINATION_NAME("destination_name");
		const string PTJourneyPlannerService::DATA_HANDICAPPED_FILTER_STATUS("handicapped_filter_status");
		const string PTJourneyPlannerService::DATA_HANDICAPPED_PLACES_NUMBER("handicapped_places_number");
		const string PTJourneyPlannerService::DATA_BIKE_FILTER_STATUS("bike_filter_status");
		const string PTJourneyPlannerService::DATA_BIKE_PLACES_NUMBER("bike_places_number");
		const string PTJourneyPlannerService::DATA_WKT("wkt");
		const string PTJourneyPlannerService::DATA_LINE_MARKERS("line_markers");
		const string PTJourneyPlannerService::DATA_IS_REAL_TIME("realTime");

		// Commercial lines
		const string PTJourneyPlannerService::ITEM_COMMERCIAL_LINE("commercial_line");


		PTJourneyPlannerService::PTJourneyPlannerService(
		):	_startDate(not_a_date_time),
			_endDate(not_a_date_time),
			_startArrivalDate(not_a_date_time),
			_endArrivalDate(not_a_date_time),
			_period(NULL),
			_logger(new AlgorithmLogger()),
			_broadcastPoint(NULL),
			_startWithCar(false),
			_endWithCar(false),
			_page(NULL)
		{}



		ParametersMap PTJourneyPlannerService::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());

			// Configuration
			if(_configuration.get())
			{
				map.insert(PARAMETER_CONFIG_ID, _configuration->getKey());
			}

			// Page
			if(_page)
			{
				map.insert(PARAMETER_PAGE, _page->getKey());
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

			// Log path
			if(_logger && !_logger->getDirectory().empty())
			{
				map.insert(PARAMETER_LOG_PATH, _logger->getDirectory().filename().string());
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

			// Start journey using car
			if(_startWithCar)
			{
				map.insert(PARAMETER_START_WITH_CAR, _startWithCar);
			}

			// End journey using car
			if(_endWithCar)
			{
				map.insert(PARAMETER_END_WITH_CAR, _endWithCar);
			}

			// Departure parking
			if(_departure_parking.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_departure_parking.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PARKING_TEXT,
						dynamic_cast<NamedPlace*>(_departure_parking.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_departure_parking.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_DEPARTURE_PARKING_TEXT,
						dynamic_cast<City*>(_departure_parking.placeResult.value.get())->getName()
					);
				}
			}

			// Arrival parking
			if(_arrival_parking.placeResult.value.get())
			{
				if(dynamic_cast<NamedPlace*>(_arrival_parking.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PARKING_TEXT,
						dynamic_cast<NamedPlace*>(_arrival_parking.placeResult.value.get())->getFullName()
					);
				}
				else if(dynamic_cast<City*>(_arrival_parking.placeResult.value.get()))
				{
					map.insert(
						PARAMETER_ARRIVAL_PARKING_TEXT,
						dynamic_cast<City*>(_arrival_parking.placeResult.value.get())->getName()
					);
				}
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



		void PTJourneyPlannerService::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);

			// Configuration
			RegistryKeyType configurationId(map.getDefault<RegistryKeyType>(PARAMETER_CONFIG_ID, 0));
			if(configurationId) try
			{
				_configuration = PTServiceConfigTableSync::Get(configurationId, *_env);
			}
			catch (ObjectNotFoundException<PTServiceConfig>&)
			{
				throw RequestException("No such configuration id");
			}

			// Road approach detail
			_outputRoadApproachDetail =
				_configuration.get() ?
				_configuration->get<DisplayRoadApproachDetails>() :
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
			if(map.getOptional<int>(PARAMETER_MAX_TRANSFER_DURATION))
			{
				_maxTransferDuration = minutes(map.get<int>(PARAMETER_MAX_TRANSFER_DURATION));
			}

			// Min max duration filter
			if(map.getDefault<double>(PARAMETER_MIN_MAX_DURATION_RATIO_FILTER, 0) > 0)
			{
				_minMaxDurationRatioFilter = map.get<double>(PARAMETER_MIN_MAX_DURATION_RATIO_FILTER);
			}

			// Origin and destination places
			optional<RegistryKeyType> favoriteId(map.getOptional<RegistryKeyType>(PARAMETER_FAVORITE_ID));
			if (favoriteId) // Favorite places
			{
				/// TODO implement it
			}
			else
			{
				// Departure
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
							}

						}
						else
						{
							_departure_place = _configuration.get() ?
								_configuration->extendedFetchPlace(_originCityText, _originPlaceText) :
								RoadModule::ExtendedFetchPlace(_originCityText, _originPlaceText)
							;
						}
					}
				}
				// One field input
				else if(map.isDefined(PARAMETER_DEPARTURE_PLACE_TEXT))
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					placesListService.setClassFilter(map.getDefault<string>(PARAMETER_DEPARTURE_CLASS_FILTER));
					placesListService.setText(map.get<string>(PARAMETER_DEPARTURE_PLACE_TEXT));
					_departure_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}
				// XY input
				else if(map.isDefined(PARAMETER_DEPARTURE_PLACE_XY))
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					placesListService.setCoordinatesXY(map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_XY), map.getDefault<bool>(PARAMETER_INVERT_XY));
					_departure_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}

				// Destination
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
							}
						}
						else
						{
							_arrival_place = _configuration.get() ?
								_configuration->extendedFetchPlace(_destinationCityText, _destinationPlaceText) :
								RoadModule::ExtendedFetchPlace(_destinationCityText, _destinationPlaceText)
							;
						}
					}
				}
				// One field input
				else if(map.isDefined(PARAMETER_ARRIVAL_PLACE_TEXT))
				{
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					// Arrival
					placesListService.setClassFilter(map.getDefault<string>(PARAMETER_ARRIVAL_CLASS_FILTER));
					placesListService.setText(map.get<string>(PARAMETER_ARRIVAL_PLACE_TEXT));
					_arrival_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}
				// XY input
				else if(
					map.isDefined(PARAMETER_ARRIVAL_PLACE_XY)
				){
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					placesListService.setCoordinatesXY(map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_XY), map.getDefault<bool>(PARAMETER_INVERT_XY));
					_arrival_place.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);
				}

				// Mixed mode behaviour : start OR end journey using car
				_startWithCar = map.getDefault<bool>(PARAMETER_START_WITH_CAR, false);
				_endWithCar = map.getDefault<bool>(PARAMETER_END_WITH_CAR, false);

				// Departure parking
				// One field input
				if(map.isDefined(PARAMETER_DEPARTURE_PARKING_TEXT))
				{
					_originParkingText = map.getDefault<string>(PARAMETER_DEPARTURE_PARKING_TEXT);

					if(!_originParkingText.empty())
					{
						PlacesListService placesListService;
						placesListService.setNumber(1);
						placesListService.setCoordinatesSystem(_coordinatesSystem);

						// Departure
						placesListService.setText(_originParkingText);
						_departure_parking.placeResult = placesListService.getPlaceFromBestResult(
							placesListService.runWithoutOutput()
						);

						// The journey is mixed mode and starts with car
						_startWithCar = true;
					}
				}
				// XY input
				else if(
					map.isDefined(PARAMETER_DEPARTURE_PARKING_XY)
				){
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					placesListService.setCoordinatesXY(map.getDefault<string>(PARAMETER_DEPARTURE_PARKING_XY), map.getDefault<bool>(PARAMETER_INVERT_XY));
					_departure_parking.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);

					// The journey is mixed mode and starts with car
					_startWithCar = true;
				}

				// Destination parking
				// One field input
				if(map.isDefined(PARAMETER_ARRIVAL_PARKING_TEXT))
				{
					_destinationParkingText = map.getDefault<string>(PARAMETER_ARRIVAL_PARKING_TEXT);

					if(!_destinationParkingText.empty())
					{
						PlacesListService placesListService;
						placesListService.setNumber(1);
						placesListService.setCoordinatesSystem(_coordinatesSystem);

						// Arrival
						placesListService.setText(_destinationParkingText);
						_arrival_parking.placeResult = placesListService.getPlaceFromBestResult(
							placesListService.runWithoutOutput()
						);

						// The journey is mixed mode and ends with car
						_endWithCar = true;
					}
				}
				// XY input
				else if(
					map.isDefined(PARAMETER_ARRIVAL_PARKING_XY)
				){
					PlacesListService placesListService;
					placesListService.setNumber(1);
					placesListService.setCoordinatesSystem(_coordinatesSystem);

					placesListService.setCoordinatesXY(map.getDefault<string>(PARAMETER_ARRIVAL_PARKING_XY), map.getDefault<bool>(PARAMETER_INVERT_XY));
					_arrival_parking.placeResult = placesListService.getPlaceFromBestResult(
						placesListService.runWithoutOutput()
					);

					// The journey is mixed mode and ends with car
					_endWithCar = true;
				}
			}

			if(_startWithCar && _endWithCar)
			{
				// This configuration is not supported by the algorithm: throw an exception
				throw RequestException("Cannot both start AND end a journey using car");
			}

			// If the start/end of the journey uses the car and the departure/arrival place is empty
			// the route planner may fail so we try to guess the most likely departure/arrival place
			if(_startWithCar)
			{
				if(_originPlaceText.empty() && dynamic_pointer_cast<City, Place>(_departure_place.placeResult.value))
				{
					const City* originCity(_departure_place.cityResult.value.get());
					computeDeparturePlace(originCity);
				}
			}

			if(_endWithCar)
			{
				if(_destinationPlaceText.empty() && dynamic_pointer_cast<City, Place>(_arrival_place.placeResult.value))
				{
					const City* arrivalCity(_arrival_place.cityResult.value.get());
					computeArrivalPlace(arrivalCity);
				}
			}

			// Date parameters
			try
			{
				// 1a : by day and time period
				if(!map.getDefault<string>(PARAMETER_DAY).empty())
				{
					// Site check
					if(!_configuration.get())
					{
						throw RequestException("A configuration id must be defined to use this date specification method.");
					}

					// Day
					_day = from_string(map.get<string>(PARAMETER_DAY));

					// Time period
					_periodId = map.get<size_t>(PARAMETER_PERIOD_ID);
					if (_periodId >= _configuration->get<HourPeriods>().size())
					{
						throw RequestException("Bad value for period id");
					}
					_period = &_configuration->get<HourPeriods>().at(_periodId);
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

			try
			{
				// Rolling stock filter
				if(map.getOptional<RegistryKeyType>(PARAMETER_ROLLING_STOCK_FILTER_ID))
				{
					_rollingStockFilter = Env::GetOfficialEnv().get<RollingStockFilter>(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_FILTER_ID));
				}
			}
			catch(ObjectNotFoundException<RollingStockFilter>&)
			{
			}

			// Max solutions number
			_maxSolutionsNumber = map.getOptional<size_t>(PARAMETER_MAX_SOLUTIONS_NUMBER);

			// Fare Calculation
			_fareCalculation = map.getDefault<bool>(PARAMETER_FARE_CALCULATION,false);

			// Accessibility
			optional<unsigned int> acint(map.getOptional<unsigned int>(PARAMETER_ACCESSIBILITY));
			if(_configuration.get())
			{
				_accessParameters = _configuration->getAccessParameters(
					acint ? static_cast<UserClassCode>(*acint) : USER_PEDESTRIAN,
					_rollingStockFilter.get() ? _rollingStockFilter->getAllowedPathClasses() : AccessParameters::AllowedPathClasses(),
					AccessParameters::AllowedNetworks()
				);
			}
			else
			{
				if(acint && *acint == USER_HANDICAPPED)
				{
					_accessParameters = AccessParameters(
						*acint, false, false, 300, posix_time::minutes(23), 0.556
					);
				}
				else if(acint && *acint == USER_BIKE)
				{
					_accessParameters = AccessParameters(
						*acint, false, false, 3000, posix_time::minutes(23), 4.167
					);
				}
				else
				{
					_accessParameters = AccessParameters(
						USER_PEDESTRIAN, false, false, 1000, posix_time::minutes(23), 1.111
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

			// Max Approach distance
			if(map.getOptional<string>(PARAMETER_MAX_APPROACH_DISTANCE))
			{
				optional<string> strMad = map.getOptional<string>(PARAMETER_MAX_APPROACH_DISTANCE);
				if (strMad)
					split(_vectMad, *strMad, is_any_of(","));
				if (_vectMad.size() == 1)
					_accessParameters.setMaxApproachDistance(*(map.getOptional<double>(PARAMETER_MAX_APPROACH_DISTANCE)));
			}
			
			if(	!_departure_place.placeResult.value || !_arrival_place.placeResult.value
			){
				return;
			}

			// Pages
			if(getSite())
			{
				_page = getSite()->getPageByIdOrSmartURL(map.getDefault<string>(PARAMETER_PAGE));
			}
			else try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE));
				if(id)
				{
					_page = Env::GetOfficialEnv().get<Webpage>(*id).get();
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such main page : "+ e.getMessage());
			}
			// Other output format
			if(!_page)
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
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

			// Param to concatenate contiguous foot legs
			_concatenateContiguousFootLegs = map.getDefault<bool>(PARAMETER_CONCATENATE_CONTIGUOUS_FOOT_LEGS, false);
		}



		util::ParametersMap PTJourneyPlannerService::run(
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
				_configuration->applyPeriod(*_period, startDate, endDate);

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

			// loop on run if multiple Max Approach Distance
			if (_vectMad.size() > 1)
			{
				for (size_t cptMad=0;cptMad<_vectMad.size();cptMad++)
				{
					graph::AccessParameters localAccessParameters(_accessParameters);
					try {
						localAccessParameters.setMaxApproachDistance(lexical_cast<int>(_vectMad[cptMad]));
					}
					catch (bad_lexical_cast&)
					{}
					// Initialization
					PTTimeSlotRoutePlanner r(
						_departure_place.placeResult.value.get(),
						_arrival_place.placeResult.value.get(),
						_departure_parking.placeResult.value.get(),
						_arrival_parking.placeResult.value.get(),
						_startWithCar,
						_endWithCar,
						startDate,
						endDate,
						startArrivalDate,
						endArrivalDate,
						_maxSolutionsNumber,
						localAccessParameters,
						planningOrder,
						false,
						*_logger,
						_maxTransferDuration,
						_minMaxDurationRatioFilter
					);
					_result.reset(new PTRoutePlannerResult(r.run()));
					if (_result->getJourneys().size() > 0)
						break;
				}
			}
			else
			{
				// Initialization
				PTTimeSlotRoutePlanner r(
					_departure_place.placeResult.value.get(),
					_arrival_place.placeResult.value.get(),
					_departure_parking.placeResult.value.get(),
					_arrival_parking.placeResult.value.get(),
					_startWithCar,
					_endWithCar,
					startDate,
					endDate,
					startArrivalDate,
					endArrivalDate,
					_maxSolutionsNumber,
					_accessParameters,
					planningOrder,
					false,
					*_logger,
					_maxTransferDuration,
					_minMaxDurationRatioFilter
				);
				// Computing
				_result.reset(new PTRoutePlannerResult(r.run()));
			}

			// Min waiting time filter
			if(_minWaitingTimeFilter)
			{
				_result->filterOnWaitingTime(*_minWaitingTimeFilter);
			}


			//////////////////////////////////////////////////////////////////////////
			// Display
			ParametersMap pm;

			const Place* originPlace(_departure_place.placeResult.value.get());
			const Place* destinationPlace(_arrival_place.placeResult.value.get());

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

			pm.insert(DATA_INTERNAL_DATE, to_iso_extended_string(startDate.date()));
			pm.insert(DATA_ORIGIN_CITY_TEXT, originCity->getName());
			pm.insert(DATA_HANDICAPPED_FILTER, _accessParameters.getUserClass() == USER_HANDICAPPED);
			pm.insert(DATA_ORIGIN_PLACE_TEXT, originPlaceName);
			pm.insert(DATA_BIKE_FILTER, _accessParameters.getUserClass() == USER_BIKE);
			pm.insert(DATA_DESTINATION_CITY_TEXT, destinationCity->getName());
			//pm.insert("" /*lexical_cast<string>(destinationPlace->getKey())*/);
			pm.insert(DATA_DESTINATION_PLACE_TEXT, destinationPlaceName);
			pm.insert(DATA_PERIOD_ID, _periodId);
			pm.insert(DATA_FILTERED_JOURNEYS, _result->getFiltered());

			// Text formatted date
			boost::shared_ptr<ParametersMap> datePM(new ParametersMap);
			DateTimeInterfacePage::fillParametersMap(*datePM, startDate.date());
			pm.insert(ARRAY_DATE, datePM);

			if(_period)
			{
				pm.insert(DATA_PERIOD, _period->getCaption());
			}
			pm.insert(DATA_SOLUTIONS_NUMBER, _result->getJourneys().size());
			pm.insert(DATA_ACCESSIBILITY_CODE, static_cast<int>(_accessParameters.getUserClass()));
			if(_page)
			{
				pm.insert(DATA_SITE_ID, _page->getRoot()->getKey());
			}
			pm.insert(DATA_IS_SAME_PLACES, _result->getSamePlaces());
			if(request.getUser().get())
			{
				pm.insert(DATA_USER_FULL_NAME, request.getUser()->getFullName());
				pm.insert(DATA_USER_PHONE, request.getUser()->getPhone());
				pm.insert(DATA_USER_ID, request.getUser()->getKey());
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

			// Schedule rows
			stringstream rows;
			const PTRoutePlannerResult::PlacesListConfiguration::List& placesList(
				_result->getOrderedPlaces().getResult()
			);
			PlacesContentVector sheetRows(placesList.size());
			BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
			{
				stream.reset(new ParametersMap);
			}

			// Cells

			// Set of commercial lines used by the journeys
			set<const CommercialLine*> lines;

			// Loop on each journey
			int i=1;
			for(PTRoutePlannerResult::Journeys::const_iterator it(_result->getJourneys().begin());
				it != _result->getJourneys().end();
				++it, ++i
			){
				bool pedestrianMode = false;
				bool lastPedestrianMode = false;
				bool carMode = false;

				PlacesContentVector::iterator itSheetRow(sheetRows.begin());
				PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator itPlaces(placesList.begin());

				// Loop on each leg
				const Journey::ServiceUses& jl(it->getServiceUses());
				for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
				{
					const ServicePointer& leg(*itl);

					if(dynamic_cast<const JourneyPattern*>(leg.getService()->getPath()))
					{
						lines.insert(static_cast<const JourneyPattern*>(leg.getService()->getPath())->getCommercialLine());
					}

					if(	PTRoutePlannerResult::HaveToDisplayDepartureStopOnGrid(itl, jl, false)
					){
						const NamedPlace* placeToSearch(
							PTRoutePlannerResult::GetNamedPlaceForDeparture(
								leg.getService()->getPath()->isPedestrianMode(),
								itl == jl.begin() ? NULL : &(*(itl-1)),
								leg,
								*placesList.begin()->place
						)	);

						bool isParking = false;
						ptime lastDateTime(leg.getDepartureDateTime());
						lastDateTime += it->getContinuousServiceRange();

						_displayEmptyCells(placesList, itSheetRow, itPlaces, *placeToSearch, i, pedestrianMode, carMode, sheetRows.end());

						pedestrianMode = leg.getService()->getPath()->isPedestrianMode();
						carMode = pedestrianMode && (USER_CAR == (leg.getUserClassRank() + USER_CLASS_CODE_OFFSET));

						// this place is represented as a parking if :
						// * it is a stop area equipped with a relay park
						// * the leg pointed to it uses car
						// * the leg is neither the first nor the last of the journey
						bool isIntermediateLeg = (itl != jl.begin()) && (itl+1 != jl.end());
						if(isIntermediateLeg && carMode && (NULL != placeToSearch))
						{
							const StopArea* stopArea = dynamic_cast<const StopArea*>(placeToSearch);
							if((NULL != stopArea) && (true == stopArea->getIsRelayPark()))
							{
								isParking = true;
							}
						}

						if(itSheetRow == sheetRows.end())
						{
							Log::GetInstance().warn("Inconsistent sheet rows on journey planner");
							break;
						}

						// Saving of the columns on each lines
						boost::shared_ptr<ParametersMap> cellPM(new ParametersMap);
						_displayScheduleCell(
							*cellPM,
							i,
							pedestrianMode,
							carMode,
							leg.getDepartureDateTime().time_of_day(),
							lastDateTime.time_of_day(),
							it->getContinuousServiceRange().total_seconds() > 0,
							itPlaces->isOrigin && itl == jl.begin(),
							true,
							pedestrianMode && !lastPedestrianMode,
							itPlaces->isOrigin,
							itPlaces->isDestination,
							/* isEnteringParking */ false,
							/* isLeavingParking */ isParking
						);
						(*itSheetRow)->insert(ITEM_CELL, cellPM);
						++itPlaces; ++itSheetRow;
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

						_displayEmptyCells(placesList, itSheetRow, itPlaces, *placeToSearch, i, pedestrianMode, carMode, sheetRows.end());

						bool isParking = false;
						bool isIntermediateLeg = (itl != jl.begin()) && (itl+1 != jl.end());

						// this place is represented as a parking if :
						// * it is a stop area equipped with a relay park
						// * the leg pointed to it uses car
						// * the leg is neither the first nor the last of the journey
						if(isIntermediateLeg && carMode && (NULL != placeToSearch))
						{
							const StopArea* stopArea = dynamic_cast<const StopArea*>(placeToSearch);
							if((NULL != stopArea) && (true == stopArea->getIsRelayPark()))
							{
								isParking = true;
							}
						}

						ptime lastDateTime(leg.getArrivalDateTime());
						lastDateTime += it->getContinuousServiceRange();

						if(itSheetRow == sheetRows.end())
						{
							Log::GetInstance().warn("Inconsistent sheet rows on journey planner");
							break;
						}

						boost::shared_ptr<ParametersMap> cellPM(new ParametersMap);
						_displayScheduleCell(
							*cellPM,
							i,
							pedestrianMode,
							carMode,
							leg.getArrivalDateTime().time_of_day(),
							lastDateTime.time_of_day(),
							it->getContinuousServiceRange().total_seconds() > 0,
							true,
							itPlaces->isDestination && itl+1 == jl.end(),
							false,
							itPlaces->isOrigin,
							itPlaces->isDestination,
							/* isEnteringParking */ isParking,
							/* isLeavingParking */ false
						);
						(*itSheetRow)->insert(ITEM_CELL, cellPM);
					}
				}

				// Fill in the last cells
				for (++itPlaces, ++itSheetRow; itPlaces != placesList.end(); ++itPlaces, ++itSheetRow)
				{
					if(itSheetRow == sheetRows.end())
					{
						Log::GetInstance().warn("Inconsistent sheet rows on journey planner");
						break;
					}

					boost::shared_ptr<ParametersMap> cellPM(new ParametersMap);
					_displayScheduleCell(
						*cellPM,
						i,
						false,
						false,
						time_duration(not_a_date_time),
						time_duration(not_a_date_time),
						false,
						true,
						true,
						false,
						itPlaces->isOrigin,
						itPlaces->isDestination,
						/* isEnteringParking */ false,
						/* isLeavingParking */ false
					);
					(*itSheetRow)->insert(ITEM_CELL, cellPM);
				}
			}

			// Initialization of text lines
			bool color(false);
			PlacesContentVector::const_iterator it(sheetRows.begin());
			BOOST_FOREACH(const PTRoutePlannerResult::PlacesListConfiguration::List::value_type& pi, placesList)
			{
				boost::shared_ptr<ParametersMap> rowPM(new ParametersMap);
				_displayRow(
					*rowPM,
					*pi.place,
					it,
					color,
					pi.isOrigin,
					pi.isDestination
				);
				color = !color;
				++it;
				pm.insert(ITEM_SCHEDULE, rowPM);
			}

			// Journeys
			logic::tribool hFilter(
				false
				//Conversion::ToTribool(_handicappedFilter->getValue(parameters, variables, object, request))
			);
			logic::tribool bFilter(
				false
				//Conversion::ToTribool(_bikeFilter->getValue(parameters, variables, object, request))
			);

			{
				size_t i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(_result->getJourneys().begin());
					it != _result->getJourneys().end();
					++it, ++i
				){
					boost::shared_ptr<ParametersMap> boardPM(new ParametersMap);
					_displayJourney(
						*boardPM,
						i,
						*it,
						*_result->getDeparturePlace(),
						*_result->getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 == _result->getJourneys().end(),
						messagesOnBroadCastPoint
					);
					pm.insert(ITEM_BOARD, boardPM);
				}
			}

			// Warning levels
			PTRoutePlannerResult::MaxAlarmLevels alarmLevels(_result->getMaxAlarmLevels());
			pm.insert(DATA_MAX_WARNING_LEVEL_ON_LINE, alarmLevels.lineLevel);
			pm.insert(DATA_MAX_WARNING_LEVEL_ON_STOP, alarmLevels.stopLevel);

			// Reservations
			bool hasReservation(false);
			BOOST_FOREACH(PTRoutePlannerResult::Journeys::value_type journey, _result->getJourneys())
			{
				// Register the reservation availability
				hasReservation |= bool(journey.getReservationCompliance(false) != false);
			}
			pm.insert(DATA_HAS_RESERVATION, hasReservation);

			// Display of each line
			boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
			BOOST_FOREACH(const CommercialLine* line, lines)
			{
				line->toParametersMap(*linePM, false);
				pm.insert(ITEM_COMMERCIAL_LINE, linePM);
				linePM.reset(new ParametersMap());
			}

			//////////////////////////////////////////////////////////////////////////
			// Output

			// CMS display
			if(_page)
			{
				pm.merge(getTemplateParameters());
				_page->display(stream, request, pm);
			}
			else if(_outputFormat == VALUE_ADMIN_HTML)
			{
				_result->displayHTMLTable(stream, optional<HTMLForm&>(), string(), false);
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					ITEM_JOURNEY_PLANNER_RESULT,
					""
				);
			}

			return pm;
		}



		bool PTJourneyPlannerService::isAuthorized(
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



		std::string PTJourneyPlannerService::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : _outputFormat;
		}



		void PTJourneyPlannerService::_displayEmptyCells(
			const PTRoutePlannerResult::PlacesListConfiguration::List& placesList,
			PlacesContentVector::iterator& itSheetRow,
			PTRoutePlannerResult::PlacesListConfiguration::List::const_iterator& itPlaces,
			const NamedPlace& placeToSearch,
			size_t columnNumber,
			bool displayFoot,
			bool displayCar,
			PlacesContentVector::iterator itSheetRowEnd
		) const {
			for (; itPlaces != placesList.end() && itPlaces->place != &placeToSearch; ++itPlaces, ++itSheetRow)
			{
				if(itSheetRow == itSheetRowEnd)
				{
					Log::GetInstance().warn("Inconsistent sheet rows on journey planner");
					return;
				}

				boost::shared_ptr<ParametersMap> cellPM(new ParametersMap);
				_displayScheduleCell(
					*cellPM,
					columnNumber,
					displayFoot,
					displayCar,
					time_duration(not_a_date_time),
					time_duration(not_a_date_time),
					false,
					true,
					true,
					false,
					itPlaces->isOrigin,
					itPlaces->isDestination,
					/* isEnteringParking */ false,
					/* isLeavingParking */ false
				);
				(*itSheetRow)->insert(ITEM_CELL, cellPM);
			}
		}



		void PTJourneyPlannerService::_displayRow(
			ParametersMap& pm,
			const geography::NamedPlace& place,
			PlacesContentVector::const_iterator cells,
			bool alternateColor,
			bool isOrigin,
			bool isDestination
		) const	{
			BOOST_FOREACH(const PlacesContentVector::value_type& cell, (*cells)->getSubMaps(ITEM_CELL))
			{
				pm.insert(ITEM_CELL, cell);
			}
			pm.insert(Request::PARAMETER_OBJECT_ID, place.getKey());
			pm.insert(DATA_IS_DESTINATION_ROW, isDestination);
			pm.insert(DATA_IS_ODD_ROW, alternateColor);
			pm.insert(DATA_IS_ORIGIN_ROW, isOrigin);
			pm.insert(DATA_PLACE_NAME, place.getFullName());
		}



		void PTJourneyPlannerService::_displayScheduleCell(
			util::ParametersMap& pm,
			size_t columnNumber,
			bool isItFootLine,
			bool isItCarLine,
			const boost::posix_time::time_duration& firstTime,
			const boost::posix_time::time_duration& lastTime,
			bool isItContinuousService,
			bool isFirstWriting,
			bool isLastWriting,
			bool isFirstFoot,
			bool isOriginRow,
			bool isDestinationRow,
			bool isEnteringParking,
			bool isLeavingParking
		) const {

			pm.insert(DATA_IS_DESTINATION_ROW, isDestinationRow);
			pm.insert(DATA_IS_ORIGIN_ROW, isOriginRow);
			pm.insert(DATA_COLUMN_NUMBER, columnNumber);
			pm.insert(DATA_IS_FOOT, isItFootLine);
			pm.insert(DATA_IS_CAR, isItCarLine);
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
			pm.insert(DATA_IS_ENTERING_PARKING, isEnteringParking);
			pm.insert(DATA_IS_LEAVING_PARKING, isLeavingParking);
		}



		void PTJourneyPlannerService::_displayJourney(
			util::ParametersMap& pm,
			std::size_t n,
			const graph::Journey& journey,
			const geography::Place& departurePlace,
			const geography::Place& arrivalPlace,
			boost::logic::tribool handicappedFilter,
			boost::logic::tribool bikeFilter,
			bool isTheLast,
			util::ParametersMap messagesOnBroadCastPoint
		) const	{
			// Rank
			pm.insert(DATA_RANK, n);
			pm.insert(DATA_IS_THE_LAST_JOURNEY_BOARD, isTheLast);

			// Filters
			pm.insert(DATA_HANDICAPPED_FILTER, handicappedFilter);
			pm.insert(DATA_BIKE_FILTER, bikeFilter);

			// Count the number of PT services used by this journey
			size_t serviceNumber(0);
			BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
			{
				const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
				if(NULL != line)
				{
					serviceNumber++;
				}
			}
			pm.insert(DATA_SERVICE_NUMBER, serviceNumber);

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
			// TODO : set precision outside of PTJourneyPlannerService
			stringstream sCO2Emissions, sEnergyConsumption;
			sCO2Emissions << std::fixed << setprecision(2) << co2Emissions;
			sEnergyConsumption << std::fixed << setprecision(2) << energyConsumption;
			cout.unsetf(ios::fixed);
			pm.insert(DATA_CO2_EMISSIONS, sCO2Emissions.str());
			pm.insert(DATA_ENERGY_CONSUMPTION, sEnergyConsumption.str());
			pm.insert(DATA_DISTANCE, totalDistance);

			// Fare calculation
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

			BOOST_FOREACH(const FareTicket& ticket, ticketsList)
			{
				boost::shared_ptr<ParametersMap> pmTicket(new ParametersMap);
				pmTicket->insert(DATA_TICKET_PRICE, ticket.getPrice());
				pmTicket->insert(DATA_TICKET_NAME, ticket.getFare() ? ticket.getFare()->getName() : string());
				pmTicket->insert(DATA_TICKET_CURRENCY, ticket.getFare() ? ticket.getFare()->get<Currency>() : string());
				pm.insert(ITEM_TICKET, pmTicket);
			}
			ticketsList.clear();
			solutions.clear();

			// Departure time
			{
				stringstream s;
				s << setw(2) << setfill('0') << journey.getFirstDepartureTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << journey.getFirstDepartureTime().time_of_day().minutes();
				pm.insert(DATA_DEPARTURE_TIME, s.str());
			}
			{
				boost::shared_ptr<ParametersMap> datePM(new ParametersMap);
				DateTimeInterfacePage::fillParametersMap(*datePM, journey.getFirstDepartureTime());
				pm.insert(ITEM_DEPARTURE_TIME_OBJ, datePM);
			}
			pm.insert(DATA_DEPARTURE_TIME_INTERNAL_FORMAT, to_iso_extended_string(journey.getFirstDepartureTime()));

			if(journey.getContinuousServiceRange().total_seconds())
			{
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
			{
				boost::shared_ptr<ParametersMap> datePM(new ParametersMap);
				DateTimeInterfacePage::fillParametersMap(*datePM, journey.getFirstArrivalTime());
				pm.insert(ITEM_ARRIVAL_TIME_OBJ, datePM);
			}

			if(journey.getContinuousServiceRange().total_seconds())
			{
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
			{
				boost::shared_ptr<ParametersMap> durationPM(new ParametersMap);
				DateTimeInterfacePage::fillParametersMap(*durationPM, journey.getDuration());
				pm.insert(DATA_DURATION, durationPM);
			}
			pm.insert(DATA_DURATION, journey.getDuration());

			// Reservation
			ptime now(second_clock::local_time());
			ptime resaDeadLine(journey.getReservationDeadLine());
			logic::tribool resaCompliance(journey.getReservationCompliance(false));
			pm.insert(DATA_RESERVATION_AVAILABLE, resaCompliance && resaDeadLine > now);
			pm.insert(DATA_RESERVATION_COMPULSORY, resaCompliance == true);
			pm.insert(DATA_RESERVATION_DELAY, resaDeadLine.is_not_a_date_time() ? 0 : (resaDeadLine - now).total_seconds() / 60);

			if(!journey.getReservationDeadLine().is_not_a_date_time())
			{
				{
					boost::shared_ptr<ParametersMap> datePM(new ParametersMap);
					DateTimeInterfacePage::fillParametersMap(*datePM, journey.getReservationDeadLine());
					pm.insert(ITEM_RESERVATION_DEADLINE_OBJ, datePM);
				}
				pm.insert(DATA_RESERVATION_DEADLINE, journey.getReservationDeadLine());
			}

			// Reservation contact
			set<const ReservationContact*> resaRules;
			BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
			{
				const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
				if(line == NULL) continue;

				if(	line->getCommercialLine()->getReservationContact() &&
					UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su, false))
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

			//////////////////////////////////////////////////////////////////////////
			// Service pointers

			// Loop on lines of the board
			bool __Couleur = false;
			bool isFirstFoot(true);
			vector<Journey::ServiceUses::const_iterator> roadServiceUses;
			vector<Journey::ServiceUses::const_iterator> contiguousFootLegs;
			bool concatenatingFootLegs = false;
			bool moreThanOneLeg = false;
			bool isFirstLeg = true;

			const Hub* lastPlace(NULL);
			const StopPoint* lastStop(NULL);

			// This variable is used to detect road chunks with the same
			// name or anonymous and concatenate them
			string currentRoadName("");

			const Journey::ServiceUses& services(journey.getServiceUses());
			for (Journey::ServiceUses::const_iterator it = services.begin(); it != services.end(); ++it)
			{
				boost::shared_ptr<ParametersMap> legPM(new ParametersMap);
				const ServicePointer& leg(*it);
				bool legWritten = false;

				const RoadPath* road(dynamic_cast<const RoadPath*> (leg.getService()->getPath()));
				const Junction* junction(dynamic_cast<const Junction*> (leg.getService()->getPath()));

				bool isEnteringParking = false;
				bool isLeavingParking  = false;
				graph::UserClassCode userClass = leg.getUserClassRank() + USER_CLASS_CODE_OFFSET;

				// if there is a transition from car to foot or public transportation and the stop area has a relay park
				// then mark the stop as "entering parking"
				if(services.begin() != it)
				{
					const ServicePointer& previousLeg(*(it - 1));
					graph::UserClassCode previousUserClass = previousLeg.getUserClassRank() + USER_CLASS_CODE_OFFSET;

					if((USER_CAR != userClass) && (USER_CAR == previousUserClass))
					{
						const StopArea* departureStopArea = dynamic_cast<const StopArea*>(leg.getDepartureEdge()->getHub());

						if(departureStopArea && departureStopArea->getIsRelayPark())
						{
							isEnteringParking = true;
						}
					}
				}

				// if there is a transition from foot or public transportation to car and the stop area has a relay park
				// then mark the stop as "leaving parking"
				if(services.end() != (it + 1))
				{
					const ServicePointer& nextLeg(*(it + 1));
					graph::UserClassCode nextUserClass = nextLeg.getUserClassRank() + USER_CLASS_CODE_OFFSET;

					if((USER_CAR != userClass) && (USER_CAR == nextUserClass))
					{
						const StopArea* arrivalStopArea = dynamic_cast<const StopArea*>(leg.getArrivalEdge()->getHub());

						if(arrivalStopArea && arrivalStopArea->getIsRelayPark())
						{
							isLeavingParking = true;
						}
					}
				}


				if(	road == NULL &&
					junction == NULL &&
					(!_concatenateContiguousFootLegs || !concatenatingFootLegs)
				){
					isFirstFoot = true;
					moreThanOneLeg = true;

					// Departure stop
					_displayStopCell(
						*legPM,
						false,
						false,
						leg.getDepartureEdge()->getHub() == lastPlace,
						dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()),
						dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()) != lastStop,
						__Couleur,
						leg.getDepartureDateTime(),
						journey.getContinuousServiceRange(),
						isEnteringParking,
						/* isLeavingParking */ false
					);

					lastPlace = leg.getDepartureEdge()->getHub();
					__Couleur = !__Couleur;

					// Service
					_displayServiceCell(
						*legPM,
						leg,
						journey.getContinuousServiceRange(),
						handicappedFilter,
						bikeFilter,
						__Couleur,
						messagesOnBroadCastPoint
					);

					__Couleur = !__Couleur;

					// Arrival stop
					_displayStopCell(
						*legPM,
						true,
						leg.getArrivalEdge()->getHub() == leg.getService()->getPath()->getEdges().back()->getHub(),
						false,
						static_cast<const StopPoint*>(leg.getArrivalEdge()->getFromVertex()),
						false,
						__Couleur,
						leg.getArrivalDateTime(),
						journey.getContinuousServiceRange(),
						/* isEnteringParking */ false,
						isLeavingParking
					);

					lastPlace = leg.getArrivalEdge()->getHub();
					lastStop = dynamic_cast<const StopPoint*>(leg.getArrivalEdge()->getFromVertex());
					__Couleur = !__Couleur;
					legWritten = true;
					isFirstLeg = false;
				}
				else if (road == NULL &&
					(!_concatenateContiguousFootLegs || !concatenatingFootLegs)
				)
				{
					boost::shared_ptr<Geometry> geometryProjected(
						_coordinatesSystem->convertGeometry(
							*static_cast<Geometry*>(it->getGeometry().get())
					)	);
					
					vector<Geometry*> geometries;
					geometries.push_back(geometryProjected.get());
					boost::shared_ptr<MultiLineString> multiLineString(
						_coordinatesSystem->getGeometryFactory().createMultiLineString(
							geometries
					)	);
					
					_displayJunctionCell(
						*legPM,
						__Couleur,
						it->getDistance(),
						multiLineString.get(),
						static_cast<const RoadPath*>(leg.getService()->getPath())->getRoad(),
						*leg.getDepartureEdge()->getFromVertex(),
						*leg.getArrivalEdge()->getFromVertex(),
						isFirstFoot,
						false,
						leg.getUserClassRank()
					);
					
					roadServiceUses.clear();
					__Couleur = !__Couleur;
					isFirstFoot = false;
					legWritten = true;
					isFirstLeg = false;
				}
				else if (road == NULL &&
					junction == NULL &&
					_concatenateContiguousFootLegs &&
					concatenatingFootLegs
				)
				{
					// Concatenate and write the foot leg
					// Distance and geometry
					moreThanOneLeg = true;
					double distance(0);
					vector<Geometry*> geometries;
					vector<boost::shared_ptr<Geometry> > geometriesSPtr;

					BOOST_FOREACH(Journey::ServiceUses::const_iterator itLeg, contiguousFootLegs)
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
					
					_displayJunctionCell(
						*legPM,
						__Couleur,
						distance,
						multiLineString.get(),
						static_cast<const RoadPath*>((*contiguousFootLegs.begin())->getService()->getPath())->getRoad(),
						*(*contiguousFootLegs.begin())->getDepartureEdge()->getFromVertex(),
						*(*contiguousFootLegs.rbegin())->getArrivalEdge()->getFromVertex(),
						isFirstFoot,
						true,
						leg.getUserClassRank()
					);
					
					concatenatingFootLegs = false;
					contiguousFootLegs.clear();
					
					legPM->insert(DATA_IS_LAST_LEG, false);
					legPM->insert(DATA_IS_FIRST_LEG, isFirstLeg);
					
					pm.insert(ITEM_LEG, legPM);
					
					legPM.reset(new ParametersMap);
					
					// Write the service
					isFirstFoot = true;
					
					// Departure stop
					_displayStopCell(
						*legPM,
						false,
						false,
						leg.getDepartureEdge()->getHub() == lastPlace,
						dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()),
						dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()) != lastStop,
						__Couleur,
						leg.getDepartureDateTime(),
						journey.getContinuousServiceRange(),
						isEnteringParking,
						/* isLeavingParking */ false
						);
					
					lastPlace = leg.getDepartureEdge()->getHub();
					__Couleur = !__Couleur;
					
					// Service
					_displayServiceCell(
						*legPM,
						leg,
						journey.getContinuousServiceRange(),
						handicappedFilter,
						bikeFilter,
						__Couleur,
						messagesOnBroadCastPoint
					);
					
					__Couleur = !__Couleur;
					
					// Arrival stop
					_displayStopCell(
						*legPM,
						true,
						leg.getArrivalEdge()->getHub() == leg.getService()->getPath()->getEdges().back()->getHub(),
						false,
						static_cast<const StopPoint*>(leg.getArrivalEdge()->getFromVertex()),
						false,
						__Couleur,
						leg.getArrivalDateTime(),
						journey.getContinuousServiceRange(),
						/* isEnteringParking */ false,
						isLeavingParking
					);
					
					lastPlace = leg.getArrivalEdge()->getHub();
					lastStop = dynamic_cast<const StopPoint*>(leg.getArrivalEdge()->getFromVertex());
					__Couleur = !__Couleur;
					legWritten = true;
					isFirstLeg = false;
				}
				else if (road == NULL &&
					_concatenateContiguousFootLegs &&
					concatenatingFootLegs
				)
				{
					// Concatenate and write the foot leg
					// Distance and geometry
					moreThanOneLeg = true;
					double distance(0);
					vector<Geometry*> geometries;
					vector<boost::shared_ptr<Geometry> > geometriesSPtr;
					BOOST_FOREACH(Journey::ServiceUses::const_iterator itLeg, contiguousFootLegs)
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
					
					_displayJunctionCell(
						*legPM,
						__Couleur,
						distance,
						multiLineString.get(),
						static_cast<const RoadPath*>((*contiguousFootLegs.begin())->getService()->getPath())->getRoad(),
						*(*contiguousFootLegs.begin())->getDepartureEdge()->getFromVertex(),
						*(*contiguousFootLegs.rbegin())->getArrivalEdge()->getFromVertex(),
						isFirstFoot,
						true,
						leg.getUserClassRank()
					);
					
					concatenatingFootLegs = false;
					contiguousFootLegs.clear();
					
					legPM->insert(DATA_IS_LAST_LEG, false);
					legPM->insert(DATA_IS_FIRST_LEG, isFirstLeg);
					
					pm.insert(ITEM_LEG, legPM);
					
					legPM.reset(new ParametersMap);
					
					// Write the service
					isFirstFoot = true;
					
					// Departure stop
					_displayStopCell(
						*legPM,
						false,
						false,
						leg.getDepartureEdge()->getHub() == lastPlace,
						dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()),
						dynamic_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()) != lastStop,
						__Couleur,
						leg.getDepartureDateTime(),
						journey.getContinuousServiceRange(),
						/* isEnteringParking */ false,
						/* isLeavingParking */ false
					);
					
					lastPlace = leg.getDepartureEdge()->getHub();
					__Couleur = !__Couleur;
					
					boost::shared_ptr<Geometry> geometryProjected(
						_coordinatesSystem->convertGeometry(
							*static_cast<Geometry*>(it->getGeometry().get())
					)	);
					
					geometries.clear();
					geometries.push_back(geometryProjected.get());
					
					multiLineString.reset(
						_coordinatesSystem->getGeometryFactory().createMultiLineString(
							geometries
					)	);
					
					_displayJunctionCell(
						*legPM,
						__Couleur,
						it->getDistance(),
						multiLineString.get(),
						static_cast<const RoadPath*>(leg.getService()->getPath())->getRoad(),
						*leg.getDepartureEdge()->getFromVertex(),
						*leg.getArrivalEdge()->getFromVertex(),
						isFirstFoot,
						false,
						leg.getUserClassRank()
					);
					__Couleur = !__Couleur;
					isFirstFoot = false;
					legWritten = true;
					isFirstLeg = false;
				}
				else if (_concatenateContiguousFootLegs)
				{
					contiguousFootLegs.push_back(it);
					concatenatingFootLegs = true;
				}
				else
				{
					roadServiceUses.push_back(it);

					if (road && it + 1 != services.end())
					{
						// concatenate road segments with same name or empty name
						const ServicePointer& nextLeg(*(it+1));
						const RoadPath* nextRoad(dynamic_cast<const RoadPath*>(nextLeg.getService()->getPath()));

						if (currentRoadName.empty())
						{
							currentRoadName = road->getRoadPlace()->getName();
						}

						if (nextRoad)
						{
							string nextRoadName = nextRoad->getRoadPlace()->getName();

							if (
								(nextRoad->getRoadPlace() == road->getRoadPlace()) ||
								(nextRoadName == road->getRoadPlace()->getName()) ||
								(nextRoadName.empty()) ||
								(nextRoadName == currentRoadName)
							)
							{
								continue;
							}
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

					const ServicePointer& firstLeg(*roadServiceUses.front());
					const RoadPath* actualRoadPath(dynamic_cast<const RoadPath*> (firstLeg.getService()->getPath()));

					const Vertex* departureVertex = (*roadServiceUses.begin())->getDepartureEdge()->getFromVertex();
					const Vertex* arrivalVertex = (*roadServiceUses.rbegin())->getArrivalEdge()->getFromVertex();

					_displayJunctionCell(
						*legPM,
						__Couleur,
						distance,
						multiLineString.get(),
						actualRoadPath->getRoad(),
						*departureVertex,
						*arrivalVertex,
						isFirstFoot,
						false,
						leg.getUserClassRank()
					);

					roadServiceUses.clear();
					currentRoadName.clear();
					__Couleur = !__Couleur;
					isFirstFoot = false;
					legWritten = true;
					isFirstLeg = false;
				}
				if (legWritten)
				{
					legPM->insert(DATA_IS_LAST_LEG, it+1 == services.end());
					legPM->insert(DATA_IS_FIRST_LEG, it == services.begin());
					
					pm.insert(ITEM_LEG, legPM);
				}
			}
			if (_concatenateContiguousFootLegs && concatenatingFootLegs)
			{
				boost::shared_ptr<ParametersMap> legPM(new ParametersMap);
				// Write the final foot leg
				// Distance and geometry
				double distance(0);
				vector<Geometry*> geometries;
				vector<boost::shared_ptr<Geometry> > geometriesSPtr;
				size_t userClassRank = USER_CLASS_CODE_OFFSET;

				BOOST_FOREACH(Journey::ServiceUses::const_iterator itLeg, contiguousFootLegs)
				{
					distance += itLeg->getDistance();
					userClassRank = itLeg->getUserClassRank();

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
				
				_displayJunctionCell(
					*legPM,
					__Couleur,
					distance,
					multiLineString.get(),
					static_cast<const RoadPath*>((*contiguousFootLegs.begin())->getService()->getPath())->getRoad(),
					*(*contiguousFootLegs.begin())->getDepartureEdge()->getFromVertex(),
					*(*contiguousFootLegs.rbegin())->getArrivalEdge()->getFromVertex(),
					isFirstFoot,
					true,
					userClassRank
				);
				
				concatenatingFootLegs = false;
				contiguousFootLegs.clear();
				
				legPM->insert(DATA_IS_LAST_LEG, true);
				legPM->insert(DATA_IS_FIRST_LEG, !moreThanOneLeg);
				
				pm.insert(ITEM_LEG, legPM);
			}
		}



		void PTJourneyPlannerService::_displayStopCell(
			util::ParametersMap& pm,
			bool isItArrival,
			bool isItTerminus,
			bool isSameThanLastArrivalPlace,
			const pt::StopPoint* stop,
			bool isSameThanLastArrivalStop,
			bool color,
			const boost::posix_time::ptime& time,
			boost::posix_time::time_duration continuousServiceRange,
			bool isEnteringParking,
			bool isLeavingParking
		) const {

			string prefix(isItArrival ? PREFIX_ARRIVAL : PREFIX_DEPARTURE);

			pm.insert(prefix + DATA_IS_SAME_THAN_LAST_ARRIVAL_PLACE, isSameThanLastArrivalPlace);

			ptime endRangeTime(time);
			if (continuousServiceRange.total_seconds() > 0)
			{
				endRangeTime += continuousServiceRange;
			}

			if(isItArrival)
			{
				pm.insert(DATA_ARRIVAL_IS_TERMINUS, isItTerminus);
			}

			pm.insert(prefix + DATA_IS_ENTERING_PARKING, isEnteringParking);
			pm.insert(prefix + DATA_IS_LEAVING_PARKING, isLeavingParking);

			// Place
			const NamedPlace& place(
				dynamic_cast<const NamedPlace&>(
					*stop->getHub()
			)	);
			if(dynamic_cast<const StopArea*>(&place))
			{
				dynamic_cast<const StopArea&>(place).toParametersMap(pm, _coordinatesSystem, prefix + PREFIX_PLACE);
			}
			else
			{
				pm.insert(prefix + DATA_PLACE_NAME, place.getFullName());
				if(	place.getPoint().get() &&
					!place.getPoint()->isEmpty()
				){
					boost::shared_ptr<Point> point(
						_coordinatesSystem->convertPoint(
							*place.getPoint()
					)	);
					pm.insert(prefix + DATA_LONGITUDE, point->getX());
					pm.insert(prefix + DATA_LATITUDE, point->getY());
				}
			}

			// stop
			if(stop)
			{
				pm.insert(prefix + DATA_ARRIVAL_STOP_NAME, stop->getName());
				// Point
				if(	stop->getGeometry().get() &&
					!stop->getGeometry()->isEmpty()
				){
					boost::shared_ptr<Point> point(
						_coordinatesSystem->convertPoint(
							*stop->getGeometry()
					)	);
					pm.insert(prefix + DATA_ARRIVAL_LONGITUDE, point->getX());
					pm.insert(prefix + DATA_ARRIVAL_LATITUDE, point->getY());
				}
			}

			pm.insert(prefix + DATA_ODD_ROW, color);
			{
				stringstream s;
				if(!time.is_not_a_date_time())
				{
					s << setw(2) << setfill('0') << time.time_of_day().hours() << ":" << setw(2) << setfill('0') << time.time_of_day().minutes();
				}
				pm.insert(prefix + DATA_FIRST_TIME, s.str()); // 6
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << endRangeTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << endRangeTime.time_of_day().minutes();
				}
				pm.insert(prefix + DATA_LAST_TIME, s.str()); // 7
			}
		}



		void PTJourneyPlannerService::_displayJunctionCell(
			util::ParametersMap& pm,
			bool color,
			double distance,
			const geos::geom::Geometry* geometry,
			const road::Road* road,
			const graph::Vertex& departureVertex,
			const graph::Vertex& arrivalVertex,
			bool isFirstFoot,
			bool concatenatedFootLegs,
			std::size_t userClassRank
		) const {
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
			pm.insert(DATA_REACHED_PLACE_IS_NAMED, dynamic_cast<const NamedPlace*>(arrivalVertex.getHub()) != NULL);

			pm.insert(DATA_ODD_ROW, color);
			if(road && road->get<RoadPlace>() && !concatenatedFootLegs)
			{
				pm.insert(DATA_ROAD_NAME, road->get<RoadPlace>()->getName());
			}
			pm.insert(DATA_LENGTH, static_cast<int>(floor(distance)));
			pm.insert(DATA_IS_FIRST_FOOT, isFirstFoot);
			pm.insert(DATA_USER_CLASS_RANK, userClassRank);

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
		}



		void PTJourneyPlannerService::_displayServiceCell(
			util::ParametersMap& pm,
			const graph::ServicePointer& serviceUse,
			boost::posix_time::time_duration continuousServiceRange,
			boost::logic::tribool handicappedFilterStatus,
			boost::logic::tribool bikeFilterStatus,
			bool color,
			util::ParametersMap messagesOnBroadCastPoint
		) const {
			// Continuous service
			ptime lastDepartureDateTime(serviceUse.getDepartureDateTime());
			ptime lastArrivalDateTime(serviceUse.getArrivalDateTime());
			if (continuousServiceRange.total_seconds())
			{
				lastArrivalDateTime += continuousServiceRange;
				lastDepartureDateTime += continuousServiceRange;
			}

			// JourneyPattern extraction
			const JourneyPattern* line(static_cast<const JourneyPattern*>(serviceUse.getService()->getPath()));
			const CommercialLine* commercialLine(line->getCommercialLine());
			const ContinuousService* continuousService(dynamic_cast<const ContinuousService*>(serviceUse.getService()));
			const SchedulesBasedService* schedulesBasedService(dynamic_cast<const SchedulesBasedService*>(serviceUse.getService()));

			// Build of the parameters vector
			{
				stringstream s;
				s << setw(2) << setfill('0') << serviceUse.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << serviceUse.getDepartureDateTime().time_of_day().minutes();
				pm.insert(DATA_FIRST_DEPARTURE_TIME, s.str()); // 0
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << lastDepartureDateTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << lastDepartureDateTime.time_of_day().minutes();
				}
				pm.insert(DATA_LAST_DEPARTURE_TIME, s.str()); // 1
			}
			{
				stringstream s;
				s << setw(2) << setfill('0') << serviceUse.getArrivalDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << serviceUse.getArrivalDateTime().time_of_day().minutes();
				pm.insert(DATA_FIRST_ARRIVAL_TIME, s.str()); // 2
			}
			{
				stringstream s;
				if(continuousServiceRange.total_seconds() > 0)
				{
					s << setw(2) << setfill('0') << lastArrivalDateTime.time_of_day().hours() << ":" << setw(2) << setfill('0') << lastArrivalDateTime.time_of_day().minutes();
				}
				pm.insert(DATA_LAST_ARRIVAL_TIME, s.str()); // 3
			}
			if(line->getRollingStock())
			{
				pm.insert(DATA_ROLLINGSTOCK_ID, line->getRollingStock()->getKey()); // 4
				pm.insert(DATA_ROLLINGSTOCK_NAME, line->getRollingStock()->getName()); // 5
				pm.insert(DATA_ROLLINGSTOCK_ARTICLE, line->getRollingStock()->get<Article>()); // 6
			}
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

			if (schedulesBasedService)
			{
				pm.insert(DATA_IS_REAL_TIME, schedulesBasedService->hasRealTimeData());
			}

			if(continuousService)
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_WAITING, continuousService->getMaxWaitingTime().total_seconds() / 60);
				pm.insert(DATA_IS_REAL_TIME, continuousService->hasRealTimeData());
			}

			pm.insert(DATA_ODD_ROW, color);

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
						if (pmRecipient->hasSubMaps(TAG_LINE))
						{
							BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pmRecipient->getSubMaps(TAG_LINE))
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
		}



		void PTJourneyPlannerService::setDeparturePlace(
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



		void PTJourneyPlannerService::setArrivalPlace(
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

		void PTJourneyPlannerService::computeDeparturePlace(
			const City* originCity
		){
			RoadPlaceTableSync::SearchResult roadPlaces(
						RoadPlaceTableSync::Search(
							Env::GetOfficialEnv(),
							originCity ? originCity->getKey() : optional<RegistryKeyType>(),
							optional<string>(), /* exactName */
							optional<string>(), /* likeName */
							0, /* first */
							boost::optional<std::size_t>(0), /* number */
							true, /* orderByName */
							true, /* raisingOrder */
							util::UP_LINKS_LOAD_LEVEL,
							boost::tribool(true), /*mainRoad*/
							false /* isDifferentFromExactName*/
							)	);
			if(!roadPlaces.empty())
			{
				RoadPlaceTableSync::SearchResult::const_iterator it = roadPlaces.begin();
				setDeparturePlace(const_pointer_cast<Place, const Place>(*it));
			}
			else
			{
				StopAreaTableSync::SearchResult stopAreas(
							StopAreaTableSync::Search(
								Env::GetOfficialEnv(),
								originCity ? originCity->getKey() : optional<RegistryKeyType>(),
								true,
								optional<string>(),
								optional<string>(),
								optional<string>(),
								true,
								true,
								0,
								1
								)	);
				if(!stopAreas.empty())
				{
					StopAreaTableSync::SearchResult::const_iterator it = stopAreas.begin();
					setDeparturePlace(const_pointer_cast<Place, const Place>(*it));
				}
				else
				{
					// Get the first road of the city which name is not empty
					RoadPlaceTableSync::SearchResult roadPlaces(
								RoadPlaceTableSync::Search(
									Env::GetOfficialEnv(),
									originCity ? originCity->getKey() : optional<RegistryKeyType>(),
									optional<string>(""), /* exactName */
									optional<string>(), /* likeName */
									0, /* first */
									boost::optional<std::size_t>(0), /* number */
									true, /* orderByName */
									true, /* raisingOrder */
									util::UP_LINKS_LOAD_LEVEL,
									boost::tribool(false), /*mainRoad*/
									true /* isDifferentFromExactName*/
									)	);
					if (!roadPlaces.empty())
					{
						RoadPlaceTableSync::SearchResult::const_iterator it = roadPlaces.begin();
						setDeparturePlace(const_pointer_cast<Place, const Place>(*it));
					}
				}
			}
		}


		void PTJourneyPlannerService::computeArrivalPlace(
			const City* destinationCity
		){
			RoadPlaceTableSync::SearchResult roadPlaces(
						RoadPlaceTableSync::Search(
							Env::GetOfficialEnv(),
							destinationCity ? destinationCity->getKey() : optional<RegistryKeyType>(),
							optional<string>(), /* exactName */
							optional<string>(), /* likeName */
							0, /* first */
							boost::optional<std::size_t>(0), /* number */
							true, /* orderByName */
							true, /* raisingOrder */
							util::UP_LINKS_LOAD_LEVEL,
							boost::tribool(true), /*mainRoad*/
							false /* isDifferentFromExactName*/
							)	);
			if(!roadPlaces.empty())
			{
				RoadPlaceTableSync::SearchResult::const_iterator it = roadPlaces.begin();
				setDeparturePlace(const_pointer_cast<Place, const Place>(*it));
			}
			else
			{
				StopAreaTableSync::SearchResult stopAreas(
							StopAreaTableSync::Search(
								Env::GetOfficialEnv(),
								destinationCity ? destinationCity->getKey() : optional<RegistryKeyType>(),
								true,
								optional<string>(),
								optional<string>(),
								optional<string>(),
								true,
								true,
								0,
								1
								)	);
				if(!stopAreas.empty())
				{
					StopAreaTableSync::SearchResult::const_iterator it = stopAreas.begin();
					setArrivalPlace(const_pointer_cast<Place, const Place>(*it));
				}
				// Recuperation de la premiere rue au nom non vide
				else
				{
					RoadPlaceTableSync::SearchResult roadPlaces(
								RoadPlaceTableSync::Search(
									Env::GetOfficialEnv(),
									destinationCity ? destinationCity->getKey() : optional<RegistryKeyType>(),
									optional<string>(""), /* exactName */
									optional<string>(), /* likeName */
									0, /* first */
									boost::optional<std::size_t>(0), /* number */
									true, /* orderByName */
									true, /* raisingOrder */
									util::UP_LINKS_LOAD_LEVEL,
									boost::tribool(false), /*mainRoad*/
									true /* isDifferentFromExactName*/
									)	);
					if (!roadPlaces.empty())
					{
						RoadPlaceTableSync::SearchResult::const_iterator it = roadPlaces.begin();
						setArrivalPlace(const_pointer_cast<Place, const Place>(*it));
					}
				}
			}
		}

}	}
