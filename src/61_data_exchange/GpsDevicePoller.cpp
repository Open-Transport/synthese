
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
#include "ScheduledService.h"

#include "gps.h"

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <fstream>

#include "PermanentThread.hpp"
#include "ServerModule.h"

#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"
#include "StopPointTableSync.hpp"

#include <boost/thread.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace data_exchange;
	using namespace graph;	
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace db;
	using namespace geos::geom;
	using namespace vehicle;

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

			VehicleModule::GetCurrentVehiclePosition().setStatus(VehiclePosition::UNKNOWN_STATUS);
			VehicleModule::GetCurrentJourney().setTerminusDeparture(posix_time::not_a_date_time);

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
lon=6.921083;
lat=47.552028;
//				if(bGpsOk){
					// We have a valid gps socket opened. Use it.
//					if(g.updateFromGps()){
						//TODO: eventually check value coherency.
						double lonOld=lon;
						double latOld=lat;
//						g.getLatLong(lon,lat);
/*DEBUG(JD)
						// GPS position has changed. check if we need to change the stop point
						if(lon!=lonOld || lat!=latOld)
DEBUG(JD)*/
						{	//TODO: we could add longitude and latitude change tolerance

							// Create the coordinate point
							boost::shared_ptr<Point> point(
								CoordinatesSystem::GetCoordinatesSystem(4326/*TODO: found const*/).createPoint(lon,lat)
							);
							boost::shared_ptr<Point> projectedPoint(
								CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*point)
							);

							// Nearest stop point
							StopPoint* nearestStopPoint(NULL);
							double maxdistance(3000.0);
							double lastDistance(0.0);
							pt::StopPointTableSync::SearchResult  sr = pt::StopPointTableSync::SearchByMaxDistance(
								*projectedPoint.get(),
								maxdistance, //distance  to originPoint
								Env::GetOfficialEnv(),
								UP_LINKS_LOAD_LEVEL
							);

							Path::Edges allEdges;
							if(VehicleModule::GetCurrentVehiclePosition().getService())
							{
								allEdges = VehicleModule::GetCurrentVehiclePosition().getService()->getPath()->getAllEdges();
							}
							
							BOOST_FOREACH(boost::shared_ptr<StopPoint> sp, sr)
							{
								// Jump over stops not in the current route
								if(VehicleModule::GetCurrentVehiclePosition().getService())
								{
									bool found(false);
									BOOST_FOREACH(const Path::Edges::value_type& edge, allEdges)
									{
										if(edge->getFromVertex() == sp.get())
										{
											found = true;
											break;
										}
									}
									if(!found)
									{
										continue;
									}
								}

								double dst(sp->getGeometry()->distance(projectedPoint.get()));

								if(!nearestStopPoint || dst < lastDistance)
								{
									lastDistance = dst;
									nearestStopPoint = sp.get();
								}
							}
							VehicleModule::GetCurrentVehiclePosition().setStopPoint(nearestStopPoint);
							

							// update Vehicle position.
							VehicleModule::GetCurrentVehiclePosition().setGeometry(
								projectedPoint
							);
//						}

//					}
					//TODO Should we retry to reconnect if updateFromGps fail too much?
				}

				this_thread::sleep(seconds(1));
			}
		}
	}
}