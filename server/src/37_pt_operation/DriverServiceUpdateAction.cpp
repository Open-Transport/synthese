
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

#include "DriverServiceUpdateAction.hpp"

#include "ActionException.h"
#include "DriverService.hpp"
#include "DriverServiceTableSync.hpp"
#include "OperationUnit.hpp"
#include "ParametersMap.h"
#include "Request.h"

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
		const string DriverServiceUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "name";



		ParametersMap DriverServiceUpdateAction::getParametersMap() const
		{
			ParametersMap map;

			// Driver service
			if(_driverService.get())
			{
				map.insert(PARAMETER_DRIVER_SERVICE_ID, _driverService->getKey());
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			// Dates
			_getCalendarUpdateParametersMap(map);

			// Name
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
			_setImportableUpdateFromParametersMap(*_env, map);

			// Date
			_setCalendarUpdateFromParametersMap(*_env, map);

			// Name
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
			_doImportableUpdate(*_driverService, request);

			// Name
			if(_name)
			{
				_driverService->setName(*_name);
			}

			// Date change
			_doCalendarUpdate(*_driverService, request);

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

