
/** CalendarTemplateElementAddAction class implementation.
	@file CalendarTemplateElementAddAction.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "CalendarRight.h"
#include "CalendarTemplateElementAddAction.h"
#include "Request.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace db;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, calendar::CalendarTemplateElementAddAction>::FACTORY_KEY("CalendarTemplateElementAddAction");
	}

	namespace calendar
	{
		const string CalendarTemplateElementAddAction::PARAMETER_CALENDAR_ID= Action_PARAMETER_PREFIX + "ci";
		const string CalendarTemplateElementAddAction::PARAMETER_INCLUDE_ID= Action_PARAMETER_PREFIX + "ii";
		const string CalendarTemplateElementAddAction::PARAMETER_INTERVAL= Action_PARAMETER_PREFIX + "in";
		const string CalendarTemplateElementAddAction::PARAMETER_MAX_DATE= Action_PARAMETER_PREFIX + "xd";
		const string CalendarTemplateElementAddAction::PARAMETER_MIN_DATE= Action_PARAMETER_PREFIX + "nd";
		const string CalendarTemplateElementAddAction::PARAMETER_POSITIVE= Action_PARAMETER_PREFIX + "po";
		const string CalendarTemplateElementAddAction::PARAMETER_RANK= Action_PARAMETER_PREFIX + "rk";

		
		
		CalendarTemplateElementAddAction::CalendarTemplateElementAddAction()
			: util::FactorableTemplate<Action, CalendarTemplateElementAddAction>(),
			_interval(days(1))
		{
		}
		
		
		
		ParametersMap CalendarTemplateElementAddAction::getParametersMap() const
		{
			ParametersMap map;
			if (_calendar.get())
				map.insert(PARAMETER_CALENDAR_ID, _calendar->getKey());
			return map;
		}
		
		
		
		void CalendarTemplateElementAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_calendar = CalendarTemplateTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID), *_env);
			}
			catch (...)
			{
				throw ActionException("No such calendar");
			}

			_minDate = from_string(map.get<string>(PARAMETER_MIN_DATE));
			_maxDate = from_string(map.get<string>(PARAMETER_MAX_DATE));
			_interval = days(map.getDefault<int>(PARAMETER_INTERVAL, 1));
			_positive = map.get<bool>(PARAMETER_POSITIVE);
			_rank = map.getDefault<size_t>(PARAMETER_RANK, CalendarTemplateElementTableSync::GetMaxRank(_calendar->getKey()) + 1);
		}
		
		
		
		void CalendarTemplateElementAddAction::run()
		{
			CalendarTemplateElement e;
			e.setIncludeId(_includeId);
			e.setInterval(_interval);
			e.setMaxDate(_maxDate);
			e.setMinDate(_minDate);
			e.setPositive(_positive);
			e.setRank(_rank);

			CalendarTemplateElementTableSync::Shift(_calendar->getKey(), _rank, 1);

			CalendarTemplateElementTableSync::Save(&e);

			_calendar->addElement(e);

			CalendarTemplateTableSync::Save(_calendar.get());
		}



		void CalendarTemplateElementAddAction::setCalendar(boost::shared_ptr<CalendarTemplate> value)
		{
			_calendar = value;
		}

		void CalendarTemplateElementAddAction::setCalendar(boost::shared_ptr<const CalendarTemplate> value)
		{
			_calendar = const_pointer_cast<CalendarTemplate>(value);
		}

		
		bool CalendarTemplateElementAddAction::_isAuthorized() const
		{
			return _request->isAuthorized<CalendarRight>(WRITE);
		}
	}
}
