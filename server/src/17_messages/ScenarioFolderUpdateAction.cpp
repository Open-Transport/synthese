////////////////////////////////////////////////////////////////////////////////
/// ScenarioFolderUpdateAction class implementation.
///	@file ScenarioFolderUpdateAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#include "ScenarioFolderUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "MessagesLibraryRight.h"
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
			RegistryKeyType id(
				map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
			);
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

			// Parent folder
			if(map.isDefined(PARAMETER_PARENT_FOLDER_ID)) try
			{
				RegistryKeyType id(
					map.get<RegistryKeyType>(PARAMETER_PARENT_FOLDER_ID)
				);
				if (id > 0)
				{
					_parentFolder = ScenarioFolderTableSync::GetEditable(id, *_env);
				}
				else
				{
					_parentFolder = boost::shared_ptr<ScenarioFolder>();
				}

				if ((!_folder.get() && !_parentFolder->get()) || (_folder.get() && _parentFolder->get() && _folder->getKey() == (*_parentFolder)->getKey()))
				{
					throw ActionException("The folder and the parent folder cannot be the same");
				}
			}
			catch(...)
			{
				throw ActionException("No such parent folder");
			}

			// Name
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);

				if (_name != _folder->getName())
				{
					Env env;
					ScenarioFolderTableSync::Search(
						env,
						_parentFolder ?
							(_parentFolder.get() ? (*_parentFolder)->getKey() : 0) :
						(_folder->get<Parent>() ? _folder->get<Parent>()->getKey() : 0),
						_name,
						0,
						1
					);
					if (!env.getRegistry<ScenarioFolder>().empty())
					{
						throw ActionException("Ce nom est déjà utilisé dans le répertoire courant.");
					}
				}
			}
		}



		void ScenarioFolderUpdateAction::run(Request& request)
		{
			// Name
			if(_name)
			{
				_folder->set<Name>(*_name);
			}

			// Parent folder
			if(_parentFolder)
			{
				_folder->set<Parent>(*(_parentFolder.get()));
			}

			ScenarioFolderTableSync::Save(_folder.get());
		}



		void ScenarioFolderUpdateAction::setFolder(
			boost::shared_ptr<ScenarioFolder> value
		){
			_folder = value;
		}



		void ScenarioFolderUpdateAction::setFolder(
			boost::shared_ptr<const ScenarioFolder> value
		){
			_folder = const_pointer_cast<ScenarioFolder>(value);
		}



		bool ScenarioFolderUpdateAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
