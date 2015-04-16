
/** CallEndAction class implementation.
	@file CallEndAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "CallEndAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "ResaRight.h"
#include "ResaModule.h"
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
		template<> const string FactorableTemplate<Action, resa::CallEndAction>::FACTORY_KEY("CallEndAction");
	}

	namespace resa
	{
		CallEndAction::CallEndAction()
			: util::FactorableTemplate<Action, CallEndAction>()
		{
		}



		ParametersMap CallEndAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void CallEndAction::_setFromParametersMap(const ParametersMap& map)
		{
		}



		void CallEndAction::run(Request& request)
		{
			ResaModule::CallClose(request.getSession().get());
		}



		bool CallEndAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE);
		}
	}
}
