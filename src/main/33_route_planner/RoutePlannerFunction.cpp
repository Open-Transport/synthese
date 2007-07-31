
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

#include "33_route_planner/RoutePlannerFunction.h"
#include "33_route_planner/Site.h"
#include "33_route_planner/RoutePlannerModule.h"
#include "33_route_planner/RoutePlannerInterfacePage.h"
#include "33_route_planner/RoutePlanner.h"

#include "30_server/RequestException.h"

#include "15_env/EnvModule.h"

#include "11_interfaces/Interface.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace env;
	using namespace time;
	using namespace interfaces;

	namespace routeplanner
	{
		const std::string RoutePlannerFunction::PARAMETER_SITE = "si";
		const std::string RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_ID = "dp";
		const std::string RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_ID = "ap";
		const std::string RoutePlannerFunction::PARAMETER_DATE = "da";
		const string RoutePlannerFunction::PARAMETER_MAX_SOLUTIONS_NUMBER("msn");
		const string RoutePlannerFunction::PARAMETER_DAY("dy");

		ParametersMap RoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}

		void RoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Site
			it = map.find(PARAMETER_SITE);
			if (it == map.end())
				throw RequestException("Site not specified");
			if (!RoutePlannerModule::getSites().contains(Conversion::ToLongLong(it->second)))
				throw RequestException("Specified site not found");
			_site = RoutePlannerModule::getSites().get(Conversion::ToLongLong(it->second));
			_page = _site->getInterface()->getPage<RoutePlannerInterfacePage>();
			_accessParameters = _site->getDefaultAccessParameters();

			// Departure place
			it = map.find(PARAMETER_DEPARTURE_PLACE_ID);
			if (it == map.end())
				throw RequestException("Departure place not specified");
			_departure_place = EnvModule::fetchPlace(Conversion::ToLongLong(it->second));
			if (!_departure_place.get())
				throw RequestException("Specified departure place not found");

			// Arrival place
			it = map.find(PARAMETER_ARRIVAL_PLACE_ID);
			if (it == map.end())
				throw RequestException("Arrival place not specified");
			_arrival_place = EnvModule::fetchPlace(Conversion::ToLongLong(it->second));
			if (!_arrival_place.get())
				throw RequestException("Specified arrival place not found");

			// Day
			it = map.find(PARAMETER_DAY);
			if (it != map.end())
			{
				try
				{
					_startDate = DateTime(
						Date::FromInternalString(it->second)
						, Hour(2, 30)
						);
					_endDate.addDaysDuration(1);
				}
				catch (...)
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
						_endDate.setHour(Hour(2,30));
					}
					catch (...)
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

		void RoutePlannerFunction::_run( std::ostream& stream ) const
		{
			RoutePlanner r(
				_departure_place.get()
				, _arrival_place.get()
				, _accessParameters
				, PlanningOrder()
				, _startDate
				, _endDate
				, _maxSolutionsNumber
			);
			const RoutePlanner::Result& jv(r.computeJourneySheetDepartureArrival());
			VariablesMap vm;
			_page->display(stream, vm, jv.journeys, _request);

		}

		RoutePlannerFunction::RoutePlannerFunction()
			: _maxSolutionsNumber(UNKNOWN_VALUE)
		{

		}
	}
}
