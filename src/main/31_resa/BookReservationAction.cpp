
/** BookReservationAction class implementation.
	@file BookReservationAction.cpp

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

#include "BookReservationAction.h"

#include "33_route_planner/RoutePlannerFunction.h"

#include "31_resa/ResaRight.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "12_security/Types.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace routeplanner;
	using namespace security;
	
	namespace resa
	{
		const string BookReservationAction::PARAMETER_ORIGIN_CITY = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT;
		const string BookReservationAction::PARAMETER_ORIGIN_PLACE = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT;
		const string BookReservationAction::PARAMETER_DESTINATION_CITY = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT;
		const string BookReservationAction::PARAMETER_DESTINATION_PLACE = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT;
		const string BookReservationAction::PARAMETER_DATE_TIME = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_DATE;

		const string BookReservationAction::PARAMETER_CUSTOMER_ID = Action_PARAMETER_PREFIX + "cuid";
		const string BookReservationAction::PARAMETER_CUSTOMER_NAME = Action_PARAMETER_PREFIX + "cuna";
		const string BookReservationAction::PARAMETER_CUSTOMER_PHONE = Action_PARAMETER_PREFIX + "cuph";

		const string BookReservationAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pass";

		const string BookReservationAction::PARAMETER_SEATS_NUMBER = Action_PARAMETER_PREFIX + "senu";


		ParametersMap BookReservationAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void BookReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Right control
			if (_request->isAuthorized<ResaRight>(WRITE, WRITE))
			{	// Case operator

			}
			else if (_request->isAuthorized<ResaRight>(FORBIDDEN, WRITE))
			{

			}
			else
				throw ActionException("Not authorized");
		}

		void BookReservationAction::run()
		{
		}
	}
}
