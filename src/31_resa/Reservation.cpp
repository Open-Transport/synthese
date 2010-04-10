
/** Reservation class implementation.
	@file Reservation.cpp

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

#include "Reservation.h"
#include "ReservationTransaction.h"
#include "ResaModule.h"
#include "Registry.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<resa::Reservation>::KEY("Reservation");
	}

	namespace resa
	{
		Reservation::Reservation(
			RegistryKeyType key
		):	Registrable(key)
			, _departureTime(not_a_date_time)
			, _arrivalTime(not_a_date_time)
			, _originDateTime(not_a_date_time)
			, _reservationDeadLine(not_a_date_time)
			, _reservationRuleId(0)
		{

		}



		void Reservation::setLineCode( const std::string& code )
		{
			_lineCode = code;
		}

		const std::string& Reservation::getLineCode() const
		{
			return _lineCode;
		}

		void Reservation::setServiceCode( const std::string& code )
		{
			_serviceCode = code;
		}


		void Reservation::setDeparturePlaceName( const std::string& name )
		{
			_departurePlaceName = name;
		}

		
		void Reservation::setArrivalPlaceName( const std::string& name )
		{
			_arrivalPlaceName = name;
		}

		
		void Reservation::setArrivalAddress( const std::string& address )
		{
			_arrivalAddress = address;
		}

		void Reservation::setDepartureAddress( const std::string& address )
		{
			_departureAddress = address;
		}

		void Reservation::setDepartureTime( const ptime& time )
		{
			_departureTime = time;
		}

		void Reservation::setArrivalTime( const ptime& time )
		{
			_arrivalTime = time;
		}


		const std::string& Reservation::getServiceCode() const
		{
			return _serviceCode;
		}

		const std::string& Reservation::getDeparturePlaceName() const
		{
			return _departurePlaceName;
		}

		const std::string& Reservation::getArrivalPlaceName() const
		{
			return _arrivalPlaceName;
		}

		const std::string& Reservation::getDepartureAddress() const
		{
			return _departureAddress;
		}

		const std::string& Reservation::getArrivalAddress() const
		{
			return _arrivalAddress;
		}

		const ptime& Reservation::getDepartureTime() const
		{
			return _departureTime;
		}

		const ptime& Reservation::getArrivalTime() const
		{
			return _arrivalTime;
		}

		const ReservationTransaction* Reservation::getTransaction() const
		{
			return _transaction;
		}

		void Reservation::setTransaction(ReservationTransaction* transaction )
		{
			_transaction = transaction;
			transaction->addReservation(this);
		}

		const ptime& Reservation::getOriginDateTime() const
		{
			return _originDateTime;
		}

		void Reservation::setOriginDateTime( const ptime& time )
		{
			_originDateTime = time;
		}

		

		ReservationStatus Reservation::getStatus() const
		{
			if (_reservationRuleId == 0)
				return NO_RESERVATION;

			const ptime& cancellationTime(getTransaction()->getCancellationTime());
			const ptime now(second_clock::local_time());

			if (cancellationTime.is_not_a_date_time())
			{
				if (now < _reservationDeadLine)
					return OPTION;
				if (now < _departureTime)
					return TO_BE_DONE;
				if (now < _arrivalTime)
					return AT_WORK;
				return DONE;
			}
			else
			{
				if (cancellationTime < _reservationDeadLine)
					return CANCELLED;
				if (cancellationTime < _departureTime)
					return CANCELLED_AFTER_DELAY;
				return NO_SHOW;
			}
		}



		void Reservation::setReservationDeadLine( const ptime& time )
		{
			_reservationDeadLine = time;
		}



		const ptime& Reservation::getReservationDeadLine() const
		{
			return _reservationDeadLine;
		}



		std::string Reservation::getFullStatusText() const
		{
			ReservationStatus status(getStatus());
			string statusText(ResaModule::GetStatusText(status));
			
			switch(status)
			{
			case OPTION: return statusText + " pouvant être annulée avant le " + to_simple_string(_reservationDeadLine);
			case CANCELLED: return statusText + " le " + to_simple_string(getTransaction()->getCancellationTime());
			case CANCELLED_AFTER_DELAY: return statusText + " le " + to_simple_string(getTransaction()->getCancellationTime());
			case NO_SHOW: return statusText + " constatée le " + to_simple_string(getTransaction()->getCancellationTime());
			}

			return statusText;
		}
	}
}
