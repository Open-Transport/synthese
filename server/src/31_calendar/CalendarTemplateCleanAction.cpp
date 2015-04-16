
//////////////////////////////////////////////////////////////////////////
/// CalendarTemplateCleanAction class implementation.
/// @file CalendarTemplateCleanAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "CalendarTemplateCleanAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "CalendarRight.h"
#include "Request.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElementTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, calendar::CalendarTemplateCleanAction>::FACTORY_KEY("CalendarTemplateCleanAction");
	}

	namespace calendar
	{
		const string CalendarTemplateCleanAction::PARAMETER_CALENDAR_ID = Action_PARAMETER_PREFIX + "id";



		ParametersMap CalendarTemplateCleanAction::getParametersMap() const
		{
			ParametersMap map;
			if(_calendar.get())
			{
				map.insert(PARAMETER_CALENDAR_ID, _calendar->getKey());
			}
			return map;
		}



		void CalendarTemplateCleanAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_calendar = CalendarTemplateTableSync::Get(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID), *_env);
			}
			catch(ObjectNotFoundException<CalendarTemplate>&)
			{
				throw ActionException("No such calendar");
			}
		}



		void CalendarTemplateCleanAction::run(
			Request& request
		){
			CalendarTemplateElementTableSync::Clean(_calendar->getKey());
//			::AddDeleteEntry(*_object, request.getUser().get());
		}



		bool CalendarTemplateCleanAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CalendarRight>(WRITE);
		}
	}
}
