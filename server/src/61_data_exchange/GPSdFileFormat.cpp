
/** GPSdFileFormat class implementation.
	@file GPSdFileFormat.cpp

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

#include "GPSdFileFormat.hpp"

#include "Env.h"
#include "Exception.h"
#include "ScheduledService.h"
#include "ServerModule.h"
#include "StopArea.hpp"
#include "StopPointTableSync.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::property_tree;
using boost::asio::ip::tcp;

#define GPSD_NEWLINE "\n"

namespace synthese
{
	using namespace data_exchange;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace db;
	using namespace geos::geom;
	using namespace vehicle;

	namespace util
	{
		template<>
		const string FactorableTemplate<FileFormat, GPSdFileFormat>::FACTORY_KEY = "gpsd";
	}

	namespace data_exchange
	{
		const std::string GPSdFileFormat::Importer_::PARAMETER_ADDRESS = "address";
		const std::string GPSdFileFormat::Importer_::PARAMETER_PORT = "port";



		GPSdFileFormat::Importer_::Importer_(
			util::Env& env,
			const Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			PermanentThreadImporterTemplate<GPSdFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_gpsStatus(OFFLINE),
			_socket(_ios),
			_lastStopPoint(NULL),
			_lastStorage(not_a_date_time)
		{}



		util::ParametersMap GPSdFileFormat::Importer_::getParametersMap() const
		{
			ParametersMap map;

			map.insert(PARAMETER_PORT, _port);
			map.insert(PARAMETER_ADDRESS, _address);

			return map;
		}



		void GPSdFileFormat::Importer_::setFromParametersMap(
			const util::ParametersMap& map,
			bool doImport
		){
			_address = map.getDefault<string>(PARAMETER_ADDRESS, "127.0.0.1");
			_port = map.getDefault<int>(PARAMETER_PORT, 2947);
		}



		void GPSdFileFormat::Importer_::_onStart() const
		{
			_onStop();
			_gpsStatus = WAITING;

  			try
			{	
				tcp::endpoint endpoint(boost::asio::ip::address::from_string(_address), _port);
				_socket.connect(endpoint);

				// Flush "{"class":"VERSION", ... }
				boost::asio::streambuf response;
				boost::asio::read_until(_socket, response, GPSD_NEWLINE);

				// Start gpsd daemon polling
				if(boost::asio::write(_socket, boost::asio::buffer("?WATCH={\"enable\":true}"))>0)
				{
					_gpsStatus = ONLINE;

					boost::asio::streambuf response;
					// flush {"class":"DEVICES"
					boost::asio::read_until(_socket, response, GPSD_NEWLINE);
					// flush {"class":"WATCH",
					boost::asio::read_until(_socket, response, GPSD_NEWLINE);

					return;
				}
				else
				{
					Log::GetInstance().error("GPSdFileFormat failed to open GPS connection socket");
				}
			}
			catch (std::exception const& e)
			{
				Log::GetInstance().error("GPSdFileFormat: " + string(e.what()));
			}	
		}



		void GPSdFileFormat::Importer_::_loop() const
		{
			if(_gpsStatus != ONLINE)
			{
				_onStart();
			}

			if(_gpsStatus != ONLINE)
			{
				return;
			}

			// We have a valid gps socket opened. Use it.
			double lat;
			double lon;
			if(!_updateFromGps(lat, lon))
			{
				_gpsStatus = WAITING;
				return;
			}

			try
			{
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
				pt::StopPointTableSync::SearchResult  sr = pt::StopPointTableSync::SearchByMaxDistance(
					*projectedPoint.get(),
					maxdistance, //distance  to originPoint
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);

				Path::Edges allEdges;
				if(VehicleModule::GetCurrentVehiclePosition().getService())
				{
					allEdges = VehicleModule::GetCurrentVehiclePosition().getService()->getPath()->getEdges();
				}
				
				double lastDistance(0.0);
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
				// if nearestStopPoint differs from previous one, register the time of change of nearestStopPoint
				// This time is used to compute the onboard advance/delay of the vehicle
				if (nearestStopPoint)
				{
					ptime stopPointFoundTime(second_clock::local_time());
					if (!VehicleModule::GetCurrentVehiclePosition().getStopPoint()
							|| (VehicleModule::GetCurrentVehiclePosition().getStopPoint()
								&& (VehicleModule::GetCurrentVehiclePosition().getStopPoint()->getKey() != nearestStopPoint->getKey())
								)
					) {
						VehicleModule::GetCurrentVehiclePosition().setNextStopFoundTime(stopPointFoundTime);
					}
				}
				VehicleModule::GetCurrentVehiclePosition().setStopPoint(nearestStopPoint);
				// FIXME: Distance should be configurable
				if(lastDistance < 20)
				{
					VehicleModule::GetCurrentVehiclePosition().setInStopArea(true);
				} else
				{
					VehicleModule::GetCurrentVehiclePosition().setInStopArea(false);
				}
				if(nearestStopPoint)
				{
					util::Log::GetInstance().debug("GPSdFileFormat : Stop is "+ nearestStopPoint->getCodeBySources() +
												   string(" name=") + nearestStopPoint->getName() +
												   string(" lat=") + boost::lexical_cast<std::string>(lat) +
												   string("lon=") + boost::lexical_cast<std::string>(lon));
				} else {
					util::Log::GetInstance().debug("GPSdFileFormat : No nearest stop found for " +
												   string("lat=") + boost::lexical_cast<std::string>(lat) +
												   string("lon=") + boost::lexical_cast<std::string>(lon));
				}

				// update Vehicle position.
				VehicleModule::GetCurrentVehiclePosition().setGeometry(
					projectedPoint
				);

				// Storage only if the new point is far enough from the last point or if the last update is too old
				ptime now(second_clock::local_time());
				if(	(	_lastStorage.is_not_a_date_time() ||
						now - _lastStorage > seconds(30)
				//	) || (
				//		!_lastPosition ||
				//		_lastPosition->distance(projectedPoint.get()) > 100
					) || (
						(!_lastStopPoint && nearestStopPoint) ||
						(_lastStopPoint && !nearestStopPoint) ||
						(_lastStopPoint && nearestStopPoint && _lastStopPoint != nearestStopPoint)
				)	){
					util::Log::GetInstance().debug("GPSdFileFormat : storage of position.");

					// Store the position in the table if the current vehicle is known
					VehicleModule::StoreCurrentVehiclePosition();

					// Update values for the next check
					_lastStorage = now;
					_lastPosition = projectedPoint;
					_lastStopPoint = nearestStopPoint;
					
					// Update the next stops if service is defined
					if(VehicleModule::GetCurrentVehiclePosition().getService())
					{
						bool found(false);
						size_t rank(0);
						BOOST_FOREACH(const Path::Edges::value_type& edge, allEdges)
						{
							if(edge->getFromVertex() == _lastStopPoint)
							{
								found = true;
								rank = edge->getRankInPath();
								break;
							}
						}
						if(found)
						{
							// Update the next stops
							CurrentJourney::NextStops nextStops;
							{
								BOOST_FOREACH(const Path::Edges::value_type& edge, allEdges)
								{
									if(edge->getRankInPath() > rank &&
										dynamic_cast<StopPoint*>(edge->getFromVertex()))
									{
										NextStop nextStop;
										nextStop.setStop(static_cast<StopPoint*>(edge->getFromVertex()));
										nextStop.setRank(edge->getRankInPath());
										nextStops.push_back(nextStop);
									}
								}
							}
							VehicleModule::GetCurrentJourney().setNextStops(nextStops);
						}
					}
				}
			}
			catch(bad_lexical_cast&)
			{
				Log::GetInstance().error("GPSdFileFormat: Bad cast in JSON properties reading. Current position was not updated by GPSd import");
				return;
			}
		}



		boost::posix_time::time_duration GPSdFileFormat::Importer_::_getWaitingTime() const
		{
			if(_gpsStatus == WAITING)
			{
				return seconds(10);
			}

			return seconds(10);
		}



		void GPSdFileFormat::Importer_::_onStop() const
		{
			_socket.close();
			_gpsStatus = OFFLINE;
		}



		bool GPSdFileFormat::Importer_::_updateFromGps(
			double &lat,
			double &lon
		) const {
			try
			{
				// Request a POLL to gpsd
				if(boost::asio::write(_socket, boost::asio::buffer("?POLL;")) == 0)
				{
					Log::GetInstance().error("GPSdFileFormat failed to request a poll");
					return false;
				}

				boost::asio::streambuf response;
				boost::asio::read_until(_socket, response, GPSD_NEWLINE);
				if(_loadPositionJSON(response, lat, lon))
				{
					return true;
				}
			}
			catch (std::exception const& e)
			{
				Log::GetInstance().error("GPSdFileFormat updateFromGps: " + string(e.what()));
			}
			return false;
		}


		bool GPSdFileFormat::Importer_::_loadPositionJSON(boost::asio::streambuf &ss,
			double &lat,
			double &lon) const {

			std::string token;
			std::istream str(&ss); 

			while(std::getline(str, token))
			{
				try
				{	
					boost::property_tree::ptree pt;
					istringstream iss(token);
					boost::property_tree::json_parser::read_json(iss, pt);

					// The returned data looks like this:
					// {"class":"POLL","time":"2012-04-05T15:00:01.501Z","active":1,
					//	"tpv":[
					//	{"class":"TPV","tag":"MID7","device":"/dev/ttyUSB0","mode":3,"time":"2012-04-05T15:00:00.000Z",
					//		"ept":0.005,"lat":40.035083522,"lon":-75.519982905,"alt":166.145,"epx":9.125,"epy":17.778,
					//		"epv":34.134,"track":0.0000,"speed":0.000,"climb":0.000,"eps":36.61}],
					//   ...

					if(pt.get<string>("class") == "POLL")
					{
						// We assume there is a single answer in tpv[]
						// In a multi device configuration we should find our one in the
						// device property.
						if(!pt.get_child("tpv").empty())
						{
							lat = pt.get_child("tpv").front().second.get<double>("lat");
							lon = pt.get_child("tpv").front().second.get<double>("lon");
							// gpsd mode 1 means it has no valid data
							if(pt.get_child("mode").front().second.get<int>("mode") == 1)
							{
								Log::GetInstance().error("GPSdFileFormat invalid position");
								return false;
							}

							return true;
						}
						else
						{
							Log::GetInstance().error("GPSdFileFormat got no tpv position");
							return false;
						}

					}
				}
				catch (std::exception const& e)
				{
					Log::GetInstance().error("GPSdFileFormat failed to parse json: " + string(e.what()));
				}
			} 
			
			Log::GetInstance().error("GPSdFileFormat failed to parse json");
			return false;
		}
}	}
