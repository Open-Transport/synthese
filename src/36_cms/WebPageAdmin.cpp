
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
#include "WebPageTableSync.h"
#include "PropertiesHTMLTable.h"
#include "WebPageUpdateAction.h"
#include "WebPageContentUpdateAction.hpp"
#include "WebPageDisplayFunction.h"
#include "AdminActionFunctionRequest.hpp"
#include "Interface.h"
#include "WebPageInterfacePage.h"
#include "ResultHTMLTable.h"
#include "HTMLModule.h"
#include "WebPageAddAction.h"
#include "WebPageRemoveAction.h"
#include "WebPageLinkAddAction.hpp"
#include "WebPageLinkRemoveAction.hpp"
#include "WebPageMoveAction.hpp"
#include "AdminFunctionRequest.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace cms;
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

	namespace cms
	{
		const string WebPageAdmin::TAB_CONTENT("co");
		const string WebPageAdmin::TAB_TREE("tr");
		const string WebPageAdmin::TAB_LINKS("li");



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
				throw AdminParametersException("No such page");
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
			return true;
			//return user.getProfile()->isAuthorized<TransportWebsiteRight>(READ);
		}



		void WebPageAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB CONTENT
			if (openTabContent(stream, TAB_CONTENT))
			{
				stream << "<h1>Visualisation</h1>";

				StaticFunctionRequest<WebPageDisplayFunction> viewRequest(request, false);
				if(	!_page->getRoot()->getClientURL().empty()
				){
					viewRequest.setClientURL(_page->getRoot()->getClientURL());
				}
				viewRequest.getFunction()->setPage(_page);
				stream << "<p>" << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "page_go.png") << "</p>";

				stream << "<h1>Contenu</h1>";

				{
					AdminActionFunctionRequest<WebPageContentUpdateAction, WebPageAdmin> contentUpdateRequest(request);
					contentUpdateRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));
					PropertiesHTMLTable t(contentUpdateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("Titre", t.getForm().getTextInput(WebPageContentUpdateAction::PARAMETER_TITLE, _page->getName()));
					stream << t.cell("Contenu", t.getForm().getTextAreaInput(WebPageContentUpdateAction::PARAMETER_CONTENT1, _page->getContent(), 15, 60));
					stream << t.cell("Résumé", t.getForm().getTextAreaInput(WebPageContentUpdateAction::PARAMETER_ABSTRACT, _page->getAbstract(), 5, 60));
					stream << t.cell("Image", t.getForm().getTextInput(WebPageContentUpdateAction::PARAMETER_IMAGE, _page->getImage()));
					stream << t.cell("Ignorer caractères invisibles", t.getForm().getOuiNonRadioInput(WebPageContentUpdateAction::PARAMETER_IGNORE_WHITE_CHARS, _page->getIgnoreWhiteChars()));
					stream << t.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB TREE
			if (openTabContent(stream, TAB_TREE))
			{
				stream << "<h1>Propriétés</h1>";

				{
					AdminActionFunctionRequest<WebPageUpdateAction, WebPageAdmin> updateRequest(request);
					updateRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));
					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("ID", lexical_cast<string>(_page->getKey()));
					stream << t.cell("Modèle (défaut : modèle du site)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_TEMPLATE_ID, _page->_getTemplate() ? lexical_cast<string>(_page->_getTemplate()->getKey()) : "0"));
					stream << t.cell("Ne pas utiliser le modèle", t.getForm().getOuiNonRadioInput(WebPageUpdateAction::PARAMETER_DO_NOT_USE_TEMPLATE, _page->getDoNotUseTemplate()));
					stream << t.cell("Inclure forum", t.getForm().getOuiNonRadioInput(WebPageUpdateAction::PARAMETER_HAS_FORUM, _page->getHasForum()));
					stream << t.cell("Type MIME (défaut : text/html)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_MIME_TYPE, _page->_getMimeType()));
					stream << t.cell("Début publication", t.getForm().getCalendarInput(WebPageUpdateAction::PARAMETER_START_DATE, _page->getStartDate()));
					stream << t.cell("Fin publication", t.getForm().getCalendarInput(WebPageUpdateAction::PARAMETER_END_DATE, _page->getEndDate()));
					stream << t.cell(
						"Page supérieure",
						t.getForm().getSelectInput(
							WebPageUpdateAction::PARAMETER_UP_ID,
							WebPageTableSync::GetPagesList(_page->getRoot()->getKey(), "(racine)"),
							optional<RegistryKeyType>(_page->getParent() ? _page->getParent()->getKey() : 0)
					)	);
					stream << t.cell("Chemin URL (facultatif)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_SMART_URL_PATH, _page->getSmartURLPath()));
					stream << t.cell("Champ par défaut paramètre (facultatif)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME, _page->getSmartURLDefaultParameterName()));
					stream << t.close();
				}

				stream << "<h1>Sous-pages</h1>";
				{
					AdminActionFunctionRequest<WebPageAddAction, WebPageAdmin> addRequest(request);
					addRequest.getAction()->setParent(const_pointer_cast<Webpage>(_page));

					AdminActionFunctionRequest<WebPageRemoveAction, WebPageAdmin> deleteRequest(request);

					AdminActionFunctionRequest<WebPageMoveAction, WebPageAdmin> moveRequest(request);

					WebPageAdmin::DisplaySubPages(stream, _page->getKey(), addRequest, deleteRequest, moveRequest, request);
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB LINKS
			if (openTabContent(stream, TAB_LINKS))
			{
				stream << "<h1>Liens</h1>";

				AdminFunctionRequest<WebPageAdmin> openRequest(request);

				AdminActionFunctionRequest<WebPageLinkAddAction, WebPageAdmin> addRequest(request);
				addRequest.getAction()->setPage(const_pointer_cast<Webpage>(_page));

				AdminActionFunctionRequest<WebPageLinkRemoveAction, WebPageAdmin> removeRequest(request);
				removeRequest.getAction()->setPage(const_pointer_cast<Webpage>(_page));

				HTMLForm f(addRequest.getHTMLForm());
				stream << f.open();

				HTMLTable::ColsVector h;
				h.push_back("ID");
				h.push_back("Titre");
				h.push_back("Actions");
				h.push_back("Actions");
				HTMLTable t(h, ResultHTMLTable::CSS_CLASS);
				stream << t.open();
				BOOST_FOREACH(const Webpage::Links::value_type& link, _page->getLinks())
				{
					openRequest.getPage()->setPage(Env::GetOfficialEnv().getEditableSPtr(link));
					removeRequest.getAction()->setDestinationPage(Env::GetOfficialEnv().getEditableSPtr(link));

					stream << t.row();
					stream << t.col() << link->getKey();
					stream << t.col() << link->getName();
					stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), ICON);
					stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le lien ?", "page_delete.png");
				}

				stream << t.row();
				stream << t.col() << f.getTextInput(WebPageLinkAddAction::PARAMETER_DESTINATION_ID, string(), "(id page à lier)");
				stream << t.col();
				stream << t.col(2) << f.getSubmitButton("Ajouter");

				stream << t.close() << f.close();
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
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
			BOOST_FOREACH(shared_ptr<Webpage> page, pages)
			{
				shared_ptr<WebPageAdmin> p(
					getNewOtherPage<WebPageAdmin>(false)
				);
				p->setPage(const_pointer_cast<const Webpage>(page));
				links.push_back(p);
			}

			return links;
		}



/*		AdminInterfaceElement::PageLinks WebPageAdmin::_getCurrentTreeBranch() const
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
				shared_ptr<WebsiteAdmin> p(
					getNewOtherPage<WebsiteAdmin>()
				);
				p->setSite(Env::GetOfficialEnv().getSPtr(_page->getRoot()));
				PageLinks links(p->getCurrentTreeBranch());
				links.push_back(p);
				return links;
			}
		}
*/


		void WebPageAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Contenu", TAB_CONTENT, true));
			_tabs.push_back(Tab("Arborescence", TAB_TREE, true));
			_tabs.push_back(Tab("Liens", TAB_LINKS, true));

			_tabBuilded = true;
		}


		void WebPageAdmin::_displaySubPages(
			std::ostream& stream,
			const WebPageTableSync::SearchResult& pages,
			StaticActionRequest<WebPageRemoveAction>& deleteRequest,
			StaticActionRequest<WebPageMoveAction>& moveRequest,
			const admin::AdminRequest& request,
			HTMLTable& t,
			HTMLForm& f,
			size_t depth
		){

			AdminFunctionRequest<WebPageAdmin> openRequest(request);
			StaticFunctionRequest<WebPageDisplayFunction> viewRequest(request, false);
			
			for(WebPageTableSync::SearchResult::const_iterator it(pages.begin()); it != pages.end(); ++it)
			{
				shared_ptr<Webpage> page(*it);

				openRequest.getPage()->setPage(const_pointer_cast<const Webpage>(page));
				viewRequest.getFunction()->setPage(const_pointer_cast<const Webpage>(page));
				moveRequest.getAction()->setPage(page);

				if(	!page->getRoot()->getClientURL().empty()
				){
					viewRequest.setClientURL(page->getRoot()->getClientURL());
				}
				deleteRequest.getAction()->setPage(page);

				stream << t.row();
				stream << t.col();
				if(depth == 0)
				{
					stream << f.getRadioInput(WebPageAddAction::PARAMETER_TEMPLATE_ID, optional<RegistryKeyType>(page->getKey()), optional<RegistryKeyType>(), string(), false);
				}
				stream << t.col();
				for(size_t i(0); i<depth; ++i)
					stream << "&nbsp;&nbsp;&nbsp;";
				stream << page->getRank();
				if(depth == 0)
				{
					stream << t.col();
					if(it != pages.begin())
					{
						moveRequest.getAction()->setUp(true);
						stream << HTMLModule::getHTMLLink(moveRequest.getURL(), HTMLModule::getHTMLImage("arrow_up.png", "up"));
					}
					stream << t.col();
					if(it+1 != pages.end())
					{
						moveRequest.getAction()->setUp(false);
						stream << HTMLModule::getHTMLLink(moveRequest.getURL(), HTMLModule::getHTMLImage("arrow_down.png", "down"));
					}
				}
				else
				{
					stream << t.col();
					stream << t.col();
				}
				stream << t.col() << page->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), WebPageAdmin::ICON);
				stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "page_go.png");

				WebPageTableSync::SearchResult result(
					WebPageTableSync::Search(
						Env::GetOfficialEnv(),
						optional<RegistryKeyType>(),
						page->getKey()
				)	);

				stream << t.col();
				if(result.empty())
				{
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la page "+ page->getName() +" ?", "page_delete.png");
				}

				_displaySubPages(stream, result, deleteRequest, moveRequest, request, t, f, depth+1);
			}
		}



		void WebPageAdmin::DisplaySubPages(
			std::ostream& stream,
			RegistryKeyType parentId,
			server::StaticActionRequest<WebPageAddAction>& createRequest,
			server::StaticActionRequest<WebPageRemoveAction>& deleteRequest,
			server::StaticActionRequest<WebPageMoveAction>& moveRequest,
			const admin::AdminRequest& request
		){

			WebPageTableSync::SearchResult result(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					decodeTableId(parentId) == WebPageTableSync::TABLE.ID ? optional<RegistryKeyType>() : parentId,
					decodeTableId(parentId) == WebPageTableSync::TABLE.ID ? parentId : RegistryKeyType(0)
			)	);

			HTMLForm f(createRequest.getHTMLForm());

			HTMLTable::ColsVector h;
			h.push_back("Position");
			h.push_back("Position");
			h.push_back("Position");
			h.push_back("Position");
			h.push_back("Titre");
			h.push_back("Actions");
			h.push_back("Actions");
			h.push_back("Actions");
			HTMLTable t(h, ResultHTMLTable::CSS_CLASS);

			stream << f.open();
			stream << t.open();

			_displaySubPages(stream, result, deleteRequest, moveRequest, request, t, f);

			stream << t.row();
			stream << t.col() << f.getRadioInput(WebPageAddAction::PARAMETER_TEMPLATE_ID, optional<RegistryKeyType>(),optional<RegistryKeyType>(),string(), false);
			stream << t.col(3) << result.size();
			stream << t.col() << f.getTextInput(WebPageAddAction::PARAMETER_TITLE, string(), "(Entrez le titre ici)");
			stream << t.col(3) << f.getSubmitButton("Créer");
			stream << t.close();
			stream << f.close();
		}
	}
}
