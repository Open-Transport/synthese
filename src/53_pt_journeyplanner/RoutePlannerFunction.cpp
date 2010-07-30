
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
#include "RoutePlannerInterfacePage.h"
#include "PTTimeSlotRoutePlanner.h"
#include "RequestException.h"
#include "Request.h"
#include "Interface.h"
#include "ObjectNotFoundException.h"
#include "GeoPoint.h"
#include "Projection.h"
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

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace interfaces;
	using namespace transportwebsite;
	using namespace db;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace algorithm;
	using namespace pt_journeyplanner;
	using namespace resa;
	using namespace pt;
	using namespace cms;

	template<> const string util::FactorableTemplate<pt_journeyplanner::RoutePlannerFunction::_FunctionWithSite,pt_journeyplanner::RoutePlannerFunction>::FACTORY_KEY("rp");

	namespace pt_journeyplanner
	{
		const string RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER("msn");
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
						!_departure_place.placeResult.value->getPoint().isUnknown() &&
						!_arrival_place.placeResult.value->getPoint().isUnknown()
					){
						_endArrivalDate += minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000));
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
							!_departure_place.placeResult.value->getPoint().isUnknown() &&
							!_arrival_place.placeResult.value->getPoint().isUnknown()
						){
							_endArrivalDate += minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000));
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
							!_departure_place.placeResult.value->getPoint().isUnknown() &&
							!_arrival_place.placeResult.value->getPoint().isUnknown()
						){
							_startDate -= minutes(2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000));
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

			if(	!_departure_place.placeResult.value || !_arrival_place.placeResult.value
			){
				throw RequestException("No calculation");
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
				_departure_place.placeResult.value,
				_arrival_place.placeResult.value,
				startDate,
				endDate,
				_planningOrder == DEPARTURE_FIRST ? _startArrivalDate : startDate,
				_endArrivalDate,
				_planningOrder == DEPARTURE_FIRST ? _maxSolutionsNumber : optional<size_t>(),
				_accessParameters,
				DEPARTURE_FIRST
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
				RoutePlannerInterfacePage::Display(
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
					_startDate.date(),
					_periodId,
					_departure_place.placeResult.value,
					_arrival_place.placeResult.value,
					_period,
					_accessParameters
				);
			}
			else
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
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
				if(dynamic_cast<const Place*>(_departure_place.cityResult.value) != dynamic_cast<const Place*>(_departure_place.placeResult.value))
				{
					stream <<
						" departureStop=\"" << _departure_place.placeResult.key.getSource() << "\" departureStopNameTrust=\"" << _departure_place.placeResult.score.phoneticScore << "\""
					;
				}
				if(dynamic_cast<const Place*>(_arrival_place.cityResult.value) != dynamic_cast<const Place*>(_arrival_place.placeResult.value))
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

					if(journey.getReservationCompliance() != false)
					{
						set<const ReservationContact*> resaRules;
						BOOST_FOREACH(const ServicePointer& su, journey.getServiceUses())
						{
							const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(su.getService()->getPath()));
							if(line == NULL) continue;

							if(	line->getCommercialLine()->getReservationContact() &&
								UseRule::IsReservationPossible(su.getUseRule().getReservationAvailability(su))
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
							" type=\"" << (journey.getReservationCompliance() == true ? "compulsory" : "optional") << "\""
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
								)?
								dynamic_cast<const NamedPlace*>(_departure_place.placeResult.value) :
								dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub())
							);

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
								itl == jl.end()-1 && dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getHub()) ?
								dynamic_cast<const NamedPlace*>(_arrival_place.placeResult.value) :
								dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub())
							);
							
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
											if(dynamic_cast<const NamedPlace*>(jl.begin()->getDepartureEdge()->getHub()))
											{
												_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*jl.begin()->getDepartureEdge()->getHub()));
											}
											else if(dynamic_cast<const Address*>(jl.begin()->getDepartureEdge()->getFromVertex()))
											{
												if(dynamic_cast<const RoadPlace*>(_departure_place.placeResult.value))
												{
													_XMLDisplayRoadPlace(
														stream,
														dynamic_cast<const RoadPlace&>(*_departure_place.placeResult.value)
														);
												}
												else
												{
													_XMLDisplayAddress(
														stream,
														*dynamic_cast<const Address*>(jl.begin()->getDepartureEdge()->getFromVertex()),
														*road->getRoadPlace()
														);
												}
											}
											stream <<
												"</startAddress>" <<
												"<endAddress>";
											if(dynamic_cast<const NamedPlace*>((itl-1)->getArrivalEdge()->getHub()))
											{
												_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*(itl-1)->getArrivalEdge()->getHub()));
											}
											else if(dynamic_cast<const Address*>((itl-1)->getArrivalEdge()->getFromVertex()))
											{
												_XMLDisplayAddress(
													stream,
													*dynamic_cast<const Address*>((itl-1)->getArrivalEdge()->getFromVertex()),
													*road->getRoadPlace()
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
							_XMLDisplayPhysicalStop(stream, "startStop", dynamic_cast<const StopPoint&>(*curET.getDepartureEdge()->getFromVertex()));
							_XMLDisplayPhysicalStop(stream, "endStop", dynamic_cast<const StopPoint&>(*curET.getArrivalEdge()->getFromVertex()));
							_XMLDisplayPhysicalStop(stream, "destinationStop", dynamic_cast<const StopPoint&>(*line->getLastEdge()->getFromVertex()));
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
									_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*curET.getDepartureEdge()->getHub()));
								}
								else if(dynamic_cast<const Address*>(curET.getDepartureEdge()->getFromVertex()))
								{
									if(itl == jl.begin() && dynamic_cast<const RoadPlace*>(_departure_place.placeResult.value))
									{
										_XMLDisplayRoadPlace(
											stream,
											dynamic_cast<const RoadPlace&>(*_departure_place.placeResult.value)
											);
									}
									else
									{
										_XMLDisplayAddress(
											stream,
											*dynamic_cast<const Address*>(curET.getDepartureEdge()->getFromVertex()),
											*road->getRoadPlace()
											);
									}
								}
								stream <<
									"</startAddress>" <<
									"<endAddress>";
								if(dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub()))
								{
									_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*curET.getArrivalEdge()->getHub()));
								}
								else if(dynamic_cast<const Address*>(curET.getArrivalEdge()->getFromVertex()))
								{
									if(itl == jl.end() - 1 && dynamic_cast<const RoadPlace*>(_arrival_place.placeResult.value))
									{
										_XMLDisplayRoadPlace(
											stream,
											dynamic_cast<const RoadPlace&>(*_arrival_place.placeResult.value)
											);
									}
									else
									{
										_XMLDisplayAddress(
											stream,
											*dynamic_cast<const Address*>(curET.getArrivalEdge()->getFromVertex()),
											*road->getRoadPlace()
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
					}

					// Bug 7694 : fulfill unused arrival rows
					for(++itSheetRow; itSheetRow != sheetRows.end(); ++itSheetRow)
					{
						**itSheetRow << "<cell />";
					}

					if(!_outputRoadApproachDetail && lastApproachBeginning != jl.end())
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
						if(dynamic_cast<const NamedPlace*>(lastApproachBeginning->getDepartureEdge()->getHub()))
						{
							_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*lastApproachBeginning->getDepartureEdge()->getHub()));
						}
						else if(dynamic_cast<const Address*>(lastApproachBeginning->getDepartureEdge()->getFromVertex()))
						{
							_XMLDisplayAddress(
								stream,
								*dynamic_cast<const Address*>(lastApproachBeginning->getDepartureEdge()->getFromVertex()),
								*road->getRoadPlace()
							);
						}
						stream <<
							"</startAddress>" <<
							"<endAddress>";
						if(dynamic_cast<const NamedPlace*>((jl.end()-1)->getArrivalEdge()->getHub()))
						{
							_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*(jl.end()-1)->getArrivalEdge()->getHub()));
						}
						else if(dynamic_cast<const Address*>((jl.end()-1)->getArrivalEdge()->getFromVertex()))
						{
							if(dynamic_cast<const RoadPlace*>(_arrival_place.placeResult.value))
							{
								_XMLDisplayRoadPlace(
									stream,
									dynamic_cast<const RoadPlace&>(*_arrival_place.placeResult.value)
								);
							}
							else
							{
								_XMLDisplayAddress(
									stream,
									*dynamic_cast<const Address*>((jl.end()-1)->getArrivalEdge()->getFromVertex()),
									*road->getRoadPlace()
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
					"</journeys>" <<
					"<resultTable>";

				PlacesContentVector::iterator itSheetRow(sheetRows.begin());
				BOOST_FOREACH(const PTRoutePlannerResult::PlacesList::value_type& row, result.getOrderedPlaces())
				{
					GeoPoint gp(WGS84FromLambert(row.place->getPoint()));
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
						_XMLDisplayRoadPlace(stream, dynamic_cast<const RoadPlace&>(*row.place));
					}
					else
					{
						_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*row.place));
					}

					stream <<
						"</place>" <<
						"</row>";
					++itSheetRow;
				}
				stream <<
					"</resultTable>" <<
					"</routePlannerResult>"
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
			const NamedPlace& np
		){
			GeoPoint gp(WGS84FromLambert(np.getPoint()));

			stream <<
				"<connectionPlace" <<
					" latitude=\"" << gp.getLatitude() << "\"" <<
					" longitude=\"" << gp.getLongitude() << "\"" <<
					" id=\"" << np.getKey() << "\"" <<
					" city=\"" << np.getCity()->getName() << "\"" <<
					" x=\"" << static_cast<int>(np.getPoint().getX()) << "\"" <<
					" y=\"" << static_cast<int>(np.getPoint().getY()) << "\""
					" name=\"" << np.getName() << "\"" <<
				">";
			if(false) // Test if alarm on place
			{
				stream <<
					"<alert" <<
						" id=\"9834\"" <<
						" level=\"interruption\"" <<
						" startValidity=\"2000-01-01T00:00:07.0Z\"" <<
						" endValidity=\"2099-12-31T23:59:00.0Z\"" <<
					">SNCF en grève</alert>"
				;
			}
			stream << "</connectionPlace>";
		}



		void RoutePlannerFunction::_XMLDisplayPhysicalStop(
			std::ostream& stream,
			const std::string& tag,
			const pt::StopPoint& stop
		){
			GeoPoint gp(WGS84FromLambert(stop));

			stream <<
				"<" << tag <<
					" latitude=\"" << gp.getLatitude() << "\"" <<
					" longitude=\"" << gp.getLongitude() << "\"" <<
					" id=\"" << stop.getKey() << "\"" <<
					" x=\"" << static_cast<int>(stop.getX()) << "\"" <<
					" y=\"" << static_cast<int>(stop.getY()) << "\"" <<
					" name=\"" << stop.getName() << "\"" <<
				">";
			_XMLDisplayConnectionPlace(stream, dynamic_cast<const NamedPlace&>(*stop.getHub()));
			stream << "</" << tag << ">";
		}



		void RoutePlannerFunction::_XMLDisplayAddress(
			std::ostream& stream,
			const road::Address& address,
			const road::RoadPlace& roadPlace
		){
			GeoPoint gp(WGS84FromLambert(address));

			stream <<
				"<address" <<
				" latitude=\"" << gp.getLatitude() << "\"" <<
				" longitude=\"" << gp.getLongitude() << "\"" <<
				" id=\"" << address.getKey() << "\"" <<
				" x=\"" << static_cast<int>(address.getX()) << "\"" <<
				" y=\"" << static_cast<int>(address.getY()) << "\"" <<
				" city=\"" << roadPlace.getCity()->getName() << "\"" <<
				" streetName=\"" << roadPlace.getName() << "\"" <<
				" />";
		}



		void RoutePlannerFunction::_XMLDisplayRoadPlace(
			std::ostream& stream,
			const road::RoadPlace& roadPlace
		){
			GeoPoint gp(WGS84FromLambert(roadPlace.getPoint()));

			stream <<
				"<address" <<
				" latitude=\"" << gp.getLatitude() << "\"" <<
				" longitude=\"" << gp.getLongitude() << "\"" <<
				" id=\"" << roadPlace.getKey() << "\"" <<
				" x=\"" << static_cast<int>(roadPlace.getPoint().getX()) << "\"" <<
				" y=\"" << static_cast<int>(roadPlace.getPoint().getY()) << "\"" <<
				" city=\"" << roadPlace.getCity()->getName() << "\"" <<
				" streetName=\"" << roadPlace.getName() << "\"" <<
				" />";
		}
	}
}
