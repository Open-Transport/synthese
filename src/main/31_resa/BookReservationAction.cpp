
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

#include "36_places_list/Site.h"

#include "33_route_planner/RoutePlannerFunction.h"
#include "33_route_planner/RoutePlanner.h"
#include "33_route_planner/JourneysResult.h"

#include "31_resa/ResaRight.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/Reservation.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/ReservationTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "15_env/Place.h"
#include "15_env/Journey.h"

#include "12_security/Types.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace routeplanner;
	using namespace security;
	using namespace env;
	using namespace transportwebsite;
	using namespace time;

	namespace util
	{
		const string FactorableTemplate<Action, resa::BookReservationAction>::FACTORY_KEY("bra");
	}
	
	namespace resa
	{
		const string BookReservationAction::PARAMETER_SITE = Action_PARAMETER_PREFIX + "sit";
		const string BookReservationAction::PARAMETER_ACCESSIBILITY = Action_PARAMETER_PREFIX + "acc";

		const string BookReservationAction::PARAMETER_ORIGIN_CITY = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT;
		const string BookReservationAction::PARAMETER_ORIGIN_PLACE = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT;
		const string BookReservationAction::PARAMETER_DESTINATION_CITY = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT;
		const string BookReservationAction::PARAMETER_DESTINATION_PLACE = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT;
		const string BookReservationAction::PARAMETER_DATE_TIME = Action_PARAMETER_PREFIX + RoutePlannerFunction::PARAMETER_DATE;

		const string BookReservationAction::PARAMETER_CUSTOMER_ID = Action_PARAMETER_PREFIX + "cuid";
		const string BookReservationAction::PARAMETER_CUSTOMER_NAME = Action_PARAMETER_PREFIX + "cuna";
		const string BookReservationAction::PARAMETER_CUSTOMER_PHONE = Action_PARAMETER_PREFIX + "cuph";
		const string BookReservationAction::PARAMETER_CUSTOMER_EMAIL = Action_PARAMETER_PREFIX + "cupe";

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
			// Right control
			if (_request->isAuthorized<ResaRight>(WRITE, WRITE))
			{	// Case operator

				// Customer ID
				uid id(Request::getUidFromParameterMap(map, PARAMETER_CUSTOMER_ID, false, FACTORY_KEY));
				if (id != UNKNOWN_VALUE)
					_customer = UserTableSync::get(id);

				// Customer name
				_customerName = Request::getStringFormParameterMap(map, PARAMETER_CUSTOMER_NAME, !_customer.get(), FACTORY_KEY);
				if (_customerName.empty())
				{
					if (!_customer.get())
						throw ActionException("Empty name");
					_customerName = _customer->getFullName();
				}

				// CUstomer email
				_customerEMail = Request::getStringFormParameterMap(map, PARAMETER_CUSTOMER_EMAIL, false, FACTORY_KEY);
				if (_customerEMail.empty() && _customer.get())
					_customerEMail = _customer->getEMail();

			}
			else if (_request->isAuthorized<ResaRight>(FORBIDDEN, WRITE))
			{
				_customer = _request->getUser();
			}
			else
				throw ActionException("Not authorized");

			// Site
			shared_ptr<const Site> site(Site::Get(Request::getUidFromParameterMap(map, PARAMETER_SITE, true, FACTORY_KEY)));

			// Customer contact phone
			_customerPhone = Request::getStringFormParameterMap(map, PARAMETER_CUSTOMER_PHONE, !_customer.get(), FACTORY_KEY);
			if (_customerPhone.empty())
			{
				if(!_customer.get())
					throw ActionException("Empty phone number");
				_customerPhone = _customer->getPhone();
			}

			// Seats number
			_seatsNumber = Request::getIntFromParameterMap(map, PARAMETER_SEATS_NUMBER, true, FACTORY_KEY);
			if (_seatsNumber < 1 || _seatsNumber > 99)
				throw ActionException("Invalid seats number");

			// Journey
			const Place* originPlace(site->fetchPlace(
				Request::getStringFormParameterMap(map, PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
				, Request::getStringFormParameterMap(map, PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
			));
			const Place* destinationPlace(site->fetchPlace(
				Request::getStringFormParameterMap(map, PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
				, Request::getStringFormParameterMap(map, PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
			));
			DateTime departureDateTime(Request::getDateTimeFromParameterMap(map, PARAMETER_DATE_TIME, true, FACTORY_KEY));
			// Accessibility
			Site::AccessibilityParameter accessibility(static_cast<Site::AccessibilityParameter>(
				Request::getIntFromParameterMap(map, PARAMETER_ACCESSIBILITY, false, string()))
			);
			AccessParameters ap(site->getAccessParameters(accessibility));

			RoutePlanner rp(
				originPlace
				, destinationPlace
				, ap
				, PlanningOrder()
				, departureDateTime
				, departureDateTime
				, 1
			);
			const RoutePlanner::Result& jr(rp.computeJourneySheetDepartureArrival());

			if (jr.journeys.empty())
				throw ActionException("The route planning does not find a journey to book");

			_journey = *jr.journeys.front();

		}

		void BookReservationAction::run()
		{
			// New ReservationTransaction
			const DateTime now(TIME_CURRENT);
			ReservationTransaction rt;
			rt.setBookingTime(now);
			rt.setBookingUserId(_request->getUser()->getKey());
			rt.setCustomerName(_customerName);
			rt.setCustomerPhone(_customerPhone);
			rt.setCustomerEMail(_customerEMail);
			rt.setCustomerUserId(_customer.get() ? _customer->getKey() : UNKNOWN_VALUE);
			rt.setSeats(_seatsNumber);
			
			// New reservation for each bookable journey leg
			for (Journey::ServiceUses::const_iterator su(_journey.getServiceUses().begin()); su != _journey.getServiceUses().end(); ++su)
			{
				Reservation r;
				
			}
		}

		BookReservationAction::BookReservationAction()
			: FactorableTemplate<Action, BookReservationAction>()
			, _journey(TO_DESTINATION)
		{

		}
	}
}
