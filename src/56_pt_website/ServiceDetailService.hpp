
//////////////////////////////////////////////////////////////////////////////////////////
///	ServiceDetailService class header.
///	@file ServiceDetailService.hpp
///	@author hromain
///	@date 2014
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

#ifndef SYNTHESE_ServiceDetailService_H__
#define SYNTHESE_ServiceDetailService_H__

#include "FactorableTemplate.h"
#include "Function.h"
#include "ReservationTableSync.h"

#include <boost/tuple/tuple.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace resa
	{
		class Reservation;
	}

	namespace pt
	{
		class ScheduledService;
		class StopArea;
	}
	
	namespace vehicle
	{
		class Descent;
	}

	namespace pt_website
	{
		//////////////////////////////////////////////////////////////////////////
		///	56.15 Function : ServiceDetailService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Service_detail
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m56Functions refFunctions
		///	@author hromain
		///	@date 2014
		/// @since 
		class ServiceDetailService:
			public util::FactorableTemplate<server::Function,ServiceDetailService>
		{
		public:
			static const std::string PARAMETER_BASE_CALENDAR_ID;
			static const std::string PARAMETER_READ_RESERVATIONS_FROM_DAY;
			static const std::string PARAMETER_READ_DESCENTS_FROM_DAY;

			static const std::string ATTR_DEPARTURE_SCHEDULE;
			static const std::string ATTR_DEPARTURE_PLACE_NAME;
			static const std::string ATTR_ARRIVAL_SCHEDULE;
			static const std::string ATTR_ARRIVAL_PLACE_NAME;
			static const std::string ATTR_IS_RESERVABLE;

			static const std::string TAG_CALENDAR;

			static const std::string TAG_STOP;
			static const std::string ATTR_CITY_ID;
			static const std::string ATTR_CITY_NAME;
			static const std::string ATTR_STOP_NAME;
			static const std::string ATTR_DEPARTURE_TIME;
			static const std::string ATTR_ARRIVAL_TIME;
			static const std::string ATTR_SCHEDULE_INPUT;
			static const std::string ATTR_WITH_RESERVATION;
			static const std::string ATTR_WITH_DESCENT;
			static const std::string ATTR_DESCENT_ID;
			static const std::string ATTR_FIRST_IN_AREA;
			static const std::string ATTR_LAST_IN_AREA;
			static const std::string ATTR_IS_AREA;
			static const std::string ATTR_STOP_POINT_ID;
			static const std::string ATTR_STOP_AREA_ID;
			static const std::string TAG_RESERVATION_AT_DEPARTURE;
			static const std::string TAG_RESERVATION_AT_ARRIVAL;
			static const std::string TAG_RESERVATION_WITH_ARRIVAL_BEFORE_DEPARTURE;
			
		protected:
			//! \name parameters
			//@{
				const pt::ScheduledService* _service;
				boost::shared_ptr<const calendar::CalendarTemplate> _baseCalendar;
				boost::gregorian::date _readReservationsFromDay;
				boost::gregorian::date _readDescentsFromDay;
			//@}

		
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Service_detail#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author hromain
			/// @date 2014
			/// @since 
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Service_detail#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author hromain
			/// @date 2014
			/// @since 
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			typedef boost::tuple<
				std::set<const resa::Reservation*>, // Departures
				std::set<const resa::Reservation*>, // Arrivals
				std::set<const resa::Reservation*> // At least one arrival is displayed before its departure
			> StopInstructions;
			typedef std::vector<
				std::pair<
					const resa::Reservation*,
					bool	// True if the departure has been already read
			>	> Resas;
			StopInstructions _hasToStop(
				const pt::StopArea& stopArea,
				size_t rank,
				Resas& resas
			) const;
			
			typedef std::vector<
				std::pair<
					const vehicle::Descent*,
					bool	// True if the departure has been already read
			>	> Descents;

			static void _exportReservations(
				util::ParametersMap& pm,
				const StopInstructions resas
			);

			void _exportStop(
				util::ParametersMap& pm,
				const pt::StopArea& stopArea,
				bool isDeparture,
				bool isArrival,
				size_t rank,
				bool scheduleInput,
				bool withReservation,
				Resas& resas,
				bool isArea,
				bool firstInArea,
				bool lastInArea,
				Descents& descents,
				util::RegistryKeyType stopPointId,
				util::RegistryKeyType stopAreaId
			) const;
			
		public:
			ServiceDetailService();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author hromain
			/// @date 2014
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author hromain
			/// @date 2014
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author hromain
			/// @date 2014
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_ServiceDetailService_H__

