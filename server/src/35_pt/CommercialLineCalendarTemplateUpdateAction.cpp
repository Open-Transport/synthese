
//////////////////////////////////////////////////////////////////////////
/// CommercialLineCalendarTemplateUpdateAction class implementation.
/// @file CommercialLineCalendarTemplateUpdateAction.cpp
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

#include "CommercialLineCalendarTemplateUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "CommercialLineTableSync.h"
#include "CalendarTemplateTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::CommercialLineCalendarTemplateUpdateAction>::FACTORY_KEY("CommercialLineCalendarTemplateUpdateAction");
	}

	namespace pt
	{
		const string CommercialLineCalendarTemplateUpdateAction::PARAMETER_LINE_ID = Action_PARAMETER_PREFIX + "id";
		const string CommercialLineCalendarTemplateUpdateAction::PARAMETER_CALENDAR_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ci";



		ParametersMap CommercialLineCalendarTemplateUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_line.get())
			{
				map.insert(PARAMETER_LINE_ID, _line->getKey());
				if(_calendarTemplate.get())
				{
					map.insert(PARAMETER_CALENDAR_TEMPLATE_ID, _calendarTemplate->getKey());
				}
			}
			return map;
		}



		void CommercialLineCalendarTemplateUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_line = CommercialLineTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_LINE_ID), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("No such line");
			}

			try
			{
				_calendarTemplate = CalendarTemplateTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_CALENDAR_TEMPLATE_ID), *_env);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw ActionException("No such calendar template");
			}
		}



		void CommercialLineCalendarTemplateUpdateAction::run(
			Request& request
		){
			//stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			_line->setCalendarTemplate(_calendarTemplate.get());

			CommercialLineTableSync::Save(_line.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool CommercialLineCalendarTemplateUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
		}
	}
}
