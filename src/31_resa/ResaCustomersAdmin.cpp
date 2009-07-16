
/** ResaCustomersAdmin class implementation.
	@file ResaCustomersAdmin.cpp
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

#include "ResaCustomersAdmin.h"

#include "User.h"
#include "UserTableSync.h"

#include "ResaModule.h"
#include "ResaRight.h"
#include "ResaCustomerAdmin.h"

#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"

#include "SearchFormHTMLTable.h"
#include "ResultHTMLTable.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace admin;
	using namespace security;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaCustomersAdmin>::FACTORY_KEY("ResaCustomersAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaCustomersAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<ResaCustomersAdmin>::DEFAULT_TITLE("Clients");
	}

	namespace resa
	{
		const string ResaCustomersAdmin::PARAM_SEARCH_LOGIN("sl");
		const string ResaCustomersAdmin::PARAM_SEARCH_NAME("sn");
		const string ResaCustomersAdmin::PARAM_SEARCH_SURNAME("ss");

		
		void ResaCustomersAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			if(!map.getDefault<string>(PARAM_SEARCH_NAME).empty())
			{
				_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);
			}
			if(!map.getDefault<string>(PARAM_SEARCH_SURNAME).empty())
			{
				_searchSurname = map.getOptional<string>(PARAM_SEARCH_SURNAME);
			}
			if(!map.getDefault<string>(PARAM_SEARCH_LOGIN).empty())
			{
				_searchLogin = map.getOptional<string>(PARAM_SEARCH_LOGIN);
			}
			
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 30);
		}
		
		
		
		server::ParametersMap ResaCustomersAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			if(_searchName)
				m.insert(PARAM_SEARCH_NAME, *_searchName);
			if(_searchSurname)
				m.insert(PARAM_SEARCH_SURNAME, *_searchSurname);
			if(_searchLogin)
				m.insert(PARAM_SEARCH_LOGIN, *_searchLogin);
			return m;
		}

		
		void ResaCustomersAdmin::display(ostream& stream, VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const
		{
			// Requests
			AdminFunctionRequest<ResaCustomersAdmin> searchRequest(_request);

			AdminFunctionRequest<ResaCustomerAdmin> openRequest(_request);
			
			// Form
			SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
			stream << "<h1>Recherche</h1>";
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName ? *_searchName : string()));
			stream << st.cell("Pr�nom", st.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname ? *_searchSurname : string()));
			stream << st.cell("Login", st.getForm().getTextInput(PARAM_SEARCH_LOGIN, _searchLogin ? *_searchLogin : string()));
			stream << st.close();
			stream << st.getForm().setFocus(PARAM_SEARCH_NAME);

			stream << "<h1>R�sultats</h1>";

			// Search
			UserTableSync::SearchResult users(
				UserTableSync::Search(
					_getEnv(),
					_searchLogin ? "%" + *_searchLogin + "%" : _searchLogin,
					_searchName ? "%" + *_searchName + "%" : _searchName,
					_searchSurname ? "%" + *_searchSurname + "%" : _searchSurname,
					optional<string>(),
					optional<RegistryKeyType>(),
					logic::indeterminate,
					logic::indeterminate,
					optional<RegistryKeyType>(),
					_requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAM_SEARCH_LOGIN
					, _requestParameters.orderField == PARAM_SEARCH_NAME
					, false
					, _requestParameters.raisingOrder
			)	);

			// Results
			if (users.empty())
				stream << "<p>Aucun client trouv�.</p>";
			else
			{
				ResultHTMLTable::HeaderVector h;
				h.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
				h.push_back(make_pair(string(), "Pr�nom"));
				h.push_back(make_pair(string(), "T�l�phone"));
				h.push_back(make_pair(PARAM_SEARCH_LOGIN, "Login"));
				h.push_back(make_pair(string(), "Action"));
				
				ResultHTMLTable t(h, searchRequest.getHTMLForm(), _requestParameters, users);

				stream << t.open();

				BOOST_FOREACH(shared_ptr<User> user, users)
				{
					openRequest.getPage()->setUser(user);

					stream << t.row();

					stream << t.col() << user->getName();
					stream << t.col() << user->getSurname();
					stream << t.col() << user->getPhone();
					stream << t.col() << user->getLogin();

					stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "user.png");
				}

				stream << t.close();
		}	}



		bool ResaCustomersAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			return _request.isAuthorized<ResaRight>(READ);
		}



		AdminInterfaceElement::PageLinks ResaCustomersAdmin::getSubPagesOfModule(
			const string& moduleKey,
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(moduleKey == ResaModule::FACTORY_KEY && isAuthorized(request))
			{
				if(dynamic_cast<const ResaCustomersAdmin*>(currentPage.get()))
				{
					AddToLinks(links,currentPage);
				}
				else
				{
					AddToLinks(links, getNewPage());
			}	}
			return links;
		}
	
	
		AdminInterfaceElement::PageLinks ResaCustomersAdmin::getSubPages(
			shared_ptr<const AdminInterfaceElement> currentPage,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(dynamic_cast<const ResaCustomerAdmin*>(currentPage.get()))
			{
				AddToLinks(links,currentPage);
			}
			
			return links;
		}
}	}
