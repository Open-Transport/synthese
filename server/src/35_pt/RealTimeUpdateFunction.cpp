
/** RealTimeUpdateFunction class implementation.
	@file RealTimeUpdateFunction.cpp
	@author Hugues
	@date 2009

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

#include "RequestException.h"
#include "Request.h"
#include "TransportNetworkRight.h"
#include "RealTimeUpdateFunction.h"
#include "StopArea.hpp"
#include "LineStop.h"
#include "ScheduledService.h"
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
	using namespace cms;
	using namespace graph;

	template<>
	const string FactorableTemplate<Function, RealTimeUpdateFunction>::FACTORY_KEY = "rtu";

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
			FactorableTemplate<Function, RealTimeUpdateFunction>()
		{}



		ParametersMap RealTimeUpdateFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			return map;
		}

		void RealTimeUpdateFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// CMS page
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

			// Service
			try
			{
				_service = Env::GetOfficialEnv().getRegistry<ScheduledService>().get(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw RequestException("No such service");
			}

			// Line stop rank
			_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);
			if(_lineStopRank >= _service->getArrivalSchedules(true, false).size())
			{
				throw RequestException("Inconsistent line stop number");
			}
		}



		util::ParametersMap RealTimeUpdateFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(_cmsTemplate.get())
			{
				_display(stream, request, *_service, *_service->getRoute()->getLineStop(_lineStopRank));
			}

			return util::ParametersMap();
		}



		bool RealTimeUpdateFunction::isAuthorized(const server::Session* session) const
		{
#if 1
			return true;
#else // See https://extranet.rcsmobility.com/issues/16043
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
#endif
		}



		std::string RealTimeUpdateFunction::getOutputMimeType() const
		{
			return
				_cmsTemplate.get() ? _cmsTemplate->getMimeType() : "text/plain"
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
			ParametersMap pm(getTemplateParameters());

			// Current location
			if(lineStop.get<LineNode>() && dynamic_cast<const StopPoint*>(&*lineStop.get<LineNode>()))
			{
				dynamic_cast<const StopPoint*>(&*lineStop.get<LineNode>())->getConnectionPlace()->toParametersMap(
					pm,
					NULL,
					DATA_LOCATION_
				);
			}

			// Destination
			lineStop.get<Line>()->getDestination()->getConnectionPlace()->toParametersMap(
				pm,
				NULL,
				DATA_DESTINATION_
			);

			// Line
			lineStop.get<Line>()->getCommercialLine()->toParametersMap(pm, true);

			// service_number
			pm.insert(DATA_SERVICE_NUMBER, service.getServiceNumber());

			// realtime_quay_name
			pm.insert(DATA_REALTIME_QUAY, static_cast<const StopPoint*>(service.getRealTimeVertex(lineStop.get<RankInPath>()))->getName());

			// transport_mode_id
			if(lineStop.get<Line>()->getRollingStock())
			{
				pm.insert(DATA_TRANSPORT_MODE_ID, lineStop.get<Line>()->getRollingStock()->getKey());
			}

			// planned_schedule
			{
				stringstream s;
				s << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(false, lineStop.get<RankInPath>())).hours() << ":" << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(false, lineStop.get<RankInPath>())).minutes();
				pm.insert(DATA_PLANNED_SCHEDULE, s.str());
			}

			// realtime_schedule
			{
				stringstream s;
				s << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(true, lineStop.get<RankInPath>())).hours() << ":" << setw(2) << setfill('0') << Service::GetTimeOfDay(service.getDepartureSchedule(true, lineStop.get<RankInPath>())).minutes();
				pm.insert(DATA_REALTIME_SCHEDULE, s.str());
			}

			// delay
			pm.insert(
				DATA_DELAY,
				(service.getDepartureSchedule(true, lineStop.get<RankInPath>()) - service.getDepartureSchedule(false, lineStop.get<RankInPath>())).total_seconds() / 60
			);

			// delay_update_url
			StaticActionFunctionRequest<ScheduleRealTimeUpdateAction,WebPageDisplayFunction> scheduleUpdateRequest(request, true);
			scheduleUpdateRequest.getAction()->setService(Env::GetOfficialEnv().getSPtr(&service));
			scheduleUpdateRequest.getAction()->setLineStopRank(lineStop.get<RankInPath>());
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
			vertexUpdateRequest.getAction()->setLineStopRank(lineStop.get<RankInPath>());
			pm.insert(
				DATA_QUAY_UPDATE_URL,
				vertexUpdateRequest.getURL() + URI::PARAMETER_SEPARATOR + ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID + URI::PARAMETER_ASSIGNMENT
			);

			_cmsTemplate->display(stream, request, pm);
		}
}	}
