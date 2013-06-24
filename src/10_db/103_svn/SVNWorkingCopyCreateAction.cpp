
//////////////////////////////////////////////////////////////////////////
/// SVNWorkingCopyCreateAction class implementation.
/// @file SVNWorkingCopyCreateAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "SVNWorkingCopyCreateAction.hpp"

#include "ActionException.h"
#include "DBDirectTableSync.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "ObjectBase.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "SVNWorkingCopy.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, db::svn::SVNWorkingCopyCreateAction>::FACTORY_KEY = "SVNWorkingCopyCreate";

	namespace db
	{
		namespace svn
		{
			const string SVNWorkingCopyCreateAction::PARAMETER_OBJECT_ID = Action_PARAMETER_PREFIX + "_object_id";
			const string SVNWorkingCopyCreateAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "_user";
			const string SVNWorkingCopyCreateAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "_password";
			const string SVNWorkingCopyCreateAction::PARAMETER_REPO_URL = Action_PARAMETER_PREFIX + "_repo_url";
			const string SVNWorkingCopyCreateAction::PARAMETER_NO_COMMIT = Action_PARAMETER_PREFIX + "_no_commit";
			
			
			
			ParametersMap SVNWorkingCopyCreateAction::getParametersMap() const
			{
				ParametersMap map;

				// Object
				if(_object.get())
				{
					map.insert(PARAMETER_OBJECT_ID, _object->getKey());
				}

				return map;
			}
			
			
			
			void SVNWorkingCopyCreateAction::_setFromParametersMap(const ParametersMap& map)
			{
				// Object
				try
				{
					_object = dynamic_pointer_cast<ObjectBase, Registrable>(
						DBModule::GetEditableObject(
							map.get<RegistryKeyType>(PARAMETER_OBJECT_ID),
							Env::GetOfficialEnv()
					)	);
				}
				catch(ObjectNotFoundException<Registrable>&)
				{
					throw ActionException("No such object");
				}
				catch(Exception&)
				{
					throw ActionException("This object cannot be saved into a subversion repository");
				}
				if(!_object.get())
				{
					throw ActionException("This object cannot be saved into a subversion repository");
				}

				// Check if the object has SVN storage capabilities
				if(!_object->hasField<SVNWorkingCopy>())
				{
					throw ActionException("This object cannot be saved into a subversion repository");
				}

				// User
				_user = map.getDefault<string>(PARAMETER_USER);

				// Password
				_password = map.getDefault<string>(PARAMETER_PASSWORD);

				// No commit
				_noCommit = map.getDefault<bool>(PARAMETER_NO_COMMIT, false);

				// Repository URL
				_repo = SVNRepository(map.getDefault<string>(PARAMETER_REPO_URL));
				if(_repo.getURL().empty() && !_noCommit)
				{
					throw ActionException("Repository URL must be non empty");
				}
			}
			
			
			
			void SVNWorkingCopyCreateAction::run(
				Request& request
			){
				SVNWorkingCopy wc;
				wc.setObject(_object.get());

				wc.setRepo(_repo);
				_object->dynamic_set<SVNWorkingCopy>(wc);
				wc.create(_user, _password, _noCommit);

				RegistryTableType tableId(decodeTableId(_object->getKey()));
				boost::shared_ptr<DBDirectTableSync> tableSync(
					dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
						DBModule::GetTableSync(tableId)
				)	);

				DBTransaction transaction;
				tableSync->saveRegistrable(*_object, transaction);
				transaction.run();
			}
			
			
			
			bool SVNWorkingCopyCreateAction::isAuthorized(
				const Session* session
			) const {
	//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
				return true;
			}
}	}	}
