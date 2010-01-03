
/** CancelReservationAction class implementation.
	@file CancelReservationAction.cpp
	@author Hugues Romain
	@date 2007

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

#include "CancelReservationAction.h"

#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "ResaRight.h"
#include "ResaDBLog.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "OnlineReservationRule.h"
#include "OnlineReservationRuleTableSync.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "User.h"
#include "UserTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace time;
	using namespace env;
	
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::CancelReservationAction>::FACTORY_KEY("cancelreservation");
	}

	namespace resa
	{
		const string CancelReservationAction::PARAMETER_RESERVATION_TRANSACTION_ID = Action_PARAMETER_PREFIX + "rt";

		
		
		ParametersMap CancelReservationAction::getParametersMap() const
		{
			ParametersMap map;
			if (_transaction->getKey()) map.insert(PARAMETER_RESERVATION_TRANSACTION_ID, _transaction->getKey());
			return map;
		}
		
		
		
		void CancelReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Load of the transaction
			try
			{
				_transaction = ReservationTransactionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RESERVATION_TRANSACTION_ID), *_env, FIELDS_ONLY_LOAD_LEVEL);
			}
			catch(...)
			{
				throw ActionException("No such reservation");
			}

			// Control of the date : a cancellation has no sense if after the arrival time
			DateTime now(TIME_CURRENT);
			ReservationTableSync::SearchResult reservations(
				ReservationTableSync::Search(*_env, _transaction->getKey())
			);
			BOOST_FOREACH(shared_ptr<Reservation> resa, reservations)
			{
				if (resa->getReservationRuleId() != UNKNOWN_VALUE)
				{
					if (now > resa->getArrivalTime())
						throw ActionException("Le statut de la r�servation ne permet pas de l'annuler");
					break;
				}
			}

			// Tests if the reservation is already cancelled
			if(!_transaction->getCancellationTime().isUnknown())
			{
				throw ActionException("Cette r�servation est d�j� annul�e.");
			}
		}
		
		
		
		void CancelReservationAction::run(Request& request)
		{
			// Store old parameters
			ReservationStatus oldStatus(_transaction->getStatus());

			// Write the cancellation
			DateTime now(TIME_CURRENT);
			_transaction->setCancellationTime(now);
			_transaction->setCancelUserId(request.getUser()->getKey());
			ReservationTransactionTableSync::Save(_transaction.get());

			// Write the log
			ResaDBLog::AddCancelReservationEntry(request.getSession(), *_transaction, oldStatus);

			// Mail
			shared_ptr<const User> customer(UserTableSync::Get(_transaction->getCustomerUserId(), *_env));
			const OnlineReservationRule* reservationContact(NULL);
			BOOST_FOREACH(const Reservation* resa, _transaction->getReservations())
			{
				try
				{
					shared_ptr<const CommercialLine> line(CommercialLineTableSync::Get(resa->getLineId(), *_env));
					const OnlineReservationRule* onlineContact(OnlineReservationRule::GetOnlineReservationRule(
							line->getReservationContact()
					)	);
					if(onlineContact)
					{
						reservationContact = onlineContact;
						break;
					}
				}
				catch(...)
				{
					continue;
				}
			}
			if(	customer.get() && !customer->getEMail().empty() && reservationContact)
			{
				reservationContact->sendCustomerCancellationEMail(*_transaction);

				ResaDBLog::AddEMailEntry(*request.getSession(), *customer, "Annulation de r�servation");
			}
		}



		void CancelReservationAction::setTransaction( boost::shared_ptr<ReservationTransaction> transaction )
		{
			_transaction = transaction;
		}



		bool CancelReservationAction::isAuthorized(const Session* session
		) const {
			
			if(!session || !session->hasProfile())
			{
				return false;
			}

			return
				session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE) ||
				_transaction->getCustomerUserId() == session->getUser()->getKey() &&
				session->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE)
			;
		}
	}
}
