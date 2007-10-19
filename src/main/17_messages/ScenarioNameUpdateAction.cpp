
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

#include "ScenarioNameUpdateAction.h"

#include "17_messages/MessagesModule.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, messages::ScenarioNameUpdateAction>::FACTORY_KEY("snu");

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
				// Scenario
				_scenario = ScenarioTableSync::getScenario(_request->getObjectId());

				// Name
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

				// Unicity control
				if (dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario).get())
				{
					vector<shared_ptr<ScenarioTemplate> > existing = ScenarioTableSync::searchTemplate(_name, dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario).get(), 0, 1);
					if (!existing.empty())
						throw ActionException("Le nom sp�cifi� est d�j� utilis� par un autre sc�nario.");
				}
			}
			catch (ObjectNotFoundException<uid,Scenario>& e)
			{
				throw ActionException(e.getMessage());
			}
		}

		void ScenarioNameUpdateAction::run()
		{
			_scenario->setName(_name);
			ScenarioTableSync::save(_scenario.get());
		}
	}
}
