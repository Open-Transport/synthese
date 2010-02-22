
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

#include "31_resa/Types.h"

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

			//!	\name Service r�serv�
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
				std::string						_departureAddress;		//!< Adresse du d�part
				std::string						_arrivalAddress;		//!< Adresse d'arriv�e
				boost::posix_time::ptime		_departureTime;			//!< Moment de mont�e pr�vu
				boost::posix_time::ptime		_arrivalTime;			//!< Moment d'arriv�e pr�vue
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::ptime		_reservationDeadLine;
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
				void setDepartureTime		(const boost::posix_time::ptime& time);
				void setArrivalTime			(const boost::posix_time::ptime& time);
		


				/** Transaction setter.
					@param transaction the transaction which the reservation belongs
					@author Hugues Romain
					@date 2009
					The reservation is also added to the transaction.
				*/
				void setTransaction			(ReservationTransaction* transaction);
				void setOriginDateTime		(const boost::posix_time::ptime& time);
				void setReservationDeadLine	(const boost::posix_time::ptime& time);
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
				const boost::posix_time::ptime&	getDepartureTime()			const;
				const boost::posix_time::ptime&	getArrivalTime()			const;
				const ReservationTransaction*	getTransaction()			const;
				const boost::posix_time::ptime&	getOriginDateTime()			const;
				const boost::posix_time::ptime&	getReservationDeadLine()	const;
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
