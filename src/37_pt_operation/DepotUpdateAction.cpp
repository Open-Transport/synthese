
//////////////////////////////////////////////////////////////////////////
/// DepotUpdateAction class implementation.
/// @file DepotUpdateAction.cpp
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

#include "DepotUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "GlobalRight.h"
#include "Request.h"
#include "DepotTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_operation::DepotUpdateAction>::FACTORY_KEY("DepotUpdateAction");
	}

	namespace pt_operation
	{
		const string DepotUpdateAction::PARAMETER_DEPOT = Action_PARAMETER_PREFIX + "dp";
		const string DepotUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";


		ParametersMap DepotUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_depot.get())
			{
				map.insert(PARAMETER_DEPOT, _depot->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void DepotUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(PARAMETER_DEPOT))
			{
				try
				{
					_depot = DepotTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_DEPOT), *_env);
				}
				catch(ObjectNotFoundException<Depot>&)
				{
					throw ActionException("No such depot");
				}
			}
			else
			{
				_depot.reset(new Depot);
			}

			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			// Data source links
			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void DepotUpdateAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_name)
			{
				_depot->setName(*_name);
			}

			// Data source links
			_doImportableUpdate(*_depot, request);

			DepotTableSync::Save(_depot.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_depot->getKey());
			}
		}



		bool DepotUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<GlobalRight>(WRITE);
		}
	}
}
