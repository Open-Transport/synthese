
/** HikingTrailMultipleRoutePlannerFunction class implementation.
	@file HikingTrailMultipleRoutePlannerFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "HikingTrailMultipleRoutePlannerFunction.hpp"

#include "AlgorithmLogger.hpp"
#include "PTServiceConfig.hpp"
#include "RequestException.h"
#include "Request.h"
#include "NamedPlace.h"
#include "City.h"
#include "HikingTrail.h"
#include "GeographyModule.h"
#include "Webpage.h"
#include "PTTimeSlotRoutePlanner.h"
#include "StopArea.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace geography;
	using namespace cms;
	using namespace graph;
	using namespace pt_journey_planner;
	using namespace algorithm;
	using namespace pt_website;

	template<> const string util::FactorableTemplate<Function,hiking::HikingTrailMultipleRoutePlannerFunction>::FACTORY_KEY("HikingTrailMultipleRoutePlannerFunction");

	namespace hiking
	{
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_CITY("ci");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_STOP("na");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_TRAIL_BEGINNING("br");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_TRAIL_ENDING("er");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_SITE_ID("si");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_BEGINNING_DAY("bd");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_BEGINNING_PERIOD("bp");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_ENDING_DAY("ed");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_ENDING_PERIOD("ep");

		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_PAGE("page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_SCHEDULES_ROW_PAGE("schedules_row_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_SCHEDULES_CELL_PAGE("schedule_cell_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_LINES_ROW_PAGE("lines_row_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_LINE_MARKER_PAGE("line_marker_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_BOARD_PAGE("board_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_WARNING_PAGE("warning_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_RESERVATION_PAGE("reservation_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_DURATION_PAGE("duration_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_TEXT_DURATION_PAGE("text_duration_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_MAP_PAGE("map_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_MAP_LINE_PAGE("map_line_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_DATE_TIME_PAGE("date_time_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_STOP_CELL_PAGE("stop_cell_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_SERVICE_CELL_PAGE("service_cell_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_JUNCTION_CELL_PAGE("junction_cell_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_MAP_STOP_PAGE("map_stop_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_MAP_SERVICE_PAGE("map_service_page");
		const string HikingTrailMultipleRoutePlannerFunction::PARAMETER_MAP_JUNCTION_PAGE("map_junction_page");

		ParametersMap HikingTrailMultipleRoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_trail.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _trail->getKey());
				const NamedPlace* namedPlace(dynamic_cast<const NamedPlace*>(_place.placeResult.value.get()));
				if(namedPlace)
				{
					map.insert(PARAMETER_CITY, namedPlace->getCity()->getName());
					map.insert(PARAMETER_STOP, namedPlace->getName());
				}
				else if(dynamic_cast<const City*>(_place.cityResult.value.get()))
				{
					map.insert(PARAMETER_CITY, dynamic_cast<const City*>(_place.cityResult.value.get())->getName());
				}

				if(_page.get())
				{
					map.insert(PARAMETER_PAGE, _page->getKey());
				}
				if(_site.get())
				{
					map.insert(PARAMETER_SITE_ID, _site->getKey());
				}
				map.insert(PARAMETER_TRAIL_BEGINNING, _beginningRank);

				/// @todo ending rank
			}
			return map;
		}

		void HikingTrailMultipleRoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Trail
			try
			{
				_trail = Env::GetOfficialEnv().get<HikingTrail>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch(ObjectNotFoundException<HikingTrail>&)
			{
				throw RequestException("No such trail");
			}

			// Config
			try
			{
				_site = Env::GetOfficialEnv().get<PTServiceConfig>(map.get<RegistryKeyType>(PARAMETER_SITE_ID));
			}
			catch(ObjectNotFoundException<PTServiceConfig>&)
			{
				throw RequestException("No such config");
			}

			// Place
			_place = _site->extendedFetchPlace(
				map.get<string>(PARAMETER_CITY),
				map.getDefault<string>(PARAMETER_STOP)
			);


			// Ranks
			_beginningRank = map.get<size_t>(PARAMETER_TRAIL_BEGINNING);
			if(_beginningRank >= _trail->getStops().size())
			{
				throw RequestException("Beginning rank is too high");
			}
			//_endingRanks = map.get<

			// Dates
			_beginningDay = from_string(map.get<string>(PARAMETER_BEGINNING_DAY));
			_beginningPeriodId = map.get<size_t>(PARAMETER_BEGINNING_PERIOD);
			if (_beginningPeriodId >= _site->get<HourPeriods>().size())
			{
				throw RequestException("Bad value for beginning period id");
			}
			_beginningPeriod = &_site->get<HourPeriods>().at(_beginningPeriodId);

			_endingDay = from_string(map.get<string>(PARAMETER_ENDING_DAY));
			_endingPeriodId = map.get<size_t>(PARAMETER_ENDING_PERIOD);
			if (_endingPeriodId >= _site->get<HourPeriods>().size())
			{
				throw RequestException("Bad value for ending period id");
			}
			_endingPeriod = &_site->get<HourPeriods>().at(_endingPeriodId);

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
				throw RequestException("No such duration page : "+ e.getMessage());
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



		ParametersMap HikingTrailMultipleRoutePlannerFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Declarations
			AlgorithmLogger logger;

			// Beginning journey
			{
				ptime startDate;
				ptime endDate;
				_site->applyPeriod(*_beginningPeriod, startDate, endDate);
				graph::AccessParameters accessParameters(
					_site->getAccessParameters(
						USER_PEDESTRIAN,
						AccessParameters::AllowedPathClasses(),
						AccessParameters::AllowedNetworks()
				)	);

//				if(	_departure_place.placeResult.value &&
//					_arrival_place.placeResult.value &&
//					!_departure_place.placeResult.value->getPoint().isUnknown() &&
//					!_arrival_place.placeResult.value->getPoint().isUnknown()
//				){
//					_endArrivalDate += minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000));
//				}

				// Initialisation
				PTTimeSlotRoutePlanner r(
					_place.placeResult.value.get(),
					_trail->getStops().at(_beginningRank),
					startDate,
					endDate,
					startDate,
					endDate,
					optional<size_t>(),
					accessParameters,
					DEPARTURE_FIRST,
					false,
					logger
				);

				// Computing
				PTRoutePlannerResult result = r.run();

				// Display
/*				RoutePlannerInterfacePage::Display(
					stream,
					_page,
					_schedulesRowPage,
					_schedulesCellPage,
					_linesRowPage,
					_lineMarkerPage,
					_boardPage,
					_warningPage,
					_reservationPage,
					_durationPage,
					_textDurationPage,
					_mapPage,
					_mapLinePage,
					_dateTimePage,
					_stopCellPage,
					_serviceCellPage,
					_junctionPage,
					_mapStopCellPage,
					_mapServiceCellPage,
					_mapJunctionPage,
					request,
					result,
					_beginningDay,
					_beginningPeriodId,
					_place.placeResult.value.get(),
					_trail->getStops().at(_beginningRank),
					_beginningPeriod,
					accessParameters
				);
*/			}

			// Ending journeys
			BOOST_FOREACH(size_t rank, _endingRanks)
			{
				ptime startDate;
				ptime endDate;
				_site->applyPeriod(*_endingPeriod, startDate, endDate);
				graph::AccessParameters accessParameters(
					_site->getAccessParameters(
						USER_PEDESTRIAN,
						AccessParameters::AllowedPathClasses(),
						AccessParameters::AllowedNetworks()
				)	);

				// Initialisation
				PTTimeSlotRoutePlanner r(
					_trail->getStops().at(rank),
					_place.placeResult.value.get(),
					startDate,
					endDate,
					startDate,
					endDate,
					optional<size_t>(),
					accessParameters,
					DEPARTURE_FIRST,
					false,
					logger
				);

				// Computing
				PTRoutePlannerResult result = r.run();

				// Display
/*				RoutePlannerInterfacePage::Display(
					stream,
					_page,
					_schedulesRowPage,
					_schedulesCellPage,
					_linesRowPage,
					_lineMarkerPage,
					_boardPage,
					_warningPage,
					_reservationPage,
					_durationPage,
					_textDurationPage,
					_mapPage,
					_mapLinePage,
					_dateTimePage,
					_stopCellPage,
					_serviceCellPage,
					_junctionPage,
					_mapStopCellPage,
					_mapServiceCellPage,
					_mapJunctionPage,
					request,
					result,
					_endingDay,
					_endingPeriodId,
					_trail->getStops().at(rank),
					_place.placeResult.value.get(),
					_endingPeriod,
					accessParameters
				);
*/			}

			return ParametersMap();
		}



		bool HikingTrailMultipleRoutePlannerFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string HikingTrailMultipleRoutePlannerFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/html";
		}
}	}
