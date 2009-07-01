
/** ScenarioFolderRemoveAction class implementation.
	@file ScenarioFolderRemoveAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "MessagesLibraryRight.h"
#include "ScenarioFolderRemoveAction.h"
#include "ScenarioFolderTableSync.h"
#include "ScenarioTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioTemplateInheritedTableSync.h"

#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, messages::ScenarioFolderRemoveAction>::FACTORY_KEY("ScenarioFolderRemoveAction");
	}

	namespace messages
	{
		const string ScenarioFolderRemoveAction::PARAMETER_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";
		
		
		
		ScenarioFolderRemoveAction::ScenarioFolderRemoveAction()
			: util::FactorableTemplate<Action, ScenarioFolderRemoveAction>()
		{
		}
		
		
		
		ParametersMap ScenarioFolderRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_folder.get())
				map.insert(PARAMETER_FOLDER_ID, _folder->getKey());
			return map;
		}
		
		
		
		void ScenarioFolderRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_folder = ScenarioFolderTableSync::Get(map.get<RegistryKeyType>(PARAMETER_FOLDER_ID), *_env);
			}
			catch (...)
			{
				throw ActionException("No such folder");
			}

			ScenarioTemplateInheritedTableSync::Search(
				*_env, 
				_folder->getKey()
				, string(), NULL
				, 0, 1
			);
			if (!_env->getRegistry<ScenarioTemplate>().empty())
				throw ActionException("Non empty folder");
		}
		
		
		
		void ScenarioFolderRemoveAction::run()
		{
			ScenarioFolderTableSync::Remove(_folder->getKey());
		}



		void ScenarioFolderRemoveAction::setFolder(
			boost::shared_ptr<const ScenarioFolder> value
		){
			_folder = value;
		}



		bool ScenarioFolderRemoveAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
		}
	}
}
