
/** Reservation class header.
	@file 51_resa/Reservation.h

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

#ifndef SYNTHESE_Reservation_h__
#define SYNTHESE_Reservation_h__

#include "Object.hpp"

#include "ResaTypes.h"
#include "PtimeField.hpp"
#include "StringField.hpp"
#include "User.h"
#include "Vehicle.hpp"
#include "VehiclePosition.hpp"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/optional.hpp>
#include <string>

namespace synthese
{
	class Language;

	namespace util
	{
		class ParametersMap;
	}

	namespace resa
	{
		class ReservationTransaction;

		FIELD_POINTER(Transaction, ReservationTransaction)
		FIELD_ID(LineId)
		FIELD_STRING(LineCode)
		FIELD_BOOL(IsReservationPossible)
		FIELD_ID(ServiceId)
		FIELD_STRING(ServiceCode)
		FIELD_ID(DeparturePlaceId)
		FIELD_STRING(DepartureCityName)
		FIELD_STRING(DeparturePlaceNameNoCity)
		FIELD_STRING(DeparturePlaceName)
		FIELD_PTIME(DepartureTime)
		FIELD_ID(ArrivalPlaceId)
		FIELD_STRING(ArrivalCityName)
		FIELD_STRING(ArrivalPlaceNameNoCity)
		FIELD_STRING(ArrivalPlaceName)
		FIELD_PTIME(ArrivalTime)
		FIELD_ID(ReservationRuleId)
		FIELD_PTIME(OriginDateTime)
		FIELD_PTIME(ReservationDeadLine)
		FIELD_POINTER(Vehicle, vehicle::Vehicle)
		FIELD_STRING(SeatNumber)
		FIELD_POINTER(VehiclePositionAtDeparture, vehicle::VehiclePosition)
		FIELD_POINTER(VehiclePositionAtArrival, vehicle::VehiclePosition)
		FIELD_BOOL(CancelledByOperator)
		FIELD_PTIME(AcknowledgeTime)
		FIELD_POINTER(AcknowledgeUser, security::User)
		FIELD_PTIME(CancellationAcknowledgeTime)
		FIELD_POINTER(CancellationAcknowledgeUser, security::User)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Transaction),
			FIELD(LineId),
			FIELD(LineCode),
			FIELD(IsReservationPossible),
			FIELD(ServiceId),
			FIELD(ServiceCode),
			FIELD(DeparturePlaceId),
			FIELD(DepartureCityName),
			FIELD(DeparturePlaceNameNoCity),
			FIELD(DeparturePlaceName),
			FIELD(DepartureTime),
			FIELD(ArrivalPlaceId),
			FIELD(ArrivalCityName),
			FIELD(ArrivalPlaceNameNoCity),
			FIELD(ArrivalPlaceName),
			FIELD(ArrivalTime),
			FIELD(ReservationRuleId),
			FIELD(OriginDateTime),
			FIELD(ReservationDeadLine),
			FIELD(Vehicle),
			FIELD(SeatNumber),
			FIELD(VehiclePositionAtDeparture),
			FIELD(VehiclePositionAtArrival),
			FIELD(CancelledByOperator),
			FIELD(AcknowledgeTime),
			FIELD(AcknowledgeUser),
			FIELD(CancellationAcknowledgeTime),
			FIELD(CancellationAcknowledgeUser)
		> ReservationSchema;

		/** Reservation class.
			@ingroup m51

			A Reservation object can link to its departures and arrival places, commercial lines, etc.
			In order to prevent from broken links, the object contains a copy of the main informations.
		*/
		class Reservation:
			public virtual Object<Reservation, ReservationSchema>
		{
		public:
			static const std::string DATA_CLIENT_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_PHONE;
			static const std::string DATA_EMAIL;
			static const std::string DATA_ADDRESS;
			static const std::string DATA_POSTCODE;
			static const std::string DATA_CITYTEXT;
			static const std::string DATA_COUNTRY;
			static const std::string DATA_LANGUAGE;
			static const std::string DATA_RANK;
			static const std::string DATA_SEATS_NUMBER;
			static const std::string DATA_RESERVATION_ID;
			static const std::string DATA_SEAT;
			static const std::string DATA_SERVICE_NUMBER;
			static const std::string DATA_CANCELLATION_TIME;
			static const std::string DATA_COMMENT;
			static const std::string DATA_ACKNOWLEDGE_USER;
			static const std::string DATA_BOOKING_USER_ID;
			static const std::string DATA_CANCELLATION_ACKNOWLEDGE_USER;
			static const std::string DATA_STATUS;
			static const std::string DATA_FULL_TEXT;
			static const std::string DATA_MINUTES_TO_DEAD_LINE;
			static const std::string DATA_MINUTES_TO_DEPARTURE;
			static const std::string DATA_MINUTES_DEAD_LINE_TO_DEPARTURE;

			Reservation(util::RegistryKeyType key = 0);
			~Reservation();

			/** Transaction setter.
				@param transaction the transaction which the reservation belongs
				@author Hugues Romain
				@date 2009
				The reservation is also added to the transaction.
				@pre the reservation must be fully defined
			*/
			void setTransaction(ReservationTransaction* transaction);



			//! @name Queries
			//@{
				ReservationStatus	getStatus()			const;
				std::string			getFullStatusText()	const;

				//////////////////////////////////////////////////////////////////////////
				/// Reservation exporter
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservations_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				void toParametersMap(
					util::ParametersMap& pm,
					boost::optional<Language> language,
					std::string prefix = std::string()
				) const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_Reservation_h__
