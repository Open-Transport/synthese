
//////////////////////////////////////////////////////////////////////////
/// SVNCheckoutAction class implementation.
/// @file SVNCheckoutAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "SVNCheckoutAction.hpp"

#include "ActionException.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "SVNWorkingCopy.hpp"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, db::svn::SVNCheckoutAction>::FACTORY_KEY = "SVNCheckout";

	namespace db
	{
		namespace svn
		{
			const string SVNCheckoutAction::PARAMETER_REPO_URL = Action_PARAMETER_PREFIX + "_repo_url";
			const string SVNCheckoutAction::PARAMETER_USER = Action_PARAMETER_PREFIX + "_user";
			const string SVNCheckoutAction::PARAMETER_PASSWORD = Action_PARAMETER_PREFIX + "_password";
			
			
			
			ParametersMap SVNCheckoutAction::getParametersMap() const
			{
				ParametersMap map;
				map.insert(PARAMETER_PASSWORD, _password);
				map.insert(PARAMETER_REPO_URL, _repo.getURL());
				map.insert(PARAMETER_USER, _user);
				return map;
			}
			
			
			
			void SVNCheckoutAction::_setFromParametersMap(const ParametersMap& map)
			{
				// User
				_user = map.getDefault<string>(PARAMETER_USER);

				// Password
				_password = map.getDefault<string>(PARAMETER_PASSWORD);

				// Repository URL
				_repo = SVNRepository(map.getDefault<string>(PARAMETER_REPO_URL));
				if(_repo.getURL().empty())
				{
					throw ActionException("Repository URL must be non empty");
				}
			}
			
			
			
			void SVNCheckoutAction::run(
				Request& request
			){
				SVNWorkingCopy wc;
				wc.setRepo(_repo);
				wc.checkout(_user, _password);

				// Returns the id of the checked out object
				if(	request.getActionWillCreateObject() &&
					wc.getObject()
				){
					request.setActionCreatedId(wc.getObject()->getKey());
				}
			}
			
			
			
			bool SVNCheckoutAction::isAuthorized(
				const Session* session
			) const {
	//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
				return true;
			}
}	}	}

