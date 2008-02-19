
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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"

#include "ScenarioFolderRemoveAction.h"
#include "17_messages/ScenarioFolderTableSync.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/ScenarioFolder.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
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
			uid id(map.getUid(PARAMETER_FOLDER_ID, true, FACTORY_KEY));
			try
			{
				_folder = ScenarioFolderTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("No such folder");
			}

			vector<shared_ptr<ScenarioTemplate> > sv = ScenarioTableSync::searchTemplate(
				_folder->getKey()
				, string(), NULL
				, 0, 1
			);
			if (!sv.empty())
				throw ActionException("Non empty folder");
		}
		
		
		
		void ScenarioFolderRemoveAction::run()
		{
			ScenarioFolderTableSync::Remove(_folder->getKey());

			if (_request->getObjectId() == _folder->getKey())
				_request->setObjectId(_folder->getParentId());
		}



		void ScenarioFolderRemoveAction::setFolder( boost::shared_ptr<const ScenarioFolder> value )
		{
			_folder = value;
		}
	}
}
