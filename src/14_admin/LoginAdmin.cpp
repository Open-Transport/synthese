
//////////////////////////////////////////////////////////////////////////
/// LoginAdmin class implementation.
///	@file LoginAdmin.cpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "LoginAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "LoginAction.h"
#include "Request.h"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLForm.h"
#include "HomeAdmin.h"
#include "AdminFunctionRequest.hpp"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace admin;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, LoginAdmin>::FACTORY_KEY("login");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<LoginAdmin>::ICON("key.png");
		template<> const string AdminInterfaceElementTemplate<LoginAdmin>::DEFAULT_TITLE("Login");
	}

	namespace admin
	{
		const string LoginAdmin::PARAM_URL_TO_LAUNCH("ul");



		void LoginAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_urlToLaunch = map.getDefault<string>(PARAM_URL_TO_LAUNCH);
		}



		ParametersMap LoginAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_URL_TO_LAUNCH, _urlToLaunch);
			return m;
		}



		bool LoginAdmin::isAuthorized(
			const security::User& user
		) const	{
			return true;
		}



		void LoginAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			if(request.getSession())
			{
				if(_urlToLaunch.empty())
				{
					AdminFunctionRequest<HomeAdmin> r(request);
					throw Request::RedirectException(r.getURL(), false);
				}
				else
				{
					throw Request::RedirectException(_urlToLaunch, false);
				}
			}
			else
			{
				AdminActionFunctionRequest<LoginAction,LoginAdmin> homeRequest(request);
				HTMLForm f(homeRequest.getHTMLForm("login"));

				stream << f.open();

				stream << "<p>Login : " << f.getTextInput(LoginAction::PARAMETER_LOGIN, string());
				stream << f.setFocus(LoginAction::PARAMETER_LOGIN);
				stream << " Mot de passe : " << f.getPasswordInput(LoginAction::PARAMETER_PASSWORD, string());
				stream << " " << f.getSubmitButton("Login") << "</p>";

				stream << f.close();
			}
		}
}	}
