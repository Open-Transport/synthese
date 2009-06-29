
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
			if(!_site->getInterface())
			{
				throw RequestException("Site "+ lexical_cast<string>(_site->getKey()) + " is corrupted : it has no interface");
			}

			_page = _site->getInterface()->getPage<RoutePlannerInterfacePage>();

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
				_departure_place = _site->fetchPlace(_originCityText, _originPlaceText);
				_arrival_place = _site->fetchPlace(_destinationCityText, _destinationPlaceText);
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
			if (_departure_place && _arrival_place)
			{
				// Initialisation
				RoutePlanner r(
					_departure_place
					, _arrival_place
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
				result.departurePlace = _departure_place;
				result.arrivalPlace = _arrival_place;
				
				// Display
				_page->display(
					stream
					, vm
					, result
					, _startDate.getDate()
					, _periodId
					, _departure_place
					, _arrival_place
					, _period
					, _accessParameters
					, _request
					, _site.get()
					, jv.samePlaces
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
			, _departure_place(NULL)
			, _arrival_place(NULL)
			, _home(false)
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
