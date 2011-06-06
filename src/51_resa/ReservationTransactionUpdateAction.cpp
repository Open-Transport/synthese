
//////////////////////////////////////////////////////////////////////////
/// ReservationTransactionUpdateAction class implementation.
/// @file ReservationTransactionUpdateAction.cpp
/// @author RCSobility
/// @date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ReservationTransactionUpdateAction.hpp"
#include "Request.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTransaction.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ReservationTransactionUpdateAction>::FACTORY_KEY("ReservationTransactionUpdateAction");
	}

	namespace resa
	{
		const string ReservationTransactionUpdateAction::PARAMETER_RESERVATION_TRANSACTION_ID = Action_PARAMETER_PREFIX + "ti";
		const string ReservationTransactionUpdateAction::PARAMETER_SEATS = Action_PARAMETER_PREFIX + "se";
		const string ReservationTransactionUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		
		
		
		ParametersMap ReservationTransactionUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_reservationTransaction.get())
			{
				map.insert(PARAMETER_RESERVATION_TRANSACTION_ID, _reservationTransaction->getKey());
			}
			if(_seats)
			{
				map.insert(PARAMETER_SEATS, *_seats);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			return map;
		}
		
		
		
		void ReservationTransactionUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_reservationTransaction = ReservationTransactionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RESERVATION_TRANSACTION_ID), *_env);
			}
			catch(ObjectNotFoundException<ReservationTransaction>&)
			{
				throw ActionException("No such reservation transaction");
			}

			if(map.isDefined(PARAMETER_SEATS))
			{
				_seats = map.get<size_t>(PARAMETER_SEATS);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}
		}
		
		
		
		void ReservationTransactionUpdateAction::run(
			Request& request
		){
			if(_seats)
			{
				_reservationTransaction->setSeats(*_seats);
			}
			if(_name)
			{
				_reservationTransaction->setCustomerName(*_name);
			}
			ReservationTransactionTableSync::Save(_reservationTransaction.get());
		}
		
		
		
		bool ReservationTransactionUpdateAction::isAuthorized(
			const Session* session
		) const {
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
			return true;
		}
	}
}
