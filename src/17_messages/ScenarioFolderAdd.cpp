
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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/QueryString.h"
#include "30_server/Request.h"

#include "ScenarioFolderAdd.h"
#include "ScenarioFolder.h"

#include "17_messages/ScenarioFolderTableSync.h"

#include "01_util/Constants.h"
#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	
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
			map.insert(PARAMETER_PARENT_ID, _parentId);
			return map;
		}
		
		
		
		void ScenarioFolderAdd::_setFromParametersMap(const ParametersMap& map)
		{
			_parentId = map.getUid(PARAMETER_PARENT_ID, false, FACTORY_KEY);
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);

			if (_parentId > 0)
			{
				try
				{
					shared_ptr<const ScenarioFolder> folder(ScenarioFolderTableSync::Get(_parentId));
				}
				catch(...)
				{
					throw ActionException("Le répertoire parent désigné n'existe pas.");
				}
			}
			if (_parentId < 0)
				throw ActionException("Bad parent folder id");

			Env env;
			ScenarioFolderTableSync::Search(env, _parentId, _name, 0, 1);
			if (!env.getRegistry<ScenarioFolder>().empty())
				throw ActionException("Ce nom est déjà utilisé dans le répertoire courant.");

			// Anti error
			if (map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY) == UNKNOWN_VALUE)
				_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}
		
		
		
		void ScenarioFolderAdd::run()
		{
			ScenarioFolder f;
			f.setParentId(_parentId);
			f.setName(_name);

			ScenarioFolderTableSync::Save(&f);

			_request->setObjectId(f.getKey());
		}

		void ScenarioFolderAdd::setParentId( uid value )
		{
			_parentId = value;
		}
	}
}
