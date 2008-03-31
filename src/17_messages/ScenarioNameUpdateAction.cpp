
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
#include "17_messages/ScenarioFolder.h"
#include "17_messages/ScenarioFolderTableSync.h"

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
		const string ScenarioNameUpdateAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";

		ParametersMap ScenarioNameUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void ScenarioNameUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Scenario
				_scenario.reset(ScenarioTableSync::GetUpdateable(_request->getObjectId()));

				// Name
				_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

				// Unicity control
				shared_ptr<ScenarioTemplate> tscenario(dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario));
				if (tscenario.get())
				{
					uid folderId(map.getUid(PARAMETER_FOLDER_ID, true, FACTORY_KEY));

					if (folderId != 0)
						_folder = ScenarioFolderTableSync::Get(folderId);

					vector<shared_ptr<ScenarioTemplate> > existing = ScenarioTableSync::searchTemplate(folderId, _name, dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario).get(), 0, 1);
					if (!existing.empty())
						throw ActionException("Le nom spécifié est déjà utilisé par un autre scénario.");

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

			shared_ptr<ScenarioTemplate> tscenario(dynamic_pointer_cast<ScenarioTemplate, Scenario>(_scenario));
			if (tscenario.get())
				tscenario->setFolderId(_folder.get() ? _folder->getKey() : 0);

			ScenarioTableSync::Save(_scenario.get());
		}
	}
}
