
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEPackageCommitService class implementation.
///	@file InterSYNTHESEPackageCommitService.cpp
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

#include "InterSYNTHESEPackageCommitService.hpp"

#include "DBTransaction.hpp"
#include "InterSYNTHESEPackageContent.hpp"
#include "InterSYNTHESEPackageTableSync.hpp"
#include "RequestException.h"
#include "Request.h"
#include "UserException.h"
#include "UserTableSync.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEPackageCommitService>::FACTORY_KEY = "inter-synthese_package_commit";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEPackageCommitService::PARAMETER_PACKAGE_ID = "package";
		const string InterSYNTHESEPackageCommitService::PARAMETER_USER = "user";
		const string InterSYNTHESEPackageCommitService::PARAMETER_PASSWORD = "password";
		const string InterSYNTHESEPackageCommitService::PARAMETER_RELEASE_LOCK = "release_lock";
		const string InterSYNTHESEPackageCommitService::PARAMETER_CONTENT = "content";
		const string InterSYNTHESEPackageCommitService::PARAMETER_CREATE_PACKAGE = "create_package";
		


		ParametersMap InterSYNTHESEPackageCommitService::_getParametersMap() const
		{
			ParametersMap map;

			// Package
			if(_package)
			{
				map.insert(PARAMETER_PACKAGE_ID, _package->getKey());
			}

			// User / password
			map.insert(PARAMETER_USER, _userLogin);
			map.insert(PARAMETER_PASSWORD, _password);

			// Actions
			map.insert(PARAMETER_RELEASE_LOCK, _releaseLock);
			map.insert(PARAMETER_CONTENT, _contentStr);
			map.insert(PARAMETER_CREATE_PACKAGE, _createPackage);

			return map;
		}



		void InterSYNTHESEPackageCommitService::_setFromParametersMap(const ParametersMap& map)
		{
			// User
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

			// Package
			RegistryKeyType packageId(map.get<RegistryKeyType>(PARAMETER_PACKAGE_ID));
			_createPackage = map.getDefault<bool>(PARAMETER_CREATE_PACKAGE, false);
			shared_ptr<InterSYNTHESEPackage> package;
			if(	InterSYNTHESEPackageTableSync::Contains(packageId))
			{
				package = InterSYNTHESEPackageTableSync::GetEditable(
					packageId,
					*_env
				);
			}
			else
			{
				if(_createPackage)
				{
					package.reset(new InterSYNTHESEPackage);
					package->setKey(packageId);
					_env->add(package);
					ptime now(second_clock::local_time());
					package->set<LockTime>(now);
					package->set<LockUser>(*_user);
				}
				else
				{
					throw RequestException("No such package");
				}
			}

			// Check if the user has already locked the package
			if(	!package->isRepository() ||
				package->get<LockTime>().is_not_a_date_time() ||
				!package->get<LockUser>() ||
				package->get<LockUser>()->getKey() != _user->getKey()
			){
				throw Request::ForbiddenRequestException();
			}

			// Content
			_content.reset(
				new InterSYNTHESEPackageContent(
					*_env,
					map.get<string>(PARAMETER_CONTENT),
					package,
					boost::optional<const impex::Importer&>()
			)	);

			// Package
			_package = package.get();

			// Release lock
			_releaseLock = map.getDefault<bool>(PARAMETER_RELEASE_LOCK, false);
		}



		ParametersMap InterSYNTHESEPackageCommitService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// Content update
			try
			{
				// Release the lock
				if(_releaseLock)
				{
					_package->unlock();
				}

				// Prepare the saving transaction
				DBTransaction transaction;
				InterSYNTHESEPackageTableSync::Save(_package, transaction);
				_content->save(transaction);

				// Run the saving transaction
				transaction.run();

				// Success output
				stream << "1";
			}
			catch(...)
			{

			}

			return ParametersMap();
		}
		
		
		
		bool InterSYNTHESEPackageCommitService::isAuthorized(
			const Session* session
		) const {
			return true; // session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string InterSYNTHESEPackageCommitService::getOutputMimeType() const
		{
			return "text/plain";
		}



		InterSYNTHESEPackageCommitService::InterSYNTHESEPackageCommitService()
		:	_package(NULL),
			_releaseLock(false),
			_createPackage(false)
		{
			setEnv(shared_ptr<Env>(new Env));
		}
}	}
