
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"

#include "Conversion.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace time;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::CancelReservationAction>::FACTORY_KEY("cancelreservation");
	}

	namespace resa
	{
		const string CancelReservationAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pw";
		const string CancelReservationAction::PARAMETER_RESERVATION_TRANSACTION_ID = Action_PARAMETER_PREFIX + "rt";

		
		
		CancelReservationAction::CancelReservationAction()
			: util::FactorableTemplate<Action, CancelReservationAction>()
		{
		}
		
		
		
		ParametersMap CancelReservationAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_RESERVATION_TRANSACTION_ID, _transaction->getKey());
			return map;
		}
		
		
		
		void CancelReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Load of the transaction
			try
			{
				_transaction = ReservationTransactionTableSync::GetEditable(map.getUid(PARAMETER_RESERVATION_TRANSACTION_ID, true, FACTORY_KEY));
			}
			catch(...)
			{
				throw ActionException("No such reservation");
			}

			// Minimal right is standard user
			if (!_request->isAuthorized<ResaRight>(FORBIDDEN, WRITE))
				throw ActionException("Non autorisé");

			// Standard user
			if (!_request->isAuthorized<ResaRight>(WRITE, WRITE))
			{
				if (_request->getUser()->getKey() != _transaction->getCustomerUserId())
					throw ActionException("Non autorisé");

				string password(map.getString(PARAMETER_PASSWORD, true, FACTORY_KEY));
				if (password.empty())
					throw ActionException("Le mot de passe doit être fourni");

				if (password != _request->getUser()->getPassword())
				{
					throw ActionException("Mot de passe erronné");
				}
			}

			// Control of the date : a cancellation has no sense if after the arrival time
			DateTime now(TIME_CURRENT);
			Env env;
			ReservationTableSync::Search(env, _transaction.get());
			BOOST_FOREACH(shared_ptr<Reservation> resa, env.template getRegistry<Reservation>())
			{
				if (resa->getReservationRuleId() != UNKNOWN_VALUE)
				{
					if (now > resa->getArrivalTime())
						throw ActionException("Le statut de la réservation ne permet pas de l'annuler");
					break;
				}
			}
		}
		
		
		
		void CancelReservationAction::run()
		{
			// Store old parameters
			ReservationStatus oldStatus(_transaction->getStatus());

			// Write the cancellation
			DateTime now(TIME_CURRENT);
			_transaction->setCancellationTime(now);
			_transaction->setCancelUserId(_request->getUser()->getKey());
			ReservationTransactionTableSync::Save(_transaction.get());

			// Write the log
			ResaDBLog::AddCancelReservationEntry(_request->getSession(), *_transaction, oldStatus);
		}



		void CancelReservationAction::setTransaction( boost::shared_ptr<ReservationTransaction> transaction )
		{
			_transaction = transaction;
		}
	}
}
