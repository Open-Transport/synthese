
/** BookReservationAction class header.
	@file BookReservationAction.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_BookReservationAction_H__
#define SYNTHESE_BookReservationAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include "AccessParameters.h"
#include "Journey.h"
#include "RoutePlannerFunction.h"

#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace pt_website
	{
		class PTServiceConfig;
		class RollingStockFilter;
	}

	namespace pt
	{
		class ScheduledService;
		class FreeDRTTimeSlot;
	}

	namespace geography
	{
		class Place;
		class NamedPlace;
	}

	namespace resa
	{
		//////////////////////////////////////////////////////////////////////////
		/// Reservation booking action.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservation_booking
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m51Actions refActions
		class BookReservationAction:
			public util::FactorableTemplate<server::Action, BookReservationAction>
		{
		public:

			// Customer information
			static const std::string PARAMETER_CREATE_CUSTOMER;
			static const std::string PARAMETER_CUSTOMER_ID;
			static const std::string PARAMETER_SEARCH_CUSTOMER_BY_EXACT_NAME;
			static const std::string PARAMETER_CUSTOMER_NAME;
			static const std::string PARAMETER_CUSTOMER_SURNAME;
			static const std::string PARAMETER_CUSTOMER_ALLOW_EMPTY_SURNAME;
			static const std::string PARAMETER_CUSTOMER_PHONE;
			static const std::string PARAMETER_CUSTOMER_EMAIL;
			static const std::string PARAMETER_CUSTOMER_ADDRESS;
			static const std::string PARAMETER_CUSTOMER_POSTCODE;
			static const std::string PARAMETER_CUSTOMER_CITYTEXT;
			static const std::string PARAMETER_CUSTOMER_COUNTRY;
			static const std::string PARAMETER_CUSTOMER_LANGUAGE;

			// Requester information
			static const std::string PARAMETER_PASSWORD;

			// Reservation information
			static const std::string PARAMETER_SEATS_NUMBER;
			static const std::string PARAMETER_COMMENT;

			// Reservation by service
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_DEPARTURE_RANK;
			static const std::string PARAMETER_ARRIVAL_RANK;

			static const std::string PARAMETER_IGNORE_RESERVATION_RULES;
			static const std::string PARAMETER_APPROACH_SPEED;

		private:
			void updatePlace();


			//! @name All reservation modes
			//@{
			boost::shared_ptr<security::User>	_customer;
			bool								_createCustomer;
			graph::AccessParameters				_accessParameters;
			std::size_t							_seatsNumber;
			bool								_ignoreReservation;
			double								_approachSpeed;
			std::string _comment;

			//! @name Reservation on a scheduled service
			//@{
				boost::shared_ptr<const pt::ScheduledService> _service;
			//@}

			//! @name Reservation on a free DRT service
			//@{
				boost::shared_ptr<const pt::FreeDRTTimeSlot> _freeDRTTimeSlot;
				boost::posix_time::ptime _departureDateTime;
				boost::posix_time::ptime _arrivalDateTime;
				size_t _userClassCode;
				boost::shared_ptr<const geography::Place> _departurePlace;
				boost::shared_ptr<const geography::Place> _arrivalPlace;
			//@}

			//! @name Reservation of a full journey
			//@{
				pt_journey_planner::RoutePlannerFunction _journeyPlanner;
				graph::Journey _journey;
				boost::shared_ptr<const pt_website::PTServiceConfig> _site;
				boost::shared_ptr<const pt_website::RollingStockFilter>	_rollingStockFilter;
			//@}


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservation_booking
			//////////////////////////////////////////////////////////////////////////
			/// @return Generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			///	Conversion from generic parameters map to attributes.
			///	Removes the used parameters from the map.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservation_booking
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			///	@exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			BookReservationAction();

			//////////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Reservation_booking
			//////////////////////////////////////////////////////////////////////////
			/// @param request the request which launched the action
			void run(server::Request& request);



			//! @name Setters
			//@{
				void setJourney(const graph::Journey& journey){ _journey = journey;	}
				void setAccessParameters(const graph::AccessParameters& value){ _accessParameters = value; }
				void setSite(boost::shared_ptr<const pt_website::PTServiceConfig> value){ _site = value; }
				void setCreateCustomer(bool value){ _createCustomer = value; }
				void setIgnoreReservationRules(bool value){ _ignoreReservation = value; }
				void setApproachSpeed(double value){ _approachSpeed = value; }
				void setFreeDRTTimeSlot(boost::shared_ptr<const pt::FreeDRTTimeSlot> value){ _freeDRTTimeSlot = value; }
				void setDeparturePlace(boost::shared_ptr<const geography::Place> value){ _departurePlace = value; }
				void setArrivalPlace(boost::shared_ptr<const geography::Place> value){ _arrivalPlace = value; }
			//@}

			/// @name Getters
			//@{
				const pt_journey_planner::RoutePlannerFunction& getJourneyPlanner() const { return _journeyPlanner; }
				pt_journey_planner::RoutePlannerFunction& getJourneyPlanner(){ return _journeyPlanner; }
			//@}



			virtual bool isAuthorized(const server::Session* session) const;



			static const geography::NamedPlace* GetPlaceFromOrigin(
				const graph::Journey& journey,
				boost::shared_ptr<geography::Place> originPlace
			);

			static const geography::NamedPlace* GetPlaceFromDestination(
				const graph::Journey& journey,
				boost::shared_ptr<geography::Place> destinationPlace
			);
		};
	}
}

#endif // SYNTHESE_BookReservationAction_H__
