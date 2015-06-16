//////////////////////////////////////////////////////////////////////////////////////////
///	HardwareInformationService class implementation.
///	@file HardwareInformationService.cpp
///	@author bcoudoin
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

#include "HardwareInformationService.hpp"

#include "Request.h"
#include "ServerModule.h"

#ifndef WIN32
#include <sys/socket.h>
#include <ifaddrs.h>
#endif
#include <stdio.h>
#include <boost/asio.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::HardwareInformationService>::FACTORY_KEY = "hardware_information";

	namespace server
	{
		const string HardwareInformationService::ATTR_HOSTNAME("hostname");
		const string HardwareInformationService::ATTR_NETWORKS("networks");
		const string HardwareInformationService::ATTR_NETWORK_INTERFACE("network_interface");
		const string HardwareInformationService::ATTR_NETWORK_IP("network_ip");
		const string HardwareInformationService::ATTR_NETWORK_STATUS("network_status");
		const string HardwareInformationService::ATTR_NETWORK_BROADCAST("network_broadcast");
		const string HardwareInformationService::ATTR_NETWORK_SUBNETMASK("network_netmask");
		const string HardwareInformationService::ATTR_NETWORK_MAC("network_mac");
		const string HardwareInformationService::ATTR_HARDWARE("hardware");
		const string HardwareInformationService::ATTR_SENSOR("sensor");
		const string HardwareInformationService::ATTR_TEMPERATURES("temperatures");
		const string HardwareInformationService::ATTR_TEMPERATURE("temperature");


		FunctionAPI HardwareInformationService::getAPI() const
		{
			FunctionAPI api(
				"15_server",
				"Returns hardware informations about the server we are running on.",
				"The informations are returned as a parameters map. "
				"It includes the host name and the networking details."
				"Example:\n"
				"<?hardware_information&\n"
				"  template=<{networks&\n"
				"  template=<@@>\n"
				"  }>\n"
				"?>\n"
			);

			return api;
		}


		ParametersMap HardwareInformationService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void HardwareInformationService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap HardwareInformationService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			map.insert(ATTR_HOSTNAME, boost::asio::ip::host_name());
			getNetworks(map);
			return map;
		}

#ifndef WIN32
		const sockaddr_in* HardwareInformationService::castToIP4(const sockaddr* addr) const
		{
			if (addr == NULL) {
				return NULL;
			} else if (addr->sa_family == AF_INET) {
				// An IPv4 address
				return reinterpret_cast<const sockaddr_in*>(addr);
			} else {
				// Not an IPv4 address
				return NULL;
			}
		}
#endif

		void HardwareInformationService::getNetworks(ParametersMap &pm) const
		{
#ifndef WIN32
			struct ifaddrs *ifap, *ifa;

			// Warning, the MAC address and the interfaceAddress come in as
			// different interfaces with the same name.
			// We hold a hash for this reason
			typedef map<string, boost::shared_ptr<ParametersMap> > NetConfigType;
			NetConfigType netConfigs;

			getifaddrs (&ifap);
			for(ifa = ifap; ifa; ifa = ifa->ifa_next)
			{
				if(!ifa->ifa_addr)
				{
					continue;
				}
				const sockaddr_in* interfaceAddress = castToIP4(ifa->ifa_addr);
				const sockaddr_in* broadcastAddress = castToIP4(ifa->ifa_dstaddr);
				const sockaddr_in* subnetMask       = castToIP4(ifa->ifa_netmask);
				string ifName(ifa->ifa_name);

				boost::shared_ptr<ParametersMap> &networkMap = netConfigs[ifName];
				if(!networkMap.get())
				{
					networkMap.reset(new ParametersMap);
				}

				networkMap->insert(ATTR_NETWORK_INTERFACE, ifName);
				networkMap->insert(ATTR_NETWORK_STATUS,
								   (ifa->ifa_flags & IFF_UP) ? string("Online") : string("Down"));

				if(interfaceAddress)
				{
					networkMap->insert(ATTR_NETWORK_IP, string(inet_ntoa(interfaceAddress->sin_addr)));
				}
				if(broadcastAddress)
				{
					networkMap->insert(ATTR_NETWORK_BROADCAST, string(inet_ntoa(broadcastAddress->sin_addr)));
				}
				if(subnetMask)
				{
					networkMap->insert(ATTR_NETWORK_SUBNETMASK, string(inet_ntoa(subnetMask->sin_addr)));
				}

				if (ifa->ifa_addr->sa_family == AF_PACKET)
				{
					struct ifreq ifr;
					int fd = socket(AF_INET, SOCK_DGRAM, 0);

					ifr.ifr_addr.sa_family = AF_INET;
					strcpy(ifr.ifr_name, ifa->ifa_name);
					ioctl(fd, SIOCGIFHWADDR, &ifr);
					close(fd);

					uint8_t* MAC = reinterpret_cast<uint8_t*>(ifr.ifr_hwaddr.sa_data);

					char mac[18];
					sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
					networkMap->insert(ATTR_NETWORK_MAC, string(mac));
				}
			}

			BOOST_FOREACH(NetConfigType::value_type &netConfig, netConfigs)
			{
				pm.insert(ATTR_NETWORKS, netConfig.second);
			}

			freeifaddrs(ifap);
#endif
		}

		bool HardwareInformationService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string HardwareInformationService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
