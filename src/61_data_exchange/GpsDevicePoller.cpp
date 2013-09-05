
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

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	
	namespace data_exchange
	{	
		boost::shared_ptr<GpsDevicePoller> GpsDevicePoller::_theConnection(new GpsDevicePoller);
		
		GpsDevicePoller::GpsDevicePoller():
			_longitude(0.0),
			_latitude(0.0),
			_bGpsOk(false)
		{}

		void GpsDevicePoller::RunThread()
		{
			gps g;
			_theConnection->_bGpsOk = false;
			double lon=0.0;
			double lat=0.0;
//TODO(JD): move GpsDevicePoller into "FileFormat"
			// Main loop (never ends)
			while(true)
			{
				// get actual GPS location
				if(!_theConnection->_bGpsOk)	// goes into this at least once!
				{
					g.initSocket("127.0.0.1", GPS_POLLER_SOCKET_PORT);
					_theConnection->_bGpsOk = g.enableTalk();
					if(!_theConnection->_bGpsOk)
					{
						// fail to open connection to GOS socket.
						// Add a timeout before to retry
						std::cout << "GpsDevicePoller failed to open GPS connection socket" << std::endl;
						this_thread::sleep(seconds(30));
					}
				}

				if(_theConnection->_bGpsOk){
					// We have a valid gps socket opened. Use it.
					if(g.updateFromGps()){
						//TODO: eventually check value coherency.
						g.getLatLong(lon,lat);
						_theConnection->_longitude=lon;
						_theConnection->_latitude=lat;

						//TODO(JD): remove this log garbage when working fine.
						std::cout << "longitude=" << lon <<std::endl;
						std::cout << "latitude=" << lat <<std::endl;
					}
					//TODO Should we retry to reconnect if updateFromGps fail too much?
				}

				this_thread::sleep(seconds(1));
			}
		}


		void GpsDevicePoller::ParameterCallback(
			const std::string& name,
			const std::string& value )
		{
	
		}

}	}

