////////////////////////////////////////////////////////////////////////////////
/// AddScenarioAction class implementation.
///	@file AddScenarioAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "17_messages/AddScenarioAction.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTemplateInheritedTableSync.h"
#include "17_messages/ScenarioInheritedTableSync.h"
#include "17_messages/MessagesLibraryLog.h"
#include "17_messages/AlarmObjectLink.h"
#include "17_messages/AlarmObjectLinkTableSync.h"
#include "17_messages/ScenarioFolder.h"
#include "17_messages/ScenarioFolderTableSync.h"
#include "MessagesLibraryRight.h"
#include "30_server/ActionException.h"
#include "30_server/Request.h"
#include "30_server/QueryString.h"
#include "30_server/ParametersMap.h"

#include "01_util/Conversion.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace time;
	using namespace util;
	using namespace db;
	using namespace security;
	
	
	template<> const string util::FactorableTemplate<Action, messages::AddScenarioAction>::FACTORY_KEY("masca");

	namespace messages
	{
		const string AddScenarioAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ti";
		const string AddScenarioAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string AddScenarioAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";


		ParametersMap AddScenarioAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_FOLDER_ID, _folderId);
			return map;
		}

		void AddScenarioAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Template to copy
			uid id(map.getUid(PARAMETER_TEMPLATE_ID, false, FACTORY_KEY));
			if (id != UNKNOWN_VALUE)
			{
				try
				{
					_template = ScenarioTemplateInheritedTableSync::Get(id, _env);
				}
				catch(...)
				{
					throw ActionException("specified scenario template not found");
				}
			}

			// Folder
			setFolderId(map.getUid(PARAMETER_FOLDER_ID, true, FACTORY_KEY));
			
			// Name
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			if(_name.empty())
				throw ActionException("Le scénario doit avoir un nom.");
			Env env;
			ScenarioTemplateInheritedTableSync::Search(env, _folder.get() ? _folder->getKey() : 0, _name, NULL, 0, 1);
			if (!env.getRegistry<ScenarioTemplate>().empty())
				throw ActionException("Un scénario de même nom existe déjà");

			// Anti error
			if (map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY) == UNKNOWN_VALUE)
				_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}

		void AddScenarioAction::run()
		{
			// The action on the scenario
			shared_ptr<ScenarioTemplate> scenario;
			if (_template.get())
				scenario.reset(new ScenarioTemplate(*_template, _name));
			else
				scenario.reset(new ScenarioTemplate(_name));
			scenario->setFolderId(_folder.get() ? _folder->getKey() : 0);
			ScenarioTableSync::Save (scenario.get());

			// Remember of the id of created object to view it after the action
			_request->setObjectId(scenario->getKey());

			// The action on the alarms
			if (_template.get())
			{
				const ScenarioTemplate::AlarmsSet& alarms(_template->getAlarms());
				BOOST_FOREACH(const AlarmTemplate* sourceAlarm, alarms)
				{
					AlarmTemplate alarm(scenario.get(), *sourceAlarm);
					AlarmTableSync::Save(&alarm);

					AlarmObjectLinkTableSync::CopyRecipients(sourceAlarm->getKey(), alarm.getKey());
				}
			}

			// Log
			MessagesLibraryLog::addCreateEntry(scenario.get(), _template.get(), _request->getUser().get());
		}

		void AddScenarioAction::setFolderId( uid id)
		{
			if (id > 0)
			{
				try
				{
					_folderId = id;
					_folder = ScenarioFolderTableSync::Get(_folderId, _env);
				}
				catch (...)
				{
					throw ActionException("Bad folder ID");
				}
			}
			else
			{
				_folderId = 0;
			}

		}



		bool AddScenarioAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
