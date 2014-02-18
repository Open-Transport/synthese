
//////////////////////////////////////////////////////////////////////////////////////////
///	ServicesListService class header.
///	@file ServicesListService.hpp
///	@author Hugues Romain
///	@date 2012
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

#ifndef SYNTHESE_ServicesListService_H__
#define SYNTHESE_ServicesListService_H__

#include "FactorableTemplate.h"
#include "FunctionWithSite.h"
#include "JourneyPattern.hpp"
#include "ReservationTableSync.h"

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
		class CommercialLine;
		class ScheduledService;
		class StopArea;

		//////////////////////////////////////////////////////////////////////////
		///	35.15 Function : ServicesListService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Services_List
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35Functions refFunctions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.4.0
		class ServicesListService:
			public util::FactorableTemplate<server::Function, ServicesListService>
		{
		public:
			static const std::string PARAMETER_WAYBACK;
			static const std::string PARAMETER_DISPLAY_DATE;
			static const std::string PARAMETER_BASE_CALENDAR_ID;
			static const std::string PARAMETER_MIN_DEPARTURE_TIME;
			static const std::string PARAMETER_MAX_DEPARTURE_TIME;
			static const std::string PARAMETER_DEPARTURE_PLACE;
			static const std::string PARAMETER_READ_RESERVATIONS_FROM_DAY;

			static const std::string DATA_ID;
			static const std::string DATA_DEPARTURE_SCHEDULE;
			static const std::string DATA_DEPARTURE_PLACE_NAME;
			static const std::string DATA_ARRIVAL_SCHEDULE;
			static const std::string DATA_ARRIVAL_PLACE_NAME;
			static const std::string DATA_RUNS_AT_DATE;
			static const std::string DATA_SERVICE;
			static const std::string ATTR_NUMBER;
			static const std::string ATTR_PATH_ID;
			static const std::string TAG_SERVICES;

			static const std::string TAG_CALENDAR;

			static const std::string TAG_STOP;
			static const std::string ATTR_CITY_ID;
			static const std::string ATTR_CITY_NAME;
			static const std::string ATTR_STOP_NAME;
			static const std::string ATTR_DEPARTURE_TIME;
			static const std::string ATTR_ARRIVAL_TIME;
			static const std::string ATTR_SCHEDULE_INPUT;
			static const std::string ATTR_WITH_RESERVATION;
			static const std::string ATTR_FIRST_IN_AREA;
			static const std::string ATTR_LAST_IN_AREA;
			static const std::string ATTR_IS_AREA;
			static const std::string TAG_RESERVATION_AT_DEPARTURE;
			static const std::string TAG_RESERVATION_AT_ARRIVAL;

		protected:
			//! \name Page parameters
			//@{
				boost::logic::tribool _wayBack;
				boost::shared_ptr<const ScheduledService> _service;
				boost::shared_ptr<const CommercialLine> _line;
				boost::gregorian::date _displayDate;
				boost::shared_ptr<const calendar::CalendarTemplate> _baseCalendar;
				boost::optional<boost::posix_time::time_duration> _minDepartureTime;
				boost::optional<boost::posix_time::time_duration> _maxDepartureTime;
				boost::optional<util::RegistryKeyType> _departurePlaceId;
				boost::gregorian::date _readReservationsFromDay;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Services_List#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Services_List#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);

			typedef std::pair<
				std::set<const resa::Reservation*>,
				std::set<const resa::Reservation*>
			> StopInstructions;
			typedef std::vector<const resa::Reservation*> Resas;
			StopInstructions _hasToStop(
				const StopArea& stopArea,
				size_t rank,
				const Resas& resas
			) const;

			static void _exportReservations(
				util::ParametersMap& pm,
				const StopInstructions resas
			);

		public:
			ServicesListService();


			//! @name Setters
			//@{
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2012
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2012
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2012
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_ServicesListService_H__

