
//////////////////////////////////////////////////////////////////////////
/// InterfacePageAddAction class implementation.
/// @file InterfacePageAddAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "InterfacePageAddAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "InterfaceRight.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "InterfaceTableSync.h"
#include "InterfacePageTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, interfaces::InterfacePageAddAction>::FACTORY_KEY("InterfacePageAddAction");
	}

	namespace interfaces
	{
		const string InterfacePageAddAction::PARAMETER_CLASS = Action_PARAMETER_PREFIX + "cl";
		const string InterfacePageAddAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "ii";
		const string InterfacePageAddAction::PARAMETER_VARIANT = Action_PARAMETER_PREFIX + "va";



		ParametersMap InterfacePageAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_interface.get())
			{
				map.insert(PARAMETER_INTERFACE_ID, _interface->getKey());
			}
			map.insert(PARAMETER_CLASS, _class);
			map.insert(PARAMETER_VARIANT, _variant);
			return map;
		}



		void InterfacePageAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Interface id
			try
			{
				_interface = InterfaceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_INTERFACE_ID), *_env);
			}
			catch(ObjectNotFoundException<Interface>&)
			{
				throw ActionException("No such interface");
			}

			// Class
			_class = map.get<string>(PARAMETER_CLASS);
			if(!Factory<InterfacePage>::contains(_class))
			{
				throw ActionException("No such class");
			}

			// Variant
			_variant = map.get<string>(PARAMETER_VARIANT);
		}



		void InterfacePageAddAction::run(
			Request& request
		){
			boost::shared_ptr<InterfacePage> object(Factory<InterfacePage>::create(_class));
			object->setInterface(_interface.get());
			object->setPageCode(_variant);
			InterfacePageTableSync::Save(object.get());
			request.setActionCreatedId(object->getKey());
		}



		bool InterfacePageAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<InterfaceRight>(WRITE);
		}
	}
}
