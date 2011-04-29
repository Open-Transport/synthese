
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

#include "ResaTypes.h"

#include "Registrable.h"
#include "Registry.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

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

			/// Chosen registry class.
			typedef util::Registry<ReservationTransaction>	Registry;

			typedef std::vector<Reservation*> Reservations;

		private:

			//!	\name Caractéristiques de la réservation
			//@{
				Reservations		_reservations;
				util::RegistryKeyType		_lastReservation;		//!< Code de la réservation annulée en cas de modification
				int					_seats;			//!< Nombre de places
				boost::posix_time::ptime		_bookingTime;		//!< Date de la réservation
				boost::posix_time::ptime	_cancellationTime;		//!< Date de l'annulation (unknown = not cancelled)
			//@}

			//!	\name Personnes
			//@{
				util::RegistryKeyType		_customerUserId;
				std::string			_customerName;
				std::string			_customerPhone;
				std::string			_customerEMail;
				util::RegistryKeyType		_bookingUserId;
				util::RegistryKeyType	_cancelUserId;
			//@}

			//! \name Journey
			//@{
				util::RegistryKeyType	_originPlaceId;
				std::string			_originPlaceText;
				util::RegistryKeyType	_destinationPlaceId;
				std::string			_destinationPlaceText;
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::ptime		_destinationDateTime;
				std::string			_htmlJourneyBoard;
			//@}

		public:
			ReservationTransaction(util::RegistryKeyType key = 0);

			void setLastReservation	(util::RegistryKeyType id) { _lastReservation = id; }
			void setSeats			(int seats);
			void setBookingTime		(const boost::posix_time::ptime& time);
			void setCancellationTime(const boost::posix_time::ptime& time);
			void setCustomerUserId	(util::RegistryKeyType id) { _customerUserId = id; }
			void setCustomerName	(const std::string& name);
			void setCustomerPhone	(const std::string& phone);
			void setBookingUserId	(util::RegistryKeyType id) { _bookingUserId = id; }
			void setCancelUserId	(util::RegistryKeyType id) { _cancelUserId = id; }
			void setCustomerEMail	(const std::string& email);

			util::RegistryKeyType					getLastReservation()	const { return _lastReservation; }
			int						getSeats()				const;
			const boost::posix_time::ptime&	getBookingTime()		const;
			const boost::posix_time::ptime&	getCancellationTime()	const;
			util::RegistryKeyType	getCustomerUserId()		const { return _customerUserId; }
			const std::string&		getCustomerName()		const;
			const std::string&		getCustomerPhone()		const;
			util::RegistryKeyType	getBookingUserId()		const { return _bookingUserId; }
			util::RegistryKeyType	getCancelUserId()		const { return _cancelUserId; }
			const std::string&		getCustomerEMail()		const;
			const Reservations&		getReservations()		const;

			/** Creation of a child reservation.
				@return boost::shared_ptr<Reservation> The created reservation
				@author Hugues Romain
				@date 2007
			*/
			Reservation* newReservation();


			void addReservation(Reservation* resa);

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
					@return the deadline
					@author Hugues Romain
					@date 2008

					The dead line of the reservation transaction is the time when the cancellation is not allowed anymore.
					This is the first reservation dead line in chronological order.
				*/
				boost::posix_time::ptime getReservationDeadLine() const;
			//@}
		};
	}
}

#endif // SYNTHESE_resa_ReservationTransaction_h__
