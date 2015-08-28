////////////////////////////////////////////////////////////////////////////////
/// AddScenarioAction class implementation.
///	@file AddScenarioAction.cpp
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

#include "AddScenarioAction.h"

#include "Profile.h"
#include "ScenarioTemplate.h"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"
#include "Session.h"
#include "User.h"
#include "MessagesLibraryLog.h"
#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "MessagesLibraryRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
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
			optional<RegistryKeyType> id(
				map.getOptional<RegistryKeyType>(PARAMETER_TEMPLATE_ID)
			);
			if (id)
			{
				try
				{
					_template = ScenarioTemplateTableSync::GetCast<ScenarioTemplate>(*id, *_env);
				}
				catch(...)
				{
					throw ActionException("specified scenario template not found");
				}
			}

			// Folder
			id = map.getOptional<RegistryKeyType>(PARAMETER_FOLDER_ID);
			if(id && *id > 0)
			{
				try
				{
					_folder = ScenarioFolderTableSync::GetEditable(*id, *_env);
				}
				catch (...)
				{
					throw ActionException("Bad folder ID");
				}
			}

			// Name
			_name = map.get<string>(PARAMETER_NAME);
			if(_name.empty())
			{
				throw ActionException("Le scénario doit avoir un nom.");
			}
			Env env;
			ScenarioTemplateTableSync::SearchResult r(
				ScenarioTemplateTableSync::Search(env, _folder.get() ? _folder->getKey() : 0, _name, NULL, 0, 1)
			);
			if (!r.empty())
			{
				throw ActionException("Un scénario de même nom existe déjà");
			}
		}



		void AddScenarioAction::run(Request& request)
		{
			if (_template.get())
			{
				// Not implemented

			} else {
				ScenarioTemplate scenario;
				scenario.set<Name>(_name);
				scenario.setFolder(_folder.get());

				ScenarioTemplateTableSync::Save(&scenario);

				// Remember of the id of created object to view it after the action
				request.setActionCreatedId(scenario.getKey());

				MessagesLibraryLog::addCreateEntry(
					scenario, request.getUser().get()
				);
			}
		}

		void AddScenarioAction::setFolder(
			boost::shared_ptr<ScenarioFolder> value
		){
			_folder = value;
		}
		void AddScenarioAction::setFolder(
			boost::shared_ptr<const ScenarioFolder> value
		){
			_folder = const_pointer_cast<ScenarioFolder>(value);
		}



		bool AddScenarioAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
