
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
#include "Site.h"
#include "HourPeriod.h"
#include "RoutePlannerInterfacePage.h"
#include "PTTimeSlotRoutePlanner.h"
#include "RequestException.h"
#include "Request.h"
#include "Interface.h"
#include "TimeParseException.h"
#include "ObjectNotFoundException.h"
#include "GeoPoint.h"
#include "Projection.h"
#include "PTRoutePlannerResult.h"
#include "Edge.h"
#include "Line.h"
#include "Road.h"
#include "RoadPlace.h"
#include "Hub.h"
#include "Service.h"
#include "CommercialLine.h"
#include "RollingStock.h"
#include "RGBColor.h"
#include "Crossing.h"
#include "PhysicalStop.h"
#include "OnlineReservationRule.h"
#include "ReservationContact.h"
#include "RollingStockFilter.h"
#include "Address.h"
#include "ContinuousService.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace env;
	using namespace time;
	using namespace interfaces;
	using namespace transportwebsite;
	using namespace db;
	using namespace graph;
	using namespace geography;
	using namespace road;
	using namespace algorithm;
	using namespace ptrouteplanner;
	using namespace resa;

	template<> const string util::FactorableTemplate<transportwebsite::FunctionWithSite,routeplanner::RoutePlannerFunction>::FACTORY_KEY("rp");

	namespace routeplanner
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



		ParametersMap RoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSite::_getParametersMap());
			return map;
		}



		void RoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			FunctionWithSite::_setFromParametersMap(map);
			if(_site->getInterface())
			{
				_page = _site->getInterface()->getPage<RoutePlannerInterfacePage>();
			}

			// Origin and destination places
			optional<RegistryKeyType> favoriteId(map.getOptional<RegistryKeyType>(PARAMETER_FAVORITE_ID));
			if (favoriteId) // 2b
			{
				try
				{
					_favorite = UserFavoriteJourneyTableSync::Get(*favoriteId, Env::GetOfficialEnv());
					_originCityText = _favorite->getOriginCityName();
					_originPlaceText = _favorite->getOriginPlaceName();
					_destinationCityText = _favorite->getDestinationCityName();
					_destinationPlaceText = _favorite->getDestinationPlaceName();
				}
				catch(ObjectNotFoundException<UserFavoriteJourney> e)
				{
					throw RequestException(e.getMessage());
				}
			}
			else // 2a
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
				_departure_place = _site->extendedFetchPlace(_originCityText, _originPlaceText);
				_arrival_place = _site->extendedFetchPlace(_destinationCityText, _destinationPlaceText);
			}

			try
			{
				// Date
				Date day(map.getDate(PARAMETER_DAY, false, string()));
				if (day.isUnknown()) // 1b
				{
					if(	map.getDateTime(PARAMETER_HIGHEST_ARRIVAL_TIME, false, string()).isUnknown()
					){ // All default values
						_planningOrder = DEPARTURE_FIRST;
						_startDate = map.getDateTime(PARAMETER_LOWEST_DEPARTURE_TIME, false, string());
						if(_startDate.isUnknown())
						{
							_startDate = DateTime(TIME_CURRENT);
						}
						_startArrivalDate = _startDate;
						_endDate = map.getDateTime(PARAMETER_HIGHEST_DEPARTURE_TIME, false, string());
						if(_endDate.isUnknown())
						{
							_endDate = _startDate;
							_endDate.addDaysDuration(1);
						}
						_endArrivalDate = _endDate;
						if(	_departure_place.placeResult.value &&
							_arrival_place.placeResult.value &&
							!_departure_place.placeResult.value->getPoint().isUnknown() &&
							!_arrival_place.placeResult.value->getPoint().isUnknown()
						){
							_endArrivalDate += 2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000);
						}
					}
					else if(map.getDateTime(PARAMETER_LOWEST_DEPARTURE_TIME, false, string()).isUnknown())
					{ // Arrival to departure from the specified arrival time
						_planningOrder = ARRIVAL_FIRST;
						_endArrivalDate = map.getDateTime(PARAMETER_HIGHEST_ARRIVAL_TIME, false, string());
						_startArrivalDate = map.getDateTime(PARAMETER_LOWEST_ARRIVAL_TIME, false, string());
						if(_startArrivalDate.isUnknown())
						{
							_startArrivalDate = _endArrivalDate;
							_startArrivalDate.subDaysDuration(1);
						}
						_startDate = _startArrivalDate;
						if(	_departure_place.placeResult.value &&
							_arrival_place.placeResult.value &&
							!_departure_place.placeResult.value->getPoint().isUnknown() &&
							!_arrival_place.placeResult.value->getPoint().isUnknown()
						){
							_startDate -= 2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000);
						}
					}
				}
				else // 1a
				{
					_planningOrder = DEPARTURE_FIRST;
					_periodId = map.get<size_t>(PARAMETER_PERIOD_ID);
					if (_periodId >= _site->getPeriods().size())
						throw RequestException("Bad value for period id");
					_startDate = DateTime(day, Hour(0, 0));
					_endDate = _startDate;
					_period = &_site->getPeriods().at(_periodId);
					_site->applyPeriod(*_period, _startDate, _endDate);
					_startArrivalDate = _startDate;
					_endArrivalDate = _endDate;
					if(	_departure_place.placeResult.value &&
						_arrival_place.placeResult.value &&
						!_departure_place.placeResult.value->getPoint().isUnknown() &&
						!_arrival_place.placeResult.value->getPoint().isUnknown()
					){
						_endArrivalDate += 2 * static_cast<int>(_departure_place.placeResult.value->getPoint().getDistanceTo(_arrival_place.placeResult.value->getPoint()) / 1000);
					}
				}
			}
			catch(Site::ForbiddenDateException)
			{
				throw RequestException("Date in the past is forbidden");
			}
			catch (time::TimeParseException)
			{
				throw RequestException("Bad date");
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
			catch(ObjectNotFoundException<RollingStockFilter>& e)
			{
			}

			// Max solutions number
			_maxSolutionsNumber = map.getOptional<size_t>(PARAMETER_MAX_SOLUTIONS_NUMBER);

			// Accessibility
			optional<unsigned int> acint(map.getOptional<unsigned int>(PARAMETER_ACCESSIBILITY));
			_accessParameters = _site->getAccessParameters(
				acint ? static_cast<UserClassCode>(*acint) : USER_PEDESTRIAN,
				_rollingStockFilter.get() ? _rollingStockFilter->getAllowedPathClasses() : AccessParameters::AllowedPathClasses()
			);

			if(	(!_departure_place.placeResult.value || !_arrival_place.placeResult.value) &&
				!_page
			){
				throw RequestException("No calculation");
			}

		}



		void RoutePlannerFunction::run(
			ostream& stream,
			const Request& request
		) const	{
			VariablesMap vm;
			if (_departure_place.placeResult.value && _arrival_place.placeResult.value)
			{
				// Initialisation
				PTTimeSlotRoutePlanner r(
					_departure_place.placeResult.value,
					_arrival_place.placeResult.value,
					_startDate,
					_endDate,
					_startArrivalDate,
					_endArrivalDate,
					_maxSolutionsNumber,
					_accessParameters,
					_planningOrder
				);
				
				// Computing
				const PTRoutePlannerResult result(r.run());
				
				// Display
				if(_page)
				{
					_page->display(
						stream
						, vm
						, result
						, _startDate.getDate()
						, _periodId
						, _departure_place.placeResult.value
						, _arrival_place.placeResult.value
						, _period
						, _accessParameters
						, &request
						, _site.get()
						, result.getSamePlaces()
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
						stream << " maxSolutions=\"" << _maxSolutionsNumber << "\"";
					}
					stream << " userProfile=\"" << _accessParameters.getUserClass() << "\"";
					if(request.getSession())
					{
						stream << " sessionId=\"" << request.getSession()->getKey() << "\"";
					}
					stream <<
						" siteId=\"" << _site->getKey() << "\">" <<
						"<timeBounds" <<
							" minDepartureHour=\"" << posix_time::to_iso_extended_string(r.getLowestDepartureTime().toPosixTime()) << "\"" <<
							" minArrivalHour=\"" << posix_time::to_iso_extended_string(r.getLowestArrivalTime().toPosixTime()) << "\"" <<
							" maxArrivalHour=\"" << posix_time::to_iso_extended_string(r.getHighestArrivalTime().toPosixTime()) << "\"" <<
							" maxDepartureHour=\"" << posix_time::to_iso_extended_string(r.getHighestDepartureTime().toPosixTime()) << "\"" <<
						" />"
					;
					if(_period)
					{
						stream <<
							"<timePeriod id=\"" << _periodId << "\" date=\"" <<
							to_iso_extended_string(_startDate.getDate().toGregorianDate()) << "\" name=\"" <<
							_period->getCaption() << "\" />"
						;
					}
					stream <<
						"<places departureCity=\"" << _departure_place.cityResult.key.getSource() << "\" departureCityNameTrust=\"" << _departure_place.cityResult.score << "\"" <<
						" arrivalCity=\"" << _arrival_place.cityResult.key.getSource() << "\" arrivalCityNameTrust=\"" << _arrival_place.cityResult.score << "\""
					;
					if(dynamic_cast<const Place*>(_departure_place.cityResult.value) != dynamic_cast<const Place*>(_departure_place.placeResult.value))
					{
						stream <<
							" departureStop=\"" << _departure_place.placeResult.key.getSource() << "\" departureStopNameTrust=\"" << _departure_place.placeResult.score << "\""
						;
					}
					if(dynamic_cast<const Place*>(_arrival_place.cityResult.value) != dynamic_cast<const Place*>(_arrival_place.placeResult.value))
					{
						stream <<
							" arrivalStop=\"" << _arrival_place.placeResult.key.getSource() << "\" arrivalStopNameTrust=\"" << _arrival_place.placeResult.score << "\""
						;
					}
					stream << " />";
					if(_favorite.get())
					{
						stream <<
							"<favorite id=\"" << _favorite->getKey() << "\" />"
						;
					}

					if(_rollingStockFilter.get())
					{
						stream << "<transportModeFilter id=\"" << _rollingStockFilter->getKey() << "\" name=\"" << _rollingStockFilter->getName() << "\"/>";
					}

					stream <<
						"</query>" <<
						"<journeys>"
					;
					const PTRoutePlannerResult::PlaceList& placesList(
						result.getOrderedPlaces()
					);
					typedef vector<ostringstream*> PlacesContentVector;
					PlacesContentVector sheetRows(placesList.size());
					BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
					{
						stream = new ostringstream;
					}

					BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, result.getJourneys())
					{
						bool hasALineAlert(false); // Interactive
						bool hasAStopAlert(false); // Interactive
						bool pedestrianMode = false;
						bool lastPedestrianMode = false;

						PlacesContentVector::iterator itSheetRow(sheetRows.begin());
						PTRoutePlannerResult::PlaceList::const_iterator itPlaces(placesList.begin());

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
							BOOST_FOREACH(const ServiceUse& su, journey.getServiceUses())
							{
								const Line* line(dynamic_cast<const Line*>(su.getService()->getPath()));
								if(line == NULL) continue;

								if(	line->getCommercialLine()->getReservationContact() &&
									UseRule::IsReservationPossible(su.getUseRule()->getReservationAvailability(su))
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
							stream << " deadLine=\"" << posix_time::to_iso_extended_string(journey.getReservationDeadLine().toPosixTime()) << "\" />";
						}
						stream << "<chunks>";

						// Loop on each leg
						const Journey::ServiceUses& jl(journey.getServiceUses());
						for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
						{
							const ServiceUse& curET(*itl);

							if(	itl == jl.begin() ||
								!curET.getEdge()->getParentPath()->isPedestrianMode() ||
								lastPedestrianMode != curET.getEdge()->getParentPath()->isPedestrianMode()
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

								pedestrianMode = curET.getEdge()->getParentPath()->isPedestrianMode();
								
								// Saving of the columns on each lines
								if(itl == jl.begin())
								{
									**itSheetRow << "<cell";
								}
								**itSheetRow <<
									" departureDateTime=\"" <<
									posix_time::to_iso_extended_string(curET.getDepartureDateTime().toPosixTime()) << "\"";
								if(journey.getContinuousServiceRange().total_seconds() > 0)
								{
									posix_time::ptime edTime(curET.getDepartureDateTime().toPosixTime());
									edTime += journey.getContinuousServiceRange();
									**itSheetRow << " endDepartureDateTime=\"" << 
										posix_time::to_iso_extended_string(edTime) << "\"";
								}
								**itSheetRow << " />";

								++itPlaces; ++itSheetRow;
								lastPedestrianMode = pedestrianMode;
							}
							
							if(	itl == jl.end()-1
							||	!(itl+1)->getEdge()->getParentPath()->isPedestrianMode()
							||	!curET.getEdge()->getParentPath()->isPedestrianMode()
							){
								const NamedPlace* placeToSearch(
									itl == jl.end()-1 && dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getHub()) ?
									dynamic_cast<const NamedPlace*>(_arrival_place.placeResult.value) :
									dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub())
								);
								
								for (; itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow )
								{
									**itSheetRow << "<cell />";
								}
								**itSheetRow << "<cell arrivalDateTime=\"" <<
									posix_time::to_iso_extended_string(curET.getArrivalDateTime().toPosixTime()) << "\"";
								if(journey.getContinuousServiceRange().total_seconds() > 0)
								{
									posix_time::ptime eaTime(curET.getArrivalDateTime().toPosixTime());
									eaTime += journey.getContinuousServiceRange();
									**itSheetRow << " endArrivalDateTime=\"" <<
										posix_time::to_iso_extended_string(eaTime) << "\"";
								}
								if(	itl == jl.end() - 1)
								{
									**itSheetRow << " />";
								}
							}


							const Line* line(dynamic_cast<const Line*> (curET.getService()->getPath()));
							if(line != NULL)
							{
								stream <<
									"<" << (line->isPedestrianMode() ? "connection" : "transport") <<
										" length=\"" << ceil(curET.getDistance()) << "\"" <<
										" departureTime=\"" << posix_time::to_iso_extended_string(curET.getDepartureDateTime().toPosixTime()) << "\"" <<
										" arrivalTime=\"" << posix_time::to_iso_extended_string(curET.getArrivalDateTime().toPosixTime()) << "\"";
								if(journey.getContinuousServiceRange().total_seconds() > 0)
								{
									posix_time::ptime edTime(curET.getDepartureDateTime().toPosixTime());
									edTime += journey.getContinuousServiceRange();
									posix_time::ptime eaTime(curET.getArrivalDateTime().toPosixTime());
									eaTime += journey.getContinuousServiceRange();
									stream <<
										" endDepartureTime=\"" << posix_time::to_iso_extended_string(edTime) << "\"" <<
										" endArrivalTime=\"" << posix_time::to_iso_extended_string(eaTime) << "\"";
								}
								const ContinuousService* cserv(dynamic_cast<const ContinuousService*>(curET.getService()));
								if(cserv && cserv->getMaxWaitingTime() > 0)
								{
									stream << " possibleWaitingTime=\"" << cserv->getMaxWaitingTime() << "\"";
								}
								stream <<
										" startStopIsTerminus=\"" << (curET.getDepartureEdge()->getRankInPath() == 0 ? "true" : "false") << "\"" <<
										" endStopIsTerminus=\"" << (curET.getArrivalEdge()->getRankInPath() == curET.getArrivalEdge()->getParentPath()->getEdges().size() - 1 ? "true" : "false") << "\"";
								if(!line->getDirection().empty())
								{
									stream << " destinationText=\"" << line->getDirection() << "\"";
								}
								stream <<
									">";
								_XMLDisplayPhysicalStop(stream, "startStop", dynamic_cast<const PhysicalStop&>(*curET.getDepartureEdge()->getFromVertex()));
								_XMLDisplayPhysicalStop(stream, "endStop", dynamic_cast<const PhysicalStop&>(*curET.getArrivalEdge()->getFromVertex()));
								_XMLDisplayPhysicalStop(stream, "destinationStop", dynamic_cast<const PhysicalStop&>(*line->getLastEdge()->getFromVertex()));
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
								stream << 
									"<street" <<
										" length=\"" << ceil(curET.getDistance()) << "\"" <<
										" city=\"" << road->getRoadPlace()->getCity()->getName() << "\"" <<
										" name=\"" << road->getRoadPlace()->getName() << "\"" <<
										" departureTime=\"" << posix_time::to_iso_extended_string(curET.getDepartureDateTime().toPosixTime()) << "\"" <<
										" arrivalTime=\"" << posix_time::to_iso_extended_string(curET.getArrivalDateTime().toPosixTime()) << "\"";
								if(journey.getContinuousServiceRange().total_seconds() > 0)
								{
									posix_time::ptime edTime(curET.getDepartureDateTime().toPosixTime());
									edTime += journey.getContinuousServiceRange();
									posix_time::ptime eaTime(curET.getArrivalDateTime().toPosixTime());
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
						}

						stream << "</chunks></journey>";
					}
					stream <<
						"</journeys>" <<
						"<resultTable>";

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					BOOST_FOREACH(const PTRoutePlannerResult::PlaceList::value_type& row, result.getOrderedPlaces())
					{
						GeoPoint gp(WGS84FromLambert(row.place->getPoint()));
						assert(dynamic_cast<const NamedPlace*>(row.place));
						const NamedPlace* np(dynamic_cast<const NamedPlace*>(row.place));

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
			else if(_page)
			{
				_page->display(
					stream
					, vm
					, _startDate.getDate()
					, _periodId
					, _home
					, _originCityText
					, _originPlaceText
					, _destinationCityText
					, _destinationPlaceText
					, _period
					, _accessParameters
					, &request
					, _site.get()
				);
			}
		}



		RoutePlannerFunction::RoutePlannerFunction()
			: _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
			, _period(NULL)
			, _home(false),
			_page(NULL),
			_startArrivalDate(TIME_UNKNOWN),
			_endArrivalDate(TIME_UNKNOWN)
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
					!session->getUser() ||
					_favorite->getUser()->getKey() != session->getUser()->getKey()
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
			const env::PhysicalStop& stop
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
