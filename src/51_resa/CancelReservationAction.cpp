
/** CancelReservationAction class implementation.
	@file CancelReservationAction.cpp
	@author Hugues Romain
	@date 2007

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#include "ResaModule.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "OnlineReservationRule.h"
#include "OnlineReservationRuleTableSync.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "UserTableSync.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;


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
				_transaction = ReservationTransactionTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_RESERVATION_TRANSACTION_ID),
					*_env,
					FIELDS_ONLY_LOAD_LEVEL
				);
			}
			catch(...)
			{
				throw ActionException("No such reservation");
			}

			// Tests if the reservation is already cancelled
			if(!_transaction->getCancellationTime().is_not_a_date_time())
			{
				throw ActionException("Cette réservation est déjà annulée.");
			}
		}



		void CancelReservationAction::run(Request& request)
		{
			// Check of the date : a cancellation has no sense if after the arrival time
			ptime now(second_clock::local_time());
			ReservationTableSync::SearchResult reservations(
				ReservationTableSync::Search(*_env, _transaction->getKey())
			);
			BOOST_FOREACH(const boost::shared_ptr<Reservation>& resa, reservations)
			{
				if (resa->getReservationRuleId() != 0)
				{
					// If the resa should be done, we should throw an exception
					// but if the user has CANCEL right (but no more), he should be a driver
					// and may want to log an absence by cancelling the resa too late
					// (back at the deposit for example), we let him 24h to do that
					if (now > resa->getArrivalTime() &&
						(!request.getSession()->getUser()->getProfile()->isAuthorized<ResaRight>(CANCEL) ||
						request.getSession()->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE))
					)
						throw ActionException("Le statut de la réservation ne permet pas de l'annuler");
					else if (request.getSession()->getUser()->getProfile()->isAuthorized<ResaRight>(CANCEL) &&
						now > resa->getArrivalTime() + hours(24)
					)
						throw ActionException("Le statut de la réservation ne permet pas de l'annuler");
					break;
				}
			}

			// Store old parameters
			ReservationStatus oldStatus(_transaction->getStatus());

			// Write the cancellation
			_transaction->setCancellationTime(now);
			_transaction->setCancelUserId(request.getUser()->getKey());
			ReservationTransactionTableSync::Save(_transaction.get());

			// Write the log
			ResaDBLog::AddCancelReservationEntry(request.getSession().get(), *_transaction, oldStatus);

			// Mail
            boost::shared_ptr<const User> customer(UserTableSync::Get(_transaction->getCustomerUserId(), *_env));
			const OnlineReservationRule* reservationContact(NULL);
			BOOST_FOREACH(const Reservation* resa, _transaction->getReservations())
			{
				try
				{
					boost::shared_ptr<const CommercialLine> line(CommercialLineTableSync::Get(resa->getLineId(), *_env));
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
            _transaction->setCustomer(UserTableSync::GetEditable(_transaction->getCustomerUserId(), *_env, UP_LINKS_LOAD_LEVEL).get());
			if(	customer.get() && !customer->getEMail().empty() && reservationContact)
			{
				reservationContact->sendCustomerCancellationEMail(*_transaction);

				ResaDBLog::AddEMailEntry(*request.getSession(), *customer, "Annulation de réservation");
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
				(_transaction->getCustomerUserId() == session->getUser()->getKey() &&
				session->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE)) ||
				session->getUser()->getProfile()->isAuthorized<ResaRight>(CANCEL) ||
				(_transaction->getCustomerUserId() == session->getUser()->getKey() &&
				session->getUser()->getProfile()->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, CANCEL))
			;
		}
	}
}
