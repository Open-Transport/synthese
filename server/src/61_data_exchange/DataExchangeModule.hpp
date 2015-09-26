
/** DataExchangeModule class header.
	@file DataExchangeModule.h

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

#ifndef SYNTHESE_DataExchangeModule_H__
#define SYNTHESE_DataExchangeModule_H__


#include "GraphModuleTemplate.h"
#include "SecurityTypes.hpp"
#include "Registry.h"
#include "LexicalMatcher.h"
#include "RoadModule.h"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 61 Data exchange module namespace.
	///	@author Hugues Romain
	///	@ingroup m61
	namespace data_exchange
	{
		class VDVClient;
		class VDVServer;

		/**	@defgroup m61Actions 61 Actions
			@ingroup m61

			@defgroup m61Pages 61 Pages
			@ingroup m61

			@defgroup m61Library 61 Interface library
			@ingroup m61

			@defgroup m61Functions 61 Functions
			@ingroup m61

			@defgroup m61LS 61 Table synchronizers
			@ingroup m61

			@defgroup m61Admin 61 Administration pages
			@ingroup m61

			@defgroup m61Rights 61 Rights
			@ingroup m61

			@defgroup m61Logs 61 DB Logs
			@ingroup m61

			@defgroup m61File 61.16 File formats
			@ingroup m61

			@defgroup m61WFS 61.39 WFS types
			@ingroup m61

			@defgroup m61 61 Transport
			@ingroup m5

		@{
		*/

		/** 61 Data exchange module class.
		*/
		class DataExchangeModule:
			public graph::GraphModuleTemplate<DataExchangeModule>
		{
		public:
#ifdef WITH_61_DATA_EXCHANGE_VDV
			typedef std::map<std::string, VDVClient*> VDVClients;
			typedef std::map<std::string, VDVServer*> VDVServers;

			static const std::string MODULE_PARAM_VDV_SERVER_ACTIVE;
			static const std::string MODULE_PARAM_VDV_CLIENT_ACTIVE;

		private:
			static VDVClients _vdvClients;
			static VDVServers _vdvServers;
			static bool _vdvServerActive;
			static bool _vdvClientActive;
			static boost::posix_time::ptime _vdvStartingTime;

		public:
			static void AddVDVClient(VDVClient& value);
			static void RemoveVDVClient(const std::string& key);
			static VDVClient& GetVDVClient(const std::string& name);
			static const VDVClients& GetVDVClients(){ return _vdvClients; }

			static void AddVDVServer(VDVServer& value);
			static void RemoveVDVServer(const std::string& key);
			static VDVServer& GetVDVServer(const std::string& name);
			static bool GetVDVClientActive() { return _vdvClientActive; }
			static bool GetVDVServerActive() { return _vdvServerActive; }
			static boost::posix_time::ptime& GetVDVStartingTime() { return _vdvStartingTime; }

#endif
		public:
			static void ClientsPoller();
			static void ServersConnector();

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

			static void Init();
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
