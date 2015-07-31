
/** IneoTerminusModule class implementation.
	@file IneoTerminusModule.cpp

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

#include "IneoTerminusModule.hpp"

#include "IneoTerminusConnection.hpp"
#include "ServerModule.h"

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace ineo_terminus;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,IneoTerminusModule>::FACTORY_KEY = "36_ineo_terminus";
	}


	namespace server
	{
		template<> const string ModuleClassTemplate<IneoTerminusModule>::NAME = "Echange de messages avec Ineo";

		template<> void ModuleClassTemplate<IneoTerminusModule>::PreInit()
		{
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PORT, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_NETWORK, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DATASOURCE, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PASSENGER_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DRIVER_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PPDS_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_GIROUETTE_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONOPASSENGER_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONODRIVER_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONOSTOPPOINT_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVGENERAL_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVLINEAUTO_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
			RegisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVLINEMAN_FAKE_BROADCAST, "0", &IneoTerminusConnection::ParameterCallback);
		}

		template<> void ModuleClassTemplate<IneoTerminusModule>::Init()
		{
			// In the init section in order to read this parameter after the data load (DBModule::Init)
			IneoTerminusConnection::Initialize();
		}

		template<> void ModuleClassTemplate<IneoTerminusModule>::Start()
		{
			// Ineo Terminus connector
			ServerModule::AddThread(&IneoTerminusConnection::RunThread, "IneoTerminusConnector");

			// Ineo Terminus connector message sender
			ServerModule::AddThread(&IneoTerminusConnection::MessageSender, "IneoTerminus Message sender");
		}

		template<> void ModuleClassTemplate<IneoTerminusModule>::End()
		{
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PORT);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_NETWORK);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DATASOURCE);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PASSENGER_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DRIVER_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PPDS_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_GIROUETTE_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONOPASSENGER_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONODRIVER_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONOSTOPPOINT_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVGENERAL_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVLINEAUTO_FAKE_BROADCAST);
			UnregisterParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVLINEMAN_FAKE_BROADCAST);
		}



		template<> void ModuleClassTemplate<IneoTerminusModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<IneoTerminusModule>::CloseThread(
		){
		}
	}

}
