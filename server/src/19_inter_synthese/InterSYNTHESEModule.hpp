
/** InterSYNTHESEModule class header.
	@file InterSYNTHESEModule.hpp
	@author Hugues Romain
	@date 2012

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

#ifndef SYNTHESE_InterSYNTHESEModule_H__
#define SYNTHESE_InterSYNTHESEModule_H__

#include "ModuleClassTemplate.hpp"

#include "UtilTypes.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		class DBTransaction;
	}
	
	namespace util
	{
		class Registrable;
	}

	//////////////////////////////////////////////////////////////////////////
	/// 19 Inter-SYNTHESE Module namespace.
	/// @author Hugues Romain
	/// @ingroup m19
	namespace inter_synthese
	{
		class InterSYNTHESEContent;
		class InterSYNTHESEPackage;
		class InterSYNTHESESlave;

		/**	@defgroup m19Actions 19.15 Actions
			@ingroup m19

			@defgroup m19Functions 19.15 Functions
			@ingroup m19

			@defgroup m19Exceptions 19.01 Exceptions
			@ingroup m19

			@defgroup m19Alarm 19.17 Messages recipient
			@ingroup m19

			@defgroup m19LS 19.10 Table synchronizers
			@ingroup m19

			@defgroup m19Rights 19.12 Rights
			@ingroup m19

			@defgroup m19Logs 19.13 DB Logs
			@ingroup m19

			@defgroup m19 19 Inter SYNTHESE
			@ingroup m1

			@{
		*/

		//////////////////////////////////////////////////////////////////////////
		/// 19 Inter SYNTHESE Module class.
		///	@author Hugues Romain
		class InterSYNTHESEModule:
			public server::ModuleClassTemplate<InterSYNTHESEModule>
		{
		public:
			static const std::string MODULE_PARAM_INTER_SYNTHESE_MASTER_HOST;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_MASTER_PORT;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_WAITING_TIME;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_SLAVE_ACTIVE;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_SLAVE_ID;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_PASSIVE_IMPORT_ID;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_SLAVE_TO_MASTER_IP;
			static const std::string MODULE_PARAM_INTER_SYNTHESE_POST_INSTALL_TABLES;
			static const util::RegistryKeyType FAKE_IMPORT_ID;

			typedef std::map<std::string, InterSYNTHESEPackage*> PackagesBySmartURL;

		private:
			static boost::posix_time::time_duration _syncWaitingTime;
			static std::string _masterHost;
			static std::string _masterPort;
			static bool _slaveActive;
			static bool _postInstall;
			static util::RegistryKeyType _slaveId;
			static util::RegistryKeyType _postInstallPassiveImportId;
			static std::string _postInstallSlaveToMasterIp;
			static std::string _postInstallTables;
			static PackagesBySmartURL _packagesBySmartURL;
			static bool _passiveSlaveUpdaterSelector(const InterSYNTHESESlave& object);

		public:
			static void Enqueue(
				const InterSYNTHESEContent& content,
				boost::optional<db::DBTransaction&> transaction,
				util::Registrable* objectToRemember = NULL
			);

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

			static InterSYNTHESEPackage* GetPackageBySmartURL(
				const std::string& smartURL
			);

			static void AddPackage(
				InterSYNTHESEPackage& package
			);

			static void RemovePackage(
				const std::string& smartURL
			);

			static void QueueCleaner();

			static void PassiveSlavesUpdater();
			static void GenerateFakeImport();
			static void PostInstall();
		};

		/** @} */
	}
}

#endif // SYNTHESE_InterSYNTHESEModule_H__
