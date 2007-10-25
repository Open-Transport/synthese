
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

#include "31_resa/ReservationTransaction.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/ReservationTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "01_util/Conversion.h"

using namespace std;
/*
namespace synthese
{
	using namespace server;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::CancelReservationAction>::FACTORY_KEY("cancelreservation");
	}

	namespace resa
	{
		const string CancelReservationAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "pw";
		const string CancelReservationAction::PARAMETER_RESERVATION_ID = Action_PARAMETER_PREFIX + "ri";
		const string CancelReservationAction::PARAMETER_RESERVATION_TRANSACTION_ID = Action_PARAMETER_PREFIX + "rt";

		
		
		CancelReservationAction::CancelReservationAction()
			: util::FactorableTemplate<Action, CancelReservationAction>()
		{
		}
		
		
		
		ParametersMap CancelReservationAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(make_pair(PARAMETER_RESERVATION_ID, Conversion::ToString(_resa->getKey())));
			map.insert(make_pair(PARAMETER_RESERVATION_TRANSACTION_ID, Conversion::ToString(_transaction->getKey())));
			return map;
		}
		
		
		
		void CancelReservationAction::_setFromParametersMap(const ParametersMap& map)
		{
			_resaId = map->getUid(PARAMETER_RESERVATION_ID);
			if (_resaId != UNKNOWN_VALUE)
			{
				uid id(ReservationTableSync::GetFieldValue(_resaId, ReservationTableSync::COL_TRANSACTION_ID));
				_transaction = ReservationTransactionTableSync::Get(id);
			}
			else
			{
				_transaction = ReservationTransactionTableSync::Get(map->getUid(PARAMETER_RESERVATION_TRANSACTION_ID));
			}

			// Password control
		}
		
		
		
		void CancelReservationAction::run()
		{
			DateTime now(TIME_CURRENT);
			_transaction->setCancellationTime(now);
			_transaction->setCancelUserId(_request->getUser()->getKey());
			ReservationTransactionTableSync::save(_transaction.get());
		}
	}
}*/
