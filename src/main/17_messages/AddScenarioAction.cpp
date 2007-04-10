
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

#include "30_server/ActionException.h"
#include "30_server/Request.h"

#include "17_messages/AddScenarioAction.h"
#include "17_messages/MessagesModule.h"
#include "17_messages/ScenarioTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
	namespace messages
	{
		const string AddScenarioAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "t";
		const string AddScenarioAction::PARAMETER_IS_TEMPLATE = Action_PARAMETER_PREFIX + "i";
		const string AddScenarioAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "n";


		ParametersMap AddScenarioAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(make_pair(PARAMETER_xxx, _xxx));
			return map;
		}

		void AddScenarioAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_TEMPLATE_ID);
				if (it != map.end())
				{
					_template = MessagesModule::getScenarii().get(Conversion::ToLongLong(it->second));
				}

				it = map.find(PARAMETER_IS_TEMPLATE);
				if (it == map.end())
					throw ActionException("Scenario is template not found");
				_isTemplate = Conversion::ToBool(it->second);
				
				if (_isTemplate)
				{
					it = map.find(PARAMETER_NAME);
					if (it == map.end())
						throw ActionException("Name not found");
					_name = it->second;
					if(_name.empty())
						throw ActionException("Le scénario doit avoir un nom.");
					vector<shared_ptr<Scenario> > v = ScenarioTableSync::search(_isTemplate, _name, 0, 1);
					if (!v.empty())
					{
						throw ActionException("Un scénario de même nom existe déjà");
					}
				}
				else if (!_template)
					throw ActionException("A template must be specified");

				if (map.find(Request::PARAMETER_OBJECT_ID) == map.end())
					_request->setObjectId(Request::UID_WILL_BE_GENERATED_BY_THE_ACTION);
			}
			catch (Scenario::RegistryKeyException e)
			{
				throw ActionException("Specified scenario not found");
			}
		}

		void AddScenarioAction::run()
		{
			shared_ptr<Scenario> scenario;
			if (_template.get())
				scenario = _template->createCopy();
			else
				scenario.reset(new Scenario);
			scenario->setIsATemplate(_isTemplate);
			if (_isTemplate)
				scenario->setName(_name);
			ScenarioTableSync::save(scenario.get());

			_request->setObjectId(scenario->getKey());
		}
	}
}
