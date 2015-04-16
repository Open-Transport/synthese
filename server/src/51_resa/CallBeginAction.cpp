////////////////////////////////////////////////////////////////////////////////
/// CallBeginAction class implementation.
///	@file CallBeginAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "CallBeginAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "ResaModule.h"
#include "ResaRight.h"
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
		template<> const string FactorableTemplate<Action, resa::CallBeginAction>::FACTORY_KEY("CallBeginAction");
	}

	namespace resa
	{
		CallBeginAction::CallBeginAction()
			: util::FactorableTemplate<Action, CallBeginAction>()
		{
		}



		ParametersMap CallBeginAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void CallBeginAction::_setFromParametersMap(const ParametersMap& map)
		{
		}



		void CallBeginAction::run(Request& request)
		{
			ResaModule::CallOpen(request.getSession().get());
		}



		bool CallBeginAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE);
		}
	}
}
