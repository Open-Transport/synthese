
//////////////////////////////////////////////////////////////////////////
/// ReservationTransactionUpdateAction class implementation.
/// @file ReservationTransactionUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "ActionException.h"
#include "City.h"
#include "ParametersMap.h"
#include "ReservationTransactionUpdateAction.hpp"
#include "Request.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTransaction.h"
#include "StopAreaTableSync.hpp"
#include "Reservation.h"
#include "ReservationTableSync.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ReservationTransactionUpdateAction>::FACTORY_KEY("ReservationTransactionUpdateAction");
	}

	namespace resa
	{
		const string ReservationTransactionUpdateAction::PARAMETER_RESERVATION_TRANSACTION_ID = Action_PARAMETER_PREFIX + "ti";
		const string ReservationTransactionUpdateAction::PARAMETER_SEATS = Action_PARAMETER_PREFIX + "se";
		const string ReservationTransactionUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string ReservationTransactionUpdateAction::PARAMETER_DEPARTURE_PLACE_ID = Action_PARAMETER_PREFIX + "dr";
		const string ReservationTransactionUpdateAction::PARAMETER_ARRIVAL_PLACE_ID = Action_PARAMETER_PREFIX + "ar";
		const string ReservationTransactionUpdateAction::PARAMETER_DEPARTURE_TIME = Action_PARAMETER_PREFIX + "dt";
		const string ReservationTransactionUpdateAction::PARAMETER_ARRIVAL_TIME = Action_PARAMETER_PREFIX + "at";



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
				_reservationTransaction = ReservationTransactionTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RESERVATION_TRANSACTION_ID), *_env, UP_DOWN_LINKS_LOAD_LEVEL);
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

			_departureTime = ptime(not_a_date_time);
			if(map.isDefined(PARAMETER_DEPARTURE_PLACE_ID))
			{
				_departurePlace = StopAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_DEPARTURE_PLACE_ID), *_env);
				if(map.isDefined(PARAMETER_DEPARTURE_TIME))
				{
					_departureTime = time_from_string(map.get<string>(PARAMETER_DEPARTURE_TIME));
				}
			}
			
			_arrivalTime = ptime(not_a_date_time);
			if(map.isDefined(PARAMETER_ARRIVAL_PLACE_ID))
			{
				_arrivalPlace = StopAreaTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ARRIVAL_PLACE_ID), *_env);
				if(map.isDefined(PARAMETER_ARRIVAL_TIME))
				{
					_arrivalTime = time_from_string(map.get<string>(PARAMETER_ARRIVAL_TIME));
				}
			}
		}



		void ReservationTransactionUpdateAction::run(
			Request& request
		){
			if(_seats)
			{
				_reservationTransaction->set<Seats>(*_seats);
			}
			if(_name)
			{
				_reservationTransaction->set<CustomerName>(*_name);
			}
			ReservationTransactionTableSync::Save(_reservationTransaction.get());

			if (_departurePlace.get())
			{
				// Only possible to modify the departure_place of the first reservation
				Reservation* reservation = NULL;
				ReservationTableSync::SearchResult reservations(
					ReservationTableSync::Search(*_env, _reservationTransaction->getKey())
				);
				BOOST_FOREACH(const boost::shared_ptr<Reservation>& resa, reservations)
				{
					reservation = resa.get();
					break;
				}
				
				reservation->set<DeparturePlaceId>(_departurePlace->getKey());
				reservation->set<DepartureCityName>(_departurePlace->getCity()->getName());
				reservation->set<DeparturePlaceNameNoCity>(_departurePlace->getName());
				reservation->set<DeparturePlaceName>(_departurePlace->getFullName());
				if (!_departureTime.is_not_a_date_time())
					reservation->set<DepartureTime>(_departureTime);
				ReservationTableSync::Save(reservation);
			}
			
			if (_arrivalPlace.get())
			{
				// Only possible to modify the arrival_place of the last reservation
				ReservationTableSync::SearchResult reservations(
					ReservationTableSync::Search(*_env, _reservationTransaction->getKey())
				);
				Reservation* reservation = NULL;
				BOOST_FOREACH(const boost::shared_ptr<Reservation>& resa, reservations)
				{
					reservation = resa.get();
				}
				reservation->set<ArrivalPlaceId>(_arrivalPlace->getKey());
				reservation->set<ArrivalCityName>(_arrivalPlace->getCity()->getName());
				reservation->set<ArrivalPlaceNameNoCity>(_arrivalPlace->getName());
				reservation->set<ArrivalPlaceName>(_arrivalPlace->getFullName());
				if (!_arrivalTime.is_not_a_date_time())
					reservation->set<ArrivalTime>(_arrivalTime);
				ReservationTableSync::Save(reservation);
			}
		}



		bool ReservationTransactionUpdateAction::isAuthorized(
			const Session* session
		) const {
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
			return true;
		}
	}
}
