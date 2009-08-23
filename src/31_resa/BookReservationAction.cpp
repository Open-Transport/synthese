
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

#include "Site.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "RoutePlannerFunction.h"
#include "RoutePlanner.h"
#include "JourneysResult.h"
#include "NamedPlace.h"
#include "ResaRight.h"
#include "ReservationTransaction.h"
#include "Reservation.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTableSync.h"
#include "ResaDBLog.h"
#include "ResaModule.h"
#include "ActionException.h"
#include "Request.h"
#include "PTUseRule.h"
#include "Place.h"
#include "City.h"
#include "Journey.h"
#include "Service.h"
#include "Edge.h"
#include "Line.h"
#include "CommercialLine.h"
#include "Road.h"
#include "UseRule.h"
#include "NamedPlace.h"
#include "RoadPlace.h"
#include "Types.h"
#include "User.h"
#include "UserTableSync.h"
#include "GeographyModule.h"
#include "OnlineReservationRule.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

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
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace pt;	
	

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::BookReservationAction>::FACTORY_KEY("bra");
	}
	
	namespace resa
	{
		const string BookReservationAction::PARAMETER_SITE = Action_PARAMETER_PREFIX + "sit";
		
		const string BookReservationAction::PARAMETER_ACCESS_PARAMETERS(Action_PARAMETER_PREFIX + "ac");

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





		BookReservationAction::BookReservationAction():
			util::FactorableTemplate<server::Action, BookReservationAction>(),
			_createCustomer(false)
		{

		}


		ParametersMap BookReservationAction::getParametersMap() const
		{
			ParametersMap map;
			if (!_journey.empty())
			{
				if (_journey.getOrigin())
				{
					const NamedPlace* place(dynamic_cast<const NamedPlace*>(_journey.getOrigin()->getHub()));
					map.insert(PARAMETER_ORIGIN_CITY, place->getCity()->getName());
					map.insert(PARAMETER_ORIGIN_PLACE, place->getName());
				}
				if (_journey.getDestination())
				{
					const NamedPlace* place(dynamic_cast<const NamedPlace*>(_journey.getDestination()->getHub()));
					map.insert(PARAMETER_DESTINATION_CITY, place->getCity()->getName());
					map.insert(PARAMETER_DESTINATION_PLACE, place->getName());
				}
				if (!_journey.getDepartureTime().isUnknown())
				{
					map.insert(PARAMETER_DATE_TIME, _journey.getDepartureTime());
				}
				map.insert(PARAMETER_ACCESS_PARAMETERS, _accessParameters.serialize());
			}
			return map;
		}

		void BookReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Right control
			if (_request->isAuthorized<ResaRight>(WRITE, WRITE))
			{
				_createCustomer = map.getDefault<bool>(PARAMETER_CREATE_CUSTOMER, false);

				if (_createCustomer)
				{
					_customer.reset(new User);
					_customer->setName(map.get<string>(PARAMETER_CUSTOMER_NAME));
					if (_customer->getName().empty())
						throw ActionException("Le nom du client doit être rempli");

					_customer->setSurname(map.get<string>(PARAMETER_CUSTOMER_SURNAME));
					if (_customer->getSurname().empty())
						throw ActionException("Le prénom du client doit être rempli");

					_customer->setPhone(map.get<string>(PARAMETER_CUSTOMER_PHONE));
					if (_customer->getPhone().empty())
						throw ActionException("Le numéro de téléphone doit être rempli");

					// Integrity test : the key is name + surname + phone
					Env env;
					UserTableSync::Search(
						env,
						optional<string>(),
						_customer->getName(),
						_customer->getSurname(),
						_customer->getPhone(),
						optional<RegistryKeyType>(),
						logic::indeterminate,
						logic::indeterminate,
						optional<RegistryKeyType>(),
						0, 1
					);
					if (!env.getRegistry<User>().empty())
						throw ActionException("Un utilisateur avec les mêmes nom, prénom, téléphone existe déjà.");
					
					_customer->setEMail(map.getDefault<string>(PARAMETER_CUSTOMER_EMAIL));
					_customer->setProfile(ResaModule::GetBasicResaCustomerProfile().get());
				}
				else
				{
					// Customer ID
					optional<RegistryKeyType> id(map.get<RegistryKeyType>(PARAMETER_CUSTOMER_ID));
					if (id)
						_customer = UserTableSync::GetEditable(*id, *_env);
				}
			}
			if(!_customer.get())
			{
				_customer = const_pointer_cast<User, const User>(_request->getUser());
			}
			if(!_customer.get())
			{
				throw ActionException("Not authorized");
			}

			// Deduce naming fields from the customer if already recognized
			if (_customer->getName().empty())
				throw ActionException("Client sans nom. Réservation impossible");

			if (_customer->getPhone().empty())
				throw ActionException("Client sans numéro de téléphone. Veuillez renseigner ce champ dans la fiche client et recommencer la réservation.");

			// Site
			uid id(map.getUid(PARAMETER_SITE, false, FACTORY_KEY));
			shared_ptr<const Site> site;
			if (id > 0 && Env::GetOfficialEnv().getRegistry<Site>().contains(id))
				site = Env::GetOfficialEnv().getRegistry<Site>().get(id);

			// Seats number
			_seatsNumber = map.get<int>(PARAMETER_SEATS_NUMBER);
			if (_seatsNumber < 1 || _seatsNumber > 99)
				throw ActionException("Invalid seats number");

			// Journey
			const Place* originPlace(site.get() 
				? site->fetchPlace(
					map.getString(PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
				) : GeographyModule::FetchPlace(
					map.getString(PARAMETER_ORIGIN_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_ORIGIN_PLACE, true, FACTORY_KEY)
				)
			);
			const Place* destinationPlace(site.get()
				? site->fetchPlace(
					map.getString(PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
				) : GeographyModule::FetchPlace(
					map.getString(PARAMETER_DESTINATION_CITY, true, FACTORY_KEY)
					, map.getString(PARAMETER_DESTINATION_PLACE, true, FACTORY_KEY)
				)
			);
			
			// Departure date time
			DateTime departureDateTime(map.getDateTime(PARAMETER_DATE_TIME, true, FACTORY_KEY));

			// Accessibility
			if(!map.getDefault<string>(PARAMETER_ACCESS_PARAMETERS).empty())
			{
				_accessParameters = AccessParameters(map.get<string>(PARAMETER_ACCESS_PARAMETERS));
			}

			RoutePlanner rp(
				originPlace
				, destinationPlace
				, _accessParameters,
				PlanningOrder()
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
			{
				UserTableSync::Save(_customer.get());

				ResaDBLog::AddCustomerCreationEntry(*_request->getSession(), *_customer);
			}

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
			ReservationTransactionTableSync::Save(&rt);

			// New reservation for each journey leg
			const OnlineReservationRule* reservationContact(NULL);
			BOOST_FOREACH(const ServiceUse& su, _journey.getServiceUses())
			{
				assert(su.getService() != NULL);
				assert(su.getDepartureEdge() != NULL);
				assert(su.getDepartureEdge()->getHub() != NULL);
				assert(su.getArrivalEdge() != NULL);
				assert(su.getArrivalEdge()->getHub() != NULL);

				shared_ptr<Reservation> r(rt.newReservation());
				r->setKey(ReservationTableSync::getId());
				_env->getEditableRegistry<Reservation>().add(r);
				r->setDeparturePlaceId(
					dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(
						su.getDepartureEdge()->getHub()
					)->getKey()
				);
				r->setDeparturePlaceName(
					dynamic_cast<const NamedPlace*>(
						su.getDepartureEdge()->getHub()
					)->getFullName()
				);
				r->setDepartureTime(su.getDepartureDateTime());
				r->setOriginDateTime(su.getOriginDateTime());
				r->setArrivalPlaceId(
					dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(
						su.getArrivalEdge()->getHub()
					)->getKey()
				);
				r->setArrivalPlaceName(
					dynamic_cast<const NamedPlace*>(
						su.getArrivalEdge()->getHub()
					)->getFullName()
				);
				r->setArrivalTime(su.getArrivalDateTime());
				
				const Line* line(dynamic_cast<const Line*>(su.getService()->getPath()));
				if (line)
				{
					assert(line->getCommercialLine() != NULL);

					r->setLineCode(line->getCommercialLine()->getName());
					r->setLineId(line->getCommercialLine()->getKey());
				}
				const Road* road(dynamic_cast<const Road*>(su.getService()->getPath()));
				if (road)
				{
					r->setLineCode(road->getRoadPlace()->getName());
					r->setLineId(road->getKey());
				}


				if(	UseRule::IsReservationPossible(su.getUseRule()->getReservationAvailability(su))
				){
					if(	dynamic_cast<const Line*>(su.getService()->getPath()) &&
						static_cast<const Line*>(su.getService()->getPath())->getCommercialLine()
					){
						const OnlineReservationRule* onlineContact(OnlineReservationRule::GetOnlineReservationRule(
								static_cast<const Line*>(su.getService()->getPath())->getCommercialLine()->getReservationContact()
						)	);
						if(onlineContact)
						{
							reservationContact = onlineContact;
						}
					}
					if(dynamic_cast<const PTUseRule*>(su.getUseRule()))
					{
						r->setReservationRuleId(static_cast<const PTUseRule*>(su.getUseRule())->getKey());
					}
					r->setReservationDeadLine(su.getUseRule()->getReservationDeadLine(
						su.getOriginDateTime()
						, su.getDepartureDateTime()
					));
				}
				r->setServiceId(su.getService()->getKey());
				r->setServiceCode(lexical_cast<string>(su.getService()->getServiceNumber()));
				ReservationTableSync::Save(r.get());
			}

			// Log
			ResaDBLog::AddBookReservationEntry(_request->getSession(), rt);

			// Mail
			if(!_customer->getEMail().empty() && reservationContact)
 			{
				reservationContact->sendCustomerEMail(rt);

				ResaDBLog::AddEMailEntry(*_request->getSession(), *_customer, "Récapitulatif de réservation");
 			}
 

			// Redirect
			_request->setActionCreatedId(rt.getKey());
		}


		void BookReservationAction::setJourney( const Journey& journey )
		{
			_journey = journey;
		}



		bool BookReservationAction::_isAuthorized(
		) const {
			return
				_request->isAuthorized<ResaRight>(WRITE) ||
				_customer->getKey() == _request->getUser()->getKey() && _request->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE);
		}



		void BookReservationAction::setAccessParameters( const AccessParameters& value )
		{
			_accessParameters = value;
		}
	}
}
