
/** ServiceVertexRealTimeUpdateAction class implementation.
	@file ServiceVertexRealTimeUpdateAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "TransportNetworkRight.h"
#include "Request.h"
#include "ScheduledService.h"
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
		template<> const string FactorableTemplate<Action, pt::ServiceVertexRealTimeUpdateAction>::FACTORY_KEY("ServiceVertexRealTimeUpdateAction");
	}

	namespace pt
	{
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_LINE_STOP_METRIC_OFFSET = Action_PARAMETER_PREFIX + "line_stop_metric_offset";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "datasource_id";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "st";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_PROPAGATE = Action_PARAMETER_PREFIX + "propagate";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_RESTORE_PLANNED_STOP = Action_PARAMETER_PREFIX + "restore_planned_stop";



		ServiceVertexRealTimeUpdateAction::ServiceVertexRealTimeUpdateAction():
			_lineStopRank(0),
			_propagate(true),
			_restorePlannedStop(false)
		{}



		ParametersMap ServiceVertexRealTimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;

			// Service
			if(_service.get())
			{
				if(_dataSource.get())
				{
					map.insert(PARAMETER_DATASOURCE_ID, _dataSource->getKey());
					map.insert(PARAMETER_SERVICE_ID, _serviceCodeBySource);
				}
				else
				{
					map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				}
			}

			// Rank
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));

			// Stop
			if(_physicalStop.get())
			{
				map.insert(PARAMETER_STOP_ID, _physicalStop->getKey());
			}

			// Propagate
			map.insert(PARAMETER_PROPAGATE, _propagate);

			// Restore planned stop
			map.insert(PARAMETER_RESTORE_PLANNED_STOP, _restorePlannedStop);

			return map;
		}



		void ServiceVertexRealTimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.getDefault<RegistryKeyType>(PARAMETER_DATASOURCE_ID, 0))
			{
				try
				{
					boost::shared_ptr<const DataSource> dataSource(
						Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID))
					);
					_serviceCodeBySource = map.get<string>(PARAMETER_SERVICE_ID);
					ScheduledService* obj(dataSource->getObjectByCode<ScheduledService>(_serviceCodeBySource));
					if(!obj)
					{
						throw ActionException("No such service");
					}
					_service = Env::GetOfficialEnv().getEditableSPtr(obj);
				}
				catch(ObjectNotFoundException<DataSource>&)
				{
					throw ActionException("No such datasource");
				}
			}
			else try
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
			else
			{
				MetricOffset offset(map.get<MetricOffset>(PARAMETER_LINE_STOP_METRIC_OFFSET));
				try
				{
					_lineStopRank = _service->getPath()->getEdgeRankAtOffset(offset);
				}
				catch (Path::InvalidOffsetException& e)
				{
					throw ActionException(e.getMessage());
				}
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

			// Propagate
			_propagate = map.getDefault<bool>(PARAMETER_PROPAGATE, true);

			// Restore planned stop
			_restorePlannedStop = map.getDefault<bool>(PARAMETER_RESTORE_PLANNED_STOP, false);
		}



		void ServiceVertexRealTimeUpdateAction::run(Request& request)
		{
			if(_restorePlannedStop)
			{
				_service->setRealTimeVertex(_lineStopRank, _service->getPath()->getEdge(_lineStopRank)->getFromVertex());

				// Propagation
				if(_propagate)
				{
					for(size_t rank(_lineStopRank+1); rank<_service->getArrivalSchedules(true, false).size(); ++rank)
					{
						_service->setRealTimeVertex(rank, _service->getPath()->getEdge(rank)->getFromVertex());
					}
				}
			}
			else
			{
				_service->setRealTimeVertex(_lineStopRank, _physicalStop.get());

				// Propagation
				if(!_physicalStop.get() && _propagate)
				{
					for(size_t rank(_lineStopRank+1); rank<_service->getArrivalSchedules(true, false).size(); ++rank)
					{
						_service->setRealTimeVertex(rank, NULL);
					}
				}
			}
		}



		bool ServiceVertexRealTimeUpdateAction::isAuthorized(const Session* session
		) const {
#if 1
			return true;
#else // See https://extranet.rcsmobility.com/issues/16043
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE);
#endif
		}



		void ServiceVertexRealTimeUpdateAction::setPhysicalStop( boost::shared_ptr<const pt::StopPoint> value )
		{
			_physicalStop = value;
		}



		void ServiceVertexRealTimeUpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}
}	}
