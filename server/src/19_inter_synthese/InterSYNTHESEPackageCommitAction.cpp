
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEPackageCommitAction class implementation.
///	@file InterSYNTHESEPackageCommitAction.cpp
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

#include "InterSYNTHESEPackageCommitAction.hpp"

#include "ActionException.h"
#include "BasicClient.h"
#include "DBTransaction.hpp"
#include "Import.hpp"
#include "InterSYNTHESEPackageContent.hpp"
#include "InterSYNTHESEPackageCommitService.hpp"
#include "InterSYNTHESEPackageFileFormat.hpp"
#include "InterSYNTHESEPackageTableSync.hpp"
#include "Request.h"
#include "StaticFunctionRequest.h"
#include "UserException.h"
#include "UserTableSync.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace inter_synthese;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Action, InterSYNTHESEPackageCommitAction>::FACTORY_KEY = "inter-synthese_package_commit";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEPackageCommitAction::PARAMETER_PACKAGE_ID = Action_PARAMETER_PREFIX + "_package";
		const string InterSYNTHESEPackageCommitAction::PARAMETER_RELEASE_LOCK = Action_PARAMETER_PREFIX + "_release_lock";
		const string InterSYNTHESEPackageCommitAction::PARAMETER_REMOTE_ADDRESS = Action_PARAMETER_PREFIX + "_remote_address";
		const string InterSYNTHESEPackageCommitAction::PARAMETER_REMOTE_PORT = Action_PARAMETER_PREFIX + "_remote_port";
		const string InterSYNTHESEPackageCommitAction::PARAMETER_REMOTE_USER = Action_PARAMETER_PREFIX + "_remote_user";
		const string InterSYNTHESEPackageCommitAction::PARAMETER_REMOTE_PASSWORD = Action_PARAMETER_PREFIX + "_remote_password";
		


		ParametersMap InterSYNTHESEPackageCommitAction::getParametersMap() const
		{
			ParametersMap map;

			// Package
			if(_package)
			{
				map.insert(PARAMETER_PACKAGE_ID, _package->getKey());
			}

			// Actions
			map.insert(PARAMETER_RELEASE_LOCK, _releaseLock);

			return map;
		}



		void InterSYNTHESEPackageCommitAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Package
			try
			{
				_package = Env::GetOfficialEnv().getEditable<InterSYNTHESEPackage>(
					map.get<RegistryKeyType>(PARAMETER_PACKAGE_ID)
				).get();
			}
			catch(ObjectNotFoundException<InterSYNTHESEPackage>&)
			{
				throw ActionException("No such package");
			}

			// Release lock
			_releaseLock = map.getDefault<bool>(PARAMETER_RELEASE_LOCK, false);

			// Remote address
			_remoteAddress = map.getDefault<string>(PARAMETER_REMOTE_ADDRESS);
			_remotePort = map.getDefault<string>(PARAMETER_REMOTE_PORT, "80");
			_remoteUser = map.getDefault<string>(PARAMETER_REMOTE_USER);
			_remotePassword = map.getDefault<string>(PARAMETER_REMOTE_PASSWORD);

			// Check that the server is not the repository of the package
			if(!_package->get<Import>() && _remoteAddress.empty())
			{
				throw ActionException("The package is already on its repository");
			}
		}



		void InterSYNTHESEPackageCommitAction::run(
			Request& request
		){
			// Get the import
			StaticFunctionRequest<InterSYNTHESEPackageCommitService> r;

			optional<InterSYNTHESEPackage::PackageAddress> address;
			if(_package->get<Import>())
			{
				const Import& import(*_package->get<Import>());
				r.getFunction()->setUser(
					import.get<Parameters>().get<string>(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_USER)
				);
				r.getFunction()->setPassword(
					import.get<Parameters>().get<string>(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_PASSWORD)
				);
				address = InterSYNTHESEPackage::PackageAddress(
					import.get<Parameters>().get<string>(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_URL)
				);
			}
			else
			{
				r.getFunction()->setUser(
					_remoteUser
				);
				r.getFunction()->setPassword(
					_remotePassword
				);
			}
			r.getFunction()->setPackage(_package);
			r.getFunction()->setReleaseLock(_releaseLock);
			r.getFunction()->setContentStr(_package->buildDump());
			r.getFunction()->setCreatePackage(!_remoteAddress.empty());
			BasicClient c(
				(address && _remoteAddress.empty()) ? address->host : _remoteAddress,
				(address && _remoteAddress.empty()) ? address->port : _remotePort
			);
			string contentStr(
				c.post(
					r.getClientURL(),
					r.getURI(),
					"application/x-www-form-urlencoded"
			)	);

			// Result
			if(contentStr == "1")
			{
				if(_releaseLock)
				{
					_package->unlock();
					InterSYNTHESEPackageTableSync::Save(_package);
				}
			}
			else
			{
				throw ActionException("Update has failed");
			}
		}
		
		
		
		bool InterSYNTHESEPackageCommitAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		InterSYNTHESEPackageCommitAction::InterSYNTHESEPackageCommitAction():
			_package(NULL),
			_releaseLock(false)
		{
			setEnv(boost::shared_ptr<Env>(new Env));
		}
}	}
