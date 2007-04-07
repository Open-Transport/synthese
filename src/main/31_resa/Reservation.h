
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

namespace synthese
{
	namespace env
	{
		class CommercialLine;
		class Place;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		class OnlineReservationRule;

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
			const env::CommercialLine*		_line;				//!< Axe de la ligne à la demande
			std::string						_lineCode;			//<! Comprehensive line code
			std::string						_serviceCode;			//!< Code du service au sein l'axe
			const env::Place*				_departurePlace;		//!< Code du point d'arrêt de montée
			std::string						_departurePlaceName;
			const env::Place*				_arrivalPlace;	//!< Code du point d'arrêt de descente
			std::string						_arrivalPlaceName;
			const OnlineReservationRule*	_reservationRule;	//!< Code de la modalité de reservation
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
			security::User*					_customerUser;
			std::string						_customerName;
			std::string						_customerPhone;
			security::User*					_bookingUser;
			security::User*					_cancelUser;
			//@}

		public:
			//!	\name Modificateurs
			//@{
			//@}

			//!	\name Modificateurs
			//@{
			//@}

			//!	\name Accesseurs
			//@{
			//@}

		};
	}
}

#endif // SYNTHESE_Reservation_h__
