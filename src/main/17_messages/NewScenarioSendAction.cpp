
/** NewScenarioSendAction class implementation.
	@file NewScenarioSendAction.cpp

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

#include "17_messages/NewScenarioSendAction.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string NewScenarioSendAction::PARAMETER_TEMPLATE = Action_PARAMETER_PREFIX + "tpl";


		ParametersMap NewScenarioSendAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void NewScenarioSendAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Template to source
			it = map.find(PARAMETER_TEMPLATE);
			if (it == map.end())
				throw ActionException("Template not specified");
			try
			{
				_template = ScenarioTableSync::getTemplate(Conversion::ToLongLong(it->second));
			}
			catch(...)
			{
				throw ActionException("specified scenario template not found");
			}
			
			// Anti error
			_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void NewScenarioSendAction::run()
		{
			shared_ptr<SentScenario> scenario(new SentScenario(*_template));
			ScenarioTableSync::saveWithAlarms(scenario.get());
			_request->setObjectId(scenario->getKey());
		}
	}
}