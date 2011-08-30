
/** DeviateServiceAction class implementation.
	@file DeviateServiceAction.cpp
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
#include "DeviateServiceAction.hpp"
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

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::DeviateServiceAction>::FACTORY_KEY("DeviateService");
	}

	namespace pt
	{
		const string DeviateServiceAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "datasource_id";
		const string DeviateServiceAction::PARAMETER_SERVICE_ID = Action_PARAMETER_PREFIX + "service_id";
		const string DeviateServiceAction::PARAMETER_CHAINAGE = Action_PARAMETER_PREFIX + "chainage";



		DeviateServiceAction::DeviateServiceAction():
			_chainage(0)
		{}



		ParametersMap DeviateServiceAction::getParametersMap() const
		{
			ParametersMap map;
			
			// Service
			if(_service.get())
			{
				if(_dataSource.get())
				{
					map.insert(PARAMETER_DATASOURCE_ID, _dataSource->getKey());
					map.insert(PARAMETER_SERVICE_ID, _service->getCodeBySource(*_dataSource));
				}
				else
				{
					map.insert(PARAMETER_SERVICE_ID, _service->getKey());
				}
			}

			// Chainage
			map.insert(PARAMETER_CHAINAGE, _chainage);

			return map;
		}



		void DeviateServiceAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Load of the service
			if(map.getDefault<RegistryKeyType>(PARAMETER_DATASOURCE_ID, 0))
			{
				try
				{
					shared_ptr<const DataSource> dataSource(
						Env::GetOfficialEnv().getRegistry<DataSource>().get(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID))
					);
					Importable* obj(dataSource->getObjectByCode(map.get<string>(PARAMETER_SERVICE_ID)));
					if(!obj || !dynamic_cast<ScheduledService*>(obj))
					{
						throw ActionException("No such service");
					}
					_service = Env::GetOfficialEnv().getEditableSPtr(static_cast<ScheduledService*>(obj));
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

			// chainage
			_chainage = map.getDefault<size_t>(PARAMETER_CHAINAGE, 0);
		}



		void DeviateServiceAction::run(Request& request)
		{
			//1. Load of the new chainage


			//2. Search one or more existing journey pattern(s) corresponding to the chainage or creation



			//3. Search an existing service in the found journey pattern(s)
		}



		bool DeviateServiceAction::isAuthorized(const Session* session
		) const {
			return true;
		}
}	}
