
/** InterSYNTHESEPackageFileFormat class implementation.
	@file InterSYNTHESEPackageFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "InterSYNTHESEPackageFileFormat.hpp"

#include "BasicClient.h"
#include "DBModule.h"
#include "Import.hpp"
#include "InterSYNTHESEPackage.hpp"
#include "InterSYNTHESEPackageContent.hpp"
#include "InterSYNTHESEPackageGetContentService.hpp"
#include "StaticFunctionRequest.h"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace inter_synthese;
	using namespace server;
	using namespace util;

	template<>
	const string FactorableTemplate<FileFormat, InterSYNTHESEPackageFileFormat>::FACTORY_KEY = "inter_synthese_package";

	namespace inter_synthese
	{
		const string InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_LOCK = "lock";
		const string InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_LOCK_SERVER_NAME = "lock_server_name";
		const string InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_PASSWORD = "password";
		const string InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_URL = "url";
		const string InterSYNTHESEPackageFileFormat::Importer_::PARAMETER_USER = "user";



		bool InterSYNTHESEPackageFileFormat::Importer_::_read(
		) const	{
			try
			{
				// Log for debugging purpose
				_logDebug(
					"Inter-SYNTHESE Package : Attempt to sync with "+ _address +":"+ _port +" for package "+ _smartURL
				);
				
				// Repository function call
				StaticFunctionRequest<InterSYNTHESEPackageGetContentService> r;
				r.getFunction()->setUser(_user);
				r.getFunction()->setPassword(_password);
				r.getFunction()->setSmartURL(_smartURL);
				r.getFunction()->setLock(_lock);
				r.getFunction()->setLockServerName(_lockServerName);
				BasicClient c(
					_address,
					_port
				);
				string result(
					c.get(r.getURL())
				);
				_content.reset(
					new InterSYNTHESEPackageContent(
						_env,
						result,
						const_cast<Import&>(this->getImport()),
						*this
				)	);
			}
			catch(std::exception& e)
			{
				_logError(
					"Inter-SYNTHESE : Synchronization with "+ _address +":"+ _port + " for package "+ _smartURL +" has failed "+ string(e.what())
				);
				return false;
			}
			return true;
		}



		void InterSYNTHESEPackageFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			// URL
			InterSYNTHESEPackage::PackageAddress address(map.getDefault<string>(PARAMETER_URL));
			_address = address.host;
			_port = address.port;
			_smartURL = address.smartURL;

			// User
			_user = map.getDefault<string>(PARAMETER_USER);

			// Password
			_password = map.getDefault<string>(PARAMETER_PASSWORD);

			// Lock
			_lock = map.getDefault<bool>(PARAMETER_LOCK, false);

			// Lock server name
			_lockServerName = map.getDefault<string>(PARAMETER_LOCK_SERVER_NAME);
			if(_lockServerName.empty())
			{
				_lockServerName = lexical_cast<string>(DBModule::GetNodeId());
			}
		}



		ParametersMap InterSYNTHESEPackageFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap pm;
			return pm;
		}



		DBTransaction InterSYNTHESEPackageFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			_content->save(transaction);
			return transaction;
		}



		InterSYNTHESEPackageFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			ConnectionImporter<InterSYNTHESEPackageFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_lock(false)
		{}
}	}

