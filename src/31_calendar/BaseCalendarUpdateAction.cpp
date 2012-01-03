
/** BaseCalendarUpdateAction class implementation.
	@file BaseCalendarUpdateAction.cpp

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

#include "BaseCalendarUpdateAction.hpp"
#include "ActionException.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "DeleteQuery.hpp"
#include "CalendarLinkTableSync.hpp"
#include "CalendarLink.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace db;
	
	namespace calendar
	{
		const string BaseCalendarUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID = Action_PARAMETER_PREFIX + "bcu_calendar_id";
		const string BaseCalendarUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID2 = Action_PARAMETER_PREFIX + "bcu_calendar_id2";
		const string BaseCalendarUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "bcu_start_date";
		const string BaseCalendarUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "bcu_end_date";
		const string BaseCalendarUpdateAction::PARAMETER_PERIOD = Action_PARAMETER_PREFIX + "bcu_period";
		const string BaseCalendarUpdateAction::PARAMETER_ADD = Action_PARAMETER_PREFIX + "bcu_add";
		const string BaseCalendarUpdateAction::PARAMETER_ADD_LINK = Action_PARAMETER_PREFIX + "bcu_add_link";
		const string BaseCalendarUpdateAction::PARAMETER_LINK_TO_REMOVE = Action_PARAMETER_PREFIX + "bcu_link_to_remove";

		const string BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_START_DATE = "bcu_admin_start_date";
		const string BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_END_DATE = "bcu_admin_end_date";
		const string BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID = "bcu_admin_calendar_template_id";
		const string BaseCalendarUpdateAction::SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2 = "bcu_admin_calendar_template_id2";



		BaseCalendarUpdateAction::BaseCalendarUpdateAction():
			_period(days(1)),
			_add(true),
			_addLink(false)
		{}



		void BaseCalendarUpdateAction::_getCalendarUpdateParametersMap(
			ParametersMap& map
		) const	{
			// Add
			if(!_add)
			{
				map.insert(PARAMETER_ADD, _add);
			}

			// Add link
			if(_addLink)
			{
				map.insert(PARAMETER_ADD_LINK, _addLink);
			}

			// Link to remove
			if(_linkToRemove)
			{
				map.insert(PARAMETER_LINK_TO_REMOVE, *_linkToRemove);
			}

			// Period
			if(!_period.is_special())
			{
				map.insert(PARAMETER_PERIOD, static_cast<int>(_period.days()));
			}

			// Calendar Template
			if(_calendarTemplate.get())
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID, _calendarTemplate->getKey());
			}

			// Calendar Template 2
			if(_calendarTemplate2.get())
			{
				map.insert(PARAMETER_CALENDAR_TEMPLATE_ID2, _calendarTemplate2->getKey());
			}

			// Start date
			if(!_startDate.is_not_a_date())
			{
				map.insert(PARAMETER_START_DATE, _startDate);
			}

			// End date
			if(!_endDate.is_not_a_date() && _endDate != _startDate)
			{
				map.insert(PARAMETER_END_DATE, _endDate);
			}
		}



		void BaseCalendarUpdateAction::_setCalendarUpdateFromParametersMap(
			Env& env,
			const util::ParametersMap& map
		){
			// Calendar template
			RegistryKeyType cid(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_TEMPLATE_ID, 0));
			if(cid > 0) try
			{
				_calendarTemplate = CalendarTemplateTableSync::GetEditable(cid, env);
			}
			catch(ObjectNotFoundException<CalendarTemplate>&)
			{
				throw ActionException("No such calendar template");
			}

			// Calendar template
			RegistryKeyType cid2(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_TEMPLATE_ID2, 0));
			if(cid2 > 0) try
			{
				_calendarTemplate2 = CalendarTemplateTableSync::GetEditable(cid2, env);
			}
			catch(ObjectNotFoundException<CalendarTemplate>&)
			{
				throw ActionException("No such calendar template 2");
			}

			// Start and end dates
			string startDateStr(
				map.getDefault<string>(PARAMETER_START_DATE)
			);
			if(!startDateStr.empty())
			{
				_startDate = from_string(startDateStr);
				_endDate = from_string(map.getDefault<string>(PARAMETER_END_DATE, startDateStr));
				if(_startDate > _endDate)
				{
					throw ActionException("The start date must be after the end date");
				}

				// Period
				_period = days(map.getDefault<size_t>(PARAMETER_PERIOD, 1));
				if(_period.days() < 1)
				{
					throw ActionException("Period must be at least one day long");
				}
			}

			// Add or remove
			_add = map.getDefault<bool>(PARAMETER_ADD, _add);

			// Add a new link
			_addLink = map.getDefault<bool>(PARAMETER_ADD_LINK, _addLink);

			// Link to remove
			_linkToRemove = map.getOptional<RegistryKeyType>(PARAMETER_LINK_TO_REMOVE);
		}



		void BaseCalendarUpdateAction::_doCalendarUpdate(
			Calendar& object,
			server::Request& request
		) const {

			// Link to remove
			if(_linkToRemove)
			{
				DeleteQuery<CalendarLinkTableSync> deleteQuery;
				deleteQuery.addWhereField(TABLE_COL_ID, *_linkToRemove);
				deleteQuery.execute();
			}

			// Add date
			if(!object.isLinked() && !_addLink)
			{
				// Preparation of the mask
				Calendar mask;
				if(	!_startDate.is_not_a_date() && !_endDate.is_not_a_date())
				{
					for (date d = _startDate; d <= _endDate; d += _period)
					{
						mask.setActive(d);
					}
				}
				if(_calendarTemplate.get())
				{
					if(!mask.empty())
					{
						mask = _calendarTemplate->getResult(mask);
					}
					else if(_calendarTemplate->isLimited())
					{
						mask = _calendarTemplate->getResult();
					}
					else if(_calendarTemplate2.get() && _calendarTemplate2->isLimited())
					{
						mask = _calendarTemplate->getResult(_calendarTemplate2->getResult());
					}
				}
				if(_calendarTemplate2.get())
				{
					if(!mask.empty())
					{
						mask = _calendarTemplate2->getResult(mask);
					}
					else if(_calendarTemplate2->isLimited())
					{
						mask = _calendarTemplate2->getResult();
					}
				}

				// Applying the mask
				if(	!mask.empty())
				{
					if(_add)
					{
						object |= mask;
					}
					else
					{
						object.subDates(mask);
					}
				}
			}
			else if(_calendarUpdateToDo()) // Add link
			{
				CalendarLink link;
				link.setCalendar(&object);
				link.setStartDate(_startDate);
				link.setEndDate(_endDate);
				link.setCalendarTemplate(_calendarTemplate.get());
				link.setCalendarTemplate2(_calendarTemplate2.get());
				CalendarLinkTableSync::Save(&link);
			}


			// Storage of parameters in session variable
			if( !_startDate.is_not_a_date() &&
				!_endDate.is_not_a_date() &&
				_startDate != _endDate
			){
				if(!_startDate.is_not_a_date() && !_startDate.is_special())
				{
					request.getSession()->setSessionVariable(
						SESSION_VARIABLE_SERVICE_ADMIN_START_DATE,
						to_iso_extended_string(_startDate)
					);
				}
				if(!_endDate.is_not_a_date() && !_endDate.is_special())
				{
					request.getSession()->setSessionVariable(
						SESSION_VARIABLE_SERVICE_ADMIN_END_DATE,
						to_iso_extended_string(_endDate)
					);
				}
			}
			if(_calendarTemplate.get())
			{
				request.getSession()->setSessionVariable(
					SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID,
					lexical_cast<string>(_calendarTemplate->getKey())
				);
			}
			if(_calendarTemplate2.get())
			{
				request.getSession()->setSessionVariable(
					SESSION_VARIABLE_SERVICE_ADMIN_CALENDAR_TEMPLATE_ID2,
					lexical_cast<string>(_calendarTemplate2->getKey())
				);
			}
		}



		void BaseCalendarUpdateAction::setDate( boost::gregorian::date value )
		{
			_startDate = value;
			_endDate = value;
		}



		bool BaseCalendarUpdateAction::_calendarUpdateToDo() const
		{
			return
				(!_startDate.is_not_a_date() && !_endDate.is_not_a_date()) ||
				(_calendarTemplate.get() && _calendarTemplate->isLimited()) ||
				(_calendarTemplate2.get() && _calendarTemplate2->isLimited())
			;
		}
}	}
