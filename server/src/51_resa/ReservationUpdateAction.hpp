
//////////////////////////////////////////////////////////////////////////
/// ReservationUpdateAction class header.
///	@file ReservationUpdateAction.hpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ReservationUpdateAction_H__
#define SYNTHESE_ReservationUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "VehiclePosition.hpp"

namespace synthese
{
	namespace vehicle
	{
		class Vehicle;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		class Reservation;

		//////////////////////////////////////////////////////////////////////////
		/// 51.15 Action : ReservationUpdateAction.
		/// @ingroup m51Actions refActions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.2.1
		//////////////////////////////////////////////////////////////////////////
		/// Key : ReservationUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		///	</dl>
		class ReservationUpdateAction:
			public util::FactorableTemplate<server::Action, ReservationUpdateAction>
		{
		public:
			static const std::string PARAMETER_RESERVATION_ID;
			static const std::string PARAMETER_VEHICLE_ID;
			static const std::string PARAMETER_SEAT_NUMBER;
			static const std::string PARAMETER_CANCELLED_BY_OPERATOR;
			static const std::string PARAMETER_REAL_DEPARTURE_TIME;
			static const std::string PARAMETER_REAL_ARRIVAL_TIME;
			static const std::string PARAMETER_DEPARTURE_METER_OFFSET;
			static const std::string PARAMETER_ARRIVAL_METER_OFFSET;
			static const std::string PARAMETER_ACKNOWLEDGE_TIME;
			static const std::string PARAMETER_ACKNOWLEDGE_USER_ID;
			static const std::string PARAMETER_CANCELLATION_ACKNOWLEDGE_TIME;
			static const std::string PARAMETER_CANCELLATION_ACKNOWLEDGE_USER_ID;
			static const std::string PARAMETER_CANCEL_ACKNOWLEDGEMENT;

		private:
			boost::shared_ptr<Reservation> _reservation;
			boost::optional<boost::shared_ptr<vehicle::Vehicle> > _vehicle;
			boost::optional<std::string> _seatNumber;
			boost::optional<boost::posix_time::ptime> _realDepartureTime;
			boost::optional<boost::posix_time::ptime> _realArrivalTime;
			boost::optional<bool> _cancelledByOperator;
			boost::optional<vehicle::VehiclePosition::Meters> _departureMeterOffset;
			boost::optional<vehicle::VehiclePosition::Meters> _arrivalMeterOffset;
			boost::optional<boost::posix_time::ptime> _acknowledgeTime;
			boost::optional<boost::shared_ptr<security::User> > _acknowledgeUser;
			boost::optional<boost::posix_time::ptime> _cancellationAcknowledgeTime;
			boost::optional<boost::shared_ptr<security::User> > _cancellationAcknowledgeUser;
			boost::optional<bool> _cancelAcknowledgement;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void setReservation(boost::shared_ptr<Reservation> value) { _reservation = value; }
			//@}
		};
}	}

#endif // SYNTHESE_ReservationUpdateAction_H__
