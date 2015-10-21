
//////////////////////////////////////////////////////////////////////////
/// PTUseRuleUpdateAction class header.
///	@file PTUseRuleUpdateAction.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_PTUseRuleUpdateAction_H__
#define SYNTHESE_PTUseRuleUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "PTUseRule.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Transport condition update.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Use_rule_update
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		class PTUseRuleUpdateAction:
			public util::FactorableTemplate<server::Action, PTUseRuleUpdateAction>
		{
		public:
			static const std::string PARAMETER_RULE_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_CAPACITY;
			static const std::string PARAMETER_ORIGIN_IS_REFERENCE;
			static const std::string PARAMETER_FARE_ID;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_MAX_DELAY_DAYS;
			static const std::string PARAMETER_MIN_DELAY_DAYS;
			static const std::string PARAMETER_MIN_DELAY_MINUTES;
			static const std::string PARAMETER_MIN_DELAY_MINUTES_EXTERNAL;
			static const std::string PARAMETER_HOUR_DEADLINE;
			static const std::string PARAMETER_RESERVATION_MIN_DEPARTURE_TIME;
			static const std::string PARAMETER_RESERVATION_FORBIDDEN_DAYS;
			static const std::string PARAMETER_FORBIDDEN_IN_TIMETABLES;
			static const std::string PARAMETER_FORBIDDEN_IN_DEPARTURE_BOARDS;
			static const std::string PARAMETER_FORBIDDEN_IN_JOURNEY_PLANNER;

		private:
			boost::shared_ptr<PTUseRule> _rule;
			boost::shared_ptr<const PTUseRule> _template;
			boost::optional<std::string> _name;
			boost::optional<graph::UseRule::AccessCapacity> _capacity;
			boost::optional<bool> _originIsReference;
			boost::optional<boost::shared_ptr<fare::Fare> > _fare;
			boost::optional<pt::ReservationRuleType> _type;
			boost::optional<boost::posix_time::time_duration> _minDelayMinutes;
			boost::optional<boost::posix_time::time_duration> _minDelayMinutesExternal;
			boost::optional<boost::gregorian::date_duration> _minDelayDays;
			boost::optional<boost::optional<boost::gregorian::date_duration> > _maxDelayDays;
			boost::optional<boost::posix_time::time_duration> _hourDeadLine;
			boost::optional<boost::posix_time::time_duration> _reservationMinDepartureTime;
			boost::optional<PTUseRule::ReservationForbiddenDays> _reservationForbiddenDays;
			boost::optional<bool> _forbiddenInTimetables;
			boost::optional<bool> _forbiddenInDepartureBoards;
			boost::optional<bool> _forbiddenInJourneyPlanner;

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
				void setRule(boost::shared_ptr<PTUseRule> value) { _rule = value; }
			//@}
		};
}	}

#endif // SYNTHESE_PTUseRuleUpdateAction_H__
