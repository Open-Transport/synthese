
//////////////////////////////////////////////////////////////////////////
/// GenerateVehicleServiceUsagesAction class implementation.
/// @file GenerateVehicleServiceUsagesAction.cpp
/// @author jdestraz
/// @date 2014
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

#include "GenerateVehicleServiceUsagesAction.hpp"

#include "ActionException.h"
#include "DBTransaction.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "VehicleServiceUsageTableSync.hpp"

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace pt_operation;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, analysis::GenerateVehicleServiceUsagesAction>::FACTORY_KEY = "generate_vehicle_service_usages";

	namespace analysis
	{
		const string GenerateVehicleServiceUsagesAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "_start_date";
		const string GenerateVehicleServiceUsagesAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "_end_date";

		
		
		ParametersMap GenerateVehicleServiceUsagesAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void GenerateVehicleServiceUsagesAction::_setFromParametersMap(const ParametersMap& map)
		{
			_startDate = from_simple_string(map.get<string>(PARAMETER_START_DATE));
			_endDate = from_simple_string(map.get<string>(PARAMETER_END_DATE));

			if(_startDate.is_not_a_date() || _endDate.is_not_a_date())
			{
				throw ActionException("Start and end dates are not defined");
			}

			if(_startDate < _endDate)
			{
				date swap(_startDate);
				_startDate = _endDate;
				_endDate = swap;
			}
		}
		
		
		
		void GenerateVehicleServiceUsagesAction::run(
			Request& request
		){
			DBTransaction transaction;
			BOOST_FOREACH(const VehicleService::Registry::value_type& itService, Env::GetOfficialEnv().getRegistry<VehicleService>())
			{
				VehicleService& vehicleService(*itService.second);

				if(vehicleService.get<Services>().empty())
				{
					continue;
				}

				for(date itDate(_startDate); itDate <= _endDate; itDate += days(1))
				{
					if(!vehicleService.isActive(itDate))
					{
						continue;
					}

					VehicleServiceUsage usage(
						VehicleServiceUsageTableSync::getId(),
						vehicleService,
						itDate
					);
					usage.generate();
					VehicleServiceUsageTableSync::Save(&usage, transaction);
			}	}

			transaction.run();
		}
		
		
		
		bool GenerateVehicleServiceUsagesAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
}	}

