
//////////////////////////////////////////////////////////////////////////
/// BaseCalendarUpdateAction class header.
///	@file BaseCalendarUpdateAction.hpp
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

#ifndef SYNTHESE_BaseCalendarUpdateAction_H__
#define SYNTHESE_BaseCalendarUpdateAction_H__

#include "UtilTypes.h"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace server
	{
		class Request;
	}

	namespace calendar
	{
		class Calendar;
		class CalendarTemplate;

		//////////////////////////////////////////////////////////////////////////
		/// Base class for calendar update actions.
		/// To use it in an action class :
		///		- include this file in the action header
		///			@code #include "BaseCalendarUpdateAction.hpp" @endcode
		///		- inherit public from this class
		///			@code public calendar::BaseCalendarUpdateAction @endcode
		///		- in getParametersMap paste somewhere :
		///			@code _getCalendarUpdateParametersMap(map); @endcode
		///		- in _setFromParametersMap paste somewhere :
		///			@code _setCalendarUpdateFromParametersMap(*_env, map); @endcode
		///		- in run paste somewhere before the Save call (replace _object by the real attribute) :
		///			@code _doCalendarUpdate(*_object, request); @endcode
		///		- be careful not to use same parameters name as defined in BaseCalendarUpdateAction
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m31
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.3.0
		class BaseCalendarUpdateAction
		{
		public:
			static const std::string PARAMETER_CALENDAR_TEMPLATE_ID;
			static const std::string PARAMETER_CALENDAR_TEMPLATE_ID2;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_PERIOD;
			static const std::string PARAMETER_ADD;
			static const std::string PARAMETER_ADD_LINK;
			static const std::string PARAMETER_LINK_TO_REMOVE;
			static const std::string PARAMETER_ADD_DATE_TO_FORCE;
			static const std::string PARAMETER_REMOVE_DATE_TO_FORCE;
			static const std::string PARAMETER_ADD_DATE_TO_BYPASS;
			static const std::string PARAMETER_REMOVE_DATE_TO_BYPASS;

			static const std::string SESSION_VARIABLE_SERVICE_ADMIN_START_DATE;
			static const std::string SESSION_VARIABLE_SERVICE_ADMIN_END_DATE;
			static const std::string SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID;
			static const std::string SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2;

		private:
			boost::shared_ptr<CalendarTemplate> _calendarTemplate;
			boost::shared_ptr<CalendarTemplate> _calendarTemplate2;
			boost::gregorian::date _startDate;
			boost::gregorian::date _endDate;
			boost::gregorian::date_duration _period;
			bool _add;
			bool _addLink;
			boost::optional<util::RegistryKeyType> _linkToRemove;
			boost::gregorian::date _dateToForceToAdd;
			boost::gregorian::date _dateToForceToRemove;
			boost::gregorian::date _dateToBypassToAdd;
			boost::gregorian::date _dateToBypassToRemove;

		protected:
			BaseCalendarUpdateAction();



			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @map the map to populate
			void _getCalendarUpdateParametersMap(
				util::ParametersMap& map
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param env the environment to populate when loading objects
			/// @param map Parameters map to read
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setCalendarUpdateFromParametersMap(
				util::Env& env,
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param object the object to update
			/// @param request the request which has launched the action
			void _doCalendarUpdate(
				Calendar& object,
				server::Request& request
			) const;

			/// @name Services
			//@{
				bool _calendarUpdateToDo() const;
			//@}


		public:
			/// @name Setters
			//@{
				void setAdd(bool value){ _add = value; }
				void setAddLink(bool value){ _addLink = value; }
				void setLinkToRemove(boost::optional<util::RegistryKeyType> value){ _linkToRemove = value; }
				void setDateToForceToAdd(const boost::gregorian::date& value){ _dateToForceToAdd = value; }
				void setDateToForceToRemove(const boost::gregorian::date& value){ _dateToForceToRemove = value; }
				void setDateToBypassToAdd(const boost::gregorian::date& value){ _dateToBypassToAdd = value; }
				void setDateToBypassToRemove(const boost::gregorian::date& value){ _dateToBypassToRemove = value; }
			//@}

			/// @name Modifiers
			//@{
				void setDate(boost::gregorian::date value);
			//@}
		};
}	}

#endif // SYNTHESE_BaseCalendarUpdateAction_H__
