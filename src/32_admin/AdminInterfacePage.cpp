
/** AdminInterfacePage class implementation.
	@file AdminInterfacePage.cpp

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

#include "AdminInterfaceElement.h"
#include "AdminInterfacePage.h"
#include "LoginAction.h"
#include "LogoutAction.h"
#include "HomeAdmin.h"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLForm.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;
	using namespace html;
	using namespace server;
		

	template<> const std::string util::FactorableTemplate<interfaces::InterfacePage,admin::AdminInterfacePage>::FACTORY_KEY("admin");

	namespace admin
	{
		const string AdminInterfacePage::DATA_USER_FULL_NAME("user_full_name");
		
		void AdminInterfacePage::display(
			std::ostream& stream,
			const AdminInterfaceElement* page,
			const boost::optional<std::string>& errorMessage,
			const AdminRequest& request
		) const	{

			VariablesMap vars;

			ParametersVector parameters;
			parameters.push_back(
				request.getSession() && request.getUser().get() ?
				request.getUser()->getFullName() :
				string()
			); //0
			parameters.push_back(errorMessage ? *errorMessage : string()); //1

			if(page)
			{
				// 2 : admin tree (not yet implemented)
				parameters.push_back(string());

				// 3 : admin position (not yet implemented)
				parameters.push_back(string());

				// 4 : admin tabs code
				stringstream tabsStream;
				page->displayTabs(
					tabsStream,
					vars,
					request
				);
				parameters.push_back(tabsStream.str());

				// 5 : admin content
				stringstream contentStream;
				page->display(
					contentStream,
					vars,
					request
				);
				parameters.push_back(contentStream.str()); //6

				parameters.push_back(string()); //6
				parameters.push_back(string()); //7
				parameters.push_back(string()); //8
				parameters.push_back(string()); //9

				// 10 : logout url
				StaticActionFunctionRequest<LogoutAction,AdminFunction> logoutRequest(request, true);
				parameters.push_back(logoutRequest.getURL());
			}
			else
			{
				parameters.push_back(string()); //2
				parameters.push_back(string()); //3
				parameters.push_back(string()); //4
				parameters.push_back(string()); //5

				// 6 : login form opening html code
				AdminActionFunctionRequest<LoginAction,HomeAdmin> homeRequest(request);
				HTMLForm f(homeRequest.getHTMLForm("login"));
				parameters.push_back(f.open());

				// 7 : login form login text field html code
				parameters.push_back(
					f.getTextInput(LoginAction::PARAMETER_LOGIN, string()) +
					f.setFocus(LoginAction::PARAMETER_LOGIN)
				);

				// 8 : login form login password field html code
				parameters.push_back(
					f.getPasswordInput(LoginAction::PARAMETER_PASSWORD, string())
				);

				// 9 : login form closing html code
				parameters.push_back(f.close());

				parameters.push_back(string()); //10
			}

			InterfacePage::_display(
				stream,
				parameters,
				vars,
				static_cast<const void*>(page),
				&request
			);
		}



		AdminInterfacePage::AdminInterfacePage()
			: Registrable(0)
		{

		}
	}
}
