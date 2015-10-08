
/** ReservationTransaction class implementation.
	@file ReservationTransaction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ReservationTransaction.h"

#include "Profile.h"
#include "Registry.h"
#include "ResaRight.h"
#include "Reservation.h"
#include "ResaModule.h"
#include "Session.h"
#include "User.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace resa;
	using namespace util;

	CLASS_DEFINITION(ReservationTransaction, "t046_reservation_transactions", 46)
	FIELD_DEFINITION_OF_OBJECT(ReservationTransaction, "transaction_id", "transaction_ids")

	FIELD_DEFINITION_OF_TYPE(LastReservationId, "last_reservation_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Seats, "seats", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(BookingTime, "booking_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(CancellationTime, "cancellation_time", SQL_DATETIME)
	FIELD_DEFINITION_OF_TYPE(Customer, "customer_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CustomerName, "customer_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CustomerPhone, "customer_phone", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(CustomerEmail, "customer_email", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(BookingUserId, "booking_user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CancelUserId, "cancel_user_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Comment, "comment", SQL_TEXT)

	namespace resa
	{
		ReservationTransaction::ReservationTransaction(
			RegistryKeyType key
		):	Registrable(key),
			Object<ReservationTransaction, ReservationTransactionSchema> (
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_VALUE_CONSTRUCTOR(LastReservationId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(Seats),
					FIELD_VALUE_CONSTRUCTOR(BookingTime, second_clock::local_time()),
					FIELD_VALUE_CONSTRUCTOR(CancellationTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(Customer),
					FIELD_DEFAULT_CONSTRUCTOR(CustomerName),
					FIELD_DEFAULT_CONSTRUCTOR(CustomerPhone),
					FIELD_DEFAULT_CONSTRUCTOR(CustomerEmail),
					FIELD_VALUE_CONSTRUCTOR(BookingUserId, 0),
					FIELD_VALUE_CONSTRUCTOR(CancelUserId, 0),
					FIELD_DEFAULT_CONSTRUCTOR(Comment)
			)	),
			_originDateTime(not_a_date_time),
			_destinationDateTime(not_a_date_time)
		{}



		ReservationTransaction::~ReservationTransaction()
		{
			for(Reservations::const_iterator it(_reservations.begin()); it != _reservations.end(); ++it)
			{
				(*it)->set<Transaction>(boost::none);
			}
		}



		const ReservationTransaction::Reservations& ReservationTransaction::getReservations() const
		{
			return _reservations;
		}



		synthese::resa::ReservationStatus ReservationTransaction::getStatus() const
		{
			ReservationStatus status(NO_RESERVATION);
			for(Reservations::const_iterator it(_reservations.begin()); it != _reservations.end(); ++it)
			{
				ReservationStatus rs((*it)->getStatus());

				if (rs == NO_SHOW)
				{
					return NO_SHOW;
				}

				if (rs < status)
				{
					status = rs;
				}
			}
			return status;
		}



		std::string ReservationTransaction::getFullStatusText() const
		{
			ReservationStatus status(getStatus());
			string statusText(ResaModule::GetStatusText(status));

			switch(status)
			{
			case OPTION:
			case ACKNOWLEDGED_OPTION:
				return statusText + " pouvant être annulée avant le " + to_simple_string(getReservationDeadLine());
			
			case CANCELLED:
			case CANCELLATION_TO_ACK:
				return statusText + " le " + to_simple_string(get<CancellationTime>());

			case CANCELLED_AFTER_DELAY:
			case ACKNOWLEDGED_CANCELLED_AFTER_DELAY:
				return statusText + " le " + to_simple_string(get<CancellationTime>());

			case NO_SHOW:
				return statusText + " constatée le " + to_simple_string(get<CancellationTime>());
				
			default:
				break;
			}

			return statusText;
		}



		ptime ReservationTransaction::getReservationDeadLine() const
		{
			ptime result(not_a_date_time);
			for (Reservations::const_iterator it(_reservations.begin()); it != _reservations.end(); ++it)
			{
				if (!(*it)->get<ReservationDeadLine>().is_not_a_date_time() && (result.is_not_a_date_time() || (*it)->get<ReservationDeadLine>() < result))
					result = (*it)->get<ReservationDeadLine>();
			}
			return result;
		}



		void ReservationTransaction::addReservation(
			Reservation& resa
		){
			// Check of the precondition
			assert((resa.get<Transaction>()).get_ptr() == this);

			// Adds the reservation to the transaction
			_reservations.insert(&resa);
		}



		void ReservationTransaction::removeReservation( Reservation& resa )
		{
			_reservations.erase(&resa);
		}



		bool ReservationTransaction::ReservationComparator::operator()( Reservation* op1, Reservation* op2 )
		{
			// Same objects
			if(op1 == op2)
			{
				return false;
			}

			// NULL after all
			if(!op1)
			{
				assert(false); // This should not happen
				return false;
			}

			// all before NULL
			if(!op2)
			{
				assert(false); // This should not happen
				return true;
			}

			// Identical departure time objects : sort by address
			if(op1->get<DepartureTime>() == op2->get<DepartureTime>())
			{
				return op1 < op2;
			}

			// Undefined departure time after all
			if(op1->get<DepartureTime>().is_not_a_date_time())
			{
				assert(false); // This should not happen
				return false;
			}

			// All before undefined departure time
			if(op2->get<DepartureTime>().is_not_a_date_time())
			{
				assert(false); // This should not happen
				return true;
			}

			// Comparison on valid departure times
			return op1->get<DepartureTime>() < op2->get<DepartureTime>();
		}

		bool ReservationTransaction::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::WRITE);
		}

		bool ReservationTransaction::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::WRITE);
		}

		bool ReservationTransaction::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::DELETE_RIGHT);
		}
}	}
