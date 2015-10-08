
//////////////////////////////////////////////////////////////////////////
/// ReservationAdmin class implementation.
///	@file ReservationAdmin.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ReservationAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ReservationTableSync.h"
#include "User.h"
#include "CommercialLine.h"
#include "ReservationTransaction.h"
#include "AdminActionFunctionRequest.hpp"
#include "ReservationUpdateAction.hpp"
#include "PropertiesHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "HTMLModule.h"
#include "ResaCustomerAdmin.h"
#include "UserTableSync.h"
#include "VehicleTableSync.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace resa;
	using namespace html;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ReservationAdmin>::FACTORY_KEY("ReservationAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ReservationAdmin>::ICON("resa_compulsory.png");
		template<> const string AdminInterfaceElementTemplate<ReservationAdmin>::DEFAULT_TITLE("Réservation");
	}

	namespace resa
	{
		ReservationAdmin::ReservationAdmin()
			: AdminInterfaceElementTemplate<ReservationAdmin>()
		{ }



		void ReservationAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_resa = ReservationTableSync::Get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID), *_env);
				ReservationTableSync::Search(*_env, _resa->get<Transaction>()->getKey());
			}
			catch(ObjectNotFoundException<Reservation>&)
			{
				throw AdminParametersException("No such reservation");
			}
		}



		ParametersMap ReservationAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_resa.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _resa->getKey());
			}
			return m;
		}



		bool ReservationAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(READ);
			if (!_resa.get())
			{
				return false;
			}

			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_resa->get<LineId>()));
		}



		void ReservationAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminActionFunctionRequest<ReservationUpdateAction, ReservationAdmin> updateRequest(request, *this);
			updateRequest.getAction()->setReservation(const_pointer_cast<Reservation>(_resa));

			AdminFunctionRequest<ReservationAdmin> openRequest(request, *this);

			AdminFunctionRequest<ResaCustomerAdmin> openCustomer(request);
			boost::shared_ptr<const User> user(UserTableSync::Get(_resa->get<Transaction>()->get<Customer>()->getKey(), *_env));
			openCustomer.getPage()->setUser(user);

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			stream << t.open();
			stream << t.title("Client");

			stream << t.cell("Nom", HTMLModule::getHTMLLink(openCustomer.getURL(), user->getFullName()));
			stream << t.cell("Seats", lexical_cast<string>(_resa->get<Transaction>()->get<Seats>()));

			stream << t.title("Evénements");

			stream << t.cell("Réservation", lexical_cast<string>(_resa->get<Transaction>()->get<BookingTime>()));
			stream << t.cell("Annulation",
				_resa->get<Transaction>()->get<CancellationTime>().is_not_a_date_time() ?
				"Annuler" :
				lexical_cast<string>(_resa->get<Transaction>()->get<CancellationTime>())
			);

			stream << t.title("Trajet");
			size_t rank(0);
			BOOST_FOREACH(Reservation* resa, _resa->get<Transaction>()->getReservations())
			{
				openRequest.getPage()->setReservation(_env->getSPtr(resa));

				stream << t.cell(
					resa == _resa.get() ?
					"<b>" + lexical_cast<string>(rank++) + "</b>" :
					HTMLModule::getHTMLLink(openRequest.getURL(), lexical_cast<string>(rank++)),
					lexical_cast<string>(resa->get<DepartureTime>()) + " " +
					resa->get<DeparturePlaceName>() + "-&gt;" +
					resa->get<ArrivalPlaceName>() + " " +
					lexical_cast<string>(resa->get<ArrivalTime>()) + " "
				);
			}
			stream << t.title("Affectation");

			stream << t.cell(
				"Voiture",
				t.getForm().getSelectInput(
					ReservationUpdateAction::PARAMETER_VEHICLE_ID,
					VehicleTableSync::GetVehiclesList(*_env, string("(non affectée)")),
					optional<RegistryKeyType>(_resa->get<Vehicle>() ? _resa->get<Vehicle>()->getKey() : RegistryKeyType(0))
			)	);
			stream << t.cell("Numéro siège", t.getForm().getTextInput(ReservationUpdateAction::PARAMETER_SEAT_NUMBER, _resa->get<SeatNumber>()));
			stream << t.cell(
				"PM départ",
				t.getForm().getTextInput(
					ReservationUpdateAction::PARAMETER_DEPARTURE_METER_OFFSET,
					_resa->get<VehiclePositionAtDeparture>() ?
					lexical_cast<string>(_resa->get<VehiclePositionAtDeparture>()->getMeterOffset()) :
					string()
			)	);
			stream << t.cell(
				"PM arrivée",
				t.getForm().getTextInput(
					ReservationUpdateAction::PARAMETER_ARRIVAL_METER_OFFSET,
					_resa->get<VehiclePositionAtArrival>() ?
					lexical_cast<string>(_resa->get<VehiclePositionAtArrival>()->getMeterOffset()) :
					string()
			)	);

			stream << t.title("Ponctualité");
			stream << t.cell("Non opéré", t.getForm().getOuiNonRadioInput(ReservationUpdateAction::PARAMETER_CANCELLED_BY_OPERATOR, _resa->get<CancelledByOperator>()));
			stream << t.cell(
				"Départ réel",
				t.getForm().getCalendarInput(
					ReservationUpdateAction::PARAMETER_REAL_DEPARTURE_TIME,
					_resa->get<VehiclePositionAtDeparture>() ?
					_resa->get<VehiclePositionAtDeparture>()->getTime() :
					ptime(not_a_date_time)
			)	);
			stream << t.cell(
				"Arrivée réelle",
					t.getForm().getCalendarInput(
						ReservationUpdateAction::PARAMETER_REAL_ARRIVAL_TIME,
						_resa->get<VehiclePositionAtArrival>() ?
						_resa->get<VehiclePositionAtArrival>()->getTime() :
					ptime(not_a_date_time)
			)	);

			stream << t.close();
		}



		bool ReservationAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _resa->getKey() == static_cast<const ReservationAdmin&>(other)._resa->getKey();
		}
}	}
