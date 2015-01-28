
/** GPSSimuFileFormat class implementation.
	@file GPSSimuFileFormat.cpp

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

#include "GPSSimuFileFormat.hpp"

#include "Env.h"
#include "Exception.h"
#include "ScheduledService.h"
#include "ServerModule.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "StopArea.hpp"
#include "StopPointTableSync.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"

#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <geos/geom/LineString.h>
#include "Log.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::property_tree;

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
		const string FactorableTemplate<FileFormat, GPSSimuFileFormat>::FACTORY_KEY = "gpssimu";
	}

	namespace data_exchange
	{
		const std::string GPSSimuFileFormat::Importer_::PARAMETER_ROUTE_ID = "route_id";
		const std::string GPSSimuFileFormat::Importer_::PARAMETER_TIME_FOR_ROUTE = "time_for_route";


		GPSSimuFileFormat::Importer_::Importer_(
			util::Env& env,
			const Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			PermanentThreadImporterTemplate<GPSSimuFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_journeyDuration(0),
			_lastStopPoint(NULL),
			_lastStorage(not_a_date_time),
			_startTime(not_a_date_time)
		{}



		util::ParametersMap GPSSimuFileFormat::Importer_::getParametersMap() const
		{
			ParametersMap map;

			map.insert(PARAMETER_ROUTE_ID, _journeyPattern->getKey());
			map.insert(PARAMETER_TIME_FOR_ROUTE, _journeyDuration);

			return map;
		}



		void GPSSimuFileFormat::Importer_::setFromParametersMap(
			const util::ParametersMap& map,
			bool doImport
		){
			if(map.isDefined(PARAMETER_ROUTE_ID)) try
			{
				_journeyPattern = JourneyPatternTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROUTE_ID), _env);
				LineStopTableSync::Search(_env, _journeyPattern->getKey());
			}
			catch (ObjectNotFoundException<JourneyPattern>&)
			{
				_logError("Route not found");
			}

			_journeyDuration = map.getDefault<long>(PARAMETER_TIME_FOR_ROUTE, 600);
		}

		boost::posix_time::time_duration GPSSimuFileFormat::Importer_::_getWaitingTime() const
		{
			return seconds(1);
		}

		void GPSSimuFileFormat::Importer_::_onStart() const
		{
			ptime now(second_clock::local_time());
			_startTime = now;
		}

		void GPSSimuFileFormat::Importer_::_onStop() const
		{
		}

		void GPSSimuFileFormat::Importer_::_loop() const
		{
			if (_startTime.is_not_a_date_time())
			{
				ptime now(second_clock::local_time());
				_startTime = now;
			}

			// Simulate GPS
			double lat;
			double lon;
			if(!_simuGps(lat, lon))
			{
				_logError("Error while simulating GPS");
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
                ptime stopPointFoundTime(second_clock::local_time());
                if (!VehicleModule::GetCurrentVehiclePosition().getStopPoint()
                        || (VehicleModule::GetCurrentVehiclePosition().getStopPoint()
                            && (VehicleModule::GetCurrentVehiclePosition().getStopPoint()->getKey() != nearestStopPoint->getKey())
                            )
                ) {
                    VehicleModule::GetCurrentVehiclePosition().setNextStopFoundTime(stopPointFoundTime);
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
					util::Log::GetInstance().debug("GPSdFileFormat : Stop is "+ nearestStopPoint->getCodeBySources());
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
				_logWarning("Bad cast in GPS Simulator");
				return;
			}
		}

		bool GPSSimuFileFormat::Importer_::_simuGps(
			double &lat,
			double &lon
		) const {
			// Simulate GPS position by
			// ((length of route / scheduled time) * ((now - startTime)%scheduled time)) = position on the route

			if (!_journeyPattern)
			{
				return false;
			}

			boost::shared_ptr<LineString> lineString(_journeyPattern->getGeometry());
			double totalDistance(lineString->getLength());

			ptime now(second_clock::local_time());
			time_duration elapsedTime(now - _startTime);

			size_t elapsedSeconds = elapsedTime.total_seconds() % _journeyDuration;

			double currentDistance = (totalDistance / _journeyDuration) * (double)(elapsedSeconds);

			// Détermination du point sur la course
			CoordinateSequence *coords = lineString->getCoordinates();
			int nbPoints = coords->getSize();
            size_t numStartPoint = (size_t)((currentDistance / totalDistance) * (double)(nbPoints));

			double x1 = coords->getX(numStartPoint);
			double y1 = coords->getY(numStartPoint);
			double x2 = 0.0;
			double y2 = 0.0;
			if (numStartPoint == (nbPoints - 1))
			{
				x2 = coords->getX(numStartPoint - 1);
				y2 = coords->getY(numStartPoint - 1);
			}
			else
			{
				x2 = coords->getX(numStartPoint + 1);
				y2 = coords->getY(numStartPoint + 1);
			}

			double distanceBetweenPoints = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
			double smallDistance = distanceBetweenPoints * (double)(elapsedSeconds - (_journeyDuration / (double)(nbPoints - 1)) * numStartPoint) / (_journeyDuration / (double)(nbPoints - 1));

			double x3 = ((smallDistance < 0.0 ? 0.0 : smallDistance) / distanceBetweenPoints) * (x2 - x1) + x1;
			double y3 = ((smallDistance < 0.0 ? 0.0 : smallDistance) / distanceBetweenPoints) * (y2 - y1) + y1;

			// Create the coordinate point
            boost::shared_ptr<Point> point(
				CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(x3,y3)
            );
			boost::shared_ptr<Point> projectedPoint(
				CoordinatesSystem::GetCoordinatesSystem(4326/*TODO: found const*/).convertPoint(*point)
			);
			lon = projectedPoint->getX();
			lat = projectedPoint->getY();

			return true;

		}
}	}
