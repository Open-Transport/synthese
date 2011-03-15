
/** Reservation class header.
	@file Reservation.h

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

#ifndef SYNTHESE_Reservation_h__
#define SYNTHESE_Reservation_h__

#include <boost/date_time/posix_time/ptime.hpp>
#include <string>

#include "ResaTypes.h"

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace pt_operation
	{
		class Vehicle;
		class VehiclePosition;
	}

	namespace resa
	{
		class ReservationTransaction;

		/** Reservation class.
			@ingroup m31

			A Reservation object can link to its departures and arrival places, commercial lines, etc.
			In order to prevent from broken links, the object contains a copy of the main informations.
		*/
		class Reservation
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Reservation>	Registry;

			Reservation(util::RegistryKeyType key = 0);


		private:

			const ReservationTransaction*		_transaction;

			//!	\name Service réservé
			//@{
				util::RegistryKeyType _lineId;				//!< ID of the booked CommercialLine
				std::string						_lineCode;				//!< Comprehensive line code
				util::RegistryKeyType			_serviceId;				//!< Code du service au sein l'axe
				std::string						_serviceCode;			//!< Code du service au sein l'axe
				util::RegistryKeyType			_departurePlaceId;		//!< ID of the departure Place
				std::string						_departurePlaceName;
				util::RegistryKeyType			_arrivalPlaceId;		//!< ID of the arrival Place
				std::string						_arrivalPlaceName;
				util::RegistryKeyType			_reservationRuleId;		//!< ID of the used OnlineReservationRule
				std::string						_departureAddress;		//!< Adresse du départ
				std::string						_arrivalAddress;		//!< Adresse d'arrivée
				boost::posix_time::ptime		_departureTime;			//!< Moment de montée prévu
				boost::posix_time::ptime		_arrivalTime;			//!< Moment d'arrivée prévue
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::ptime		_reservationDeadLine;
				const pt_operation::Vehicle*	_vehicle;
				std::string						_seatNumber;
				const pt_operation::VehiclePosition*	_vehiclePositionAtDeparture;
				const pt_operation::VehiclePosition*	_vehiclePositionAtArrival;
			//@}

		public:
			//!	\name Setters
			//@{
				void setLineId				(util::RegistryKeyType id){ _lineId = id; }
				void setLineCode			(const std::string& code){ _lineCode = code; }
				void setServiceId			(util::RegistryKeyType id){ _serviceId = id; }
				void setServiceCode			(const std::string& code){ _serviceCode = code; }
				void setDeparturePlaceId	(util::RegistryKeyType id){ _departurePlaceId = id; }
				void setDeparturePlaceName	(const std::string& name){ _departurePlaceName = name; }
				void setArrivalPlaceId		(util::RegistryKeyType id){ _arrivalPlaceId = id; }
				void setArrivalPlaceName	(const std::string& name){ _arrivalPlaceName = name; }
				void setReservationRuleId	(util::RegistryKeyType id){ _reservationRuleId = id; }
				void setDepartureAddress	(const std::string& address){ _departureAddress = address; }
				void setArrivalAddress		(const std::string& address){ _arrivalAddress = address; }
				void setDepartureTime		(const boost::posix_time::ptime& time){ _departureTime = time; }
				void setArrivalTime			(const boost::posix_time::ptime& time){ _arrivalTime = time; }
				void setVehicle				(const pt_operation::Vehicle* value){ _vehicle = value; }
				void setSeatNumber			(const std::string& value){ _seatNumber = value; }
				void setOriginDateTime		(const boost::posix_time::ptime& time){ _originDateTime = time; }
				void setReservationDeadLine	(const boost::posix_time::ptime& time){ _reservationDeadLine = time; }
				void setVehiclePositionAtDeparture	(const pt_operation::VehiclePosition* value){ _vehiclePositionAtDeparture = value; }
				void setVehiclePositionAtArrival	(const pt_operation::VehiclePosition* value){ _vehiclePositionAtArrival = value; }


				/** Transaction setter.
					@param transaction the transaction which the reservation belongs
					@author Hugues Romain
					@date 2009
					The reservation is also added to the transaction.
				*/
				void setTransaction			(ReservationTransaction* transaction);
			//@}

			//!	\name Getters
			//@{
				util::RegistryKeyType			getLineId()					const { return _lineId; }
				const std::string&				getLineCode()				const { return _lineCode; }
				util::RegistryKeyType			getServiceId()				const { return _serviceId; }
				const std::string&				getServiceCode()			const { return _serviceCode; }
				util::RegistryKeyType			getDeparturePlaceId()		const { return _departurePlaceId; }
				const std::string&				getDeparturePlaceName()		const { return _departurePlaceName; }
				util::RegistryKeyType			getArrivalPlaceId()			const { return _arrivalPlaceId; }
				const std::string&				getArrivalPlaceName()		const { return _arrivalPlaceName; }
				util::RegistryKeyType			getReservationRuleId()		const { return _reservationRuleId; }
				const std::string&				getDepartureAddress()		const { return _departureAddress; }
				const std::string&				getArrivalAddress()			const { return _arrivalAddress; }
				const boost::posix_time::ptime&	getDepartureTime()			const { return _departureTime; }
				const boost::posix_time::ptime&	getArrivalTime()			const { return _arrivalTime; }
				const ReservationTransaction*	getTransaction()			const { return _transaction; }
				const boost::posix_time::ptime&	getOriginDateTime()			const { return _originDateTime; }
				const boost::posix_time::ptime&	getReservationDeadLine()	const { return _reservationDeadLine; }
				const pt_operation::Vehicle*	getVehicle()				const { return _vehicle; }
				const std::string&				getSeatNumber()				const { return _seatNumber; }
				const pt_operation::VehiclePosition*	getVehiclePositionAtDeparture()	const { return _vehiclePositionAtDeparture; }
				const pt_operation::VehiclePosition*	getVehiclePositionAtArrival()	const { return _vehiclePositionAtArrival; }
			//@}



			//! @name Queries
			//@{
				ReservationStatus	getStatus()			const;
				std::string			getFullStatusText()	const;
			//@}
		};
	}
}

#endif // SYNTHESE_Reservation_h__
