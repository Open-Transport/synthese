
//////////////////////////////////////////////////////////////////////////
/// WebPageAdmin class implementation.
///	@file WebPageAdmin.cpp
///	@author Hugues
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

#include "WebPageAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "TransportWebsiteRight.h"
#include "WebPageTableSync.h"
#include "PropertiesHTMLTable.h"
#include "WebPageUpdateAction.h"
#include "WebPageContentUpdateAction.hpp"
#include "WebPageDisplayFunction.h"
#include "AdminActionFunctionRequest.hpp"
#include "Interface.h"
#include "WebPageInterfacePage.h"
#include "TransportSiteAdmin.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace transportwebsite;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, WebPageAdmin>::FACTORY_KEY("WebPageAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<WebPageAdmin>::ICON("page.png");
		template<> const string AdminInterfaceElementTemplate<WebPageAdmin>::DEFAULT_TITLE("Page web");
	}

	namespace transportwebsite
	{
		WebPageAdmin::WebPageAdmin()
			: AdminInterfaceElementTemplate<WebPageAdmin>()
		{ }


		
		void WebPageAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				_page = WebPageTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}
		}



		ParametersMap WebPageAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_page.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _page->getKey());
			}
			return m;
		}


		
		bool WebPageAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportWebsiteRight>(READ);
		}



		void WebPageAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const	{

			stream << "<h1>Visualisation</h1>";

			StaticFunctionRequest<WebPageDisplayFunction> viewRequest(_request, false);
			if(	_page->getRoot()->getInterface() &&
				!_page->getRoot()->getInterface()->getDefaultClientURL().empty()
			){
				viewRequest.setClientURL(_page->getRoot()->getInterface()->getDefaultClientURL());
			}
			viewRequest.getFunction()->setPage(_page);
			stream << "<p>" << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "page_go.png") << "</p>";


			stream << "<h1>Propriétés</h1>";

			{
				AdminActionFunctionRequest<WebPageUpdateAction, WebPageAdmin> updateRequest(_request);
				updateRequest.getAction()->setWebPage(const_pointer_cast<WebPage>(_page));
				PropertiesHTMLTable t(updateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("ID", lexical_cast<string>(_page->getKey()));
				stream << t.cell("Titre", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_TITLE, _page->getName()));
				stream << t.cell("Page supérieure", t.getForm().getSelectInput(WebPageUpdateAction::PARAMETER_UP_ID, WebPageTableSync::GetPagesList(_page->getRoot()->getKey(), "(racine)"), _page->getParent() ? _page->getParent()->getKey() : RegistryKeyType(0)));
				stream << t.close();
			}

			stream << "<h1>Contenu</h1>";

			{
				AdminActionFunctionRequest<WebPageContentUpdateAction, WebPageAdmin> contentUpdateRequest(_request);
				contentUpdateRequest.getAction()->setWebPage(const_pointer_cast<WebPage>(_page));
				PropertiesHTMLTable t(contentUpdateRequest.getHTMLForm());
				stream << t.open();
				stream << t.cell("Contenu 1", t.getForm().getTextAreaInput(WebPageContentUpdateAction::PARAMETER_CONTENT1, _page->getContent(), 20, 60));
				stream << t.close();
			}
		}




		std::string WebPageAdmin::getTitle() const
		{
			return _page.get() ? _page->getName() : DEFAULT_TITLE;
		}



		bool WebPageAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _page->getKey() == static_cast<const WebPageAdmin&>(other)._page->getKey();
		}



		AdminInterfaceElement::PageLinks WebPageAdmin::getSubPages( const AdminInterfaceElement& currentPage, const admin::AdminRequest& request ) const
		{
			AdminInterfaceElement::PageLinks links;

			WebPageTableSync::SearchResult pages(WebPageTableSync::Search(Env::GetOfficialEnv(), _page->getRoot()->getKey(), _page->getKey()));
			BOOST_FOREACH(shared_ptr<WebPage> page, pages)
			{
				shared_ptr<WebPageAdmin> p(
					getNewOtherPage<WebPageAdmin>(false)
				);
				p->setPage(const_pointer_cast<const WebPage>(page));
				links.push_back(p);
			}

			return links;
		}



		AdminInterfaceElement::PageLinks WebPageAdmin::_getCurrentTreeBranch() const
		{
			if(_page->getParent())
			{
				shared_ptr<WebPageAdmin> p(
					getNewOtherPage<WebPageAdmin>()
				);
				p->setPage(Env::GetOfficialEnv().getSPtr(_page->getParent()));
				PageLinks links(p->getCurrentTreeBranch());
				links.push_back(p);
				return links;
			}
			else
			{
				shared_ptr<TransportSiteAdmin> p(
					getNewOtherPage<TransportSiteAdmin>()
				);
				p->setSite(Env::GetOfficialEnv().getSPtr(_page->getRoot()));
				PageLinks links(p->getCurrentTreeBranch());
				links.push_back(p);
				return links;
			}
		}
	}
}
