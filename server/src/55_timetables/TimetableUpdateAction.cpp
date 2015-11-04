
/** TimetableUpdateAction class implementation.
	@file TimetableUpdateAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "TimetableUpdateAction.h"

#include "ActionException.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "StopArea.hpp"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableTableSync.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace calendar;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::TimetableUpdateAction>::FACTORY_KEY("TimetableUpdateAction");
	}

	namespace timetables
	{
		const string TimetableUpdateAction::PARAMETER_TIMETABLE_ID = Action_PARAMETER_PREFIX + "ti";
		const string TimetableUpdateAction::PARAMETER_BASE_CALENDAR_ID = Action_PARAMETER_PREFIX + "ci";
		const string TimetableUpdateAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "tt";
		const string TimetableUpdateAction::PARAMETER_FORMAT = Action_PARAMETER_PREFIX + "fo";
		const string TimetableUpdateAction::PARAMETER_CONTAINER_ID = Action_PARAMETER_PREFIX + "co";
		const string TimetableUpdateAction::PARAMETER_IGNORE_EMPTY_ROWS = Action_PARAMETER_PREFIX + "_ignore_empty_rows";
		const string TimetableUpdateAction::PARAMETER_FIELD_COMPRESSION = Action_PARAMETER_PREFIX + "_field_compression";



		TimetableUpdateAction::TimetableUpdateAction():
			FactorableTemplate<Action, TimetableUpdateAction>()
		{}



		ParametersMap TimetableUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_timetable.get()) map.insert(PARAMETER_TIMETABLE_ID, _timetable->getKey());
			if(_container.get()) map.insert(PARAMETER_CONTAINER_ID, _container->getKey());
			if(_calendarTemplate.get()) map.insert(PARAMETER_BASE_CALENDAR_ID, _calendarTemplate->getKey());
			map.insert(PARAMETER_TITLE, _title);
			return map;
		}



		void TimetableUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_timetable = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_TIMETABLE_ID), *_env);
			}
			catch (ObjectNotFoundException<Timetable>)
			{
				throw ActionException("No such timetable");
			}

			if(map.get<RegistryKeyType>(PARAMETER_CONTAINER_ID) > 0)
			{
				try
				{
					_container = TimetableTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CONTAINER_ID), *_env);
				}
				catch (ObjectNotFoundException<CalendarTemplate>)
				{
					throw ActionException("No such calendar");
				}
				if(_container->getContentType() != Timetable::CONTAINER)
				{
					throw ActionException("The specified id does not point to a container");
				}
			}

			if(map.get<RegistryKeyType>(PARAMETER_BASE_CALENDAR_ID) > 0)
			{
				try
				{
					_calendarTemplate = CalendarTemplateTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_BASE_CALENDAR_ID), *_env);
				}
				catch (ObjectNotFoundException<CalendarTemplate>)
				{
					throw ActionException("No such calendar");
				}
			}

			_title = map.get<string>(PARAMETER_TITLE);
			_format = static_cast<Timetable::ContentType>(map.get<int>(PARAMETER_FORMAT));

			// Compression
			if(map.isDefined(PARAMETER_FIELD_COMPRESSION))
			{
				_compression = map.get<bool>(PARAMETER_FIELD_COMPRESSION);
			}

			// Ignore empty rows
			_ignoreEmptyRows = map.getOptional<bool>(PARAMETER_IGNORE_EMPTY_ROWS);
		}



		void TimetableUpdateAction::run(Request& request)
		{
			if(_calendarTemplate.get())
			{
				_timetable->set<BaseCalendar>(*_calendarTemplate);
			}
			else
			{
				_timetable->set<BaseCalendar>(BaseCalendar::Type());
			}
			_timetable->set<Title>(_title);
			_timetable->setContentType(_format);
			if(_container.get())
			{
				_timetable->set<Book>(*_container);
			}
			else
			{
				_timetable->set<Book>(Book::Type());
			}
			if(_ignoreEmptyRows)
			{
				_timetable->set<IgnoreEmptyRows>(*_ignoreEmptyRows);
			}
			if(_compression)
			{
				_timetable->set<Compression>(*_compression);
			}

			TimetableTableSync::Save(_timetable.get());
		}



		bool TimetableUpdateAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}



		void TimetableUpdateAction::setTimetable( boost::shared_ptr<Timetable> value )
		{
			_timetable = value;
		}
}	}
