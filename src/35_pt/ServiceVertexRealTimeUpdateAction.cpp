
/** ServiceVertexRealTimeUpdateAction class implementation.
	@file ServiceVertexRealTimeUpdateAction.cpp
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "Request.h"
#include "ScheduledService.h"
#include "PhysicalStop.h"
#include "Env.h"
#include "Line.h"
#include "LineStop.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace env;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ServiceVertexRealTimeUpdateAction>::FACTORY_KEY("ServiceVertexRealTimeUpdateAction");
	}

	namespace pt
	{
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string ServiceVertexRealTimeUpdateAction::PARAMETER_STOP_ID = Action_PARAMETER_PREFIX + "st";

		
		
		ParametersMap ServiceVertexRealTimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			if(_physicalStop.get())
			{
				map.insert(PARAMETER_STOP_ID, _physicalStop->getKey());
			}
			return map;
		}
		
		
		
		void ServiceVertexRealTimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_service = Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().getEditable(
					map.get<RegistryKeyType>(PARAMETER_SERVICE_ID)
				);
				_physicalStop = Env::GetOfficialEnv().getEditableRegistry<PhysicalStop>().getEditable(
					map.get<RegistryKeyType>(PARAMETER_STOP_ID)
				);
			}
			catch(ObjectNotFoundException<ScheduledService>)
			{
				throw ActionException("No such service");
			}
			catch(ObjectNotFoundException<PhysicalStop>)
			{
				throw ActionException("No such physical stop");
			}

			_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);

			if(_lineStopRank >= _service->getArrivalSchedules(false).size())
			{
				throw ActionException("Inconsistent linestop rank");
			}

			if(_service->getPath()->getEdge(_lineStopRank)->getHub() != _physicalStop->getHub())
			{
				throw ActionException("Inconsistent physical stop");
			}
		}
		
		
		
		void ServiceVertexRealTimeUpdateAction::run()
		{
			_service->setRealTimeVertex(_lineStopRank, _physicalStop.get());
		}
		
		
		
		bool ServiceVertexRealTimeUpdateAction::_isAuthorized(
		) const {
			return true;
		}



		void ServiceVertexRealTimeUpdateAction::setPhysicalStop( boost::shared_ptr<const env::PhysicalStop> value )
		{
			_physicalStop = value;
		}



		void ServiceVertexRealTimeUpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}



		void ServiceVertexRealTimeUpdateAction::setService( boost::shared_ptr<const env::ScheduledService> service )
		{
			_service = const_pointer_cast<ScheduledService>(service);
		}
	}
}
