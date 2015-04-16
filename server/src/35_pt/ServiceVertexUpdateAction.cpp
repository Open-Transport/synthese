
/** ServiceVertexUpdateAction class implementation.
	@file ServiceVertexUpdateAction.cpp
	@author Gael Sauvanet
	@date 2012

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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ServiceVertexUpdateAction.hpp"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"
#include "StopPoint.hpp"
#include "Env.h"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "DataSource.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace pt;
	using namespace impex;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceVertexUpdateAction>::FACTORY_KEY("ServiceVertexUpdateAction");
	}

	namespace pt
	{
		const string ServiceVertexUpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string ServiceVertexUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string ServiceVertexUpdateAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "st";



		ServiceVertexUpdateAction::ServiceVertexUpdateAction():
			_lineStopRank(0)
		{}



		ParametersMap ServiceVertexUpdateAction::getParametersMap() const
		{
			ParametersMap map;

			// Service
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}

			// Rank
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));

			// Stop
			if(_physicalStop.get())
			{
				map.insert(PARAMETER_STOP_ID, _physicalStop->getKey());
			}

			return map;
		}



		void ServiceVertexUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().getEditable(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw ActionException("No such service");
			}

			if(map.isDefined(PARAMETER_LINE_STOP_RANK))
			{
				_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);
			}

			if(_lineStopRank >= _service->getArrivalSchedules(true, false).size())
			{
				throw ActionException("Inconsistent linestop rank");
			}

			// Load of the stop or NULL vertex
			RegistryKeyType stopId(map.getDefault<RegistryKeyType>(PARAMETER_STOP_ID, 0));
			if(stopId)
			{
				try
				{
					_physicalStop = Env::GetOfficialEnv().getEditableRegistry<StopPoint>().getEditable(
						stopId
					);
				}
				catch(ObjectNotFoundException<StopPoint>)
				{
					throw ActionException("No such physical stop");
				}
			}

			// Check of the consistence of the loaded data
			if(	_physicalStop.get() &&
				_service->getPath()->getEdge(_lineStopRank)->getHub() != _physicalStop->getHub()
			){
				throw ActionException("Inconsistent physical stop");
			}
		}



		void ServiceVertexUpdateAction::run(Request& request)
		{
			_service->setVertex(_lineStopRank, _physicalStop.get());

			ScheduledServiceTableSync::Save(_service.get());
		}



		bool ServiceVertexUpdateAction::isAuthorized(const Session* session
		) const {
#if 1
			return true;
#else // See https://extranet.rcsmobility.com/issues/16043
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
#endif
		}



		void ServiceVertexUpdateAction::setPhysicalStop( boost::shared_ptr<pt::StopPoint> value )
		{
			_physicalStop = value;
		}



		void ServiceVertexUpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}
}	}
