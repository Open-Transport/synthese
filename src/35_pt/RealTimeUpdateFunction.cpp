
/** RealTimeUpdateFunction class implementation.
	@file RealTimeUpdateFunction.cpp
	@author Hugues
	@date 2009

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "RequestException.h"
#include "Request.h"
#include "TransportNetworkRight.h"
#include "RealTimeUpdateFunction.h"
#include "StopArea.hpp"
#include "LinePhysicalStop.hpp"
#include "ScheduledService.h"
#include "RealTimeUpdateScreenServiceInterfacePage.h"
#include "Interface.h"
#include "JourneyPattern.hpp"
#include "Webpage.h"
#include "ScheduleRealTimeUpdateAction.h"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "StopPoint.hpp"
#include "RollingStock.hpp"
#include "StaticActionFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "CommercialLine.h"
#include "URI.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace interfaces;
	using namespace cms;
	using namespace graph;



	template<> const string util::FactorableTemplate<RequestWithInterface,pt::RealTimeUpdateFunction>::FACTORY_KEY("rtu");

	namespace pt
	{
		const string RealTimeUpdateFunction::PARAMETER_LINE_STOP_RANK("ls");
		const string RealTimeUpdateFunction::PARAMETER_SERVICE_ID("se");
		const string RealTimeUpdateFunction::PARAMETER_CMS_TEMPLATE_ID("t");

		const std::string RealTimeUpdateFunction::DATA_LOCATION_("location_");
		const std::string RealTimeUpdateFunction::DATA_DESTINATION_("destination_");
		const std::string RealTimeUpdateFunction::DATA_SERVICE_NUMBER("service_number");
		const std::string RealTimeUpdateFunction::DATA_REALTIME_QUAY("realtime_quay");
		const std::string RealTimeUpdateFunction::DATA_TRANSPORT_MODE_ID("transport_mode_id");
		const std::string RealTimeUpdateFunction::DATA_PLANNED_SCHEDULE("planned_schedule");
		const std::string RealTimeUpdateFunction::DATA_REALTIME_SCHEDULE("realtime_schedule");
		const std::string RealTimeUpdateFunction::DATA_DELAY("delay");
		const std::string RealTimeUpdateFunction::DATA_DELAY_UPDATE_URL("delay_update_url");
		const std::string RealTimeUpdateFunction::DATA_QUAY_UPDATE_URL("quay_update_url");



		RealTimeUpdateFunction::RealTimeUpdateFunction():
			FactorableTemplate<RequestWithInterface, RealTimeUpdateFunction>()
		{}

		ParametersMap RealTimeUpdateFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			return map;
		}

		void RealTimeUpdateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Interface
				RequestWithInterface::_setFromParametersMap(map);
				if(getInterface() == NULL)
				{
					try
					{
						optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_CMS_TEMPLATE_ID));
						if(id)
						{
							_cmsTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
						}
					}
					catch (ObjectNotFoundException<Webpage>& e)
					{
						throw RequestException("No such CMS template : "+ e.getMessage());
					}
				}
				else if(!getInterface()->hasPage<RealTimeUpdateScreenServiceInterfacePage>())
				{
					throw RequestException("The interface does not implement the Real Time Update Screen");
				}

				_service = Env::GetOfficialEnv().getRegistry<ScheduledService>().get(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
				_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);

				if(_lineStopRank >= _service->getArrivalSchedules(false).size())
				{
					throw RequestException("Inconsistent line stop number");
				}
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw RequestException("No such service");
			}
		}

		void RealTimeUpdateFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(getInterface())
			{
				VariablesMap vm;
				const RealTimeUpdateScreenServiceInterfacePage* page(
					getInterface()->getPage<RealTimeUpdateScreenServiceInterfacePage>()
				);
				page->display(
					stream,
					*_service,
					*_service->getRoute()->getLineStop(_lineStopRank),
					vm,
					&request
				);
			}
			else if(_cmsTemplate.get())
			{
				_display(stream, request, *_service, *_service->getRoute()->getLineStop(_lineStopRank));
			}
		}



		bool RealTimeUpdateFunction::isAuthorized(const server::Session* session) const
		{
			return true;
		}



		std::string RealTimeUpdateFunction::getOutputMimeType() const
		{
			return
				getInterface() ?
				getInterface()->getPage<RealTimeUpdateScreenServiceInterfacePage>()->getMimeType() :
				(_cmsTemplate.get() ? _cmsTemplate->getMimeType() : "text/plain")
			;
		}



		void RealTimeUpdateFunction::setService( boost::shared_ptr<const ScheduledService> value )
		{
			_service = value;
		}



		void RealTimeUpdateFunction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}



		void RealTimeUpdateFunction::_display(
			std::ostream& stream,
			const server::Request& request,
			const ScheduledService& service,
			const LineStop& lineStop
		) const	{
			ParametersMap pm(request.getFunction()->getSavedParameters());

			// Current location
			if(dynamic_cast<const LinePhysicalStop*>(&lineStop))
			{
				dynamic_cast<const LinePhysicalStop&>(lineStop).getPhysicalStop()->getConnectionPlace()->toParametersMap(
					pm,
					NULL,
					DATA_LOCATION_
				);
			}

			// Destination
			lineStop.getLine()->getDestination()->getConnectionPlace()->toParametersMap(
				pm,
				NULL,
				DATA_DESTINATION_
			);

			// Line
			lineStop.getLine()->getCommercialLine()->toParametersMap(pm);

			// service_number
			pm.insert(DATA_SERVICE_NUMBER, service.getServiceNumber());

			// realtime_quay_name
			pm.insert(DATA_REALTIME_QUAY, static_cast<const StopPoint*>(service.getRealTimeVertex(lineStop.getRankInPath()))->getName());

			// transport_mode_id
			if(lineStop.getLine()->getRollingStock())
			{
				pm.insert(DATA_TRANSPORT_MODE_ID, lineStop.getLine()->getRollingStock()->getKey());
			}

			// planned_schedule
			{
				stringstream s;
				s << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(false, lineStop.getRankInPath())).hours() << ":" << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(false, lineStop.getRankInPath())).minutes();
				pm.insert(DATA_PLANNED_SCHEDULE, s.str());
			}

			// realtime_schedule
			{
				stringstream s;
				s << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(true, lineStop.getRankInPath())).hours() << ":" << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(true, lineStop.getRankInPath())).minutes();
				pm.insert(DATA_REALTIME_SCHEDULE, s.str());
			}

			// delay
			pm.insert(
				DATA_DELAY,
				(service.getDepartureSchedule(true, lineStop.getRankInPath()) - service.getDepartureSchedule(false, lineStop.getRankInPath())).total_seconds() / 60
			);

			// delay_update_url
			StaticActionFunctionRequest<ScheduleRealTimeUpdateAction,WebPageDisplayFunction> scheduleUpdateRequest(request, true);
			scheduleUpdateRequest.getAction()->setService(Env::GetOfficialEnv().getSPtr(&service));
			scheduleUpdateRequest.getAction()->setLineStopRank(lineStop.getRankInPath());
			scheduleUpdateRequest.getAction()->setAtArrival(false);
			scheduleUpdateRequest.getAction()->setAtDeparture(true);
			scheduleUpdateRequest.getAction()->setPropagateConstantly(true);
			scheduleUpdateRequest.getAction()->setDelay(posix_time::not_a_date_time);
			pm.insert(
				DATA_DELAY_UPDATE_URL,
				scheduleUpdateRequest.getURL() + URI::PARAMETER_SEPARATOR + ScheduleRealTimeUpdateAction::PARAMETER_LATE_DURATION_MINUTES + URI::PARAMETER_ASSIGNMENT
			);

			// quay_update_url
			StaticActionFunctionRequest<ServiceVertexRealTimeUpdateAction,WebPageDisplayFunction> vertexUpdateRequest(request, true);
			vertexUpdateRequest.getAction()->setService(Env::GetOfficialEnv().getEditableSPtr(const_cast<ScheduledService*>(&service)));
			vertexUpdateRequest.getAction()->setLineStopRank(lineStop.getRankInPath());
			pm.insert(
				DATA_QUAY_UPDATE_URL,
				vertexUpdateRequest.getURL() + URI::PARAMETER_SEPARATOR + ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID + URI::PARAMETER_ASSIGNMENT
			);

			_cmsTemplate->display(stream, request, pm);
		}
}	}
