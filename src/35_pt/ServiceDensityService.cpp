
/** ServiceDensityService class implementation.
	@file ServiceDensityService.cpp
	@author Xavier Raffin
	@date 2013

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Xavier Raffin - RCSmobility <contact@rcsmobility.com>

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

#include "ServiceDensityService.hpp"

#include "MimeTypes.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ServicePointer.h"
#include "StopPoint.hpp"
#include "RollingStock.hpp"
#include "TransportNetwork.h"
#include "Edge.h"
#include "LineStop.h"
#include "SchedulesBasedService.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "City.h"
#include "Webpage.h"

#ifndef UNIX
#include <geos/util/math.h>
#endif
#include <sstream>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace geos::geom;
#ifndef UNIX
using namespace geos::util;
#endif
using namespace boost;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt::ServiceDensityService>::FACTORY_KEY("ServiceDensityService");

	namespace pt
	{
		const string ServiceDensityService::PARAMETER_START_DATE = "start_date";
		const string ServiceDensityService::PARAMETER_END_DATE = "end_date";
		const string ServiceDensityService::PARAMETER_PAGE_ID = "page_id";
		const string ServiceDensityService::PARAMETER_SRID = "srid";
		const string ServiceDensityService::PARAMETER_NETWORK_LIST = "nwl";
		const string ServiceDensityService::PARAMETER_ROLLING_STOCK_LIST = "tml";
		const string ServiceDensityService::PARAMETER_DISPLAY_SERVICE_REACHED = "ds";
		const string ServiceDensityService::PARAMETER_DENSITY_AREA_CENTER_POINT = "center";
		const string ServiceDensityService::PARAMETER_SERVICE_NUMBER = "snb";

		const string ServiceDensityService::MAX_DISTANCE_TO_CENTER_POINT = "distance_to_center";
		const string ServiceDensityService::SERVICE_NUMBER_REACHED = "snbReached";
		const string ServiceDensityService::IS_SERVICE_NUMBER_REACHED = "isSnbReached";

		const string ServiceDensityService::DATA_SERVICE = "service";
		const string ServiceDensityService::DATA_SERVICE_ID = "serviceId";
		const string ServiceDensityService::DATA_SERVICE_DEPARTURE_SCHEDULE = "serviceDepartureDateTime";
		const string ServiceDensityService::DATA_COMMERCIAL_LINE_NAME = "serviceCommercialLineName";
		const string ServiceDensityService::DATA_LINE_NAME = "serviceLineName";
		const string ServiceDensityService::DATA_SERVICE_NETWORK = "serviceNetwork";
		const string ServiceDensityService::DATA_SERVICE_ROLLING_STOCK = "serviceRollingStock";
		const string ServiceDensityService::DATA_SERVICE_RANK = "serviceRank";

		const string ServiceDensityService::DATA_STOP = "stop";
		const string ServiceDensityService::DATA_STOP_POINT_ID = "stopId";
		const string ServiceDensityService::DATA_STOP_POINT_NAME = "stopName";
		const string ServiceDensityService::DATA_STOP_DISTANCE = "stopDistance";
		const string ServiceDensityService::DATA_STOP_DATASOURCE = "stopDataSource";
		const string ServiceDensityService::DATA_STOP_RANK = "stopRank";

		ParametersMap ServiceDensityService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			// Start Date
			if(!_startDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_START_DATE, _startDate);
			}

			// End date
			if(!_endDate.is_not_a_date_time())
			{
				map.insert(PARAMETER_END_DATE, _endDate);
			}

			map.insert(PARAMETER_SERVICE_NUMBER, _serviceNumberToReach);

			return map;
		}



		void ServiceDensityService::_setFromParametersMap(const ParametersMap& map)
		{
			// Max solutions number
			_serviceNumberToReach = map.get<size_t>(PARAMETER_SERVICE_NUMBER);

			// Coordinate system
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(
					PARAMETER_SRID,
					CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID()
			)	);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = time_from_string(map.get<string>(PARAMETER_START_DATE));
			}
			else
			{
				throw RequestException("start_date is mandatory");
			}

			if(!map.getDefault<string>(PARAMETER_DISPLAY_SERVICE_REACHED).empty())
			{
				_displayServices = map.get<bool>(PARAMETER_DISPLAY_SERVICE_REACHED);
			}
			else
			{
				_displayServices = false;
			}

			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
			}
			else
			{
				_endDate = _startDate + hours(1);
			}

			string pointCoords(map.getDefault<string>(PARAMETER_DENSITY_AREA_CENTER_POINT));
			if(!pointCoords.empty())
			{
				vector< string > parsed_coord;
				split(parsed_coord, pointCoords, is_any_of(",; ") );

				if(parsed_coord.size() != 2)
				{
					throw RequestException("Invalid center point coordinates");
				}

				boost::shared_ptr<Point> pt1(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_coord[0]), lexical_cast<double>(parsed_coord[1]))
				);

				_centerPoint = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt1);
			}
			else
			{
				throw RequestException("No center point given");
			}

			// CMS output
			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID)) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}

			// Other output
			if(!_page.get())
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
			}

			AccessParameters::AllowedPathClasses allowedPathClasses = AccessParameters::AllowedPathClasses();
			string rsStr(map.getDefault<string>(PARAMETER_ROLLING_STOCK_LIST));
			try
				{
				if(!rsStr.empty())
				{
					vector<string> rsVect;
					split(rsVect, rsStr, is_any_of(",; "));
					allowedPathClasses.insert(0);
					BOOST_FOREACH(string& rsItem, rsVect)
					{
						allowedPathClasses.insert(lexical_cast<RegistryKeyType>(rsItem));
					}
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Rolling Stock List is unreadable");
			}

			AccessParameters::AllowedNetworks allowedNetworks;
			string nwlStr(map.getDefault<string>(PARAMETER_NETWORK_LIST));
			try
			{
				if(!nwlStr.empty())
				{
					vector<string> nwVect;
					split(nwVect, nwlStr, is_any_of(",; "));
					allowedNetworks.insert(0);
					BOOST_FOREACH(string& nwItem, nwVect)
					{
						allowedNetworks.insert(lexical_cast<RegistryKeyType>(nwItem));
					}
				}
			}
			catch(bad_lexical_cast&)
			{
				throw RequestException("Network List is unreadable");
			}
			_accessParameters = AccessParameters(
				USER_PEDESTRIAN, false, false, 1000, posix_time::minutes(23), 1.111, boost::optional<size_t>(), allowedPathClasses, allowedNetworks
			);
		}



		util::ParametersMap ServiceDensityService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Search for stopPoints
			StopPointSetType stopPointSet;

			BOOST_FOREACH(const Registry<StopPoint>::value_type& stopPoint, Env::GetOfficialEnv().getRegistry<StopPoint>())
			{
				if(stopPoint.second->getGeometry())
				{
					addStop(stopPointSet, *stopPoint.second, _startDate, _endDate);
				}
			}

			// Filling in the result parameters map
			ParametersMap pm;

			size_t nbService = 0;
			size_t nbStop = 0;
			size_t maxDistance = 0;
			bool isServiceNumberReadched = false;
			BOOST_FOREACH(const StopPointSetType::value_type& sp, stopPointSet)
			{
				boost::shared_ptr<ParametersMap> stopMap(new ParametersMap);
				stopMap->insert(DATA_STOP_POINT_ID, sp.getStopPoint()->getKey());
				stopMap->insert(DATA_STOP_POINT_NAME, sp.getStopPoint()->getName());
				stopMap->insert(DATA_STOP_DISTANCE, sp.getDistanceToCenter());
				string dataSourceName;
				if(!sp.getStopPoint()->getDataSourceName().empty())
				{
					dataSourceName = sp.getStopPoint()->getDataSourceName();
				}
				stopMap->insert(DATA_STOP_DATASOURCE,dataSourceName);

				size_t nbServiceInStop = 0;
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, sp.getStopPoint()->getDepartureEdges())
				{
					const LineStop* ls = static_cast<const LineStop*>(edge.second);

					ptime departureDateTime = _startDate;
					// Loop on services
					optional<Edge::DepartureServiceIndex::Value> index;
					while(true)
					{
						ServicePointer servicePointer(
							ls->getNextService(
								_accessParameters,
								departureDateTime,
								_endDate,
								false,
								index,
								false,
								false
						));
						if(!servicePointer.getService())
							break;
						const Service * service = servicePointer.getService();
						++*index;
						departureDateTime = servicePointer.getDepartureDateTime();
						if(sp.getStopPoint()->getKey() != servicePointer.getRealTimeDepartureVertex()->getKey())
							continue;

						const JourneyPattern* journeyPattern = dynamic_cast<const JourneyPattern*>(service->getPath());
						if(!journeyPattern->isCompatibleWith(_accessParameters) ||
							!_accessParameters.isAllowedPathClass
							(
								journeyPattern->getPathClass() ? journeyPattern->getPathClass()->getIdentifier() : 0,
								journeyPattern->getPathNetwork() ? journeyPattern->getPathNetwork()->getIdentifier() : 0
							)
						){
							continue;
						}

						nbService++;
						nbServiceInStop++;
						maxDistance = sp.getDistanceToCenter();
						if(_displayServices)
						{
							boost::shared_ptr<ParametersMap> serviceMap(new ParametersMap);
							serviceMap->insert(DATA_SERVICE_ID, service->getKey());

							const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
							serviceMap->insert(DATA_COMMERCIAL_LINE_NAME, commercialLine->getName());
							serviceMap->insert(DATA_LINE_NAME, journeyPattern->getName());

							// Departure schedule
							if(dynamic_cast<const SchedulesBasedService*>(service))
							{
								const SchedulesBasedService& sservice(
									dynamic_cast<const SchedulesBasedService&>(*service)
								);
								serviceMap->insert(DATA_SERVICE_DEPARTURE_SCHEDULE, to_simple_string(sservice.getDepartureSchedule(false, 0)));
							}
							serviceMap->insert(DATA_SERVICE_NETWORK, journeyPattern->getNetwork()->getName());
							serviceMap->insert(DATA_SERVICE_ROLLING_STOCK, journeyPattern->getRollingStock()->getName());
							serviceMap->insert(DATA_SERVICE_RANK, nbServiceInStop);

							stopMap->insert(DATA_SERVICE, serviceMap);
						}
						if (nbService >= _serviceNumberToReach)
						{
							isServiceNumberReadched = true;
							break;
						}
						
					} //Service pointer loop
					if(isServiceNumberReadched)break;
				} // Edge loop
				if(nbServiceInStop > 0)
				{
					nbStop++;
					stopMap->insert(DATA_STOP_RANK, nbStop);
					pm.insert(DATA_STOP, stopMap);
				}
				if(isServiceNumberReadched)break;
			} // Stop point loop

			pm.insert(MAX_DISTANCE_TO_CENTER_POINT, maxDistance);
			pm.insert(SERVICE_NUMBER_REACHED, nbService);
			pm.insert(IS_SERVICE_NUMBER_REACHED, isServiceNumberReadched);

			// Output
			if(_page.get()) // CMS output
			{
				_page->display(stream, request, pm);
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					"ServiceDensity",
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/35_pt/ServiceDensityService.xsd"
				);
			}

			return pm;
		}



		//Sort stopPoint by distance to bbox,or by code operator.Sort by code operator is applied by default.
		ServiceDensityService::SortableStopPoint::SortableStopPoint(const StopPoint * sp, int distanceToCenter):
			_sp(sp),
			_distanceToCenter(distanceToCenter),
			_opCode(_sp->getCodeBySources())
		{
		}



		bool ServiceDensityService::SortableStopPoint::operator<(SortableStopPoint const &otherStopPoint) const
		{
			if(_distanceToCenter != otherStopPoint.getDistanceToCenter())
			{
				return _distanceToCenter < otherStopPoint.getDistanceToCenter();
			}

			if(_opCode != otherStopPoint.getOpCode()
			){
				return _opCode < otherStopPoint.getOpCode(); 
			}

			return _sp < otherStopPoint._sp;
		}

		std::string ServiceDensityService::SortableStopPoint::getOpCode() const
		{
			return _opCode;
		}


		int ServiceDensityService::SortableStopPoint::getDistanceToCenter() const
		{
			return _distanceToCenter;
		}



		const StopPoint* ServiceDensityService::SortableStopPoint::getStopPoint() const
		{
			return _sp;
		}


		void ServiceDensityService::addStop(
			StopPointSetType & stopPointSet,
			const StopPoint & sp,
			const ptime & startDateTime,
			const ptime & endDateTime
		) const {
			int distanceToCenter = CalcDistanceToCenter(sp);
			SortableStopPoint keySP(&sp,distanceToCenter);

			stopPointSet.insert(keySP);
		}


		bool ServiceDensityService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		string ServiceDensityService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}


		int ServiceDensityService::CalcDistanceToCenter(const StopPoint & stopPoint) const
		{
			int distanceToCenter = 0;

			boost::shared_ptr<Point> gp = stopPoint.getGeometry();

			if(gp.get())
			{
				distanceToCenter = sqrt((gp->getX() - _centerPoint->getX()) * (gp->getX() - _centerPoint->getX())
					+ (gp->getY() - _centerPoint->getY()) * (gp->getY() - _centerPoint->getY()));
			}

			return round(distanceToCenter);
		}

}	}
