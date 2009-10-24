
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
#include "RoutePlanner.h"
#include "RequestException.h"
#include "Request.h"
#include "Interface.h"
#include "TimeParseException.h"
#include "ObjectNotFoundException.h"

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

	template<> const string util::FactorableTemplate<transportwebsite::FunctionWithSite,routeplanner::RoutePlannerFunction>::FACTORY_KEY("rp");

	namespace routeplanner
	{
		const string RoutePlannerFunction::PARAMETER_DATE = "da";
		const string RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER("msn");
		const string RoutePlannerFunction::PARAMETER_DAY("dy");
		const string RoutePlannerFunction::PARAMETER_PERIOD_ID("pi");
		const string RoutePlannerFunction::PARAMETER_ACCESSIBILITY("ac");
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT("dct");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT("act");
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT("dpt");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT("apt");
		const string RoutePlannerFunction::PARAMETER_FAVORITE_ID("fid");

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
			_favoriteId = map.getOptional<RegistryKeyType>(PARAMETER_FAVORITE_ID);
			if (_favoriteId)
			{
				try
				{
					Env env;
					shared_ptr<const UserFavoriteJourney> favorite(UserFavoriteJourneyTableSync::Get(*_favoriteId, env));
					if (favorite->getUser()->getKey() != _request->getUser()->getKey())
					{
						throw RequestException("Forbidden favorite");
					}
					_originCityText = favorite->getOriginCityName();
					_originPlaceText = favorite->getOriginPlaceName();
					_destinationCityText = favorite->getDestinationCityName();
					_destinationPlaceText = favorite->getDestinationPlaceName();
				}
				catch(ObjectNotFoundException<UserFavoriteJourney> e)
				{
					throw RequestException(e.getMessage());
				}
			}
			else
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
				if (day.isUnknown())
				{
					_startDate = map.getDateTime(PARAMETER_DATE, !_home, string());
					_endDate = _startDate;
					_endDate.addDaysDuration(1);						
				}
				else
				{
					_periodId = map.get<size_t>(PARAMETER_PERIOD_ID);
					if (_periodId >= _site->getPeriods().size())
						throw RequestException("Bad value for period id");
					_startDate = DateTime(day, Hour(0, 0));
					_endDate = _startDate;
					_period = &_site->getPeriods().at(_periodId);
					_site->applyPeriod(*_period, _startDate, _endDate);
				}
			}
			catch (time::TimeParseException)
			{
				throw RequestException("Bad date");
			}
			catch(ParametersMap::MissingParameterException& e)
			{
				throw RequestException(e.what());
			}

			// Max solutions number
			_maxSolutionsNumber = map.getOptional<size_t>(PARAMETER_MAX_SOLUTIONS_NUMBER);

			// Accessibility
			optional<unsigned int> acint(map.getOptional<unsigned int>(PARAMETER_ACCESSIBILITY));
			_accessParameters = _site->getAccessParameters(
				acint ? static_cast<UserClassCode>(*acint) : USER_PEDESTRIAN
			);
		}

		void RoutePlannerFunction::_run( ostream& stream ) const
		{
			VariablesMap vm;
			if (_departure_place.placeResult.value && _arrival_place.placeResult.value)
			{
				// Initialisation
				RoutePlanner r(
					_departure_place.placeResult.value
					, _arrival_place.placeResult.value
					, _accessParameters
					, PlanningOrder()
					, _startDate
					, _endDate
					, _maxSolutionsNumber
				);
				
				// Computing
				const RoutePlanner::Result& jv(r.computeJourneySheetDepartureArrival());
				
				// Build of the result object
				RoutePlannerResult result;
				result.result = jv.journeys;
				result.departurePlace = _departure_place.placeResult.value;
				result.arrivalPlace = _arrival_place.placeResult.value;
				
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
						, _request
						, _site.get()
						, jv.samePlaces
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
					if(_request->getSession())
					{
						stream << " sessionId=\"" << _request->getSession()->getKey() << "\"";
					}
					stream <<
						" siteId=\"" << _site->getKey() << "\">" <<
						"<timeBounds minDepartureHour=\"" << posix_time::to_iso_extended_string(r.getJourneySheetEndDepartureTime().toPosixTime()) << "\"" <<
						" minArrivalHour=\"" << posix_time::to_iso_extended_string(r.getJourneySheetStartArrivalTime().toPosixTime()) << "\"" <<
						" maxArrivalHour=\"" << posix_time::to_iso_extended_string(r.getJourneySheetEndArrivalTime().toPosixTime()) << "\"" <<
						" minDepartureHour=\"" << posix_time::to_iso_extended_string(r.getJourneySheetStartDepartureTime().toPosixTime()) << "\" />"
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
					if(_favoriteId)
					{
						stream <<
							"<favorite id=\"" << *_favoriteId << "\" />"
						;
					}
/*
					<transportModeFilter id=\"2147483647\" name=\"Tous modes\"/>
*/
					stream <<
						"</query>" <<
						"<journeys>"
					;
					BOOST_FOREACH(const JourneyBoardJourneys::value_type& journey, jv.journeys)
					{
						bool hasALineAlert(false); // Interactive
						bool hasAStopAlert(false); // Interactive

						stream <<
							"<journey hasALineAlert=\"" << (hasALineAlert ? "true" : "false") << "\" hasAStopAlert=\"" << (hasAStopAlert ? "true" : "false") << "\""
						;
						if(journey->getContinuousServiceRange() != UNKNOWN_VALUE)
						{
							stream << " continuousServiceDuration=\"" << journey->getContinuousServiceRange() << "\"";
						}
						stream << ">";

						if(journey->getReservationCompliance() != false)
						{
							bool online(true); // Interactive
							string openingHours; // Interactive
							string phoneNumber; // Interactive

							stream << "<reservation" <<
								" online=\"" << (online ? "true" : "false") << "\"" <<
								" type=\"" << (journey->getReservationCompliance() == true ? "compulsory" : "optional") << "\""
							;
							if(!openingHours.empty())
							{
								stream << " openingHours=\"" << openingHours << "\"";
							}
							if(!phoneNumber.empty())
							{
								stream << " phoneNumber=\"" << phoneNumber << "\"";
							}
							stream << " deadLine=\"" << posix_time::to_iso_extended_string(journey->getReservationDeadLine().toPosixTime()) << "\" />";
						}
						stream << "<chunks>";
/*
						<transport departureTime=\"2001-12-17T09:30:47.0Z\" arrivalTime=\"2001-12-17T19:30:47.0Z\" length=\"800000\" startStopIsTerminus=\"true\" endStopIsTerminus=\"true\" >
							<startStop latitude=\"0.0\" longitude=\"0.0\" id=\"1\" x=\"0\" y=\"0\" name=\"Quai 1\">
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"1\" city=\"TOULOUSE\" x=\"0\" y=\"0\" name=\"Matabiau\">
								</connectionPlace>
							</startStop>
							<endStop latitude=\"0.0\" longitude=\"0.0\" id=\"2\" x=\"0\" y=\"0\" name=\"Quai 26\">
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"2\" city=\"PARIS\" x=\"0\" y=\"0\" name=\"Montparnasse\">
								</connectionPlace>
							</endStop>
							<line id=\"5675\" color=\"#CCCCCC\" cssClass=\"gris\" imgURL=\"tgv.png\" longName=\"le TGV Atlantique\" shortName=\"TGV\">
							<alert id=\"9834\" level=\"interruption\" startValidity=\"2000-01-01T00:00:07.0Z\" endValidity=\"2099-12-31T23:59:00.0Z\">SNCF en grève</alert>
							</line>
							<vehicleType id=\"456\" name=\"TGV\" />
						</transport>
						<street length=\"600000\" city=\"BORDEAUX\" name=\"Autoroute A10\" departureTime=\"2001-12-17T17:30:47.0Z\" arrivalTime=\"2001-12-17T17:25:47.0Z\">
							<startAddress>
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"1\" x=\"0\" y=\"0\" city=\"BORDEAUX\"  name=\"Saint-Jean\"><alert id=\"1444\" startValidity=\"2001-12-10T09:30:47.0Z\" endValidity=\"2002-12-24T09:30:47.0Z\" level=\"info\">Travaux sur le parvis de la gare</alert>
								</connectionPlace>
							</startAddress>
							<endAddress>
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"1\" city=\"PARIS\" x=\"0\" y=\"0\" name=\"Montparnasse\">
								</connectionPlace>
							</endAddress>
						</street>
						<connection length=\"0\" departureTime=\"2001-12-17T21:00:47.0Z\" arrivalTime=\"2001-12-17T21:30:47.0Z\" endDepartureTime=\"2001-12-17T22:00:47.0Z\" endArrivalTime=\"2001-12-17T22:30:47.0Z\">
							<startStop latitude=\"0.0\" longitude=\"0.0\" id=\"1\" x=\"0\" y=\"0\" name=\"Parvis\">
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"1\" city=\"PARIS\" x=\"0\" y=\"0\" name=\"Austerlitz\">
								</connectionPlace>
							</startStop>
							<endStop latitude=\"0.0\" longitude=\"0.0\" id=\"1\" x=\"0\" y=\"0\" name=\"Parvis\">
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"1\" city=\"PARIS\" x=\"0\" y=\"0\" name=\"Montparnasse\">
								</connectionPlace>
							</endStop>
						</connection>
*/
						stream << "</chunks>";
					}
					stream <<
						"</journeys>" <<
						"<resultTable>";

//					BOOST_FOREACH()
					{
/*						<row type=\"departure\">
							<cells>
								<cell departureDateTime=\"2001-12-17T09:30:47.0Z\" />
								<cell departureDateTime=\"2001-12-17T10:30:47.0Z\" />
								<cell departureDateTime=\"2001-12-17T11:30:47.0Z\" endDepartureDateTime=\"2001-12-17T12:30:47.0Z\" />
							</cells>
							<place>
								<connectionPlace latitude=\"0.0\" longitude=\"0.0\" id=\"1\" city=\"a\" x=\"0\" y=\"0\" name=\"a\">
								</connectionPlace>
							</place>
						</row>
*/
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
					, _request
					, _site.get()
				);
			}
		}



		RoutePlannerFunction::RoutePlannerFunction()
			: _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
			, _period(NULL)
			, _home(false),
			_page(NULL)
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



		bool RoutePlannerFunction::_isAuthorized(
		) const {
			return true;
		}

		std::string RoutePlannerFunction::getOutputMimeType() const
		{
			return _page->getMimeType();
		}
	}
}
