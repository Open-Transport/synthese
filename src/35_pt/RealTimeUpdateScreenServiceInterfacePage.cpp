
/** RealTimeUpdateScreenServiceInterfacePage class implementation.
	@file RealTimeUpdateScreenServiceInterfacePage.cpp
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

#include "RealTimeUpdateScreenServiceInterfacePage.h"
#include "ScheduledService.h"
#include "LineStop.h"
#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "Line.h"
#include "CommercialLine.h"
#include "RollingStock.h"
#include "Schedule.h"
#include "ActionFunctionRequest.h"
#include "ScheduleRealTimeUpdateAction.h"
#include "RealTimeUpdateFunction.h"
#include "Interface.h"
#include "ServiceVertexRealTimeUpdateAction.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, pt::RealTimeUpdateScreenServiceInterfacePage>::FACTORY_KEY("real_time_service");
	}

	namespace pt
	{
		RealTimeUpdateScreenServiceInterfacePage::RealTimeUpdateScreenServiceInterfacePage()
			: FactorableTemplate<interfaces::InterfacePage, RealTimeUpdateScreenServiceInterfacePage>(),
			Registrable(UNKNOWN_VALUE)
		{
		}
		
		

		void RealTimeUpdateScreenServiceInterfacePage::display(
			std::ostream& stream,
			const ScheduledService& service,
			const LineStop& lineStop,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			pv.push_back(lineStop.getPhysicalStop()->getConnectionPlace()->getFullName()); //0
			
			pv.push_back(lineStop.getLine()->getCommercialLine()->getStyle()); //1
			pv.push_back(lineStop.getLine()->getCommercialLine()->getShortName()); //2
			pv.push_back(lineStop.getLine()->getCommercialLine()->getImage()); //3

			pv.push_back(service.getServiceNumber()); //4

			pv.push_back(
				lineStop.getLine()->getDestination()->getConnectionPlace()->getName26().empty() ?
				lineStop.getLine()->getDestination()->getConnectionPlace()->getFullName() :
				lineStop.getLine()->getDestination()->getConnectionPlace()->getName26()
			); //5

			pv.push_back(static_cast<const PhysicalStop*>(service.getRealTimeVertex(lineStop.getRankInPath()))->getName()); //6

			pv.push_back(
				lineStop.getLine()->getRollingStock() ?
				lexical_cast<string>(lineStop.getLine()->getRollingStock()->getKey()) :
				string()
			); //7

			pv.push_back(service.getDepartureSchedule(false, lineStop.getRankInPath()).getHour().toString()); //8
			pv.push_back(service.getDepartureSchedule(true, lineStop.getRankInPath()).getHour().toString()); //9
			pv.push_back(
				lexical_cast<string>(service.getDepartureSchedule(true, lineStop.getRankInPath()) - service.getDepartureSchedule(false, lineStop.getRankInPath()))
			); //10

			ActionFunctionRequest<ScheduleRealTimeUpdateAction,RealTimeUpdateFunction> scheduleUpdateRequest(request);
			scheduleUpdateRequest.getAction()->setService(Env::GetOfficialEnv().getSPtr(&service));
			scheduleUpdateRequest.getAction()->setLineStopRank(lineStop.getRankInPath());
			scheduleUpdateRequest.getAction()->setAtArrival(false);
			scheduleUpdateRequest.getAction()->setAtDeparture(true);
			scheduleUpdateRequest.getAction()->setPropagateConstantly(true);
			scheduleUpdateRequest.getAction()->setDelay(posix_time::not_a_date_time);
			scheduleUpdateRequest.getFunction()->setService(Env::GetOfficialEnv().getSPtr(&service));
			scheduleUpdateRequest.getFunction()->setLineStopRank(lineStop.getRankInPath());
			scheduleUpdateRequest.getFunction()->setInterface(Env::GetOfficialEnv().getSPtr(getInterface()));
			pv.push_back(
				scheduleUpdateRequest.getURL() + Request::PARAMETER_SEPARATOR + ScheduleRealTimeUpdateAction::PARAMETER_LATE_DURATION_MINUTES + Request::PARAMETER_ASSIGNMENT
			); //11

			ActionFunctionRequest<ServiceVertexRealTimeUpdateAction,RealTimeUpdateFunction> vertexUpdateRequest(request);
			vertexUpdateRequest.getAction()->setService(Env::GetOfficialEnv().getSPtr(&service));
			vertexUpdateRequest.getAction()->setLineStopRank(lineStop.getRankInPath());
			vertexUpdateRequest.getFunction()->setService(Env::GetOfficialEnv().getSPtr(&service));
			vertexUpdateRequest.getFunction()->setLineStopRank(lineStop.getRankInPath());
			vertexUpdateRequest.getFunction()->setInterface(Env::GetOfficialEnv().getSPtr(getInterface()));
			pv.push_back(
				vertexUpdateRequest.getURL() + Request::PARAMETER_SEPARATOR + ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID + Request::PARAMETER_ASSIGNMENT
			); //12


			InterfacePage::_display(
				stream
				, pv
				, variables
				, static_cast<const void*>(&service)
				, request
			);
		}
	}
}
