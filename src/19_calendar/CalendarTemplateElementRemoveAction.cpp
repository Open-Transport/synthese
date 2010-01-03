
/** CalendarTemplateElementRemoveAction class implementation.
	@file CalendarTemplateElementRemoveAction.cpp
	@author Hugues
	@date 2009

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
#include "CalendarTemplateElementRemoveAction.h"
#include "CalendarRight.h"
#include "Request.h"
#include "CalendarTemplateElement.h"
#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplate.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, calendar::CalendarTemplateElementRemoveAction>::FACTORY_KEY("CalendarTemplateElementRemoveAction");
	}

	namespace calendar
	{
		const string CalendarTemplateElementRemoveAction::PARAMETER_ELEMENT_ID = Action_PARAMETER_PREFIX + "ei";
		
		
		
		ParametersMap CalendarTemplateElementRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_element.get()) map.insert(PARAMETER_ELEMENT_ID, _element->getKey());
			return map;
		}
		
		
		
		void CalendarTemplateElementRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_element = CalendarTemplateElementTableSync::Get(map.get<RegistryKeyType>(PARAMETER_ELEMENT_ID), *_env);
			}
			catch(ObjectNotFoundException<CalendarTemplateElement>& e)
			{
				throw ActionException("No such element", e, *this);
			}
		}
		
		
		
		void CalendarTemplateElementRemoveAction::run(Request& request)
		{
			CalendarTemplateElementTableSync::Remove(_element->getKey());
			if(_element->getCalendar())
				CalendarTemplateElementTableSync::Shift(_element->getCalendar()->getKey(), _element->getRank(), -1);
		}
		
		
		
		bool CalendarTemplateElementRemoveAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CalendarRight>(WRITE);
		}



		void CalendarTemplateElementRemoveAction::setElement( boost::shared_ptr<const CalendarTemplateElement> value )
		{
			_element = value;
		}
	}
}
