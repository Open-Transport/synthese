
//////////////////////////////////////////////////////////////////////////
/// ReservationUpdateAction class implementation.
/// @file ReservationUpdateAction.cpp
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
#include "ReservationUpdateAction.hpp"
#include "Request.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "Vehicle.hpp"
#include "VehicleTableSync.hpp"
#include "ResaDBLog.h"
#include "DBLogModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt_operation;
	using namespace dblog;
	
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ReservationUpdateAction>::FACTORY_KEY("ReservationUpdateAction");
	}

	namespace resa
	{
		const string ReservationUpdateAction::PARAMETER_RESERVATION_ID = Action_PARAMETER_PREFIX + "ri";
		const string ReservationUpdateAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "vi";
		const string ReservationUpdateAction::PARAMETER_SEAT_NUMBER = Action_PARAMETER_PREFIX + "sn";

		
		
		ParametersMap ReservationUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_reservation.get())
			{
				map.insert(PARAMETER_RESERVATION_ID, _reservation->getKey());
			}
			if(_vehicle)
			{
				map.insert(PARAMETER_VEHICLE_ID, _vehicle->get() ? (*_vehicle)->getKey() : RegistryKeyType(0));
			}
			if(_seatNumber)
			{
				map.insert(PARAMETER_SEAT_NUMBER, *_seatNumber);
			}
			return map;
		}
		
		
		
		void ReservationUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_reservation = ReservationTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RESERVATION_ID), *_env);
			}
			catch(ObjectNotFoundException<Reservation>& e)
			{
				throw ActionException("No such reservation");
			}

			if(map.isDefined(PARAMETER_VEHICLE_ID)) try
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_VEHICLE_ID));
				if(id > 0)
				{
					_vehicle = VehicleTableSync::Get(id, *_env);
				}
			}
			catch(ObjectNotFoundException<Vehicle>& e)
			{
				throw ActionException("No such vehicle");
			}

			if(map.isDefined(PARAMETER_SEAT_NUMBER))
			{
				_seatNumber = map.get<string>(PARAMETER_SEAT_NUMBER);
			}
		}
		
		
		
		void ReservationUpdateAction::run(
			Request& request
		){
			stringstream text;
			if(_vehicle)
			{
				DBLogModule::appendToLogIfChange(
					text,
					"Vehicle",
					_reservation->getVehicle() ? _reservation->getVehicle()->getName() : "(undefined)",
					_vehicle->get() ? (*_vehicle)->getName() : "(undefined)"
				);
				_reservation->setVehicle(_vehicle->get());
			}

			if(_seatNumber)
			{
				DBLogModule::appendToLogIfChange(
					text,
					"Seat number",
					_reservation->getSeatNumber(),
					*_seatNumber
				);
				_reservation->setSeatNumber(*_seatNumber);
			}

			ReservationTableSync::Save(_reservation.get());

//			ResaDBLog::AddReservationUpdateEntry(
//				*request.getSession(),
//				*_reservation,
//				text.str()
//			);
		}
		
		
		
		bool ReservationUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
	}
}
