
//////////////////////////////////////////////////////////////////////////////////////////
///	ServicesListService class implementation.
///	@file ServicesListService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "ServicesListService.hpp"

#include "CommercialLine.h"
#include "Hub.h"
#include "JourneyPatternCopy.hpp"
#include "NamedPlace.h"
#include "RequestException.h"
#include "Request.h"
#include "ScheduledService.h"
#include "Vertex.h"

using namespace boost;
using namespace boost::gregorian;
using namespace boost::logic;
using namespace std;

namespace synthese
{
	using namespace cms;
	using namespace geography;
	using namespace graph;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>,pt::ServicesListService>::FACTORY_KEY = "services_list";
	
	namespace pt
	{
		const string ServicesListService::PARAMETER_PAGE_ID = "p";
		const string ServicesListService::PARAMETER_WAYBACK = "wayback";
		
		const string ServicesListService::DATA_ID = "id";
		const string ServicesListService::DATA_DEPARTURE_SCHEDULE = "departure_schedule";
		const string ServicesListService::DATA_DEPARTURE_PLACE_NAME = "departure_place_name";
		const string ServicesListService::DATA_ARRIVAL_PLACE_NAME = "arrival_place_name";
		const string ServicesListService::DATA_RUNS_AT_DATE = "runs_at_date";
		const string ServicesListService::DATA_SERVICE = "service";


		ParametersMap ServicesListService::_getParametersMap() const
		{
			ParametersMap map(
				FunctionWithSite<false>::_getParametersMap()
			);
			return map;
		}



		void ServicesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Site
			FunctionWithSite<false>::_setFromParametersMap(map);

			// Commercial line
			try
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such line");
			}

			// Wayback
			_wayBack = tribool(indeterminate);
			if(!map.getDefault<string>(PARAMETER_WAYBACK).empty())
			{
				_wayBack = map.getDefault<bool>(PARAMETER_WAYBACK, false);
			}

			// Template
			if(map.isDefined(PARAMETER_PAGE_ID))
			{
				_page = FunctionWithSite<false>::getPage(PARAMETER_PAGE_ID);
			}
		}



		ParametersMap ServicesListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			ServiceSet result;

			// Loop on routes
			BOOST_FOREACH(Path* path, _line->getPaths())
			{
				// Jump over non regular paths
				if(!dynamic_cast<JourneyPattern*>(path))
				{
					continue;
				}
				const JourneyPattern& journeyPattern(
					static_cast<JourneyPattern&>(
						*path
				)	);

				// Applies wayback filter
				if(!indeterminate(_wayBack) && journeyPattern.getWayBack() != _wayBack)
				{
					continue;
				}

				// Gets all services
				BOOST_FOREACH(Service* service, journeyPattern.getServices())
				{
					result.insert(service);
				}

				// Sub journey pattern
				BOOST_FOREACH(JourneyPatternCopy* subPath, journeyPattern.getSubLines())
				{
					BOOST_FOREACH(Service* service, subPath->getServices())
					{
						result.insert(service);
					}
				}
			}

			// The map is filled
			BOOST_FOREACH(Service* service, result)
			{
				shared_ptr<ParametersMap> serviceMap(new ParametersMap);
				serviceMap->insert(DATA_ID, service->getKey());
				if(!_displayDate.is_not_a_date() && dynamic_cast<NonPermanentService*>(service))
				{
					map.insert(
						DATA_RUNS_AT_DATE,
						dynamic_cast<NonPermanentService*>(service)->isActive(_displayDate)
					);
				}

				if(dynamic_cast<SchedulesBasedService*>(service))
				{
					SchedulesBasedService& sservice(
						dynamic_cast<SchedulesBasedService&>(*service)
					);
					serviceMap->insert(DATA_DEPARTURE_SCHEDULE, to_simple_string(sservice.getDepartureSchedule(false, 0)));
				}
				serviceMap->insert(
					DATA_DEPARTURE_PLACE_NAME,
					dynamic_cast<const NamedPlace*>(
						service->getPath()->getEdge(0)->getFromVertex()->getHub()
					)->getFullName()
				);
				serviceMap->insert(
					DATA_ARRIVAL_PLACE_NAME,
					dynamic_cast<const NamedPlace*>(
						service->getPath()->getLastEdge()->getFromVertex()->getHub()
					)->getFullName()
				);
				map.insert(DATA_SERVICE, serviceMap);
			}

			// CMS output
			if(_page && map.hasSubMaps(DATA_SERVICE))
			{
				BOOST_FOREACH(shared_ptr<ParametersMap> smap, map.getSubMaps(DATA_SERVICE))
				{
					_page->display(stream, request, *smap);
				}
			}

			return map;
		}
		
		
		
		bool ServicesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ServicesListService::getOutputMimeType() const
		{
			return "text/html";
		}



		ServicesListService::ServicesListService():
			_page(NULL),
			_displayDate(not_a_date_time)
		{}
}	}
