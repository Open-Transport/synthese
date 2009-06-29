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

#include "AddScenarioAction.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "MessagesLibraryLog.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "MessagesLibraryRight.h"
#include "ActionException.h"
#include "Request.h"
#include "Request.h"
#include "ParametersMap.h"
#include "Conversion.h"

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
			map.insert(PARAMETER_FOLDER_ID, _folder.get() ? _folder->getKey() : 0);
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
					_template = ScenarioTemplateInheritedTableSync::Get(id, *_env);
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
		}



		void AddScenarioAction::run()
		{
			if (_template.get())
			{
				ScenarioTemplate scenario(*_template, _name);
				
				ScenarioTableSync::Save(&scenario);
				
				ScenarioTemplateInheritedTableSync::CopyMessagesFromOther(
					_template->getKey(), scenario
				);

				// Remember of the id of created object to view it after the action
				if(_request->getObjectId() == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				{
					_request->setObjectId(scenario.getKey());
				}

				// Log
				MessagesLibraryLog::addCreateEntry(
					scenario, *_template, _request->getUser().get()
				);

			} else {
				ScenarioTemplate scenario(
					_name,
					_folder.get()
				);
				
				ScenarioTableSync::Save(&scenario);

				// Remember of the id of created object to view it after the action
				if(_request->getObjectId() == Request::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				{
					_request->setObjectId(scenario.getKey());
				}

				MessagesLibraryLog::addCreateEntry(
					scenario, _request->getUser().get()
				);
			}
		}

		void AddScenarioAction::setFolderId( uid id)
		{
			if (id > 0)
			{
				try
				{
					_folder = ScenarioFolderTableSync::GetEditable(id, *_env);
				}
				catch (...)
				{
					throw ActionException("Bad folder ID");
				}
			}
		}



		bool AddScenarioAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
