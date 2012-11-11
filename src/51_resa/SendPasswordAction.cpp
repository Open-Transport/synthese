
/** SendPasswordAction class implementation.
	@file SendPasswordAction.cpp
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

#include "SendPasswordAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "ResaRight.h"
#include "Request.h"
#include "Session.h"
#include "UserTableSync.h"
#include "User.h"
#include "ResaModule.h"
#include "OnlineReservationRule.h"
#include "ObjectNotFoundException.h"
#include "ResaDBLog.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, resa::SendPasswordAction>::FACTORY_KEY("SendPasswordAction");
	}

	namespace resa
	{
		const string SendPasswordAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "ui";



		ParametersMap SendPasswordAction::getParametersMap() const
		{
			ParametersMap map;
			if(_user.get()) map.insert(PARAMETER_USER, _user->getKey());
			return map;
		}



		void SendPasswordAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_user = UserTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_USER), *_env);
			}
			catch(ObjectNotFoundException<User>&)
			{
				throw ActionException("Bad User");
			}

			if(!ResaModule::GetReservationContact())
			{
				throw ActionException("Default reservation contact not defined");
			}
		}



		void SendPasswordAction::run(Request& request)
		{
			_user->setRandomPassword();

			UserTableSync::Save(_user.get());

			ResaDBLog::AddPasswordInitEntry(*request.getSession(), *_user);

			// Send confirmation email
			if(ResaModule::GetReservationContact()->sendCustomerEMail(*_user))
			{
				ResaDBLog::AddEMailEntry(*request.getSession(), *_user, "Message d'activation de réservation en ligne");
			}
		}



		bool SendPasswordAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(WRITE);
		}



		void SendPasswordAction::setUser( boost::shared_ptr<const security::User> value )
		{
			_user = const_pointer_cast<User>(value);
		}
	}
}
