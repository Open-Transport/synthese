
/** ScenarioFolderAdd class implementation.
	@file ScenarioFolderAdd.cpp
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
#include "QueryString.h"
#include "Request.h"
#include "MessagesLibraryRight.h"
#include "ScenarioFolderAdd.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "MessagesLibraryLog.h"

#include "01_util/Constants.h"
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
			setParentId(map.getUid(PARAMETER_PARENT_ID, false, FACTORY_KEY));
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

			Env env;
			ScenarioFolderTableSync::Search(env, _parent.get() ? _parent->getKey() : 0, _name, 0, 1);
			if (!env.getRegistry<ScenarioFolder>().empty())
				throw ActionException("Ce nom est déjà utilisé dans le répertoire courant.");
		}
		
		
		
		void ScenarioFolderAdd::run()
		{
			ScenarioFolder f;
			f.setParent(_parent.get());
			f.setName(_name);

			ScenarioFolderTableSync::Save(&f);

			if(_request->getObjectId() == QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
			{
				_request->setObjectId(f.getKey());
			}

			MessagesLibraryLog::AddCreateEntry(f, _request->getUser().get());
		}



		void ScenarioFolderAdd::setParentId( uid id)
		{
			if (id > 0)
			{
				try
				{
					_parent = ScenarioFolderTableSync::GetEditable(id, _env);
				}
				catch(...)
				{
					throw ActionException("Le répertoire parent désigné n'existe pas.");
				}
			}
			if (id < 0)
				throw ActionException("Bad parent folder id");
		}



		bool ScenarioFolderAdd::_isAuthorized(
		) const {
			return _request->isAuthorized<MessagesLibraryRight>(WRITE);
		}
	}
}
