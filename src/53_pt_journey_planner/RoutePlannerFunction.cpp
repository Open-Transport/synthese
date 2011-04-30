
/** RoutePlannerFunction class implementation.
	@file RoutePlannerFunction.cpp

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

#include "RoutePlannerFunction.h"
#include "UserFavoriteJourneyTableSync.h"
#include "UserFavoriteJourney.h"
#include "TransportWebsite.h"
#include "HourPeriod.h"
#include "PTTimeSlotRoutePlanner.h"
#include "RequestException.h"
#include "Request.h"
#include "Interface.h"
#include "ObjectNotFoundException.h"
#include "PTRoutePlannerResult.h"
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "Road.h"
#include "RoadPlace.h"
#include "Hub.h"
#include "Service.h"
#include "CommercialLine.h"
#include "RollingStock.h"
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
#include "AccessParameters.h"
#include "PTConstants.h"
#include "DateTimeInterfacePage.h"
#include "PTRoutePlannerResult.h"
#include "NamedPlace.h"
#include "Webpage.h"
#include "StopArea.hpp"
#include "StaticFunctionRequest.h"
#include "PTObjectsCMSExporters.hpp"
#include "RoutePlannerFunction.h"
#include "ReservationRuleInterfacePage.h"
#include "LineMarkerInterfacePage.h"
#include "SentAlarm.h"

#include <geos/io/WKTWriter.h>
#include <geos/geom/LineString.h>
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
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace interfaces;
	using namespace pt_website;
	using namespace db;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace algorithm;
	using namespace pt_journey_planner;
	using namespace resa;
	using namespace pt;
	using namespace cms;

	template<> const string util::FactorableTemplate<pt_journey_planner::RoutePlannerFunction::_FunctionWithSite,pt_journey_planner::RoutePlannerFunction>::FACTORY_KEY("rp");

	namespace pt_journey_planner
	{
		const string RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER("msn");
		const string RoutePlannerFunction::PARAMETER_MAX_DEPTH("md");
		const string RoutePlannerFunction::PARAMETER_APPROACH_SPEED("apsp");
		const string RoutePlannerFunction::PARAMETER_DAY("dy");
		const string RoutePlannerFunction::PARAMETER_PERIOD_ID("pi");
		const string RoutePlannerFunction::PARAMETER_ACCESSIBILITY("ac");
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT("dct");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT("act");
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT("dpt");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT("apt");
		const string RoutePlannerFunction::PARAMETER_FAVORITE_ID("fid");
		const string RoutePlannerFunction::PARAMETER_LOWEST_DEPARTURE_TIME("da");
		const string RoutePlannerFunction::PARAMETER_LOWEST_ARRIVAL_TIME("ii");
		const string RoutePlannerFunction::PARAMETER_HIGHEST_DEPARTURE_TIME("ha");
		const string RoutePlannerFunction::PARAMETER_HIGHEST_ARRIVAL_TIME("ia");
		const string RoutePlannerFunction::PARAMETER_ROLLING_STOCK_FILTER_ID("tm");

		const string RoutePlannerFunction::PARAMETER_PAGE("page");
		const string RoutePlannerFunction::PARAMETER_SCHEDULES_ROW_PAGE("schedules_row_page");
		const string RoutePlannerFunction::PARAMETER_SCHEDULES_CELL_PAGE("schedule_cell_page");
		const string RoutePlannerFunction::PARAMETER_LINES_ROW_PAGE("lines_row_page");
		const string RoutePlannerFunction::PARAMETER_LINE_MARKER_PAGE("line_marker_page");
		const string RoutePlannerFunction::PARAMETER_BOARD_PAGE("board_page");
		const string RoutePlannerFunction::PARAMETER_WARNING_PAGE("warning_page");
		const string RoutePlannerFunction::PARAMETER_RESERVATION_PAGE("reservation_page");
		const string RoutePlannerFunction::PARAMETER_DURATION_PAGE("duration_page");
		const string RoutePlannerFunction::PARAMETER_TEXT_DURATION_PAGE("text_duration_page");
		const string RoutePlannerFunction::PARAMETER_MAP_PAGE("map_page");
		const string RoutePlannerFunction::PARAMETER_MAP_LINE_PAGE("map_line_page");
		const string RoutePlannerFunction::PARAMETER_DATE_TIME_PAGE("date_time_page");
		const string RoutePlannerFunction::PARAMETER_STOP_CELL_PAGE("stop_cell_page");
		const string RoutePlannerFunction::PARAMETER_SERVICE_CELL_PAGE("service_cell_page");
		const string RoutePlannerFunction::PARAMETER_JUNCTION_CELL_PAGE("junction_cell_page");
		const string RoutePlannerFunction::PARAMETER_MAP_STOP_PAGE("map_stop_page");
		const string RoutePlannerFunction::PARAMETER_MAP_SERVICE_PAGE("map_service_page");
		const string RoutePlannerFunction::PARAMETER_MAP_JUNCTION_PAGE("map_junction_page");

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
		const string RoutePlannerFunction::DATA_MAPS("maps");

		const string RoutePlannerFunction::DATA_INTERNAL_DATE("internal_date");
		const string RoutePlannerFunction::DATA_ORIGIN_CITY_TEXT("origin_city_text");
		const string RoutePlannerFunction::DATA_HANDICAPPED_FILTER("handicapped_filter");
		const string RoutePlannerFunction::DATA_ORIGIN_PLACE_TEXT("origin_place_text");
		const string RoutePlannerFunction::DATA_BIKE_FILTER("bike_filter");
		const string RoutePlannerFunction::DATA_DESTINATION_CITY_TEXT("destination_city_text");
		const string RoutePlannerFunction::DATA_DESTINATION_PLACE_ID("destination_place_id");
		const string RoutePlannerFunction::DATA_DESTINATION_PLACE_TEXT("destination_place_text");
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

		const string RoutePlannerFunction::DATA_IS_FIRST_ROW("is_first_row");
		const string RoutePlannerFunction::DATA_IS_LAST_ROW("is_last_row");
		const string RoutePlannerFunction::DATA_COLUMN_NUMBER("column_number");
		const string RoutePlannerFunction::DATA_IS_FOOT("is_foot");
		const string RoutePlannerFunction::DATA_FIRST_TIME("first_time");
		const string RoutePlannerFunction::DATA_LAST_TIME("last_time");
		const string RoutePlannerFunction::DATA_IS_CONTINUOUS_SERVICE("is_continuous_service");
		const string RoutePlannerFunction::DATA_IS_FIRST_WRITING("is_first_writing");
		const string RoutePlannerFunction::DATA_IS_LAST_WRITING("is_last_writing");
		const string RoutePlannerFunction::DATA_IS_FIRST_FOOT("is_first_foot");

		const string RoutePlannerFunction::DATA_CONTENT("content");
		const string RoutePlannerFunction::DATA_RANK("rank");

		const string RoutePlannerFunction::DATA_IS_THE_LAST_JOURNEY_BOARD("is_the_last_journey_board");
		const string RoutePlannerFunction::DATA_DEPARTURE_TIME("departure_time");
		const string RoutePlannerFunction::DATA_DEPARTURE_DATE("departure_date");
		const string RoutePlannerFunction::DATA_DEPARTURE_TIME_INTERNAL_FORMAT("internal_departure_time");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME("continuous_service_last_departure_time");
		const string RoutePlannerFunction::DATA_DEPARTURE_PLACE_NAME("departure_place_name");
		const string RoutePlannerFunction::DATA_DEPARTURE_PLACE_LONGITUDE("departure_longitude");
		const string RoutePlannerFunction::DATA_DEPARTURE_PLACE_LATITUDE("departure_latitude");
		const string RoutePlannerFunction::DATA_ARRIVAL_TIME("arrival_time");
		const string RoutePlannerFunction::DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME("continuous_service_last_arrival_time");
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

		// Junction cells
		const string RoutePlannerFunction::DATA_REACHED_PLACE_IS_NAMED("reached_place_is_named");
		const string RoutePlannerFunction::DATA_ROAD_NAME("road_name");
		const string RoutePlannerFunction::DATA_LENGTH("length");

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

		ParametersMap RoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());
			return map;
		}



		void RoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_FunctionWithSite::_setFromParametersMap(map);

			const TransportWebsite* site(dynamic_cast<const TransportWebsite*>(_site.get()));
			if(!site) throw RequestException("Incorrect site");

			_outputRoadApproachDetail = site->getDisplayRoadApproachDetail();


			// Origin and destination places
			optional<RegistryKeyType> favoriteId(map.getOptional<RegistryKeyType>(PARAMETER_FAVORITE_ID));
//			if (favoriteId) // 2b
//			{
//				try
//				{
//					_favorite = UserFavoriteJourneyTableSync::Get(*favoriteId, Env::GetOfficialEnv());
//					_originCityText = _favorite->getOriginCityName();
//					_originPlaceText = _favorite->getOriginPlaceName();
//					_destinationCityText = _favorite->getDestinationCityName();
//					_destinationPlaceText = _favorite->getDestinationPlaceName();
//				}
//				catch(ObjectNotFoundException<UserFavoriteJourney> e)
//				{
//					throw RequestException(e.getMessage());
//				}
//			}
//			else // 2a
			{
				_originCityText = map.getDefault<string>(PARAMETER_DEPARTURE_CITY_TEXT);
				_destinationCityText = map.getDefault<string>(PARAMETER_ARRIVAL_CITY_TEXT);
				if (_originCityText.empty() || _destinationCityText.empty())
					_home = true;
				else
				{
					_originPlaceText = map.getDefault<string>(PARAMETER_DEPARTURE_PLACE_TEXT);
					_destinationPlaceText = map.getDefault<string>(PARAMETER_ARRIVAL_PLACE_TEXT);
				}
			}
			if (!_home)
			{
				_departure_place = site->extendedFetchPlace(_originCityText, _originPlaceText);
				_arrival_place = site->extendedFetchPlace(_destinationCityText, _destinationPlaceText);
			}

			try
			{
				// Date
				if(!map.getDefault<string>(PARAMETER_DAY).empty())
				{ // 1a
					date day(from_string(map.get<string>(PARAMETER_DAY)));

					_planningOrder = DEPARTURE_FIRST;
					_periodId = map.get<size_t>(PARAMETER_PERIOD_ID);
					if (_periodId >= site->getPeriods().size())
					{
						throw RequestException("Bad value for period id");
					}
					_startDate = ptime(day, time_duration(0, 0, 0));
					_endDate = _startDate;
					_period = &site->getPeriods().at(_periodId);
					site->applyPeriod(*_period, _startDate, _endDate);
					_startArrivalDate = _startDate;
					_endArrivalDate = _endDate;
					if(	_departure_place.placeResult.value &&
						_arrival_place.placeResult.value &&
						_departure_place.placeResult.value->getPoint().get() &&
						_arrival_place.placeResult.value->getPoint().get() &&
						!_departure_place.placeResult.value->getPoint()->isEmpty() &&
						!_arrival_place.placeResult.value->getPoint()->isEmpty()
					){
						_endArrivalDate += minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint()->distance(_arrival_place.placeResult.value->getPoint().get()) / 1000));
					}
				}
				else
				{ // 1b
					if(	!map.getOptional<string>(PARAMETER_HIGHEST_ARRIVAL_TIME)
					){ // All default values
						_planningOrder = DEPARTURE_FIRST;
						if(!map.getDefault<string>(PARAMETER_LOWEST_DEPARTURE_TIME).empty())
						{
							_startDate = time_from_string(map.get<string>(PARAMETER_LOWEST_DEPARTURE_TIME));
						}
						else
						{
							_startDate = ptime(second_clock::local_time());
						}
						_startArrivalDate = _startDate;
						if(!map.getDefault<string>(PARAMETER_HIGHEST_DEPARTURE_TIME).empty())
						{
							_endDate = time_from_string(map.get<string>(PARAMETER_HIGHEST_DEPARTURE_TIME));
						}
						else
						{
							_endDate = _startDate;
							_endDate += days(1);
						}
						_endArrivalDate = _endDate;
						if(	_departure_place.placeResult.value &&
							_arrival_place.placeResult.value &&
							_departure_place.placeResult.value->getPoint().get() &&
							_arrival_place.placeResult.value->getPoint() &&
							!_departure_place.placeResult.value->getPoint()->isEmpty() &&
							!_arrival_place.placeResult.value->getPoint()->isEmpty()
						){
							_endArrivalDate += minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint()->distance(_arrival_place.placeResult.value->getPoint().get()) / 1000));
						}
					}
					else if(!map.getOptional<string>(PARAMETER_LOWEST_DEPARTURE_TIME))
					{ // Arrival to departure from the specified arrival time
						_planningOrder = ARRIVAL_FIRST;
						_endArrivalDate = time_from_string(map.get<string>(PARAMETER_HIGHEST_ARRIVAL_TIME));
						if(!map.getDefault<string>(PARAMETER_LOWEST_ARRIVAL_TIME).empty())
						{
							_startArrivalDate = time_from_string(map.get<string>(PARAMETER_LOWEST_ARRIVAL_TIME));
						}
						else
						{
							_startArrivalDate = _endArrivalDate;
							_startArrivalDate -= days(1);
						}
						_startDate = _startArrivalDate;
						if(	_departure_place.placeResult.value &&
							_arrival_place.placeResult.value &&
							_departure_place.placeResult.value->getPoint().get() &&
							_arrival_place.placeResult.value->getPoint().get() &&
							!_departure_place.placeResult.value->getPoint()->isEmpty() &&
							!_arrival_place.placeResult.value->getPoint()->isEmpty()
						){
							_startDate -= minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint()->distance(_arrival_place.placeResult.value->getPoint().get()) / 1000));
						}
					}
				}
			}
			catch(TransportWebsite::ForbiddenDateException)
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

			// Accessibility
			optional<unsigned int> acint(map.getOptional<unsigned int>(PARAMETER_ACCESSIBILITY));
			_accessParameters = site->getAccessParameters(
				acint ? static_cast<UserClassCode>(*acint) : USER_PEDESTRIAN,
				_rollingStockFilter.get() ? _rollingStockFilter->getAllowedPathClasses() : AccessParameters::AllowedPathClasses()
			);
			if(map.getOptional<size_t>(PARAMETER_MAX_DEPTH))
			{
				_accessParameters.setMaxtransportConnectionsCount(map.getOptional<size_t>(PARAMETER_MAX_DEPTH));
			}
			if(map.getOptional<double>(PARAMETER_APPROACH_SPEED))
			{
				_accessParameters.setApproachSpeed(*(map.getOptional<double>(PARAMETER_APPROACH_SPEED)));
			}

			if(	!_departure_place.placeResult.value || !_arrival_place.placeResult.value
			){
				throw RequestException("No calculation");
			}

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
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_JUNCTION_CELL_PAGE));
				if(id)
				{
					_junctionPage = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such junction cell page : "+ e.getMessage());
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
				throw RequestException("No such map junction page : "+ e.getMessage());
			}

		}



		void RoutePlannerFunction::run(
			ostream& stream,
			const Request& request
		) const	{
			if (!_departure_place.placeResult.value || !_arrival_place.placeResult.value)
			{
				return;
			}

			ptime startDate(_planningOrder == DEPARTURE_FIRST ? _startDate : _endArrivalDate);
			if(_planningOrder == ARRIVAL_FIRST)
			{
				startDate = ptime(startDate.date(), hours(3));
			}
			ptime endDate(_planningOrder == DEPARTURE_FIRST ? _endDate : _endArrivalDate);


			// Initialisation
			PTTimeSlotRoutePlanner r(
				_departure_place.placeResult.value.get(),
				_arrival_place.placeResult.value.get(),
				startDate,
				endDate,
				_planningOrder == DEPARTURE_FIRST ? _startArrivalDate : startDate,
				_endArrivalDate,
				_planningOrder == DEPARTURE_FIRST ? _maxSolutionsNumber : optional<size_t>(),
				_accessParameters,
				DEPARTURE_FIRST,
				false
			);

			// Computing
			PTRoutePlannerResult result = r.run();

			if(	_planningOrder == ARRIVAL_FIRST &&
				_maxSolutionsNumber &&
				result.getJourneys().size() > *_maxSolutionsNumber
			){
				result.removeFirstJourneys(result.getJourneys().size() - *_maxSolutionsNumber);
			}

			// Display
			if(_page.get())
			{
				display(
					stream,
					request,
					result,
					_startDate.date(),
					_periodId,
					_departure_place.placeResult.value.get(),
					_arrival_place.placeResult.value.get(),
					_period,
					_accessParameters
				);
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
				stream <<
					" siteId=\"" << _site->getKey() << "\">" <<
					"<timeBounds" <<
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
						to_iso_extended_string(_startDate.date()) << "\" name=\"" <<
						_period->getCaption() << "\" />"
					;
				}
				stream <<
					"<places departureCity=\"" << _departure_place.cityResult.key.getSource() << "\" departureCityNameTrust=\"" << _departure_place.cityResult.score.phoneticScore << "\"" <<
					" arrivalCity=\"" << _arrival_place.cityResult.key.getSource() << "\" arrivalCityNameTrust=\"" << _arrival_place.cityResult.score.phoneticScore << "\""
				;
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
				const PTRoutePlannerResult::PlacesList& placesList(
					result.getOrderedPlaces()
				);
				typedef vector<shared_ptr<ostringstream> > PlacesContentVector;
				PlacesContentVector sheetRows(placesList.size());
				BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
				{
					stream.reset(new ostringstream);
				}

				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, result.getJourneys())
				{
					bool hasALineAlert(false); // Interactive
					bool hasAStopAlert(false); // Interactive
					bool pedestrianMode = false;
					bool lastPedestrianMode = false;

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					PTRoutePlannerResult::PlacesList::const_iterator itPlaces(placesList.begin());

					stream <<
						"<journey hasALineAlert=\"" << (hasALineAlert ? "true" : "false") << "\" hasAStopAlert=\"" << (hasAStopAlert ? "true" : "false") << "\""
					;
					if(journey.getContinuousServiceRange().total_seconds() > 0)
					{
						stream << " continuousServiceDuration=\"" << journey.getContinuousServiceRange() << "\"";
					}
					stream << ">";

					if(journey.getReservationCompliance(false) != false)
					{
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
						stringstream sOpeningHours;
						bool onlineBooking(!resaRules.empty());
						BOOST_FOREACH(const ReservationContact* rc, resaRules)
						{
							sPhones << rc->getPhoneExchangeNumber() << " ";
							sOpeningHours << rc->getPhoneExchangeOpeningHours() << " ";
							if (!OnlineReservationRule::GetOnlineReservationRule(rc))
							{
								onlineBooking = false;
							}
						}

						stream << "<reservation" <<
							" online=\"" << (onlineBooking ? "true" : "false") << "\"" <<
							" type=\"" << (journey.getReservationCompliance(false) == true ? "compulsory" : "optional") << "\""
						;
						if(!sOpeningHours.str().empty())
						{
							stream << " openingHours=\"" << sOpeningHours.str() << "\"";
						}
						if(!sPhones.str().empty())
						{
							stream << " phoneNumber=\"" << sPhones.str() << "\"";
						}
						stream << " deadLine=\"" << posix_time::to_iso_extended_string(journey.getReservationDeadLine()) << "\" />";
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
						const ServicePointer& curET(*itl);

						if(	itl == jl.begin() ||
							!curET.getService()->getPath()->isPedestrianMode() ||
							lastPedestrianMode != curET.getService()->getPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								PTRoutePlannerResult::GetNamedPlaceFromLegs(
									itl == jl.begin() ? NULL : &(*(itl - 1)),
									&curET,
									PTRoutePlannerResult::getNamedPlace(_departure_place.placeResult.value.get())
							)	);

							for (; itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow)
							{
								**itSheetRow << "<cell />";
							}

							pedestrianMode = curET.getService()->getPath()->isPedestrianMode();

							// Saving of the columns on each lines
							if(itl == jl.begin())
							{
								**itSheetRow << "<cell";
							}
							**itSheetRow <<
								" departureDateTime=\"" <<
								posix_time::to_iso_extended_string(curET.getDepartureDateTime()) << "\"";
							if(journey.getContinuousServiceRange().total_seconds() > 0)
							{
								posix_time::ptime edTime(curET.getDepartureDateTime());
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
							lastPedestrianMode = pedestrianMode;
						}

						if(	itl == jl.end()-1
						||	!(itl+1)->getService()->getPath()->isPedestrianMode()
						||	!curET.getService()->getPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								PTRoutePlannerResult::GetNamedPlaceFromLegs(
									&curET,
									itl == jl.end()-1 ? NULL : &(*(itl+1)),
									PTRoutePlannerResult::getNamedPlace(_arrival_place.placeResult.value.get())
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
								posix_time::to_iso_extended_string(curET.getArrivalDateTime()) << "\"";
							if(journey.getContinuousServiceRange().total_seconds() > 0)
							{
								posix_time::ptime eaTime(curET.getArrivalDateTime());
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


						const JourneyPattern* line(dynamic_cast<const JourneyPattern*> (curET.getService()->getPath()));
						if(line != NULL)
						{
							// Insertion of fake leg if site does not output road approach detail
							if(!_outputRoadApproachDetail)
							{
								if(firstApproach)
								{
									if(line->isPedestrianMode())
									{
										partialDistance += curET.getDistance();
									}
									else
									{
										if(itl != jl.begin())
										{
											const Road* road(dynamic_cast<const Road*> (jl.begin()->getService()->getPath ()));
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


											if(dynamic_cast<const NamedPlace*>(result.getDeparturePlace()))
											{
												_XMLDisplayAddress(stream, dynamic_cast<const NamedPlace&>(*result.getDeparturePlace()),_showCoords);
											}
											else if(dynamic_cast<const Crossing*>(jl.begin()->getDepartureEdge()->getFromVertex()))
											{
												if(dynamic_cast<const RoadPlace*>(_departure_place.placeResult.value.get()))
												{
													_XMLDisplayRoadPlace(
														stream,
														dynamic_cast<const RoadPlace&>(*_departure_place.placeResult.value.get()),
														_showCoords
													);
												}
												else
												{
													_XMLDisplayAddress(
														stream,
														*dynamic_cast<const Crossing*>(jl.begin()->getDepartureEdge()->getFromVertex()),
														*road->getRoadPlace(),
														_showCoords
													);
												}
											}
											stream <<
												"</startAddress>" <<
												"<endAddress>";
											if(dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub()))
											{
												_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*curET.getDepartureEdge()->getHub()),_showCoords);
											}
											else if(dynamic_cast<const Crossing*>((itl-1)->getArrivalEdge()->getFromVertex()))
											{
												_XMLDisplayAddress(
													stream,
													*dynamic_cast<const Crossing*>((itl-1)->getArrivalEdge()->getFromVertex()),
													*road->getRoadPlace(),
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
									" length=\"" << ceil(curET.getDistance()) << "\"" <<
									" departureTime=\"" << posix_time::to_iso_extended_string(curET.getDepartureDateTime()) << "\"" <<
									" arrivalTime=\"" << posix_time::to_iso_extended_string(curET.getArrivalDateTime()) << "\"";
							if(journey.getContinuousServiceRange().total_seconds() > 0)
							{
								posix_time::ptime edTime(curET.getDepartureDateTime());
								edTime += journey.getContinuousServiceRange();
								posix_time::ptime eaTime(curET.getArrivalDateTime());
								eaTime += journey.getContinuousServiceRange();
								stream <<
									" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
									" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
							}
							const ContinuousService* cserv(dynamic_cast<const ContinuousService*>(curET.getService()));
							if(cserv && cserv->getMaxWaitingTime().total_seconds() > 0)
							{
								stream << " possibleWaitingTime=\"" << (cserv->getMaxWaitingTime().total_seconds() / 60) << "\"";
							}
							stream <<
									" startStopIsTerminus=\"" << (curET.getDepartureEdge()->getRankInPath() == 0 ? "true" : "false") << "\"" <<
									" endStopIsTerminus=\"" << (curET.getArrivalEdge()->getRankInPath()+1 == curET.getArrivalEdge()->getParentPath()->getEdges().size() ? "true" : "false") << "\"";
							if(!line->getDirection().empty())
							{
								stream << " destinationText=\"" << line->getDirection() << "\"";
							}
							stream <<
								">";
							_XMLDisplayPhysicalStop(stream, "startStop", dynamic_cast<const StopPoint&>(*curET.getDepartureEdge()->getFromVertex()),_showCoords);
							_XMLDisplayPhysicalStop(stream, "endStop", dynamic_cast<const StopPoint&>(*curET.getArrivalEdge()->getFromVertex()),_showCoords);
							_XMLDisplayPhysicalStop(stream, "destinationStop", dynamic_cast<const StopPoint&>(*line->getLastEdge()->getFromVertex()),_showCoords);
							if(!line->isPedestrianMode())
							{
								stream <<
									"<line" <<
									" id=\"" << line->getCommercialLine()->getKey() << "\"";
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
										" name=\"" << line->getRollingStock()->getArticle() << line->getRollingStock()->getName() << "\"" <<
										" />";
								}
							}
							stream << "</transport>";
						}

						const Road* road(dynamic_cast<const Road*> (curET.getService()->getPath ()));
						if(road != NULL)
						{
							if(_outputRoadApproachDetail)
							{
								stream <<
									"<street" <<
									" length=\"" << ceil(curET.getDistance()) << "\"" <<
									" city=\"" << road->getRoadPlace()->getCity()->getName() << "\"" <<
									" name=\"" << road->getRoadPlace()->getName() << "\"" <<
									" departureTime=\"" << posix_time::to_iso_extended_string(curET.getDepartureDateTime()) << "\"" <<
									" arrivalTime=\"" << posix_time::to_iso_extended_string(curET.getArrivalDateTime()) << "\"";
								if(journey.getContinuousServiceRange().total_seconds() > 0)
								{
									posix_time::ptime edTime(curET.getDepartureDateTime());
									edTime += journey.getContinuousServiceRange();
									posix_time::ptime eaTime(curET.getArrivalDateTime());
									eaTime += journey.getContinuousServiceRange();
									stream <<
										" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
										" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
								}
								stream <<
									">" <<
									"<startAddress>";
								if(dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub()))
								{
									_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*curET.getDepartureEdge()->getHub()),_showCoords);
								}
								else if(dynamic_cast<const Crossing*>(curET.getDepartureEdge()->getFromVertex()))
								{
									if(itl == jl.begin() && dynamic_cast<const RoadPlace*>(_departure_place.placeResult.value.get()))
									{
										_XMLDisplayRoadPlace(
											stream,
											dynamic_cast<const RoadPlace&>(*_departure_place.placeResult.value),
											_showCoords
										);
									}
									else
									{
										_XMLDisplayAddress(
											stream,
											*dynamic_cast<const Crossing*>(curET.getDepartureEdge()->getFromVertex()),
											*road->getRoadPlace(),
											_showCoords
											);
									}
								}
								stream <<
									"</startAddress>" <<
									"<endAddress>";
								if(dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub()))
								{
									_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*curET.getArrivalEdge()->getHub()),_showCoords);
								}
								else if(dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getFromVertex()))
								{
									if(itl == jl.end() - 1 && dynamic_cast<const RoadPlace*>(_arrival_place.placeResult.value.get()))
									{
										_XMLDisplayRoadPlace(
											stream,
											dynamic_cast<const RoadPlace&>(*_arrival_place.placeResult.value),
											_showCoords
										);
									}
									else
									{
										_XMLDisplayAddress(
											stream,
											*dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getFromVertex()),
											*road->getRoadPlace(),
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
								partialDistance += ceil(curET.getDistance());
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
					}

					// Bug 7694 : fulfill unused arrival rows
					for(++itSheetRow; itSheetRow != sheetRows.end(); ++itSheetRow)
					{
						**itSheetRow << "<cell />";
					}

					if(!_outputRoadApproachDetail && lastApproachBeginning != jl.end() && lastTransportEnding != jl.end())
					{
						const Road* road(dynamic_cast<const Road*> ((jl.end()-1)->getService()->getPath ()));
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
							_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*lastTransportEnding->getArrivalEdge()->getHub()),_showCoords);
						}
						else if(dynamic_cast<const Crossing*>(lastApproachBeginning->getDepartureEdge()->getFromVertex()))
						{
							_XMLDisplayAddress(
								stream,
								*dynamic_cast<const Crossing*>(lastApproachBeginning->getDepartureEdge()->getFromVertex()),
								*road->getRoadPlace(),
								_showCoords
							);
						}
						stream <<
							"</startAddress>" <<
							"<endAddress>";

						if(dynamic_cast<const NamedPlace*>(result.getArrivalPlace()))
						{
							_XMLDisplayAddress(stream, dynamic_cast<const NamedPlace&>(*result.getArrivalPlace()),_showCoords);
						}
						else if(dynamic_cast<const Crossing*>((jl.end()-1)->getArrivalEdge()->getFromVertex()))
						{
							if(dynamic_cast<const RoadPlace*>(_arrival_place.placeResult.value.get()))
							{
								_XMLDisplayRoadPlace(
									stream,
									dynamic_cast<const RoadPlace&>(*_arrival_place.placeResult.value),
									_showCoords
								);
							}
							else
							{
								_XMLDisplayAddress(
									stream,
									*dynamic_cast<const Crossing*>((jl.end()-1)->getArrivalEdge()->getFromVertex()),
									*road->getRoadPlace(),
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
					BOOST_FOREACH(const PTRoutePlannerResult::PlacesList::value_type& row, result.getOrderedPlaces())
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
							_XMLDisplayRoadPlace(stream, dynamic_cast<const RoadPlace&>(*row.place),_showCoords);
						}
						else
						{
							_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*row.place),_showCoords);
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
		}



		RoutePlannerFunction::RoutePlannerFunction()
			: _startDate(not_a_date_time)
			, _endDate(not_a_date_time)
			, _period(NULL)
			, _home(false),
			_startArrivalDate(not_a_date_time),
			_endArrivalDate(not_a_date_time)
		{
		}



		const optional<std::size_t>& RoutePlannerFunction::getMaxSolutions() const
		{
			return _maxSolutionsNumber;
		}



		void RoutePlannerFunction::setMaxSolutions(boost::optional<std::size_t> number)
		{
			_maxSolutionsNumber = number;
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
			return _page ? _page->getMimeType() : "text/xml";
		}

		void RoutePlannerFunction::_XMLDisplayConnectionPlace(
			std::ostream& stream,
			const NamedPlace& np,
			bool showCoords
		){
			shared_ptr<Point> gp(
				CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
					*np.getPoint()
			)	);

			if(showCoords)
			{
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



		void RoutePlannerFunction::_XMLDisplayPhysicalStop(
			std::ostream& stream,
			const std::string& tag,
			const pt::StopPoint& stop,
			bool showCoords
		){
			shared_ptr<Point> gp;
			if(stop.getGeometry().get() && !stop.getGeometry()->isEmpty())
			{
				gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
					*stop.getGeometry()
				);
			}
			else if(stop.getConnectionPlace()->getPoint().get() && !stop.getConnectionPlace()->getPoint()->isEmpty())
			{
				gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
					*stop.getConnectionPlace()->getPoint()
				);
			}

			if(showCoords && gp.get())
			{
				shared_ptr<Point> pt(stop.getGeometry().get() ? stop.getGeometry() : stop.getConnectionPlace()->getPoint());

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
			_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*stop.getHub()),showCoords);
			stream << "</" << tag << ">";
		}



		void RoutePlannerFunction::_XMLDisplayAddress(
					std::ostream& stream,
					const NamedPlace& np,
					bool showCoords
		){
			shared_ptr<Point> gp;

			if(	np.getPoint().get() &&
				!np.getPoint()->isEmpty()
			){
				gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
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



		void RoutePlannerFunction::_XMLDisplayAddress(
			std::ostream& stream,
			const road::Crossing& address,
			const road::RoadPlace& roadPlace,
			bool showCoords
		){
			shared_ptr<Point> gp;

			if(	address.getGeometry().get() &&
				!address.getGeometry()->isEmpty()
			){
				gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
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



		void RoutePlannerFunction::_XMLDisplayRoadPlace(
			std::ostream& stream,
			const road::RoadPlace& roadPlace,
			bool showCoords
		){
			shared_ptr<Point> gp;

			if(	roadPlace.getPoint().get() &&
				!roadPlace.getPoint()->isEmpty()
			){
				gp = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
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
			const graph::AccessParameters& accessParameters
		) const	{
			ParametersMap pm(request.getFunction()->getSavedParameters());

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
			pm.insert(DATA_BIKE_FILTER, accessParameters.getUserClass() == USER_BIKE);
			pm.insert(DATA_DESTINATION_CITY_TEXT, destinationCity->getName());
			//pm.insert("" /*lexical_cast<string>(destinationPlace->getKey())*/);
			pm.insert(DATA_DESTINATION_PLACE_TEXT, destinationPlaceName);
			pm.insert(DATA_PERIOD_ID, periodId);

			// Text formatted date
			if(_dateTimePage.get())
			{
				stringstream sDate;
				DateTimeInterfacePage::Display(sDate, _dateTimePage, request, date);
				pm.insert(DATA_DATE, sDate.str());
			}

			if(period)
			{
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
				const PTRoutePlannerResult::PlacesList& placesList(
					object.getOrderedPlaces()
				);
				typedef vector<shared_ptr<ostringstream> > PlacesContentVector;
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
					PTRoutePlannerResult::PlacesList::const_iterator itPlaces(placesList.begin());

					// Loop on each leg
					const Journey::ServiceUses& jl(it->getServiceUses());
					for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
					{
						const ServicePointer& curET(*itl);

						if(	itl == jl.begin() ||
							!curET.getService()->getPath()->isPedestrianMode() ||
							lastPedestrianMode != curET.getService()->getPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								(	itl == jl.begin() &&
									dynamic_cast<const Crossing*>(curET.getDepartureEdge()->getHub())
								) ?
								dynamic_cast<const NamedPlace*>(object.getDeparturePlace()) :
								dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub())
							);
							assert(placeToSearch != NULL);

							ptime lastDateTime(curET.getDepartureDateTime());
							lastDateTime += it->getContinuousServiceRange();

							for (; itPlaces != placesList.end() && itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow)
							{
								_displayScheduleCell(
									**itSheetRow,
									request,
									itPlaces == object.getOrderedPlaces().begin(),
									(itl + 1) == jl.end(),
									i,
									pedestrianMode,
									time_duration(not_a_date_time),
									time_duration(not_a_date_time),
									false,
									true,
									true,
									false
								);
							}

							pedestrianMode = curET.getService()->getPath()->isPedestrianMode();

							// Saving of the columns on each lines
							_displayScheduleCell(
								**itSheetRow,
								request,
								itPlaces == object.getOrderedPlaces().begin()
								, true
								, i
								, pedestrianMode
								, curET.getDepartureDateTime().time_of_day()
								, lastDateTime.time_of_day()
								, it->getContinuousServiceRange().total_seconds() > 0
								, itl == jl.begin()
								, true
								, pedestrianMode && !lastPedestrianMode
							);
							++itPlaces; ++itSheetRow;
							lastPedestrianMode = pedestrianMode;
						}

						if(	itl == jl.end()-1
							||	!(itl+1)->getService()->getPath()->isPedestrianMode()
							||	!curET.getService()->getPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								itl == jl.end()-1 && dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getHub()) ?
								dynamic_cast<const NamedPlace*>(object.getArrivalPlace()) :
								dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub())
							);
							assert(placeToSearch != NULL);

							for (; itPlaces != placesList.end() && itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow )
							{
								_displayScheduleCell(
									**itSheetRow,
									request,
									true
									, true
									, i
									, pedestrianMode
									, time_duration(not_a_date_time)
									, time_duration(not_a_date_time)
									, false
									, true
									, true
									, false
								);
							}

							ptime lastDateTime(curET.getArrivalDateTime());
							lastDateTime += it->getContinuousServiceRange();

							_displayScheduleCell(
								**itSheetRow,
								request,
								true
								, (itl + 1) == jl.end()
								, i
								, pedestrianMode
								, curET.getArrivalDateTime().time_of_day()
								, lastDateTime.time_of_day()
								, it->getContinuousServiceRange().total_seconds() > 0
								, true
								, (itl + 1) == jl.end()
								, false
							);
						}
					}

					for (++itPlaces, ++itSheetRow; itPlaces != placesList.end(); ++itPlaces, ++itSheetRow)
					{
						_displayScheduleCell(
							**itSheetRow,
							request,
							true
							, true
							, i
							, false
							, time_duration(not_a_date_time)
							, time_duration(not_a_date_time)
							, false
							, true
							, true
							, false
						);
					}
				}

				// Initialization of text lines
				bool color(false);
				PlacesContentVector::const_iterator it(sheetRows.begin());
				BOOST_FOREACH(const PTRoutePlannerResult::PlacesList::value_type& pi, placesList)
				{
					assert(dynamic_cast<const NamedPlace*>(pi.place));

					_displayRow(
						rows,
						request,
						*dynamic_cast<const NamedPlace*>(pi.place),
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
				size_t n = 1;
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
						_junctionPage,
						request,
						i,
						*it,
						*object.getDeparturePlace(),
						*object.getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 != object.getJourneys().end()
					);
				}

				pm.insert(DATA_BOARDS, boards.str());
			}

			// Warnings row
			if(_warningPage.get())
			{
				stringstream warnings;

				/// @todo warnings

				pm.insert(DATA_WARNINGS, warnings.str());
			}


			// Durations row
			if(_durationPage.get())
			{
				stringstream durations;
				BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, object.getJourneys())
				{
					DateTimeInterfacePage::Display(
						durations,
						_durationPage,
						request,
						journey.getDuration()
					);
				}
				pm.insert(DATA_DURATIONS, durations.str());
			}


			// Reservations row
			if(_reservationPage.get())
			{
				stringstream reservations;
				BOOST_FOREACH(PTRoutePlannerResult::Journeys::value_type journey, object.getJourneys())
				{
					ReservationRuleInterfacePage::Display(
						reservations,
						_reservationPage,
						_dateTimePage,
						request,
						journey
					);
				}
				pm.insert(DATA_RESERVATIONS, reservations.str());
			}


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
						_mapJunctionPage,
						request,
						i,
						*it,
						*object.getDeparturePlace(),
						*object.getArrivalPlace(),
						hFilter,
						bFilter,
						it+1 != object.getJourneys().end()
					);
				}

				pm.insert(DATA_MAPS, maps.str());
			}

			_page->display(stream, request, pm);
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
			ParametersMap pm(request.getFunction()->getSavedParameters());

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
			bool isItFirstRow,
			bool isItLastRow,
			size_t columnNumber,
			bool isItFootLine,
			const boost::posix_time::time_duration& firstTime,
			const boost::posix_time::time_duration& lastTime,
			bool isItContinuousService,
			bool isFirstWriting,
			bool isLastWriting,
			bool isFirstFoot
		) const {
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(DATA_IS_FIRST_ROW, isItFirstRow);
			pm.insert(DATA_IS_LAST_ROW, isItLastRow);
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

			_schedulesCellPage->display(stream ,request, pm);
		}



		void RoutePlannerFunction::_displayLinesCell(
			std::ostream& stream,
			const server::Request& request,
			std::size_t columnNumber,
			const graph::Journey& journey
		) const	{
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(DATA_COLUMN_NUMBER, columnNumber);

			// Content
			if(_lineMarkerPage.get())
			{
				stringstream content;
				bool __AfficherLignesPied = false;
				//	Conversion::ToBool(
				//	_displayPedestrianLines->getValue(parameters, variables, object, request)
				//	);

				BOOST_FOREACH(const ServicePointer& leg, journey.getServiceUses())
				{
					if ( __AfficherLignesPied || !dynamic_cast<const Road*> (leg.getService()->getPath ()) )
					{
						LineMarkerInterfacePage::Display(
							content,
							_lineMarkerPage,
							request,
							*static_cast<const JourneyPattern*>(leg.getService()->getPath ())->getCommercialLine()
						);
					}
				}

				pm.insert(DATA_CONTENT, content.str());
			}

			_linesRowPage->display(stream ,request, pm);
		}



		void RoutePlannerFunction::_displayJourney(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> stopCellPage,
			boost::shared_ptr<const cms::Webpage> serviceCellPage,
			boost::shared_ptr<const cms::Webpage> junctionPage,
			const server::Request& request,
			std::size_t n,
			const graph::Journey& journey,
			const geography::Place& departurePlace,
			const geography::Place& arrivalPlace,
			boost::logic::tribool handicappedFilter,
			boost::logic::tribool bikeFilter,
			bool isTheLast
		) const	{
			ParametersMap pm(request.getFunction()->getSavedParameters());

			// Rank
			pm.insert(DATA_RANK, n);
			pm.insert(DATA_IS_THE_LAST_JOURNEY_BOARD, isTheLast);

			// Filters
			pm.insert(DATA_HANDICAPPED_FILTER, handicappedFilter);
			pm.insert(DATA_BIKE_FILTER, bikeFilter);

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
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_DEPARTURE_TIME, to_simple_string(journey.getLastDepartureTime().time_of_day()));
			}

			// Departure place
			if(dynamic_cast<const Crossing*>(journey.getOrigin()->getHub()))
			{
				pm.insert(DATA_DEPARTURE_PLACE_NAME, dynamic_cast<const NamedPlace&>(departurePlace).getFullName());
			}
			else
			{
				pm.insert(DATA_DEPARTURE_PLACE_NAME, dynamic_cast<const NamedPlace&>(*journey.getOrigin()->getHub()).getFullName());
			}

			shared_ptr<Point> departurePoint(
				CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
					*departurePlace.getPoint()
			)	);
			pm.insert(DATA_DEPARTURE_PLACE_LONGITUDE, departurePoint->getX());
			pm.insert(DATA_DEPARTURE_PLACE_LATITUDE, departurePoint->getY());

			// Arrival time
			{
				stringstream s;
				s << setw(2) << setfill('0') << journey.getFirstArrivalTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << journey.getFirstArrivalTime().time_of_day().minutes();
				pm.insert(DATA_ARRIVAL_TIME, s.str());
			}

			if(journey.getContinuousServiceRange().total_seconds())
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_LAST_ARRIVAL_TIME, to_simple_string(journey.getLastArrivalTime().time_of_day()));
			}

			// Arrival place
			if(dynamic_cast<const Crossing*>(journey.getDestination()->getHub()))
			{
				pm.insert(DATA_ARRIVAL_PLACE_NAME, dynamic_cast<const NamedPlace&>(arrivalPlace).getFullName());
			}
			else
			{
				pm.insert(DATA_ARRIVAL_PLACE_NAME, dynamic_cast<const NamedPlace&>(*journey.getDestination()->getHub()).getFullName());
			}

			shared_ptr<Point> arrivalPoint(
				CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
					*arrivalPlace.getPoint()
			)	);
			pm.insert(DATA_ARRIVAL_PLACE_LONGITUDE, arrivalPoint->getX());
			pm.insert(DATA_ARRIVAL_PLACE_LATITUDE, arrivalPoint->getY());

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
			ptime resaDeadLine(journey.getReservationDeadLine());
			logic::tribool resaCompliance(journey.getReservationCompliance(false));
			pm.insert(DATA_RESERVATION_AVAILABLE, resaCompliance && resaDeadLine > now);
			pm.insert(DATA_RESERVATION_COMPULSORY, resaCompliance == true);
			pm.insert(DATA_RESERVATION_DELAY, resaDeadLine.is_not_a_date_time() ? 0 : (resaDeadLine - now).total_seconds() / 60);

			if(!journey.getReservationDeadLine().is_not_a_date_time())
			{
				if(_dateTimePage.get())
				{
					stringstream sResa;
					DateTimeInterfacePage::Display(sResa, _dateTimePage, request, journey.getReservationDeadLine());
					pm.insert(DATA_RESERVATION_DEADLINE, sResa.str());
				}
				else
				{
					pm.insert(DATA_RESERVATION_DEADLINE, journey.getReservationDeadLine());
				}
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
					rc->getPhoneExchangeNumber() <<
					" (" << rc->getPhoneExchangeOpeningHours() << ") "
				;
				if (!OnlineReservationRule::GetOnlineReservationRule(rc))
				{
					onlineBooking = false;
				}
			}
			pm.insert(DATA_RESERVATION_PHONE_NUMBER, sPhones.str());
			pm.insert(DATA_ONLINE_RESERVATION, onlineBooking);

			// Content
			if(stopCellPage.get() && serviceCellPage.get() && junctionPage.get())
			{
				stringstream content;

				// Loop on lines of the board
				bool __Couleur = false;

				const Hub* lastPlace(NULL);
				double distance(0);

				const Journey::ServiceUses& services(journey.getServiceUses());
				for (Journey::ServiceUses::const_iterator it = services.begin(); it != services.end(); ++it)
				{
					const ServicePointer& leg(*it);

					const Road* road(dynamic_cast<const Road*> (leg.getService()->getPath ()));
					if (road == NULL)
					{
						distance = 0;

						// LIGNE ARRET MONTEE Si premier point d'arret et si alerte
						if (leg.getDepartureEdge()->getHub() != lastPlace)
						{
							/*					ptime debutPrem(leg.getDepartureDateTime());
							ptime finPrem(debutPrem);
							if (journey.getContinuousServiceRange () )
							finPrem += journey.getContinuousServiceRange ();
							*/

							DisplayStopCell(
								content,
								stopCellPage,
								request,
								false,
								false,
								NULL,
								static_cast<const StopPoint*>(leg.getDepartureEdge()->getFromVertex()),
								__Couleur,
								leg.getDepartureDateTime(),
								journey.getContinuousServiceRange(),
								false,
								it == services.begin()
							);

							lastPlace = leg.getDepartureEdge()->getHub();
							__Couleur = !__Couleur;
						}

						// LIGNE CIRCULATIONS
						/*					ptime debutLigne(leg.getDepartureDateTime());
						ptime finLigne(leg.getArrivalDateTime());

						if ( journey.getContinuousServiceRange () )
						{
						finLigne = lastArrivalTime;
						}
						*/

						DisplayServiceCell(
							content,
							serviceCellPage,
							request,
							leg,
							journey.getContinuousServiceRange(),
							handicappedFilter,
							bikeFilter,
							__Couleur
						);

						__Couleur = !__Couleur;

						// LIGNE ARRET DE DESCENTE

						/*					ptime debutArret(leg.getArrivalDateTime ());
						ptime finArret(debutArret);
						if ( (it + 1) < journey.getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
						if ( journey.getContinuousServiceRange () )
						finArret += journey.getContinuousServiceRange ();
						*/

						DisplayStopCell(
							content,
							stopCellPage,
							request,
							true,
							leg.getArrivalEdge()->getHub() == leg.getService()->getPath()->getEdges().back()->getHub(),
							static_cast<const StopPoint*>(leg.getArrivalEdge()->getFromVertex()),
							it+1 != services.end() ? static_cast<const StopPoint*>((it+1)->getDepartureEdge()->getFromVertex()) : NULL,
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
						// 1/2 Alerte
						/*					ptime debutArret(leg.getArrivalDateTime ());
						ptime finArret(debutArret);
						if ((it+1) < journey.getServiceUses().end())
						finArret = (it + 1)->getDepartureDateTime();
						if ( journey.getContinuousServiceRange () )
						finArret += journey.getContinuousServiceRange ();
						*/
						distance += leg.getDistance();

						if (it + 1 != services.end())
						{
							const ServicePointer& nextLeg(*(it+1));
							const Road* nextRoad(dynamic_cast<const Road*> (nextLeg.getService()->getPath ()));

							if (nextRoad && nextRoad->getRoadPlace() == road->getRoadPlace())
								continue;
						}

						DisplayJunctionCell(
							content,
							junctionPage,
							request,
							*leg.getArrivalEdge()->getFromVertex(),
							__Couleur,
							road,
							distance
						);

						distance = 0;
						__Couleur = !__Couleur;
					}
				}
				pm.insert(DATA_CONTENT, content.str());
			}

			page->display(stream, request, pm);
		}



		void RoutePlannerFunction::DisplayStopCell(
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
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

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
				shared_ptr<Point> point(
					CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
						*place.getPoint()
				)	);
				pm.insert(DATA_LONGITUDE, point->getX());
				pm.insert(DATA_LATITUDE, point->getY());
			}

			// Arrival stop
			if(arrivalPhysicalStop)
			{
				pm.insert(DATA_ARRIVAL_STOP_NAME, arrivalPhysicalStop->getName());
				// Point
				if(	arrivalPhysicalStop->getGeometry().get() &&
					!arrivalPhysicalStop->getGeometry()->isEmpty()
				){
					shared_ptr<Point> point(
						CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
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
					shared_ptr<Point> point(
						CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
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
			pm.insert(DATA_IS_LAST_LEG, isFirstLeg);

			page->display(stream, request, pm);
		}



		void RoutePlannerFunction::DisplayJunctionCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const graph::Vertex& vertex,
			bool color,
			const road::Road* road,
			double distance
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

			// Point
			if(	vertex.getGeometry().get() &&
				!vertex.getGeometry()->isEmpty()
			){
				shared_ptr<Point> point(
					CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
						*vertex.getGeometry()
				)	);
				pm.insert(DATA_LONGITUDE, point->getX());
				pm.insert(DATA_LATITUDE, point->getY());
			}
			pm.insert(DATA_REACHED_PLACE_IS_NAMED, dynamic_cast<const NamedPlace*>(vertex.getHub()) != NULL);
			pm.insert(DATA_ODD_ROW, color);
			if(road && road->getRoadPlace())
			{
				pm.insert(DATA_ROAD_NAME, road->getRoadPlace()->getName());
			}
			pm.insert(DATA_LENGTH, distance);

			page->display(stream, request, pm);
		}



		void RoutePlannerFunction::DisplayServiceCell(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const graph::ServicePointer& serviceUse,
			boost::posix_time::time_duration continuousServiceRange,
			boost::logic::tribool handicappedFilterStatus,
			boost::logic::tribool bikeFilterStatus,
			bool color
		){
			ParametersMap pm(request.getFunction()->getSavedParameters());

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
				pm.insert(DATA_ROLLINGSTOCK_ARTICLE, line->getRollingStock()->getArticle()); // 6
			}
			pm.insert(DATA_DESTINATION_NAME, line->getDirection().empty() ? line->getDestination()->getConnectionPlace()->getFullName() : line->getDirection() ); // 7
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
			PTObjectsCMSExporters::ExportLine(pm, *commercialLine);
			if(continuousService)
			{
				pm.insert(DATA_CONTINUOUS_SERVICE_WAITING, continuousService->getMaxWaitingTime().total_seconds() / 60);
			}

			pm.insert(DATA_ODD_ROW, color); // 21

			shared_ptr<LineString> geometry(serviceUse.getGeometry());
			shared_ptr<Geometry> geometry4326(
				CoordinatesSystem::GetCoordinatesSystem(4326).convertGeometry(
					*static_cast<Geometry*>(geometry.get())
			)	);

			shared_ptr<WKTWriter> wktWriter;
			if(geometry4326.get() && !geometry4326->isEmpty())
			{
				pm.insert(DATA_WKT, wktWriter->write(geometry4326.get()));
			}

			page->display(stream, request, pm);
		}
}	}
