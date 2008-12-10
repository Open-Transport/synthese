
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

#include "31_resa/Types.h"

#include "DateTime.h"

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace resa
	{
		class ReservationTransaction;

		/** Reservation class.
			@ingroup m31

			A Reservation object can link to its departures and arrival places, commercial lines, etc.
			In order to prevent from broken links, the object contains a copy of the main informations.

			@warning Do not create a single Reservation object. Use ReservationTransaction::newReservation() instead.
		*/
		class Reservation
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Reservation>	Registry;

			Reservation(util::RegistryKeyType key = UNKNOWN_VALUE);


		private:

			const ReservationTransaction*		_transaction;

			//!	\name Service réservé
			//@{
				uid								_lineId;				//!< ID of the booked CommercialLine
				std::string						_lineCode;				//!< Comprehensive line code
				uid								_serviceId;				//!< Code du service au sein l'axe
				std::string						_serviceCode;			//!< Code du service au sein l'axe
				uid								_departurePlaceId;		//!< ID of the departure Place
				std::string						_departurePlaceName;
				uid								_arrivalPlaceId;		//!< ID of the arrival Place
				std::string						_arrivalPlaceName;
				uid								_reservationRuleId;		//!< ID of the used OnlineReservationRule
				std::string						_departureAddress;		//!< Adresse du départ
				std::string						_arrivalAddress;		//!< Adresse d'arrivée
				time::DateTime					_departureTime;			//!< Moment de montée prévu
				time::DateTime					_arrivalTime;			//!< Moment d'arrivée prévue
				time::DateTime					_originDateTime;
				time::DateTime					_reservationDeadLine;
			//@}

		public:
			//!	\name Setters
			//@{
				void setLineId				(uid id);
				void setLineCode			(const std::string& code);
				void setServiceId			(uid id);
				void setServiceCode			(const std::string& code);
				void setDeparturePlaceId	(uid id);
				void setDeparturePlaceName	(const std::string& name);
				void setArrivalPlaceId		(uid id);
				void setArrivalPlaceName	(const std::string& name);
				void setReservationRuleId	(uid id);
				void setDepartureAddress	(const std::string& address);
				void setArrivalAddress		(const std::string& address);
				void setDepartureTime		(const time::DateTime& time);
				void setArrivalTime			(const time::DateTime& time);
				void setTransaction			(const ReservationTransaction* transaction);
				void setOriginDateTime		(const time::DateTime& time);
				void setReservationDeadLine	(const time::DateTime& time);
			//@}

			//!	\name Getters
			//@{
				uid								getLineId()					const;
				const std::string&				getLineCode()				const;
				uid								getServiceId()				const;
				const std::string&				getServiceCode()			const;
				uid								getDeparturePlaceId()		const;
				const std::string&				getDeparturePlaceName()		const;
				uid								getArrivalPlaceId()			const;
				const std::string&				getArrivalPlaceName()		const;
				uid								getReservationRuleId()		const;
				const std::string&				getDepartureAddress()		const;
				const std::string&				getArrivalAddress()			const;
				const time::DateTime&			getDepartureTime()			const;
				const time::DateTime&			getArrivalTime()			const;
				const ReservationTransaction*	getTransaction()			const;
				const time::DateTime&			getOriginDateTime()			const;
				const time::DateTime&			getReservationDeadLine()	const;
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
