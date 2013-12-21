
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
		):	PermanentThreadImporterTemplate<GPSdFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			Importer(env, import, minLogLevel, logPath, outputStream, pm),
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

				// Set the request format
				boost::asio::streambuf request;
				ostream request_stream(&request);
				request_stream << "?WATCH={\"enable\":true,\"json\":true}";
				if(boost::asio::write(_socket, request)>0)
				{
					_gpsStatus = ONLINE;
					return;
				}
				else
				{
					_logWarning("GPSdFileFormat failed to open GPS connection socket");
				}
			}
			catch (std::exception const& e)
			{
				_logWarning(e.what());
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
			optional<ptree> currentPosition;
			_updateFromGps(currentPosition);
			if(!currentPosition)
			{
				_gpsStatus = WAITING;
				return;
			}

			try
			{
				// JSON reading
				double lon(
					lexical_cast<double>(currentPosition->get<string>("lon"))
				);
				double lat(
					lexical_cast<double>(currentPosition->get<string>("lat"))
				);



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
					allEdges = VehicleModule::GetCurrentVehiclePosition().getService()->getPath()->getAllEdges();
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
				VehicleModule::GetCurrentVehiclePosition().setStopPoint(nearestStopPoint);
				util::Log::GetInstance().debug("GPSdFileFormat : Stop is "+ nearestStopPoint->getCodeBySources());

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
					util::Log::GetInstance().debug("GPSdFileFormat : storage of position. Stop is "+ nearestStopPoint->getCodeBySources());

					// Store the position in the table if the current vehicle is known
					VehicleModule::StoreCurrentVehiclePosition();

					// Update values for the next check
					_lastStorage = now;
					_lastPosition = projectedPoint;
					_lastStopPoint = nearestStopPoint;
				}
			}
			catch(bad_lexical_cast&)
			{
				_logWarning("Bad cast in JSON properties reading. Current position was not updated by GPSd import");
				return;
			}
		}



		boost::posix_time::time_duration GPSdFileFormat::Importer_::_getWaitingTime() const
		{
			if(_gpsStatus == WAITING)
			{
				return seconds(10);
			}

			return seconds(1);
		}



		void GPSdFileFormat::Importer_::_onStop() const
		{
			_socket.close();
			_gpsStatus = OFFLINE;
		}



		void GPSdFileFormat::Importer_::_updateFromGps(
			optional<ptree>& result
		) const {
			try
			{
				//TODO: make it loops until we got new data.
				//TODO: should we have a clean timeout and reconnect to the socket when we fail too much?
				//	But this should never fail as today we use a local port on local host.
				for(int i=0;i<100;i++)
				{	// Theoretically should success after two maximum 3 loops
					boost::asio::streambuf response;
					boost::asio::read_until(_socket, response, "\r\n");
					if(_loadPositionJSON(response, result))
					{
						break;
					}
				}
			}
			catch (std::exception const& e)
			{
				_logWarning(e.what());
			}
		}



		bool GPSdFileFormat::Importer_::_loadPositionJSON(
			boost::asio::streambuf &ss,
			optional<ptree>& result
		) const {

			// warning, we could have mode than one JSON at the time.
			// but all separated by a new line.
			std::string token;
			std::istream str(&ss); 

			while(std::getline(str, token))
			{
				try
				{	
					boost::property_tree::ptree pt;
					istringstream iss(token);
					boost::property_tree::json_parser::read_json(iss,pt);
					
					string response(pt.get<string>("class"));
					if(response == "TPV")
					{
						result = pt;
						return true;
					}
				}
				catch (std::exception const& e)
				{
					_logWarning(e.what());
				}
			} 
			
			return false;
		}
}	}
