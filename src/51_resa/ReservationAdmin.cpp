
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
#include "ResaModule.h"
#include "ResaRight.h"
#include "ReservationTableSync.h"
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
	using namespace pt_operation;

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
				ReservationTableSync::Search(*_env, _resa->getTransaction()->getKey());
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

			return user.getProfile()->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_resa->getLineId()));
		}



		void ReservationAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			AdminActionFunctionRequest<ReservationUpdateAction, ReservationAdmin> updateRequest(request);
			updateRequest.getAction()->setReservation(const_pointer_cast<Reservation>(_resa));

			AdminFunctionRequest<ReservationAdmin> openRequest(request);

			AdminFunctionRequest<ResaCustomerAdmin> openCustomer(request);
			shared_ptr<const User> user(UserTableSync::Get(_resa->getTransaction()->getCustomerUserId(), *_env));
			openCustomer.getPage()->setUser(user);

			PropertiesHTMLTable t(updateRequest.getHTMLForm("update"));
			stream << t.open();
			stream << t.title("Client");

			stream << t.cell("Nom", HTMLModule::getHTMLLink(openCustomer.getURL(), user->getFullName()));
			stream << t.cell("Seats", lexical_cast<string>(_resa->getTransaction()->getSeats()));

			stream << t.title("Evénements");

			stream << t.cell("Réservation", lexical_cast<string>(_resa->getTransaction()->getBookingTime()));
			stream << t.cell("Annulation",
				_resa->getTransaction()->getCancellationTime().is_not_a_date_time() ?
				"Annuler" :
				lexical_cast<string>(_resa->getTransaction()->getCancellationTime())
			);

			stream << t.title("Trajet");
			size_t rank(0);
			BOOST_FOREACH(Reservation* resa, _resa->getTransaction()->getReservations())
			{
				openRequest.getPage()->setReservation(_env->getSPtr(resa));

				stream << t.cell(
					resa == _resa.get() ?
					"<b>" + lexical_cast<string>(rank++) + "</b>" :
					HTMLModule::getHTMLLink(openRequest.getURL(), lexical_cast<string>(rank++)),
					lexical_cast<string>(resa->getDepartureTime()) + " " +
					resa->getDeparturePlaceName() + "-&gt;" +
					resa->getArrivalPlaceName() + " " +
					lexical_cast<string>(resa->getArrivalTime()) + " "
				);
			}
			stream << t.title("Affectation");

			stream << t.cell(
				"Voiture",
				t.getForm().getSelectInput(
					ReservationUpdateAction::PARAMETER_VEHICLE_ID,
					VehicleTableSync::GetVehiclesList(*_env, string("(non affectée)")),
					optional<RegistryKeyType>(_resa->getVehicle() ? _resa->getVehicle()->getKey() : RegistryKeyType(0))
			)	);
			stream << t.cell("Numéro siège", t.getForm().getTextInput(ReservationUpdateAction::PARAMETER_SEAT_NUMBER, _resa->getSeatNumber()));
			stream << t.cell(
				"PM départ",
				t.getForm().getTextInput(
					ReservationUpdateAction::PARAMETER_DEPARTURE_METER_OFFSET,
					_resa->getVehiclePositionAtDeparture() ?
					lexical_cast<string>(_resa->getVehiclePositionAtDeparture()->getMeterOffset()) :
					string()
			)	);
			stream << t.cell(
				"PM arrivée",
				t.getForm().getTextInput(
					ReservationUpdateAction::PARAMETER_ARRIVAL_METER_OFFSET,
					_resa->getVehiclePositionAtArrival() ?
					lexical_cast<string>(_resa->getVehiclePositionAtArrival()->getMeterOffset()) :
					string()
			)	);

			stream << t.title("Ponctualité");
			stream << t.cell("Non opéré", t.getForm().getOuiNonRadioInput(ReservationUpdateAction::PARAMETER_CANCELLED_BY_OPERATOR, _resa->getCancelledByOperator()));
			stream << t.cell(
				"Départ réel",
				t.getForm().getCalendarInput(
					ReservationUpdateAction::PARAMETER_REAL_DEPARTURE_TIME,
					_resa->getVehiclePositionAtDeparture() ?
					_resa->getVehiclePositionAtDeparture()->getTime() :
					ptime(not_a_date_time)
			)	);
			stream << t.cell(
				"Arrivée réelle",
					t.getForm().getCalendarInput(
						ReservationUpdateAction::PARAMETER_REAL_ARRIVAL_TIME,
						_resa->getVehiclePositionAtArrival() ?
						_resa->getVehiclePositionAtArrival()->getTime() :
					ptime(not_a_date_time)
			)	);

			stream << t.close();
		}



		bool ReservationAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _resa->getKey() == static_cast<const ReservationAdmin&>(other)._resa->getKey();
		}
}	}
