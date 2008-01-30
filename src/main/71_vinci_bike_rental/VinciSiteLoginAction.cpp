
/** VinciSiteLoginAction class implementation.
	@file VinciSiteLoginAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "VinciSiteLoginAction.h"

#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciBikeRentalModule.h"

#include "30_server/LoginAction.h"
#include "30_server/ActionException.h"
#include "30_server/Session.h"
#include "30_server/Request.h"
#include "30_server/ServerModule.h"
#include "30_server/ParametersMap.h"

#include "12_security/User.h"
#include "12_security/UserException.h"
#include "12_security/UserTableSync.h"
#include "12_security/UserTableSyncException.h"
#include "12_security/SecurityLog.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciSiteLoginAction>::FACTORY_KEY("vinci_login");
	}

	namespace vinci
	{
		const string VinciSiteLoginAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "login";
		const string VinciSiteLoginAction::PARAMETER_PASS = Action_PARAMETER_PREFIX + "pwd";
		const string VinciSiteLoginAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		
		
		VinciSiteLoginAction::VinciSiteLoginAction()
			: util::FactorableTemplate<Action, VinciSiteLoginAction>()
		{
		}
		
		
		
		ParametersMap VinciSiteLoginAction::getParametersMap() const
		{
			ParametersMap map;
			//map.insert(PARAMETER_xxx, _xxx);
			return map;
		}
		
		
		
		void VinciSiteLoginAction::_setFromParametersMap(const ParametersMap& map)
		{
			_login = map.getString(PARAMETER_USER, false, FACTORY_KEY);
			_password = map.getString(PARAMETER_PASS, false, FACTORY_KEY);
			_siteId = map.getUid(PARAMETER_SITE_ID, false, FACTORY_KEY);
		}
		
		
		
		void VinciSiteLoginAction::run()
		{
			// Fetch user
			try
			{
				if (_login.empty() || _password.empty())
					throw ActionException("Champ utilisateur ou mot de passe vide");

				try
				{
					shared_ptr<const VinciSite> site(VinciSiteTableSync::Get(_siteId));
				}
				catch (...)
				{
					throw ActionException("Unknown site");
				}

				shared_ptr<User> user = UserTableSync::getUserFromLogin(_login);
				user->verifyPassword(_password);

				if (!user->getConnectionAllowed())
					throw ActionException("Connexion impossible");

				Session* session = new Session(_request->getIP());
				session->setUser(user);
				_request->setSession(session);

				VinciBikeRentalModule::AddSessionSite(session, _siteId);

				SecurityLog::addUserLogin(user.get());
			}
			catch (UserTableSyncException e)
			{
				throw ActionException("Utilisateur introuvable");
			}
			catch (UserException e)
			{
				throw ActionException("Mot de passe erroné");
			}
		}

		bool VinciSiteLoginAction::_beforeSessionControl() const
		{
			return true;
		}
	}
}
