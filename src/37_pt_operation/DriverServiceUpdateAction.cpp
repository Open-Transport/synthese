
//////////////////////////////////////////////////////////////////////////
/// DriverServiceUpdateAction class implementation.
/// @file DriverServiceUpdateAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "DriverServiceUpdateAction.hpp"
#include "Request.h"
#include "ImportableAdmin.hpp"
#include "DriverService.hpp"
#include "ImportableTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "ImportableAdmin.hpp"

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_operation::DriverServiceUpdateAction>::FACTORY_KEY("DriverServiceUpdate");
	}

	namespace pt_operation
	{
		const string DriverServiceUpdateAction::PARAMETER_DRIVER_SERVICE_ID = Action_PARAMETER_PREFIX + "driver_service_id";
		const string DriverServiceUpdateAction::PARAMETER_DATE = Action_PARAMETER_PREFIX + "date";
		const string DriverServiceUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";

		
		
		ParametersMap DriverServiceUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_driverService.get())
			{
				map.insert(PARAMETER_DRIVER_SERVICE_ID, _driverService->getKey());
			}
			if(_dataSourceLinks)
			{
				map.insert(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS, ImportableTableSync::SerializeDataSourceLinks(*_dataSourceLinks));
			}
			if(_date)
			{
				map.insert(PARAMETER_DATE, *_date);
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			return map;
		}
		
		
		
		void DriverServiceUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Driver service
			RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_DRIVER_SERVICE_ID, 0));
			if(id > 0) try
			{
				_driverService = DriverServiceTableSync::GetEditable(id, *_env);
			}
			catch(ObjectNotFoundException<DriverService>&)
			{
				throw ActionException("No such driver service");
			}
			else
			{
				_driverService.reset(new DriverService);
			}

			// Creator ID
			if(map.isDefined(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(
					map.get<string>(ImportableAdmin::PARAMETER_DATA_SOURCE_LINKS),
					*_env
				);
			}

			// Date
			if(map.isDefined(PARAMETER_DATE))
			{
				_date = from_simple_string(map.get<string>(PARAMETER_DATE));
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}
		}
		
		
		
		void DriverServiceUpdateAction::run(
			Request& request
		){
//			stringstream text;
//			x::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			// Driver service creation
			if(!_driverService.get())
			{
				_driverService.reset(new DriverService);
			}

			// Data source links
			if(_dataSourceLinks)
			{
				_driverService->setDataSourceLinks(*_dataSourceLinks);
			}

			// Name
			if(_name)
			{
				_driverService->setName(*_name);
			}

			// Date change
			if(_date)
			{
				if(_driverService->isActive(*_date))
				{
					_driverService->setInactive(*_date);
				}
				else
				{
					_driverService->setActive(*_date);
				}
			}

			DriverServiceTableSync::Save(_driverService.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_driverService->getKey());
			}

//			x::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool DriverServiceUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}
	}
}

