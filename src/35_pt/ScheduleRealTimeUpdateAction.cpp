
/** ScheduleRealTimeUpdateAction class implementation.
	@file ScheduleRealTimeUpdateAction.cpp
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
#include "ScheduleRealTimeUpdateAction.h"
#include "Request.h"
#include "Env.h"
#include "ScheduledService.h"
#include "LineStop.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace graph;
	using namespace env;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ScheduleRealTimeUpdateAction>::FACTORY_KEY("ScheduleRealTimeUpdateAction");
	}

	namespace pt
	{
		const string ScheduleRealTimeUpdateAction::PARAMETER_LATE_DURATION_MINUTES = Action_PARAMETER_PREFIX + "la";
		const string ScheduleRealTimeUpdateAction::PARAMETER_LINE_STOP_RANK = Action_PARAMETER_PREFIX + "ls";
		const string ScheduleRealTimeUpdateAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "se";
		const string ScheduleRealTimeUpdateAction::PARAMETER_AT_ARRIVAL = Action_PARAMETER_PREFIX + "aa";
		const string ScheduleRealTimeUpdateAction::PARAMETER_AT_DEPARTURE = Action_PARAMETER_PREFIX + "ad";
		const string ScheduleRealTimeUpdateAction::PARAMETER_PROPAGATE_CONSTANTLY = Action_PARAMETER_PREFIX + "pc";


	
		ParametersMap ScheduleRealTimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			map.insert(PARAMETER_LINE_STOP_RANK, static_cast<int>(_lineStopRank));
			if(!_lateDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_LATE_DURATION_MINUTES, _lateDuration.total_seconds() / 60);
			}
			map.insert(PARAMETER_AT_ARRIVAL, _atArrival);
			map.insert(PARAMETER_AT_DEPARTURE, _atDeparture);
			map.insert(PARAMETER_PROPAGATE_CONSTANTLY, _propagateConstantly);
			return map;
		}
		
		
		
		void ScheduleRealTimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			_lateDuration = posix_time::minutes(map.get<int>(PARAMETER_LATE_DURATION_MINUTES));

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

			_lineStopRank = map.get<size_t>(PARAMETER_LINE_STOP_RANK);

			if(_lineStopRank >= _service->getArrivalSchedules(false).size())
			{
				throw ActionException("Inconsistent linestop rank");
			}

			_atArrival = map.getDefault<bool>(PARAMETER_AT_ARRIVAL, true);
			_atDeparture = map.getDefault<bool>(PARAMETER_AT_DEPARTURE, true);
			_propagateConstantly = map.getDefault<bool>(PARAMETER_PROPAGATE_CONSTANTLY, true);
		}
		
		
		
		void ScheduleRealTimeUpdateAction::run(Request& request)
		{
			_service->applyRealTimeLateDuration(
				_lineStopRank,
				_lateDuration,
				_atArrival,
				_atDeparture,
				_propagateConstantly
			);
		}
		
		
		
		bool ScheduleRealTimeUpdateAction::isAuthorized(const Session* session
		) const {
			return true;
		}



		void ScheduleRealTimeUpdateAction::setService(
			boost::shared_ptr<const ScheduledService> service
		){
			_service = const_pointer_cast<ScheduledService>(service);
		}



		void ScheduleRealTimeUpdateAction::setLineStopRank( std::size_t value )
		{
			_lineStopRank = value;
		}



		void ScheduleRealTimeUpdateAction::setAtArrival( bool value )
		{
			_atArrival = value;
		}



		void ScheduleRealTimeUpdateAction::setAtDeparture( bool value )
		{
			_atDeparture = value;
		}



		void ScheduleRealTimeUpdateAction::setPropagateConstantly( bool value )
		{
			_propagateConstantly = value;
		}



		void ScheduleRealTimeUpdateAction::setDelay( boost::posix_time::time_duration value )
		{
			_lateDuration = value;
		}
	}
}
