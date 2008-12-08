
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

#include "31_resa/Types.h"

#include "DateTime.h"

#include "Registrable.h"

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
		class ReservationTransaction
		:	public virtual util::Registrable
		{
		public:
			typedef std::vector<boost::shared_ptr<Reservation> > Reservations;
			
		private:

			//!	\name Caractéristiques de la réservation
			//@{
				Reservations		_reservations;
				uid					_lastReservation;		//!< Code de la réservation annulée en cas de modification
				int					_seats;			//!< Nombre de places
				time::DateTime		_bookingTime;		//!< Date de la réservation
				time::DateTime		_cancellationTime;		//!< Date de l'annulation (unknown = not cancelled)
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
			ReservationTransaction(util::RegistryKeyType key = UNKNOWN_VALUE);

			void setLastReservation	(uid id);
			void setSeats			(int seats);
			void setBookingTime		(const time::DateTime& time);
			void setCancellationTime(const time::DateTime& time);
			void setCustomerUserId	(uid id);
			void setCustomerName	(const std::string& name);
			void setCustomerPhone	(const std::string& phone);
			void setBookingUserId	(uid id);
			void setCancelUserId	(uid id);
			void setCustomerEMail	(const std::string& email);

			uid						getLastReservation()	const;
			int						getSeats()				const;
			const time::DateTime&	getBookingTime()		const;
			const time::DateTime&	getCancellationTime()	const;
			uid						getCustomerUserId()		const;
			const std::string&		getCustomerName()		const;
			const std::string&		getCustomerPhone()		const;
			uid						getBookingUserId()		const;
			uid						getCancelUserId()		const;
			const std::string&		getCustomerEMail()		const;
			const Reservations&		getReservations()		const;

			/** Creation of a child reservation.
				@return boost::shared_ptr<Reservation> The created reservation
				@author Hugues Romain
				@date 2007				
			*/
			boost::shared_ptr<Reservation> newReservation();


			void addReservation(shared_ptr<Reservation> resa);

			//! @name Queries
			//@{				
				/** Reservation transaction status computing.
					@return ReservationStatus the status of the reservation transaction
					@author Hugues Romain
					@date 2008
					
					The reservation status of a reservation transaction is :
						- NO_SHOW if at least one reservation is at NO_SHOW status
						- else the minimum of the status of each reservation
				*/
				ReservationStatus getStatus() const;

				std::string	getFullStatusText()	const;

				
				
				/** Dead line of the reservation transaction.
					@return time::DateTime the dead line
					@author Hugues Romain
					@date 2008
					
					The dead line of the reservation transaction is the time when the cancellation is not allowed anymore.
					This is the first reservation dead line in chronological order.
				*/
				time::DateTime getReservationDeadLine() const;
			//@}
		};
	}
}

#endif // SYNTHESE_resa_ReservationTransaction_h__
