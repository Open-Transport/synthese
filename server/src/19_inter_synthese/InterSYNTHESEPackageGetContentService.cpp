
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEPackageGetContentService class implementation.
///	@file InterSYNTHESEPackageGetContentService.cpp
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

#include "InterSYNTHESEPackageGetContentService.hpp"

#include "InterSYNTHESEModule.hpp"
#include "InterSYNTHESEPackageTableSync.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ServerConstants.h"
#include "UserException.h"
#include "UserTableSync.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEPackageGetContentService>::FACTORY_KEY = "inter_synthese_package_get_content";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEPackageGetContentService::PARAMETER_LOCK = "lock";
		const string InterSYNTHESEPackageGetContentService::PARAMETER_LOCK_SERVER_NAME = "lock_server_name";
		const string InterSYNTHESEPackageGetContentService::PARAMETER_PASSWORD = "password";
		const string InterSYNTHESEPackageGetContentService::PARAMETER_SMART_URL = "smart_url";
		const string InterSYNTHESEPackageGetContentService::PARAMETER_USER = "user";
		


		ParametersMap InterSYNTHESEPackageGetContentService::_getParametersMap() const
		{
			ParametersMap map;

			// Lock
			if(_lock)
			{
				map.insert(PARAMETER_LOCK, _lock);
				if(!_lockServerName.empty())
				{
					map.insert(PARAMETER_LOCK_SERVER_NAME, _lockServerName);
				}
			}

			// User / password
			if(!_userLogin.empty())
			{
				map.insert(PARAMETER_USER, _userLogin);
				map.insert(PARAMETER_PASSWORD, _password);
			}

			// Smart URL
			map.insert(PARAMETER_SMART_URL, _smartURL);

			return map;
		}



		void InterSYNTHESEPackageGetContentService::_setFromParametersMap(const ParametersMap& map)
		{
			// Package
			_package = InterSYNTHESEModule::GetPackageBySmartURL(
				map.get<string>(PARAMETER_SMART_URL)
			);
			if(!_package)
			{
				throw RequestException("Bad package");
			}

			// Lock
			_lock = map.getDefault<bool>(PARAMETER_LOCK, false);

			// User
			if(!_package->get<Public>() || _lock)
			{
				try
				{
					_user = UserTableSync::getUserFromLogin(
						map.get<string>(PARAMETER_USER)
					);
					_user->verifyPassword(map.getDefault<string>(PARAMETER_PASSWORD));
				}
				catch(UserException&)
				{
					throw RequestException("Bad user");
				}
			}

			// Lock server name
			if(_lock)
			{
				_lockServerName = map.getDefault<string>(PARAMETER_LOCK_SERVER_NAME);
				if(_lockServerName.empty())
				{
					_lockServerName = map.getDefault<string>(PARAMETER_CLIENT_ADDRESS);
				}

				// Check if the lock is possible
				if(	!_package->isRepository())
				{
					throw RequestException("The package cannot be locked because this server is not the package repository.");
				}

				// TODO user right check
				if(false)
				{
					throw Request::ForbiddenRequestException();
				}
			}
		}



		ParametersMap InterSYNTHESEPackageGetContentService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Lock
			boost::mutex::scoped_lock lockLock(_package->getLockMutex());
			bool lock(_lock);
			if(_lock)
			{
				// Check for locking ability
				if(	!_package->get<LockTime>().is_not_a_date_time())
				{
					lock = false;
				}
			}

			// Dump output
			if(lock)
			{
				stream << _package->lockAndBuildDump(*_user, _lockServerName);
				InterSYNTHESEPackageTableSync::Save(_package);
			}
			else
			{
				stream << _package->buildDump();
			}

			return ParametersMap(); // No parameters map : this is an output service
		}
		
		
		
		bool InterSYNTHESEPackageGetContentService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string InterSYNTHESEPackageGetContentService::getOutputMimeType() const
		{
			return "binary/octetstream";
		}



		InterSYNTHESEPackageGetContentService::InterSYNTHESEPackageGetContentService():
			_lock(false),
			_package(NULL)
		{

		}
}	}
