
//////////////////////////////////////////////////////////////////////////
/// InterSYNTHESEPackageAutoLockAction class implementation.
/// @file InterSYNTHESEPackageAutoLockAction.cpp
/// @author hromain
/// @date 2013
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

#include "InterSYNTHESEPackageAutoLockAction.hpp"

#include "ActionException.h"
#include "InterSYNTHESEPackageTableSync.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "Session.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, inter_synthese::InterSYNTHESEPackageAutoLockAction>::FACTORY_KEY = "inter-synthese_package_auto_lock";

	namespace inter_synthese
	{
		const string InterSYNTHESEPackageAutoLockAction::PARAMETER_PACKAGE_ID = Action_PARAMETER_PREFIX + "_package_id";
		const string InterSYNTHESEPackageAutoLockAction::PARAMETER_LOCK = Action_PARAMETER_PREFIX + "_lock";
		
		
		
		ParametersMap InterSYNTHESEPackageAutoLockAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void InterSYNTHESEPackageAutoLockAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Package
			try
			{
				_package = Env::GetOfficialEnv().getEditable<InterSYNTHESEPackage>(
					map.get<RegistryKeyType>(PARAMETER_PACKAGE_ID)
				).get();
			}
			catch (ObjectNotFoundException<InterSYNTHESEPackage>&)
			{
				throw ActionException("Bad package");
			}

			// Mutex
			_lockMutex.reset(new mutex::scoped_lock(_package->getLockMutex()));

			// Lock value
			_lock = map.get<bool>(PARAMETER_LOCK);

			// Check if the package status is compatible with the action
			if(_lock)
			{
				// Check if the package is not already locked
				if(!_package->get<LockTime>().is_not_a_date_time())
				{
					throw ActionException("The package is already locked");
				}
			}
			else
			{
				// Check if the package is already locked on the current server
				if(!_package->get<LockServerName>().empty())
				{
					throw ActionException("The package is locked on an other server");
				}
			}
		}
		
		
		
		void InterSYNTHESEPackageAutoLockAction::run(
			Request& request
		){
			// Update of the package
			if(_lock)
			{
				_package->localLock(*request.getSession()->getUser());
			}
			else
			{
				_package->unlock();
			}

			// Todo : log

			// Save
			InterSYNTHESEPackageTableSync::Save(_package);
		}
		
		
		
		bool InterSYNTHESEPackageAutoLockAction::isAuthorized(
			const Session* session
		) const {
			return session && session->getUser(); // && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		InterSYNTHESEPackageAutoLockAction::InterSYNTHESEPackageAutoLockAction():
			_lock(false),
			_package(NULL)
		{

		}
}	}