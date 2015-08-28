
/** ScenarioFolderAdd class implementation.
	@file ScenarioFolderAdd.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ScenarioFolderAdd.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "MessagesLibraryRight.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "MessagesLibraryLog.h"
#include "UtilConstants.h"
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
		template<> const string FactorableTemplate<Action, messages::ScenarioFolderAdd>::FACTORY_KEY("ScenarioFolderAdd");
	}

	namespace messages
	{
		const string ScenarioFolderAdd::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string ScenarioFolderAdd::PARAMETER_PARENT_ID = Action_PARAMETER_PREFIX + "pi";



		ScenarioFolderAdd::ScenarioFolderAdd()
			: util::FactorableTemplate<Action, ScenarioFolderAdd>()
		{
		}



		ParametersMap ScenarioFolderAdd::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_PARENT_ID, _parent.get() ? _parent->getKey() : 0);
			return map;
		}



		void ScenarioFolderAdd::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_PARENT_ID));
			if (id > 0)
			{
				try
				{
					_parent = ScenarioFolderTableSync::GetEditable(id, *_env);
				}
				catch(...)
				{
					throw ActionException("Le répertoire parent désigné n'existe pas.");
				}
			}

			_name = map.get<string>(PARAMETER_NAME);

			Env env;
			ScenarioFolderTableSync::Search(
				env,
				_parent.get() ? _parent->getKey() : 0,
				_name, 0, 1
			);
			if (!env.getRegistry<ScenarioFolder>().empty())
				throw ActionException("Ce nom est déjà utilisé dans le répertoire courant.");
		}



		void ScenarioFolderAdd::run(
			Request& request
		){
			ScenarioFolder f;
			if (_parent) f.set<Parent>(*_parent);
			f.set<Name>(_name);

			ScenarioFolderTableSync::Save(&f);

			request.setActionCreatedId(f.getKey());

			MessagesLibraryLog::AddCreateEntry(f, request.getUser().get());
		}



		void ScenarioFolderAdd::setParent(
			boost::shared_ptr<ScenarioFolder> value
		){
			_parent = value;
		}



		void ScenarioFolderAdd::setParent(
			boost::shared_ptr<const ScenarioFolder> value
		){
			_parent = const_pointer_cast<ScenarioFolder>(value);
		}



		bool ScenarioFolderAdd::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
