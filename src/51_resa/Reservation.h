
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

#include "Registrable.h"

#include "ResaTypes.h"
#include "Registry.h"

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

	namespace vehicle
	{
		class Vehicle;
		class VehiclePosition;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		class ReservationTransaction;

		/** Reservation class.
			@ingroup m51

			A Reservation object can link to its departures and arrival places, commercial lines, etc.
			In order to prevent from broken links, the object contains a copy of the main informations.
		*/
		class Reservation:
			public virtual util::Registrable
		{
		public:
			static const std::string DATA_NAME;
			static const std::string DATA_PHONE;
			static const std::string DATA_LANGUAGE;
            static const std::string DATA_DEPARTURE_CITY_NAME;
            static const std::string DATA_DEPARTURE_PLACE_NAME_NO_CITY;
			static const std::string DATA_DEPARTURE_PLACE_NAME;
            static const std::string DATA_ARRIVAL_CITY_NAME;
            static const std::string DATA_ARRIVAL_PLACE_NAME_NO_CITY;
			static const std::string DATA_ARRIVAL_PLACE_NAME;
			static const std::string DATA_DEPARTURE_PLACE_ID;
			static const std::string DATA_ARRIVAL_PLACE_ID;
			static const std::string DATA_RANK;
			static const std::string DATA_TRANSACTION_ID;
			static const std::string DATA_SEATS_NUMBER;
			static const std::string DATA_VEHICLE_ID;
			static const std::string DATA_RESERVATION_ID;
			static const std::string DATA_SEAT;
			static const std::string DATA_SERVICE_NUMBER;
			static const std::string DATA_SERVICE_ID;
			static const std::string DATA_DEPARTURE_TIME;
			static const std::string DATA_ARRIVAL_TIME;
			static const std::string DATA_CANCELLATION_TIME;
			static const std::string DATA_COMMENT;
			static const std::string DATA_ACKNOWLEDGE_TIME;
			static const std::string DATA_ACKNOWLEDGE_USER;
			static const std::string DATA_BOOKING_USER_ID;
			static const std::string DATA_CANCELLATION_ACKNOWLEDGE_TIME;
			static const std::string DATA_CANCELLATION_ACKNOWLEDGE_USER;
			static const std::string DATA_STATUS;
			static const std::string DATA_FULL_TEXT;
			static const std::string DATA_RESERVATION_DEAD_LINE;
			static const std::string DATA_MINUTES_TO_DEAD_LINE;
			static const std::string DATA_MINUTES_TO_DEPARTURE;
			static const std::string DATA_MINUTES_DEAD_LINE_TO_DEPARTURE;

			/// Chosen registry class.
			typedef util::Registry<Reservation>	Registry;

			Reservation(util::RegistryKeyType key = 0);


		private:

			ReservationTransaction*		_transaction;

			//!	\name Service réservé
			//@{
				util::RegistryKeyType _lineId;				//!< ID of the booked CommercialLine
				std::string						_lineCode;				//!< Comprehensive line code
                bool    						_isReservationPossible;
				util::RegistryKeyType			_serviceId;				//!< Code du service au sein l'axe
				std::string						_serviceCode;			//!< Code du service au sein l'axe
				util::RegistryKeyType			_departurePlaceId;		//!< ID of the departure Place
				std::string						_departureCityName;
				std::string						_departurePlaceNameNoCity;
				std::string						_departurePlaceName;	// This concatenates place and city
				util::RegistryKeyType			_arrivalPlaceId;		//!< ID of the arrival Place
				std::string						_arrivalCityName;
				std::string						_arrivalPlaceNameNoCity;
				std::string						_arrivalPlaceName;		// This concatenates place and city
				util::RegistryKeyType			_reservationRuleId;		//!< ID of the used OnlineReservationRule
				std::string						_departureAddress;		//!< Adresse du départ
				std::string						_arrivalAddress;		//!< Adresse d'arrivée
				boost::posix_time::ptime		_departureTime;			//!< Moment de montée prévu
				boost::posix_time::ptime		_arrivalTime;			//!< Moment d'arrivée prévue
				boost::posix_time::ptime		_originDateTime;
				boost::posix_time::ptime		_reservationDeadLine;
				const vehicle::Vehicle*	_vehicle;
				std::string						_seatNumber;
				const vehicle::VehiclePosition*	_vehiclePositionAtDeparture;
				const vehicle::VehiclePosition*	_vehiclePositionAtArrival;
				bool							_cancelledByOperator;
				boost::posix_time::ptime		_acknowledgeTime;
				security::User*					_acknowledgeUser;
				boost::posix_time::ptime		_cancellationAcknowledgeTime;
				security::User*					_cancellationAcknowledgeUser;
			//@}

		public:
			//!	\name Setters
			//@{
				void setLineId				(util::RegistryKeyType id){ _lineId = id; }
				void setLineCode			(const std::string& code){ _lineCode = code; }
                void setReservationPossible	(bool isReservationPossible){ _isReservationPossible = isReservationPossible; }
				void setServiceId			(util::RegistryKeyType id){ _serviceId = id; }
				void setServiceCode			(const std::string& code){ _serviceCode = code; }
				void setDeparturePlaceId	(util::RegistryKeyType id){ _departurePlaceId = id; }
				void setDepartureCityName	(const std::string& cityName){ _departureCityName = cityName; }
				void setDeparturePlaceNameNoCity	(const std::string& name){ _departurePlaceNameNoCity = name; }
				void setDeparturePlaceName	(const std::string& name){ _departurePlaceName = name; }
				void setArrivalPlaceId		(util::RegistryKeyType id){ _arrivalPlaceId = id; }
				void setArrivalCityName		(const std::string& cityName){ _arrivalCityName = cityName; }
				void setArrivalPlaceNameNoCity	(const std::string& name){ _arrivalPlaceNameNoCity = name; }
				void setArrivalPlaceName	(const std::string& name){ _arrivalPlaceName = name; }
				void setReservationRuleId	(util::RegistryKeyType id){ _reservationRuleId = id; }
				void setDepartureAddress	(const std::string& address){ _departureAddress = address; }
				void setArrivalAddress		(const std::string& address){ _arrivalAddress = address; }
				void setDepartureTime		(const boost::posix_time::ptime& time){ _departureTime = time; }
				void setArrivalTime			(const boost::posix_time::ptime& time){ _arrivalTime = time; }
				void setVehicle				(const vehicle::Vehicle* value){ _vehicle = value; }
				void setSeatNumber			(const std::string& value){ _seatNumber = value; }
				void setOriginDateTime		(const boost::posix_time::ptime& time){ _originDateTime = time; }
				void setReservationDeadLine	(const boost::posix_time::ptime& time){ _reservationDeadLine = time; }
				void setVehiclePositionAtDeparture	(const vehicle::VehiclePosition* value){ _vehiclePositionAtDeparture = value; }
				void setVehiclePositionAtArrival	(const vehicle::VehiclePosition* value){ _vehiclePositionAtArrival = value; }
				void setCancelledByOperator(bool value){ _cancelledByOperator = value; }
				void setAcknowledgeTime(const boost::posix_time::ptime& value){ _acknowledgeTime = value; }
				void setAcknowledgeUser(security::User* value){ _acknowledgeUser = value; }
				void setCancellationAcknowledgeTime(const boost::posix_time::ptime& value){ _cancellationAcknowledgeTime = value; }
				void setCancellationAcknowledgeUser(security::User* value){ _cancellationAcknowledgeUser = value; }

				/** Transaction setter.
					@param transaction the transaction which the reservation belongs
					@author Hugues Romain
					@date 2009
					The reservation is also added to the transaction.
					@pre the reservation must be fully defined
				*/
				void setTransaction(ReservationTransaction* transaction);
			//@}

			//!	\name Getters
			//@{
				util::RegistryKeyType			getLineId()					const { return _lineId; }
				const std::string&				getLineCode()				const { return _lineCode; }
                bool            				getReservationPossible()	const { return _isReservationPossible; }
				util::RegistryKeyType			getServiceId()				const { return _serviceId; }
				const std::string&				getServiceCode()			const { return _serviceCode; }
				util::RegistryKeyType			getDeparturePlaceId()		const { return _departurePlaceId; }
				const std::string&				getDepartureCityName()		const { return _departureCityName; }
				const std::string&				getDeparturePlaceNameNoCity()		const { return _departurePlaceNameNoCity; }
				const std::string&				getDeparturePlaceName()		const { return _departurePlaceName; }
				util::RegistryKeyType			getArrivalPlaceId()			const { return _arrivalPlaceId; }
				const std::string&				getArrivalCityName()		const { return _arrivalCityName; }
				const std::string&				getArrivalPlaceNameNoCity()		const { return _arrivalPlaceNameNoCity; }
				const std::string&				getArrivalPlaceName()		const { return _arrivalPlaceName; }
				util::RegistryKeyType			getReservationRuleId()		const { return _reservationRuleId; }
				const std::string&				getDepartureAddress()		const { return _departureAddress; }
				const std::string&				getArrivalAddress()			const { return _arrivalAddress; }
				const boost::posix_time::ptime&	getDepartureTime()			const { return _departureTime; }
				const boost::posix_time::ptime&	getArrivalTime()			const { return _arrivalTime; }
				ReservationTransaction*	getTransaction()			const { return _transaction; }
				const boost::posix_time::ptime&	getOriginDateTime()			const { return _originDateTime; }
				const boost::posix_time::ptime&	getReservationDeadLine()	const { return _reservationDeadLine; }
				const vehicle::Vehicle*	getVehicle()				const { return _vehicle; }
				const std::string&				getSeatNumber()				const { return _seatNumber; }
				const vehicle::VehiclePosition*	getVehiclePositionAtDeparture()	const { return _vehiclePositionAtDeparture; }
				const vehicle::VehiclePosition*	getVehiclePositionAtArrival()	const { return _vehiclePositionAtArrival; }
				bool							getCancelledByOperator()	const { return _cancelledByOperator; }
				const boost::posix_time::ptime& getAcknowledgeTime() const { return _acknowledgeTime; }
				security::User* getAcknowledgeUser() const { return _acknowledgeUser; }
				const boost::posix_time::ptime& getCancellationAcknowledgeTime() const { return _cancellationAcknowledgeTime; }
				security::User* getCancellationAcknowledgeUser() const { return _cancellationAcknowledgeUser; }
			//@}



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
		};
}	}

#endif // SYNTHESE_Reservation_h__
