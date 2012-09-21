
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
	/// 36 Data exchange module namespace.
	///	@author Hugues Romain
	///	@ingroup m36
	namespace data_exchange
	{
		class VDVClient;

		/**	@defgroup m36Actions 35 Actions
			@ingroup m36

			@defgroup m36Pages 35 Pages
			@ingroup m36

			@defgroup m36Library 35 Interface library
			@ingroup m36

			@defgroup m36Functions 35 Functions
			@ingroup m36

			@defgroup m36LS 35 Table synchronizers
			@ingroup m36

			@defgroup m36Admin 35 Administration pages
			@ingroup m36

			@defgroup m36Rights 35 Rights
			@ingroup m36

			@defgroup m36Logs 35 DB Logs
			@ingroup m36

			@defgroup m36File 35.16 File formats
			@ingroup m36

			@defgroup m36WFS 35.39 WFS types
			@ingroup m36

			@defgroup m36 35 Transport
			@ingroup m3

		@{
		*/

		/** 36 Data exchange module class.
		*/
		class DataExchangeModule:
			public graph::GraphModuleTemplate<DataExchangeModule>
		{
		public:
			typedef std::map<std::string, VDVClient*> VDVClients;

		private:
			static VDVClients _vdvClients;

		public:
			static void AddVDVClient(VDVClient& value);
			static void RemoveVDVClient(const std::string& key);
			static VDVClient& GetVDVClient(const std::string& name);

			static void ClientsPoller();
			static void ServersConnector();
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
