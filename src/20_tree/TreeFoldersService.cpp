
//////////////////////////////////////////////////////////////////////////////////////////
///	TreeFoldersService class implementation.
///	@file TreeFoldersService.cpp
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

#include "TreeFoldersService.hpp"

#include "DBException.hpp"
#include "DBModule.h"
#include "TreeFolder.hpp"
#include "TreeFolderRoot.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,tree::TreeFoldersService>::FACTORY_KEY = "tree_folders";
	
	namespace tree
	{
		const string TreeFoldersService::PARAMETER_RECURSIVE = "recursive";
		const string TreeFoldersService::TAG_FOLDER = "folder";



		ParametersMap TreeFoldersService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void TreeFoldersService::_setFromParametersMap(const ParametersMap& map)
		{
			// Root
			RegistryKeyType rootId(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			if(decodeTableId(rootId))
			{
				try
				{
					_rootObject = boost::dynamic_pointer_cast<const TreeFolderRoot, const Registrable>(
						DBModule::GetObject(rootId, Env::GetOfficialEnv())
					);

					// Check if the object is a tree folder root
					if(!_rootObject.get())
					{
						throw RequestException("Invalid root");
					}
				}
				catch(ObjectNotFoundException<Registrable>& e)
				{
					throw RequestException("Invalid root "+ e.getMessage());
				}
			}
			else
			{
				try
				{
					_rootTable = DBModule::GetTableSync(rootId);
				}
				catch(DBException& e)
				{
					throw RequestException("No such table ("+ e.getMessage() +")");
				}
			}

			// Recursive
			_recursive = map.getDefault<bool>(PARAMETER_RECURSIVE, true);
		}



		ParametersMap TreeFoldersService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			if(_rootObject.get())
			{
				BOOST_FOREACH(TreeFolder* subFolder, _rootObject->getSubFolders())
				{
					_exportFolder(*subFolder, map, _recursive);
				}
			}

			return map;
		}



		void TreeFoldersService::_exportFolder(
			const TreeFolder& folder,
			util::ParametersMap& pm,
			bool recursive
		) const	{

			boost::shared_ptr<ParametersMap> folderPM(new ParametersMap);
			folder.toParametersMap(*folderPM);
			if(recursive)
			{
				BOOST_FOREACH(TreeFolder* subFolder, folder.getSubFolders())
				{
					_exportFolder(*subFolder, *folderPM, true);
				}
			}
			pm.insert(TAG_FOLDER, folderPM);
		}
		
		
		
		bool TreeFoldersService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string TreeFoldersService::getOutputMimeType() const
		{
			return "text/html";
		}



		TreeFoldersService::TreeFoldersService():
			_recursive(true)
		{

		}
}	}
