////////////////////////////////////////////////////////////////////////////////
/// ScenarioFolderUpdateAction class implementation.
///	@file ScenarioFolderUpdateAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "MessagesLibraryRight.h"
#include "ScenarioFolderUpdateAction.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "Env.h"
#include "Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace security;
	
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, messages::ScenarioFolderUpdateAction>::FACTORY_KEY("ScenarioFolderUpdateAction");
	}

	namespace messages
	{
		const string ScenarioFolderUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string ScenarioFolderUpdateAction::PARAMETER_PARENT_FOLDER_ID = Action_PARAMETER_PREFIX + "fi";
		
		
		
		ScenarioFolderUpdateAction::ScenarioFolderUpdateAction()
			: util::FactorableTemplate<Action, ScenarioFolderUpdateAction>()
		{
		}
		
		
		
		ParametersMap ScenarioFolderUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(Request::PARAMETER_OBJECT_ID, _folder.get() ? _folder->getKey() : 0);
			return map;
		}
		
		
		
		void ScenarioFolderUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setFolderId(map.getUid(Request::PARAMETER_OBJECT_ID, true, FACTORY_KEY));

			try
			{
				uid id(map.getUid(PARAMETER_PARENT_FOLDER_ID, true, FACTORY_KEY));
				if (id > 0)
				{
					_parentFolder = ScenarioFolderTableSync::GetEditable(id, *_env);
				}
			}
			catch(...)
			{
				throw ActionException("No such parent folder");
			}

			if ((!_folder.get() && !_parentFolder.get()) || (_folder.get() && _parentFolder.get() && _folder->getKey() == _parentFolder->getKey()))
				throw ActionException("The folder and the parent folder cannot be the same");

			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

			if (_name != _folder->getName())
			{
				Env env;
				ScenarioFolderTableSync::Search(env, _parentFolder.get() ? _parentFolder->getKey() : 0, _name, 0, 1);
				if (!env.getRegistry<ScenarioFolder>().empty())
					throw ActionException("Ce nom est déjà utilisé dans le répertoire courant.");
			}
		}
		
		
		
		void ScenarioFolderUpdateAction::run()
		{
			_folder->setName(_name);
			_folder->setParent(_parentFolder.get());

			ScenarioFolderTableSync::Save(_folder.get());
		}



		void ScenarioFolderUpdateAction::setFolderId( uid id )
		{
			try
			{
				if (id > 0)
				{
					_folder = ScenarioFolderTableSync::GetEditable(id, *_env);
				}
			}
			catch(...)
			{
				throw ActionException("No such folder");
			}

		}



		bool ScenarioFolderUpdateAction::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
