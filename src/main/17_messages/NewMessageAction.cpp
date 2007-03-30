
/** NewMessageAction class implementation.
	@file NewMessageAction.cpp

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

#include "17_messages/NewMessageAction.h"
#include "17_messages/Alarm.h"
#include "17_messages/AlarmTableSync.h"
#include "17_messages/MessagesModule.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string NewMessageAction::PARAMETER_IS_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";
		const string NewMessageAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "tps";

		Request::ParametersMap NewMessageAction::getParametersMap() const
		{
			Request::ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void NewMessageAction::setFromParametersMap(Request::ParametersMap& map)
		{
			Request::ParametersMap::iterator it;

			it = map.find(PARAMETER_SCENARIO_ID);
			if (it != map.end())
			{
				if (!MessagesModule::getScenarii().contains(Conversion::ToLongLong(it->second)))
					throw ActionException("Specified scenario not found");
				_scenario = MessagesModule::getScenarii().get(Conversion::ToLongLong(it->second));
                map.erase(it);
			} 

			it = map.find(PARAMETER_IS_TEMPLATE);
			if (it != map.end())
			{
				_isTemplate = Conversion::ToBool(it->second);
				map.erase(it);
			} 

			map.insert(make_pair(Request::PARAMETER_OBJECT_ID, Conversion::ToString(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)));
		}

		NewMessageAction::NewMessageAction()
			: Action()
			, _scenario(NULL)
			, _isTemplate(false)
		{}

		void NewMessageAction::run()
		{
			Alarm* alarm = new Alarm;
			alarm->setIsATemplate(_isTemplate);
			alarm->setScenario(_scenario);
			AlarmTableSync::save(alarm);
			_request->setObjectId(alarm->getKey());
			delete alarm;
		}

		NewMessageAction::~NewMessageAction()
		{
			delete _scenario;
		}
	}
}