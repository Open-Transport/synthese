
/** ReservationTransaction class header.
	@file ReservationTransaction.h

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

#ifndef SYNTHESE_resa_ReservationTransaction_h__
#define SYNTHESE_resa_ReservationTransaction_h__

#include "04_time/DateTime.h"

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include <vector>

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace resa
	{
		class Reservation;

		/** ReservationTransaction class.
			@ingroup m31
		*/
		class ReservationTransaction : public util::Registrable<uid, ReservationTransaction>
		{
			std::vector<boost::shared_ptr<Reservation> >	_reservations;

			//!	\name Caractéristiques de la réservation
			//@{
				uid								_lastReservation;		//!< Code de la réservation annulée en cas de modification
				int								_seats;			//!< Nombre de places
				time::DateTime					_bookingTime;		//!< Date de la réservation
				time::DateTime					_cancellationTime;		//!< Date de l'annulation (unknown = not cancelled)
			//@}

			//!	\name Personnes
			//@{
				uid					_customerUserId;
				std::string			_customerName;
				std::string			_customerPhone;
				std::string			_customerEMail;
				uid					_bookingUserId;
				uid					_cancelUserId;
			//@}

			//! \name Journey
			//@{
				uid					_originPlaceId;
				std::string			_originPlaceText;
				uid					_destinationPlaceId;
				std::string			_destinationPlaceText;
				time::DateTime		_originDateTime;
				time::DateTime		_destinationDateTime;
				std::string			_htmlJourneyBoard;
			//@}

		public:
			ReservationTransaction();

			void setLastReservation(uid id);
			void setSeats(int seats);
			void setBookingTime(const time::DateTime& time);
			void setCancellationTime(const time::DateTime& time);
			void setCustomerUserId(uid id);
			void setCustomerName(const std::string& name);
			void setCustomerPhone(const std::string& phone);
			void setBookingUserId(uid id);
			void setCancelUserId(uid id);
			void setCustomerEMail(const std::string& email);

			uid						getLastReservation() const;
			int						getSeats() const;
			const time::DateTime&	getBookingTime() const;
			const time::DateTime&	getCancellationTime() const;
			uid						getCustomerUserId() const;
			const std::string&		getCustomerName() const;
			const std::string&		getCustomerPhone() const;
			uid						getBookingUserId() const;
			uid						getCancelUserId() const;
			const std::string&		getCustomerEMail() const;
		};
	}
}

#endif // SYNTHESE_resa_ReservationTransaction_h__
