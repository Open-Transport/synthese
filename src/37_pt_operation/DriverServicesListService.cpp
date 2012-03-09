
/** DriverServicesListService class implementation.
	@file DriverServicesListService.cpp
	@author Hugues Romain
	@date 2011

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

#include "DriverServicesListService.hpp"

#include "DriverService.hpp"
#include "RequestException.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "Webpage.h"
#include "PTOperationModule.hpp"
#include "StopPoint.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace pt;
	using namespace graph;


	template<> const string util::FactorableTemplate<Function,pt_operation::DriverServicesListService>::FACTORY_KEY("DriverServicesListService");

	namespace pt_operation
	{
		const string DriverServicesListService::PARAMETER_DATE("da");
		const string DriverServicesListService::PARAMETER_PAGE_ID("pa");

		const string DriverServicesListService::TAG_SERVICE("vehicleService");

		ParametersMap DriverServicesListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return map;
		}

		void DriverServicesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Date
			if(map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = day_clock::local_day();
			}
			else
			{
				_date = from_simple_string(map.get<string>(PARAMETER_DATE));
			}

			// Composition display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_PAGE_ID, 0));
				if(id > 0)
				{
					_page = Env::GetOfficialEnv().get<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such composition page");
			}
		}



		util::ParametersMap DriverServicesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;
			size_t rank(0);
			BOOST_FOREACH(const DriverService::Registry::value_type& service, Env::GetOfficialEnv().getRegistry<DriverService>())
			{
				if(!service.second->isActive(_date))
				{
					continue;
				}

				shared_ptr<ParametersMap> servicePM(new ParametersMap);
				servicePM->insert("id", service.second->getKey());
			
				BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, service.second->getChunks())
				{
					if(chunk.elements.empty())
					{
						continue;
					}

					shared_ptr<ParametersMap> chunkPM(new ParametersMap);
					chunkPM->insert("start_time", chunk.elements.begin()->service->getDepartureSchedule(false, chunk.elements.begin()->startRank));
					chunkPM->insert("end_time", chunk.elements.rbegin()->service->getArrivalSchedule(false, chunk.elements.rbegin()->endRank));
					const StopPoint* startStopPoint(dynamic_cast<StopPoint*>(chunk.elements.begin()->service->getPath()->getEdge(chunk.elements.begin()->startRank)->getFromVertex()));
					if(startStopPoint)
					{
						chunkPM->insert("start_stop", startStopPoint->getCodeBySources());
					}
					const StopPoint* endStopPoint(dynamic_cast<StopPoint*>(chunk.elements.rbegin()->service->getPath()->getEdge(chunk.elements.begin()->endRank)->getFromVertex()));
					if(endStopPoint)
					{
						chunkPM->insert("end_stop", endStopPoint->getCodeBySources());
					}
					
					BOOST_FOREACH(const DriverService::Chunk::Element& element, chunk.elements)
					{
						shared_ptr<ParametersMap> elementPM(new ParametersMap);
						elementPM->insert("service_id", element.service->getKey());
						elementPM->insert("start_time", element.service->getDepartureSchedule(false, element.startRank));
						elementPM->insert("end_time", element.service->getArrivalSchedule(false, element.endRank));

						const StopPoint* startStopPoint(dynamic_cast<StopPoint*>(element.service->getPath()->getEdge(element.startRank)->getFromVertex()));
						if(startStopPoint)
						{
							elementPM->insert("start_stop", startStopPoint->getCodeBySources());
						}
						const StopPoint* endStopPoint(dynamic_cast<StopPoint*>(element.service->getPath()->getEdge(element.endRank)->getFromVertex()));
						if(endStopPoint)
						{
							elementPM->insert("end_stop", endStopPoint->getCodeBySources());
						}
					}

					servicePM->insert("chunk", chunkPM);
				}

				map.insert(TAG_SERVICE, servicePM);
			}

			if(_page.get())
			{
				BOOST_FOREACH(const shared_ptr<ParametersMap>& servicePM, map.getSubMaps(TAG_SERVICE))
				_page->display(stream, request, *servicePM);
			}

			return map;
		}



		bool DriverServicesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DriverServicesListService::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/html";
		}
}	}
