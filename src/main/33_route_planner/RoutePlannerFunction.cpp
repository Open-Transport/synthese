
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

#include "36_places_list/Site.h"
#include "36_places_list/HourPeriod.h"

#include "33_route_planner/RoutePlannerInterfacePage.h"
#include "33_route_planner/RoutePlanner.h"

#include "30_server/RequestException.h"

#include "15_env/EnvModule.h"

#include "11_interfaces/Interface.h"

#include "07_lex_matcher/LexicalMatcher.h"

#include "04_time/TimeParseException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace env;
	using namespace time;
	using namespace interfaces;
	using namespace lexmatcher;

	namespace routeplanner
	{
		const string RoutePlannerFunction::PARAMETER_DATE = "da";
		const string RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER("msn");
		const string RoutePlannerFunction::PARAMETER_DAY("dy");
		const string RoutePlannerFunction::PARAMETER_PERIOD_ID("pi");

		const string RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT("dct");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT("act");
		const string RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT("dpt");
		const string RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT("apt");

		ParametersMap RoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSite::_getParametersMap());
			return map;
		}

		void RoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			FunctionWithSite::_setFromParametersMap(map);

			ParametersMap::const_iterator it;

			_page = _site->getInterface()->getPage<RoutePlannerInterfacePage>();
			_accessParameters = _site->getDefaultAccessParameters();

			// Temporary
			ReservationRule* resa(new ReservationRule);
			resa->setCompliant(boost::logic::indeterminate);
			_accessParameters.complyer.setReservationRule(resa);

			// Departure place
			it = map.find(PARAMETER_DEPARTURE_CITY_TEXT);
			if (it != map.end())
			{
				shared_ptr<const City> city;
				_originCityText = it->second;
				CityList cityList = EnvModule::guessCity(_originCityText, 1);
				if (!cityList.empty())
				{
					city = cityList.front();
					_departure_place = city.get();
				}

				it = map.find(PARAMETER_DEPARTURE_PLACE_TEXT);
				if (it != map.end())
				{
					_originPlaceText = it->second;
					if (city.get() && !_originPlaceText.empty())
					{
						LexicalMatcher<const ConnectionPlace*>::MatchResult places = city->getConnectionPlacesMatcher().bestMatches(_originPlaceText, 1);
						if (!places.empty())
						{
							_departure_place = places.front().value;
						}
					}
				}
			}
			else
				_home = true;

			// Arrival place
			it = map.find(PARAMETER_ARRIVAL_CITY_TEXT);
			if (it != map.end())
			{
				_destinationCityText = it->second;
				shared_ptr<const City> city;
				CityList cityList = EnvModule::guessCity(_destinationCityText, 1);
				if (!cityList.empty())
				{
					city = cityList.front();
					_arrival_place = city.get();
				}

				it = map.find(PARAMETER_ARRIVAL_PLACE_TEXT);
				if (it != map.end())
				{
					_destinationPlaceText = it->second;
					if (city.get() && !_destinationPlaceText.empty())
					{
						LexicalMatcher<const ConnectionPlace*>::MatchResult places = city->getConnectionPlacesMatcher().bestMatches(_destinationPlaceText, 1);
						if (!places.empty())
						{
							_arrival_place = places.front().value;
						}
					}
				}
			}
			else
				_home = true;

			// Day
			it = map.find(PARAMETER_DAY);
			if (it != map.end())
			{
				try
				{
					Date day(Date::FromInternalString(it->second));
					it = map.find(PARAMETER_PERIOD_ID);
					if (it == map.end())
						throw RequestException("Period not specified");
					_periodId = Conversion::ToInt(it->second);
					if (_periodId < 0 || _periodId >= _site->getPeriods().size())
						throw RequestException("Bad value for period id");
					_startDate = DateTime(day, Hour(0, 0));
					_endDate = _startDate;
					_period = &_site->getPeriods().at(_periodId);
					_site->applyPeriod(*_period, _startDate, _endDate);
				}
				catch (time::TimeParseException)
				{
					throw RequestException("Bad date");
				}
			}
			else
			{
				// Date
				it = map.find(PARAMETER_DATE);
				if (it != map.end())
				{
					try
					{
						_startDate = DateTime::FromInternalString(it->second);
						_endDate = _startDate;
						_endDate.addDaysDuration(1);						
					}
					catch (time::TimeParseException)
					{
						throw RequestException("Bad date");
					}
				}
			}

			// Max solutions number
			it = map.find(PARAMETER_MAX_SOLUTIONS_NUMBER);
			if (it != map.end())
				_maxSolutionsNumber = Conversion::ToInt(it->second);
			if (_maxSolutionsNumber < 0)
				throw RequestException("Bad max solutions number");
		}

		void RoutePlannerFunction::_run( ostream& stream ) const
		{
			VariablesMap vm;
			if (_departure_place && _arrival_place)
			{
				RoutePlanner r(
					_departure_place
					, _arrival_place
					, _accessParameters
					, PlanningOrder()
					, _startDate
					, _endDate
					, _maxSolutionsNumber
					);
				const RoutePlanner::Result& jv(r.computeJourneySheetDepartureArrival());
				_page->display(
					stream
					, vm
					, jv.journeys
					, _startDate.getDate()
					, _periodId
					, _departure_place
					, _arrival_place
					, _period
					, _request
					);
			}
			else
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
					, _request
					);

			}
		}

		RoutePlannerFunction::RoutePlannerFunction()
			: _maxSolutionsNumber(numeric_limits<int>::max())
			, _startDate(TIME_UNKNOWN)
			, _endDate(TIME_UNKNOWN)
			, _periodId(UNKNOWN_VALUE)
			, _period(NULL)
			, _departure_place(NULL)
			, _arrival_place(NULL)
			, _home(false)
		{

		}

		int RoutePlannerFunction::getMaxSolutions() const
		{
			return _maxSolutionsNumber;
		}

		void RoutePlannerFunction::setMaxSolutions( int number )
		{
			_maxSolutionsNumber = number;
		}
	}
}
