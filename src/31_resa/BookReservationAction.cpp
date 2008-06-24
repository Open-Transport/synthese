
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
#include "31_resa/ResaDBLog.h"
#include "31_resa/ResaModule.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "15_env/Place.h"
#include "15_env/City.h"
#include "15_env/Journey.h"
#include "15_env/Service.h"
#include "15_env/Edge.h"
#include "15_env/Line.h"
#include "15_env/CommercialLine.h"
#include "15_env/Road.h"
#include "15_env/ReservationRule.h"
#include "15_env/AddressablePlace.h"

#include "12_security/Types.h"
#include "12_security/User.h"
#include "12_security/UserTableSync.h"

#include "01_util/Conversion.h"

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
	using namespace util;
	

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::BookReservationAction>::FACTORY_KEY("bra");
	}
	
	namespace resa
	{
		const string BookReservationAction::PARAMETER_SITE = Action_PARAMETER_PREFIX + "sit";
		
		const string BookReservationAction::PARAMETER_ACCESSIBILITY(Action_PARAMETER_PREFIX + "ac");
		const string BookReservationAction::PARAMETER_DISABLED_CUSTOMER(Action_PARAMETER_PREFIX + "dc");
		const string BookReservationAction::PARAMETER_DRT_ONLY(Action_PARAMETER_PREFIX + "do");

		const string BookReservationAction::PARAMETER_ORIGIN_CITY = Action_PARAMETER_PREFIX + "dct";
		const string BookReservationAction::PARAMETER_ORIGIN_PLACE = Action_PARAMETER_PREFIX + "dpt";
		const string BookReservationAction::PARAMETER_DESTINATION_CITY = Action_PARAMETER_PREFIX + "act";
		const string BookReservationAction::PARAMETER_DESTINATION_PLACE = Action_PARAMETER_PREFIX + "apt";
        const string BookReservationAction::PARAMETER_DATE_TIME = Action_PARAMETER_PREFIX + "da";


		const string BookReservationAction::PARAMETER_CREATE_CUSTOMER = Action_PARAMETER_PREFIX + "cc";
		const string BookReservationAction::PARAMETER_CUSTOMER_ID = Action_PARAMETER_PREFIX + "cuid";
		const string BookReservationAction::PARAMETER_CUSTOMER_NAME = Action_PARAMETER_PREFIX + "cuna";
		const string BookReservationAction::PARAMETER_CUSTOMER_SURNAME = Action_PARAMETER_PREFIX + "cs";
		const string BookReservationAction::PARAMETER_CUSTOMER_PHONE = Action_PARAMETER_PREFIX + "cuph";
		const string BookReservationAction::PARAMETER_CUSTOMER_EMAIL = Action_PARAMETER_PREFIX + "cupe";

		const string BookReservationAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pass";

		const string BookReservationAction::PARAMETER_SEATS_NUMBER = Action_PARAMETER_PREFIX + "senu";


		ParametersMap BookReservationAction::getParametersMap() const
		{
			ParametersMap map;
			if (!_journey.empty())
			{
				if (_journey.getOrigin())
				{
					map.insert(PARAMETER_ORIGIN_CITY, _journey.getOrigin()->getPlace()->getCity()->getName());
					map.insert(PARAMETER_ORIGIN_PLACE, _journey.getOrigin()->getPlace()->getName());
				}
				if (_journey.getDestination())
				{
					map.insert(PARAMETER_DESTINATION_CITY, _journey.getDestination()->getPlace()->getCity()->getName());
					map.insert(PARAMETER_DESTINATION_PLACE, _journey.getOrigin()->getPlace()->getName());
				}
				if (!_journey.getDepartureTime().isUnknown())
					map.insert(PARAMETER_DATE_TIME, _journey.getDepartureTime());
			}
			return map;
		}

		void BookReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Right control
			if (_request->isAuthorized<ResaRight>(WRITE, WRITE))
			{
				_createCustomer = map.getBool(PARAMETER_CREATE_CUSTOMER, false, false, FACTORY_KEY);

				if (_createCustomer)
				{
					_customer.reset(new User);
					_customer->setName(map.getString(PARAMETER_CUSTOMER_NAME, true, FACTORY_KEY));
					if (_customer->getName().empty())
						throw ActionException("Le nom du client doit être rempli");

					_customer->setSurname(map.getString(PARAMETER_CUSTOMER_SURNAME, true, FACTORY_KEY));
					if (_customer->getSurname().empty())
						throw ActionException("Le prénom du client doit être rempli");

					_customer->setPhone(map.getString(PARAMETER_CUSTOMER_PHONE, true, FACTORY_KEY));
					if (_customer->getPhone().empty())
						throw ActionException("Le numéro de téléphone doit être rempli");

					// Integrity test : the key is name + surname + phone
					vector<shared_ptr<User> > users(UserTableSync::Search("%",_customer->getName(), _customer->getSurname(), _customer->getPhone(), UNKNOWN_VALUE, logic::indeterminate, 0, 1));
					if (!users.empty())
						throw ActionException("Un utilisateur avec les mêmes nom, prénom, téléphone existe déjà.");
					
					_customer->setEMail(map.getString(PARAMETER_CUSTOMER_EMAIL, false, FACTORY_KEY));
					_customer->setProfile(ResaModule::GetBasicResaCustomerProfile().get());
				}
				else
				{
					// Customer ID
					uid id(map.getUid(PARAMETER_CUSTOMER_ID, false, FACTORY_KEY));
					if (id != UNKNOWN_VALUE)
						_customer = UserTableSync::GetUpdateable(id);
				}
			}
			else if (_request->isAuthorized<ResaRight>(FORBIDDEN, WRITE))
			{
				_customer = const_pointer_cast<User, const User>(_request->getUser());

				// Password control
				string password(map.getString(PARAMETER_PASSWORD, true, FACTORY_KEY));
				if (password.empty())
					throw ActionException("Le mot de passe doit être fourni");

				if (password != _customer->getPassword())
				{
					throw ActionException("Mot de passe erronné");
				}
			}
			else
				throw ActionException("Not authorized");

			assert(_customer.get());

			// Deduce naming fields from the customer if already recognized
			if (_customer->getName().empty())
				throw ActionException("Client sans nom. Réservation impossible");

			if (_customer->getPhone().empty())
				throw ActionException("Client sans numéro de téléphone. Veuillez renseigner ce champ dans la fiche client et recommencer la réservation.");

			// Site
			uid id(map.getUid(PARAMETER_SITE, false, FACTORY_KEY));
			shared_ptr<const Site> site;
			if (id > 0 && Site::Contains(id))
				site = Site::Get(id);

			// Seats number
			_seatsNumber = map.getInt(PARAMETER_SEATS_NUMBER, true, FACTORY_KEY);
			if (_seatsNumber < 1 || _seatsNumber > 99)
				throw ActionException("Invalid seats number");

			// Journey
			const Place* originPlace(site.get() 
				? site->fetchPlace(
					map.getString(PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
				) : EnvModule::FetchPlace(
					map.getString(PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
				)
			);
			const Place* destinationPlace(site.get()
				? site->fetchPlace(
					map.getString(PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
				) : EnvModule::FetchPlace(
					map.getString(PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
				)
			);
			
			// Departure date time
			DateTime departureDateTime(map.getDateTime(PARAMETER_DATE_TIME, true, FACTORY_KEY));

			// Accessibility
			AccessParameters ap;

			
			if (site.get())
			{			
				AccessibilityParameter accessibility(static_cast<AccessibilityParameter>(
					map.getInt(PARAMETER_ACCESSIBILITY, false, string()))
				);
				ap = site->getAccessParameters(accessibility);
			}
			else
			{
				ap = AccessParameters(false, NULL, _disabledCustomer, true, _drtOnly, false);
			}

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
			// Save customer if necessary
			if (_createCustomer)
				UserTableSync::save(_customer.get());

			// New ReservationTransaction
			const DateTime now(TIME_CURRENT);
			ReservationTransaction rt;
			rt.setBookingTime(now);
			rt.setBookingUserId(_request->getUser()->getKey());
			rt.setCustomerName(_customer->getName() + " " + _customer->getSurname());
			rt.setCustomerPhone(_customer->getPhone());
			rt.setCustomerEMail(_customer->getEMail());
			rt.setCustomerUserId(_customer->getKey());
			rt.setSeats(_seatsNumber);
			ReservationTransactionTableSync::save(&rt);

			// New reservation for each journey leg
			for (Journey::ServiceUses::const_iterator su(_journey.getServiceUses().begin()); su != _journey.getServiceUses().end(); ++su)
			{
				shared_ptr<Reservation> r(rt.newReservation());
				r->setDeparturePlaceId(su->getDepartureEdge()->getPlace()->getId());
				r->setDeparturePlaceName(su->getDepartureEdge()->getPlace()->getFullName());
				r->setDepartureTime(su->getDepartureDateTime());
				r->setOriginDateTime(su->getOriginDateTime());
				r->setArrivalPlaceId(su->getArrivalEdge()->getPlace()->getId());
				r->setArrivalPlaceName(su->getArrivalEdge()->getPlace()->getFullName());
				r->setArrivalTime(su->getArrivalDateTime());
				
				const Line* line(dynamic_cast<const Line*>(su->getService()->getPath()));
				if (line)
				{
					r->setLineCode(line->getCommercialLine()->getName());
					r->setLineId(line->getCommercialLine()->getKey());
				}
				const Road* road(dynamic_cast<const Road*>(su->getService()->getPath()));
				if (road)
				{
					r->setLineCode(road->getName());
					r->setLineId(road->getKey());
				}


				if(	su->getService()->getReservationRule()->isReservationPossible(
					su->getOriginDateTime()
					, now
					, su->getDepartureDateTime()
				))
				{
					r->setReservationRuleId(su->getService()->getReservationRule()->getKey());
					r->setReservationDeadLine(su->getService()->getReservationRule()->getReservationDeadLine(
						su->getOriginDateTime()
						, su->getDepartureDateTime()
					));
				}
				r->setServiceId(su->getService()->getId());
				r->setServiceCode(Conversion::ToString(su->getService()->getServiceNumber()));
				ReservationTableSync::save(r.get());
			}

			// Log
			ResaDBLog::AddBookReservationEntry(_request->getSession(), rt);

			// Redirect
			_request->setObjectId(rt.getKey());
		}

		BookReservationAction::BookReservationAction()
			: FactorableTemplate<Action, BookReservationAction>()
			, _journey(DEPARTURE_TO_ARRIVAL)
			, _disabledCustomer(false)
			, _drtOnly(false)
		{

		}

		void BookReservationAction::setJourney( const env::Journey& journey )
		{
			_journey = journey;
		}
	}
}
