
/** GpsDevicePoller class implementation.
	@file GpsDevicePoller.cpp

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

#include "GpsDevicePoller.hpp"

#include "Env.h"
#include "Exception.h"
#include "Log.h"

#include "gps.h"

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <fstream>


#include "PermanentThread.hpp"
#include "ServerModule.h"

#include <boost/thread.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace server;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Device, GpsDevicePoller>::FACTORY_KEY = "GpsDevice_Poller";
	}

	namespace data_exchange
	{
		const std::string GpsDevicePoller::Poller_::PARAMETER_VALIDATOR_NET_PORT_NUMBER("gps_net_port_number");
		int GpsDevicePoller::Poller_::_NetPortNb=0;

		bool GpsDevicePoller::Poller_::launchPoller(
			) const {

				// Launch the thread and returns true
				ServerModule::AddThread(boost::bind(&GpsDevicePoller::Poller_::startPolling, this), "GpsDevicePoller");

				return true;
		}

		GpsDevicePoller::Poller_::Poller_(
			util::Env& env,
			const server::PermanentThread& permanentThread,
			util::ParametersMap& pm
			): Poller(env, permanentThread, pm)
		{}

		util::ParametersMap GpsDevicePoller::Poller_::getParametersMap() const
		{
			ParametersMap map;

			map.insert(PARAMETER_VALIDATOR_NET_PORT_NUMBER, _NetPortNb);

			return map;
		}

		void GpsDevicePoller::Poller_::setFromParametersMap(const util::ParametersMap& map)
		{
			_NetPortNb = map.getDefault<int>(PARAMETER_VALIDATOR_NET_PORT_NUMBER, GPS_POLLER_SOCKET_PORT);
		}

		void GpsDevicePoller::Poller_::startPolling() const
		{
			bool bGpsOk=false;
			gps g;
			double lon=0.0;
			double lat=0.0;

			Log::GetInstance().info(str(format("GpsDevicePoller: NetPortNumber=%d") % _NetPortNb));

			while (true)
			{

				// get actual GPS location
				if(!bGpsOk)	// goes into this at least once!
				{
					g.initSocket("127.0.0.1", _NetPortNb);
					bGpsOk = g.enableTalk();
					if(!bGpsOk)
					{
						// fail to open connection to GOS socket.
						// Add a timeout before to retry
						util::Log::GetInstance().warn("GpsDevicePoller failed to open GPS connection socket");
						this_thread::sleep(seconds(30));
					}
				}

				if(bGpsOk){
					// We have a valid gps socket opened. Use it.
					if(g.updateFromGps()){
						//TODO: eventually check value coherency.
						g.getLatLong(lon,lat);
					}
					//TODO Should we retry to reconnect if updateFromGps fail too much?
				}

				this_thread::sleep(seconds(1));
			}
		}
	}
}