
//////////////////////////////////////////////////////////////////////////
/// ReservationUpdateAction class implementation.
/// @file ReservationUpdateAction.cpp
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
#include "ParametersMap.h"
#include "ReservationUpdateAction.hpp"
#include "Request.h"
#include "Reservation.h"
#include "ReservationTableSync.h"
#include "Vehicle.hpp"
#include "VehicleTableSync.hpp"
#include "ResaDBLog.h"
#include "ResaModule.h"
#include "DBLogModule.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "UserTableSync.h"
#include "VehiclePosition.hpp"
#include "VehiclePositionTableSync.hpp"
#include "ReservationTransaction.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace vehicle;
	using namespace dblog;
	using namespace pt;
	using namespace graph;
	using namespace db;


	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::ReservationUpdateAction>::FACTORY_KEY("ReservationUpdateAction");
	}

	namespace resa
	{
		const string ReservationUpdateAction::PARAMETER_RESERVATION_ID = Action_PARAMETER_PREFIX + "ri";
		const string ReservationUpdateAction::PARAMETER_VEHICLE_ID = Action_PARAMETER_PREFIX + "vi";
		const string ReservationUpdateAction::PARAMETER_SEAT_NUMBER = Action_PARAMETER_PREFIX + "sn";
		const string ReservationUpdateAction::PARAMETER_REAL_DEPARTURE_TIME(Action_PARAMETER_PREFIX + "rd");
		const string ReservationUpdateAction::PARAMETER_REAL_ARRIVAL_TIME(Action_PARAMETER_PREFIX + "ra");
		const string ReservationUpdateAction::PARAMETER_CANCELLED_BY_OPERATOR(Action_PARAMETER_PREFIX + "co");
		const string ReservationUpdateAction::PARAMETER_DEPARTURE_METER_OFFSET(Action_PARAMETER_PREFIX + "dm");
		const string ReservationUpdateAction::PARAMETER_ACKNOWLEDGE_TIME = Action_PARAMETER_PREFIX + "_acknowledge_time";
		const string ReservationUpdateAction::PARAMETER_ACKNOWLEDGE_USER_ID = Action_PARAMETER_PREFIX + "_acknowledge_user_id";
		const string ReservationUpdateAction::PARAMETER_ARRIVAL_METER_OFFSET(Action_PARAMETER_PREFIX + "am");
		const string ReservationUpdateAction::PARAMETER_CANCELLATION_ACKNOWLEDGE_TIME = Action_PARAMETER_PREFIX + "_cancellation_acknowledge_time";
		const string ReservationUpdateAction::PARAMETER_CANCELLATION_ACKNOWLEDGE_USER_ID = Action_PARAMETER_PREFIX + "_cancellation_acknowledge_user_id";
		const string ReservationUpdateAction::PARAMETER_CANCEL_ACKNOWLEDGEMENT = Action_PARAMETER_PREFIX + "_cancel_acknowledgement";



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
			if(_realDepartureTime)
			{
				map.insert(PARAMETER_REAL_DEPARTURE_TIME, *_realDepartureTime);
			}
			if(_realArrivalTime)
			{
				map.insert(PARAMETER_REAL_ARRIVAL_TIME, *_realArrivalTime);
			}
			if(_cancelledByOperator)
			{
				map.insert(PARAMETER_CANCELLED_BY_OPERATOR, *_cancelledByOperator);
			}
			if(_departureMeterOffset)
			{
				map.insert(PARAMETER_DEPARTURE_METER_OFFSET, *_departureMeterOffset);
			}
			if(_arrivalMeterOffset)
			{
				map.insert(PARAMETER_ARRIVAL_METER_OFFSET, *_arrivalMeterOffset);
			}
			return map;
		}



		void ReservationUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_reservation = ReservationTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_RESERVATION_ID), *_env);
			}
			catch(ObjectNotFoundException<Reservation>&)
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
				else
				{
					_vehicle = boost::shared_ptr<Vehicle>();
				}
			}
			catch(ObjectNotFoundException<Vehicle>&)
			{
				throw ActionException("No such vehicle");
			}

			if(map.isDefined(PARAMETER_SEAT_NUMBER))
			{
				_seatNumber = map.get<string>(PARAMETER_SEAT_NUMBER);
			}

			if(map.isDefined(PARAMETER_REAL_DEPARTURE_TIME))
			{
				_realDepartureTime = time_from_string(map.get<string>(PARAMETER_REAL_DEPARTURE_TIME));
			}

			if(map.isDefined(PARAMETER_REAL_ARRIVAL_TIME))
			{
				_realArrivalTime = time_from_string(map.get<string>(PARAMETER_REAL_ARRIVAL_TIME));
				if(_realDepartureTime && *_realDepartureTime > *_realArrivalTime)
				{
					throw ActionException("Arrival time must be after than departure time");
				}
			}

			// Acknowledge time
			if(map.isDefined(PARAMETER_ACKNOWLEDGE_TIME))
			{
				_acknowledgeTime = time_from_string(map.get<string>(PARAMETER_ACKNOWLEDGE_TIME));
			}

			// Acknowledge user
			if(map.isDefined(PARAMETER_ACKNOWLEDGE_USER_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_ACKNOWLEDGE_USER_ID));
				if(id > 0)
				{
					_acknowledgeUser = UserTableSync::GetEditable(id, *_env);
				}
				else
				{
					_acknowledgeUser = boost::shared_ptr<User>();
				}
			}

			if (map.isDefined(PARAMETER_CANCEL_ACKNOWLEDGEMENT) &&
				!map.isDefined(PARAMETER_ACKNOWLEDGE_TIME))
			{
				_cancelAcknowledgement = map.get<bool>(PARAMETER_CANCEL_ACKNOWLEDGEMENT);
			}

			// Cancellation acknowledge time
			if(map.isDefined(PARAMETER_CANCELLATION_ACKNOWLEDGE_TIME))
			{
				_cancellationAcknowledgeTime = time_from_string(map.get<string>(PARAMETER_CANCELLATION_ACKNOWLEDGE_TIME));
			}

			// Cancellation acknowledge user
			if(map.isDefined(PARAMETER_CANCELLATION_ACKNOWLEDGE_USER_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CANCELLATION_ACKNOWLEDGE_USER_ID));
				if(id > 0)
				{
					_cancellationAcknowledgeUser = UserTableSync::GetEditable(id, *_env);
				}
				else
				{
					_cancellationAcknowledgeUser = boost::shared_ptr<User>();
				}
			}

			if(map.isDefined(PARAMETER_CANCELLED_BY_OPERATOR))
			{
				_cancelledByOperator = map.get<bool>(PARAMETER_CANCELLED_BY_OPERATOR);
			}

			if(map.isDefined(PARAMETER_DEPARTURE_METER_OFFSET) && !map.get<string>(PARAMETER_DEPARTURE_METER_OFFSET).empty())
			{
				_departureMeterOffset = map.get<VehiclePosition::Meters>(PARAMETER_DEPARTURE_METER_OFFSET);
			}

			if(map.isDefined(PARAMETER_ARRIVAL_METER_OFFSET) && !map.get<string>(PARAMETER_ARRIVAL_METER_OFFSET).empty())
			{
				_arrivalMeterOffset = map.get<VehiclePosition::Meters>(PARAMETER_ARRIVAL_METER_OFFSET);
				if(!_departureMeterOffset)
				{
					throw ActionException("Departure meter offset must be defined too");
				}
				if(*_departureMeterOffset > *_arrivalMeterOffset)
				{
					throw ActionException("Arrival meter offset must be greater than departure meter offset");
				}
			}
			else if(_departureMeterOffset)
			{
				throw ActionException("Arrival meter offset must be defined too");
			}

			if(	_realDepartureTime || _realArrivalTime || _departureMeterOffset || _arrivalMeterOffset)
			{
				if(!_reservation->getVehicle() && (!_vehicle || !_vehicle->get()))
				{
					throw ActionException("A vehicle must be associated to the reservation");
				}

				if(!_realDepartureTime || _realDepartureTime->is_not_a_date_time())
				{
					_realDepartureTime = _reservation->getDepartureTime();
				}

				if(!_realArrivalTime || _realArrivalTime->is_not_a_date_time())
				{
					_realArrivalTime = _reservation->getArrivalTime();
				}

				if(_cancelledByOperator && *_cancelledByOperator)
				{
					throw ActionException("The reservation was cancelled by the operator");
				}
			}
		}


		void ReservationUpdateAction::run(
			Request& request
		){
			stringstream text;
			if(_vehicle && _reservation->getVehicle() != _vehicle->get())
			{
				DBLogModule::appendToLogIfChange(
					text,
					"Affectation véhicule",
					_reservation->getVehicle() ? _reservation->getVehicle()->getName() : "(undefined)",
					_vehicle->get() ? (*_vehicle)->getName() : "(undefined)"
				);
				_reservation->setVehicle(_vehicle->get());
			}

			// Acknowledge user
			if(_acknowledgeUser)
			{
				_reservation->setAcknowledgeUser(_acknowledgeUser->get());
			}
			else if(_cancelAcknowledgement)
			{
				_reservation->setAcknowledgeUser(NULL);
			}

			// Acknowledge time
			if(_acknowledgeTime)
			{
				_reservation->setAcknowledgeTime(*_acknowledgeTime);
			}
			else if(_cancelAcknowledgement)
			{
				_reservation->setAcknowledgeTime(not_a_date_time);
			}
			if(_acknowledgeUser && !_acknowledgeTime)
			{
				ptime now(second_clock::local_time());
				_reservation->setAcknowledgeTime(now);
			}

			// Cancellation acknowledge user
			if(_cancellationAcknowledgeUser)
			{
				_reservation->setCancellationAcknowledgeUser(
					_cancellationAcknowledgeUser->get()
				);
			}

			// Cancellation acknowledge time
			if(_cancellationAcknowledgeTime)
			{
				_reservation->setCancellationAcknowledgeTime(
					*_cancellationAcknowledgeTime
				);
			}
			if(_cancellationAcknowledgeUser && !_cancellationAcknowledgeTime)
			{
				ptime now(second_clock::local_time());
				_reservation->setCancellationAcknowledgeTime(now);
			}

			if(_seatNumber && _reservation->getSeatNumber() != *_seatNumber)
			{
				DBLogModule::appendToLogIfChange(
					text,
					"Numéro de siège",
					_reservation->getSeatNumber(),
					*_seatNumber
				);
				_reservation->setSeatNumber(*_seatNumber);
			}

			if(_cancelledByOperator && _reservation->getCancelledByOperator() != *_cancelledByOperator)
			{
				DBLogModule::appendToLogIfChange(
					text,
					"Annulation par le transporteur",
					_reservation->getCancelledByOperator(),
					*_cancelledByOperator
				);
				_reservation->setCancelledByOperator(*_cancelledByOperator);
			}

			if(_realDepartureTime)
			{
				// Removes passengers from existing positions
				if(_reservation->getVehiclePositionAtDeparture() && _reservation->getVehiclePositionAtArrival())
				{
					VehiclePositionTableSync::ChangePassengers(
						*_reservation->getVehiclePositionAtDeparture(),
						*_reservation->getVehiclePositionAtArrival(),
						0,
						_reservation->getTransaction()->getSeats()
					);
				}

				{
					boost::shared_ptr<VehiclePosition> position;
					if(_reservation->getVehiclePositionAtDeparture())
					{
						position = _env->getEditableSPtr(const_cast<VehiclePosition*>(_reservation->getVehiclePositionAtDeparture()));
					}
					else
					{
						VehiclePositionTableSync::SearchResult existingPositions(
							VehiclePositionTableSync::Search(
								*_env,
								_reservation->getVehicle()->getKey(),
								_realDepartureTime,
								_realDepartureTime
						)	);
						if(existingPositions.empty())
						{
							position.reset(new VehiclePosition);
							position->setKey(VehiclePositionTableSync::getId());
							position->setTime(*_realDepartureTime);
							position->setVehicle(const_cast<Vehicle*>(_reservation->getVehicle()));
							_env->getEditableRegistry<VehiclePosition>().add(position);
						}
						else
						{
							position = *existingPositions.begin();
						}
					}

					boost::shared_ptr<const StopArea> stopArea(StopAreaTableSync::Get(_reservation->getDeparturePlaceId(), *_env));
					StopPointTableSync::Search(*_env, stopArea->getKey());
					const StopPoint* stopPoint(stopArea->getPhysicalStops().begin()->second);
					boost::shared_ptr<ScheduledService> service(ScheduledServiceTableSync::GetEditable(_reservation->getServiceId(), *_env));
					Edge* edge(
						service->getEdgeFromStopAndTime(
							*stopPoint,
							_reservation->getDepartureTime() - _reservation->getOriginDateTime(),
							true
					)	);

					ptime beforeDepartureTime(*_realDepartureTime);
					beforeDepartureTime -= seconds(30);
					VehiclePositionTableSync::SearchResult oldPositions(
						VehiclePositionTableSync::Search(
							*_env,
							_reservation->getVehicle()->getKey(),
							beforeDepartureTime,
							_realDepartureTime
					)	);
					bool geomToWrite(true);
					BOOST_FOREACH(const boost::shared_ptr<VehiclePosition>& oldPosition, oldPositions)
					{
						if(oldPosition->getGeometry())
						{
							geomToWrite = false;
							break;
						}
					}
					if(geomToWrite)
					{
						position->setGeometry(stopPoint->getGeometry());
					}
					position->setStatus(VehiclePosition::COMMERCIAL);
					position->setStopPoint(const_cast<StopPoint*>(stopPoint));
					position->setService(service.get());
					if(edge)
					{
						position->setRankInPath(edge->getRankInPath());
					}
					_reservation->setVehiclePositionAtDeparture(position.get());

					if(_departureMeterOffset)
					{
						position->setMeterOffset(*_departureMeterOffset);
					}
					VehiclePositionTableSync::Save(position.get());
				}

				{
					boost::shared_ptr<VehiclePosition> position;
					if(_reservation->getVehiclePositionAtArrival())
					{
						position = _env->getEditableSPtr(const_cast<VehiclePosition*>(_reservation->getVehiclePositionAtArrival()));
					}
					else
					{
						VehiclePositionTableSync::SearchResult existingPositions(
							VehiclePositionTableSync::Search(
								*_env,
								_reservation->getVehicle()->getKey(),
								_realArrivalTime,
								_realArrivalTime
						)	);
						if(existingPositions.empty())
						{
							position.reset(new VehiclePosition);
							position->setKey(VehiclePositionTableSync::getId());
							position->setTime(*_realArrivalTime);
							position->setVehicle(const_cast<Vehicle*>(_reservation->getVehicle()));
							_env->getEditableRegistry<VehiclePosition>().add(position);
						}
						else
						{
							position = *existingPositions.begin();
						}
					}

					boost::shared_ptr<const StopArea> stopArea(StopAreaTableSync::Get(_reservation->getArrivalPlaceId(), *_env));
					StopPointTableSync::Search(*_env, stopArea->getKey());
					const StopPoint* stopPoint(stopArea->getPhysicalStops().begin()->second);
					boost::shared_ptr<ScheduledService> service(ScheduledServiceTableSync::GetEditable(_reservation->getServiceId(), *_env));
					Edge* edge(
						service->getEdgeFromStopAndTime(
							*stopPoint,
							_reservation->getArrivalTime() - _reservation->getOriginDateTime(),
							false
					)	);

					ptime beforeArrivalTime(*_realArrivalTime);
					beforeArrivalTime -= seconds(30);
					VehiclePositionTableSync::SearchResult oldPositions(
						VehiclePositionTableSync::Search(
							*_env,
							_reservation->getVehicle()->getKey(),
							beforeArrivalTime,
							_realArrivalTime
					)	);
					bool geomToWrite(true);
					BOOST_FOREACH(const boost::shared_ptr<VehiclePosition>& oldPosition, oldPositions)
					{
						if(oldPosition->getGeometry())
						{
							geomToWrite = false;
							break;
						}
					}
					if(geomToWrite)
					{
						position->setGeometry(stopPoint->getGeometry());
					}
					position->setStatus(VehiclePosition::COMMERCIAL);
					position->setStopPoint(const_cast<StopPoint*>(stopPoint));
					position->setService(service.get());
					if(edge)
					{
						position->setRankInPath(edge->getRankInPath());
					}
					_reservation->setVehiclePositionAtArrival(position.get());

					if(_arrivalMeterOffset)
					{
						position->setMeterOffset(*_arrivalMeterOffset);
					}
					VehiclePositionTableSync::Save(position.get());
				}

				if(_reservation->getVehiclePositionAtDeparture() && _reservation->getVehiclePositionAtArrival())
				{
					VehiclePositionTableSync::ChangePassengers(
						*_reservation->getVehiclePositionAtDeparture(),
						*_reservation->getVehiclePositionAtArrival(),
						_reservation->getTransaction()->getSeats(),
						0
					);
				}
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
}	}
