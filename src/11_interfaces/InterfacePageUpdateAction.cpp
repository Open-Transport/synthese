
//////////////////////////////////////////////////////////////////////////
/// InterfacePageUpdateAction class implementation.
/// @file InterfacePageUpdateAction.cpp
/// @author Hugues
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

#include "InterfacePageUpdateAction.h"

#include "ActionException.h"
#include "InterfacePage.h"
#include "InterfacePageTableSync.h"
#include "InterfaceRight.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, interfaces::InterfacePageUpdateAction>::FACTORY_KEY("InterfacePageUpdateAction");
	}

	namespace interfaces
	{
		const string InterfacePageUpdateAction::PARAMETER_PAGE_ID = Action_PARAMETER_PREFIX + "pi";
		const string InterfacePageUpdateAction::PARAMETER_SOURCE = Action_PARAMETER_PREFIX + "so";



		ParametersMap InterfacePageUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_SOURCE, _source);
			return map;
		}



		void InterfacePageUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = Env::GetOfficialEnv().getEditable<InterfacePage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch(ObjectNotFoundException<InterfacePage>&)
			{
				throw ActionException("No such page");
			}

			_source = map.getDefault<string>(PARAMETER_SOURCE);

		}



		void InterfacePageUpdateAction::run(
			Request& request
		){
			stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			_page->setSource(_source);

			InterfacePageTableSync::Save(_page.get());
			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool InterfacePageUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<InterfaceRight>(WRITE);
		}



		void InterfacePageUpdateAction::setPage( boost::shared_ptr<InterfacePage> value )
		{
			_page = value;
		}
	}
}
