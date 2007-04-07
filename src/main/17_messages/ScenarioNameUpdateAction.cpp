
/** ScenarioNameUpdateAction class implementation.
	@file ScenarioNameUpdateAction.cpp

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

#include "17_messages/ScenarioNameUpdateAction.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/Scenario.h"
#include "17_messages/ScenarioTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string ScenarioNameUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "nam";


		ParametersMap ScenarioNameUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void ScenarioNameUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_scenario = MessagesModule::getScenarii().get(_request->getObjectId());

				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_NAME);
				if (it == map.end())
					throw ActionException("Name not specified");
				_name = it->second;
			}
			catch (Scenario::RegistryKeyException e)
			{
				throw ActionException("Scenario not found");
			}
		}

		ScenarioNameUpdateAction::ScenarioNameUpdateAction()
			: Action()
			, _scenario(NULL)
		{}

		void ScenarioNameUpdateAction::run()
		{
			_scenario->setName(_name);
			ScenarioTableSync::save(_scenario);
		}
	}
}