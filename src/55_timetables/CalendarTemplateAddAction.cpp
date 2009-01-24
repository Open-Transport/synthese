
/** CalendarTemplateAddAction class implementation.
	@file CalendarTemplateAddAction.cpp
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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/QueryString.h"
#include "30_server/Request.h"

#include "CalendarTemplateAddAction.h"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, timetables::CalendarTemplateAddAction>::FACTORY_KEY("CalendarTemplateAddAction");
	}

	namespace timetables
	{
		const string CalendarTemplateAddAction::PARAMETER_TEXT = Action_PARAMETER_PREFIX + "tx";
		
		
		
		CalendarTemplateAddAction::CalendarTemplateAddAction()
			: util::FactorableTemplate<Action, CalendarTemplateAddAction>()
		{
		}
		
		
		
		ParametersMap CalendarTemplateAddAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void CalendarTemplateAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.getString(PARAMETER_TEXT, true, FACTORY_KEY);
			if (_text.empty())
				throw ActionException("Le texte doit être non vide.");
		}
		
		
		
		void CalendarTemplateAddAction::run()
		{
			CalendarTemplate c;
			c.setText(_text);

			CalendarTemplateTableSync::save(&c);

			if (_request->getObjectId() == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				_request->setObjectId(c.getKey());
		}
	}
}
