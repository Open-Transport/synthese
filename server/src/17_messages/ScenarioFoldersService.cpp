
//////////////////////////////////////////////////////////////////////////////////////////
///	ScenarioFoldersService class implementation.
///	@file ScenarioFoldersService.cpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ScenarioFoldersService.hpp"

#include "ScenarioFolderTableSync.h"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,messages::ScenarioFoldersService>::FACTORY_KEY = "ScenarioFoldersService";
	
	namespace messages
	{
		const string ScenarioFoldersService::PARAMETER_ROOT_ID = "root_id";
		const string ScenarioFoldersService::PARAMETER_MAX_DEPTH = "max_depth";
		const string ScenarioFoldersService::TAG_FOLDER = "folder";
		


		ParametersMap ScenarioFoldersService::_getParametersMap() const
		{
			ParametersMap map;
			if(_root.get())
			{
				map.insert(PARAMETER_ROOT_ID, _root->getKey());
			}
			if(_maxDepth)
			{
				map.insert(PARAMETER_MAX_DEPTH, *_maxDepth);
			}
			return map;
		}



		void ScenarioFoldersService::_setFromParametersMap(const ParametersMap& map)
		{
			// Root
			RegistryKeyType rootId(
				map.getDefault<RegistryKeyType>(PARAMETER_ROOT_ID, 0)
			);
			if(rootId)
			{
				try
				{
					_root = Env::GetOfficialEnv().get<ScenarioFolder>(rootId);
				}
				catch (ObjectNotFoundException<ScenarioFolder>&)
				{
					throw RequestException("No such root");
				}
			}

			// Max depth
			_maxDepth = map.getOptional<size_t>(PARAMETER_MAX_DEPTH);
		}



		ParametersMap ScenarioFoldersService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			_exportScenarioFolder(
				_root.get(),
				map,
				0
			);

			return map;
		}



		void ScenarioFoldersService::_exportScenarioFolder(
			const ScenarioFolder* parent,
			util::ParametersMap& pm,
			size_t depth
		) const	{

			ScenarioFolderTableSync::SearchResult folders(
				ScenarioFolderTableSync::Search(Env::GetOfficialEnv(), parent ? parent->getKey() : 0)
			);
			BOOST_FOREACH(const boost::shared_ptr<ScenarioFolder>& cfolder, folders)
			{
				boost::shared_ptr<ParametersMap> folderPM(new ParametersMap);
				cfolder->toParametersMap(*folderPM, true);
				pm.insert(TAG_FOLDER, folderPM);

				if(!_maxDepth || depth < *_maxDepth)
				{
					_exportScenarioFolder(cfolder.get(), *folderPM, depth+1);
				}
			}

		}
		
		
		
		bool ScenarioFoldersService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ScenarioFoldersService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
