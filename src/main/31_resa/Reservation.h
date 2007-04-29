
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

#include <string>

#include "04_time/DateTime.h"

#include "01_util/UId.h"

namespace synthese
{
	namespace resa
	{
		/** Reservation class.
			@ingroup m31

			A Reservation object can link to its departures and arrival places, commercial lines, etc.
			In order to prevent from broken links, the object contains a copy of the main informations.
		*/
		class Reservation
		{
		public:
			Reservation();


		private:

			//!	\name Service réservé
			//@{
				uid								_lineId;			//<! ID of the booked CommercialLine
				std::string						_lineCode;			//<! Comprehensive line code
				std::string						_serviceCode;			//!< Code du service au sein l'axe
				uid								_departurePlaceId;		//!< ID of the departure Place
				std::string						_departurePlaceName;
				uid								_arrivalPlaceId;		//!< ID of the arrival Place
				std::string						_arrivalPlaceName;
				uid								_reservationRuleId;	//!< ID of the used OnlineReservationRule
				std::string						_departureAddress;			//!< Adresse du départ
				std::string						_arrivalAddress;		//!< Adresse d'arrivée
				time::DateTime					_departureTime;			//!< Moment de montée prévu
				time::DateTime					_arrivalTime;			//!< Moment d'arrivée prévue
			//@}

			//!	\name Caractéristiques de la réservation
			//@{
				uid								_lastReservation;		//!< Code de la réservation annulée en cas de modification
				int								_seats;			//!< Nombre de places
				time::DateTime					_bookingTime;		//!< Date de la réservation
				time::DateTime					_cancellationTime;		//!< Date de l'annulation (unknown = not cancelled)
			//@}

			//!	\name Personnes
			//@{
				uid								_customerUserId;
				std::string						_customerName;
				std::string						_customerPhone;
				uid								_bookingUserId;
				uid								_cancelUserId;
			//@}

		public:
			//!	\name Setters
			//@{
				void setLineId(uid id);
				void setLineCode(const std::string& code);
				void setServiceCode(const std::string& code);
				void setDeparturePlaceId(uid id);
				void setDeparturePlaceName(const std::string& name);
				void setArrivalPlaceId(uid id);
				void setArrivalPlaceName(const std::string& name);
				void setReservationRuleId(uid id);
				void setDepartureAddress(const std::string& address);
				void setArrivalAddress(const std::string& address);
				void setDepartureTime(const time::DateTime& time);
				void setArrivalTime(const time::DateTime& time);
				void setLastReservation(uid id);
				void setSeats(int seats);
				void setBookingTime(const time::DateTime& time);
				void setCancellationTime(const time::DateTime& time);
				void setCustomerUserId(uid id);
				void setCustomerName(const std::string& name);
				void setCustomerPhone(const std::string& phone);
				void setBookingUserId(uid id);
				void setCancelUserId(uid id);
			//@}

			//!	\name Getters
			//@{
				uid getLineId() const;
				const std::string& getLineCode() const;
				void setServiceCode(const std::string& code);
				void setDeparturePlaceId(uid id);
				void setDeparturePlaceName(const std::string& name);
				void setArrivalPlaceId(uid id);
				void setArrivalPlaceName(const std::string& name);
				void setReservationRuleId(uid id);
				void setDepartureAddress(const std::string& address);
				void setArrivalAddress(const std::string& address);
				void setDepartureTime(const time::DateTime& time);
				void setArrivalTime(const time::DateTime& time);
				void setLastReservation(uid id);
				void setSeats(int seats);
				void setBookingTime(const time::DateTime& time);
				void setCancellationTime(const time::DateTime& time);
				void setCustomerUserId(uid id);
				void setCustomerName(const std::string& name);
				void setCustomerPhone(const std::string& phone);
				void setBookingUserId(uid id);
				void setCancelUserId(uid id);
			//@}

		};
	}
}

#endif // SYNTHESE_Reservation_h__
