
/** ThreadKillAction class implementation.
	@file ThreadKillAction.cpp
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

#include "ThreadKillAction.h"

#include "Profile.h"
#include "ServerModule.h"
#include "Session.h"
#include "User.h"
#include "ActionException.h"
#include "ParametersMap.h"
#include "ServerAdminRight.h"
#include "Request.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, server::ThreadKillAction>::FACTORY_KEY("ThreadKillAction");
	}

	namespace server
	{
		const string ThreadKillAction::PARAMETER_THREAD_ID = Action_PARAMETER_PREFIX + "ti";



		ParametersMap ThreadKillAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_THREAD_ID, _thread_id);
			return map;
		}



		void ThreadKillAction::_setFromParametersMap(const ParametersMap& map)
		{
			_thread_id = map.get<string>(PARAMETER_THREAD_ID);
		}



		void ThreadKillAction::run(Request& request)
		{
			ServerModule::KillThread(_thread_id);
		}



		bool ThreadKillAction::isAuthorized(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ServerAdminRight>(DELETE_RIGHT);
		}



		void ThreadKillAction::setThreadId(const string& value )
		{
			_thread_id = value;
		}
	}
}
