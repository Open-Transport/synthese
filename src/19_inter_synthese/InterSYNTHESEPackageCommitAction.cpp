
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

			// Check that the server is not the repository of the package
			if(!_package->get<Import>())
			{
				throw ActionException("The package is already on its repository");
			}

			// Release lock
			_releaseLock = map.getDefault<bool>(PARAMETER_RELEASE_LOCK, false);
		}



		void InterSYNTHESEPackageCommitAction::run(
			Request& request
		){
			// Get the import
			const Import& import(*_package->get<Import>());

			StaticFunctionRequest<InterSYNTHESEPackageCommitService> r;
			r.getFunction()->setUser(
				import.get<Parameters>().get<string>(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_USER)
			);
			r.getFunction()->setPassword(
				import.get<Parameters>().get<string>(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_PASSWORD)
			);
			r.getFunction()->setPackage(_package);
			r.getFunction()->setReleaseLock(_releaseLock);
			r.getFunction()->setContentStr(_package->buildDump());
			InterSYNTHESEPackage::PackageAddress address(
				import.get<Parameters>().get<string>(InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_URL)
			);
			BasicClient c(
				address.host,
				address.port
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
