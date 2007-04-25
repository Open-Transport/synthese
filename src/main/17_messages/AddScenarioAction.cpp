
/** AddScenarioAction class implementation.
	@file AddScenarioAction.cpp

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

#include "17_messages/AddScenarioAction.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/ScenarioTemplate.h"

#include "30_server/ActionException.h"
#include "30_server/Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	
	namespace messages
	{
		const string AddScenarioAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "t";
		const string AddScenarioAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "n";


		ParametersMap AddScenarioAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void AddScenarioAction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			// Template to copy
			it = map.find(PARAMETER_TEMPLATE_ID);
			if (it != map.end())
			{
				try
				{
					_template = ScenarioTableSync::getTemplate(Conversion::ToLongLong(it->second));
				}
				catch(...)
				{
					throw ActionException("specified scenario template not found");
				}
			}
			
			// Name
			it = map.find(PARAMETER_NAME);
			if (it == map.end())
				throw ActionException("Name not found");
			_name = it->second;
			if(_name.empty())
				throw ActionException("Le scénario doit avoir un nom.");
			vector<shared_ptr<ScenarioTemplate> > v = ScenarioTableSync::searchTemplate(_name, 0, 1);
			if (!v.empty())
				throw ActionException("Un scénario de même nom existe déjà");

			// Anti error
			if (map.find(Request::PARAMETER_OBJECT_ID) == map.end())
				_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void AddScenarioAction::run()
		{
			shared_ptr<ScenarioTemplate> scenario;
			if (_template.get())
				scenario.reset(new ScenarioTemplate(*_template, _name));
			else
				scenario.reset(new ScenarioTemplate(_name));

			ScenarioTableSync::save (scenario.get());
			ScenarioTableSync::saveAlarms(scenario.get());
			_request->setObjectId(scenario->getKey());
		}
	}
}
