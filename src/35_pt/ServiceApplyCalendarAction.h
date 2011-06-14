
//////////////////////////////////////////////////////////////////////////
/// ServiceApplyCalendarAction class header.
///	@file ServiceApplyCalendarAction.h
///	@author Hugues
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_ServiceApplyCalendarAction_H__
#define SYNTHESE_ServiceApplyCalendarAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/date_time/gregorian_calendar.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class SchedulesBasedService;
		class JourneyPattern;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Update of the dates of a service.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : ServiceApplyCalendarAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the service to update</li>
		///		<li>actionParamct (optional) : id of the calendar template to use</li>
		///		<li>actionParamsd : start date</li>
		///		<li>actionParamed : end date</li>
		///		<li>actionParampe : period in days</li>
		///		<li>actionParamad : 1 if the days must be added, 0 if they must be removed</li>
		///	</ul>
		class ServiceApplyCalendarAction:
			public util::FactorableTemplate<server::Action, ServiceApplyCalendarAction>
		{
		public:
			static const std::string PARAMETER_SERVICE_ID;
			static const std::string PARAMETER_CALENDAR_TEMPLATE_ID;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_PERIOD;
			static const std::string PARAMETER_ADD;

		private:
			boost::shared_ptr<const JourneyPattern> _journeyPattern;
			boost::shared_ptr<SchedulesBasedService> _service;
			boost::shared_ptr<const calendar::CalendarTemplate> _calendarTemplate;
			boost::gregorian::date _startDate;
			boost::gregorian::date _endDate;
			boost::gregorian::date_duration _period;
			bool _add;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			ServiceApplyCalendarAction();

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
				void setService(boost::shared_ptr<SchedulesBasedService> value) { _service = value; }
				void setJourneyPattern(boost::shared_ptr<const JourneyPattern> value) { _journeyPattern = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_ServiceApplyCalendarAction_H__
