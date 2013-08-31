
//////////////////////////////////////////////////////////////////////////
/// WebPageAdmin class implementation.
///	@file WebPageAdmin.cpp
///	@author Hugues
///	@date 2010
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

#include "WebPageAdmin.h"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AjaxForm.hpp"
#include "CMSRight.hpp"
#include "EditArea.hpp"
#include "HTMLModule.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "RemoveObjectAction.hpp"
#include "ResultHTMLTable.h"
#include "StaticActionRequest.h"
#include "TinyMCE.hpp"
#include "User.h"
#include "WebPageAddAction.h"
#include "WebpageContentUploadAction.hpp"
#include "WebPageDisplayFunction.h"
#include "WebPageLinkAddAction.hpp"
#include "WebPageLinkRemoveAction.hpp"
#include "WebPageMoveAction.hpp"
#include "WebPageTableSync.h"
#include "WebPageUpdateAction.h"
#include "WebsiteTableSync.hpp"

#include <boost/algorithm/string/find.hpp>

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
	using namespace db;

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
		const string WebPageAdmin::TAB_PROPERTIES("pr");
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
			return user.getProfile()->isAuthorized<CMSRight>(READ);
		}



		void WebPageAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			StaticFunctionRequest<WebPageDisplayFunction> viewRequest(request, false);
			if(	!_page->getRoot()->get<ClientURL>().empty()
			){
				viewRequest.setClientURL(_page->getRoot()->get<ClientURL>());
			}
			viewRequest.getFunction()->setPage(_page.get());

			////////////////////////////////////////////////////////////////////
			// TAB CONTENT
			if (openTabContent(stream, TAB_CONTENT))
			{
				const string& code(
					_page->get<WebpageContent>().getCMSScript().getCode()
				);
				bool canBeWYSIWYG(
					!boost::algorithm::find_first(code, "<?") &&
					!boost::algorithm::find_first(code, "<#") &&
					!boost::algorithm::find_first(code, "<@") &&
					!boost::algorithm::find_first(code, "<{") &&
					!boost::algorithm::find_first(code, "<%")
				);

				stream << "<h1>Visualisation</h1>";
				stream << "<p>" << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "/admin/img/page_go.png") << "</p>";


				stream << "<h1>Contenu</h1>";

				stream << "<p>";

				if(canBeWYSIWYG)
				{
					AdminActionFunctionRequest<WebPageUpdateAction,WebPageAdmin> rawEditorUpdateRequest(request, *this);
					rawEditorUpdateRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));
					rawEditorUpdateRequest.getAction()->setRawEditor(!_page->get<RawEditor>());
					stream <<
						HTMLModule::getLinkButton(
							rawEditorUpdateRequest.getHTMLForm().getURL(),
							"Passer à l'éditeur "+ string(_page->get<RawEditor>() ? "WYSIWYG" : "technique")
						);
				}
				else
				{
					stream << HTMLModule::getLinkButton("/admin/service_api", "Services API", string(), "/admin/img/help.png");
					stream << " L'éditeur technique est obligatoire à cause de la présence de balise d'appel aux services SYNTHESE dans le contenu de la page.";
				}
				stream << "</p>";

				StaticActionRequest<WebPageUpdateAction> contentUpdateRequest(request);
				contentUpdateRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));

				TinyMCE tinyMCE;
				tinyMCE.setAjaxSaveURL(contentUpdateRequest.getURL());
				stream << tinyMCE.open();

				if(	canBeWYSIWYG &&
					!_page->get<RawEditor>()
				){
					if(_page->get<WebpageContent>().getMimeType() == MimeTypes::HTML)
					{
						stream <<
							TinyMCE::GetFakeFormWithInput(
								WebPageUpdateAction::PARAMETER_CONTENT1,
								_page->get<WebpageContent>().getCMSScript().getCode()
							)
						;
					}
					else if(_page->get<WebpageContent>().getMimeType() == MimeTypes::PNG ||
						_page->get<WebpageContent>().getMimeType() == MimeTypes::GIF ||
						_page->get<WebpageContent>().getMimeType() == MimeTypes::JPEG
					){
						stream << "<p><img src=\"" << viewRequest.getURL() << "\" /></p>";
					}
					else
					{
						stream << "<iframe src=\"" << viewRequest.getURL() << "\"></iframe>";
					}
				}
				else
				{
					AjaxForm f(contentUpdateRequest.getAjaxForm("update_content"));
					stream << f.open();
					EditArea editArea(stream);
					editArea.getAjaxForm(
						stream,
						contentUpdateRequest.getURL(),
						WebPageUpdateAction::PARAMETER_CONTENT1,
						_page->get<WebpageContent>().getCMSScript().getCode(),
						20, 80
					);
					stream << f.getSubmitButton("Sauvegarder");
					stream << f.close();
				}

				// File upload
				AdminActionFunctionRequest<WebpageContentUploadAction, WebPageAdmin> uploadRequest(request, *this);
				uploadRequest.getAction()->setPage(_page);
				HTMLForm uploadForm(uploadRequest.getHTMLForm("upload"));
				stream << "<h1>Téléchargement de contenu</h1>";
				stream << uploadForm.open();
				stream << uploadForm.getFileInput(WebpageContentUploadAction::PARAMETER_CONTENT);
				stream << uploadForm.getSubmitButton("Télécharger");
				stream << uploadForm.close();

				// Summary
				stream << "<h1>Résumé</h1>";
				if(canBeWYSIWYG && !_page->get<RawEditor>())
				{
					stream << TinyMCE::GetFakeFormWithInput(WebPageUpdateAction::PARAMETER_ABSTRACT, _page->get<Abstract>());
				}
				else
				{
					AjaxForm f(contentUpdateRequest.getAjaxForm("update_abstract"));
					stream << f.open();
					stream << f.getTextAreaInput(WebPageUpdateAction::PARAMETER_ABSTRACT, _page->get<Abstract>(), 20, 40, false);
					stream << f.getSubmitButton("Sauvegarder");
					stream << f.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{
				stream << "<h1>Visualisation</h1>";
				stream << "<p>" << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "/admin/img/page_go.png") << "</p>";

				stream << "<h1>Propriétés</h1>";

				{
					AdminActionFunctionRequest<WebPageUpdateAction, WebPageAdmin> contentUpdateRequest(request, *this);
					contentUpdateRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));
					PropertiesHTMLTable t(contentUpdateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("Titre", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_TITLE, _page->get<Title>()));
					stream << t.cell("Image", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_IMAGE, _page->get<ImageURL>()));
					stream << t.cell("Modèle (défaut : modèle du site)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_TEMPLATE_ID, _page->get<SpecificTemplate>() ? lexical_cast<string>(_page->get<SpecificTemplate>()->getKey()) : "0"));
					stream << t.cell("Ne pas utiliser le modèle", t.getForm().getOuiNonRadioInput(WebPageUpdateAction::PARAMETER_DO_NOT_USE_TEMPLATE, _page->get<DoNotUseTemplate>()));
					stream << t.cell("Inclure forum", t.getForm().getOuiNonRadioInput(WebPageUpdateAction::PARAMETER_HAS_FORUM, _page->get<HasForum>()));
					stream <<
						t.cell(
							"Ignorer caractères invisibles",
							t.getForm().getOuiNonRadioInput(
								WebPageUpdateAction::PARAMETER_IGNORE_WHITE_CHARS,
								_page->get<WebpageContent>().getCMSScript().getIgnoreWhiteChars()
						)	)
					;
					stream << t.cell("Type MIME", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_MIME_TYPE, _page->getMimeType()));
					stream <<
						t.cell(
							"Ne pas evaluer le code",
							t.getForm().getOuiNonRadioInput(
								WebPageUpdateAction::PARAMETER_DO_NOT_EVALUATE,
								_page->get<WebpageContent>().getCMSScript().getDoNotEvaluate()
						)	)
					;
					stream << t.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB TREE
			if (openTabContent(stream, TAB_TREE))
			{
				stream << "<h1>Paramètres de publication</h1>";
				{
					AdminActionFunctionRequest<WebPageUpdateAction, WebPageAdmin> updateRequest(request, *this);
					updateRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));
					PropertiesHTMLTable t(updateRequest.getHTMLForm());
					stream << t.open();
					stream << t.cell("ID", lexical_cast<string>(_page->getKey()));
					stream << t.cell("Début publication", t.getForm().getCalendarInput(WebPageUpdateAction::PARAMETER_START_DATE, _page->get<StartTime>()));
					stream << t.cell("Fin publication", t.getForm().getCalendarInput(WebPageUpdateAction::PARAMETER_END_DATE, _page->get<EndTime>()));
					string minutesStr( !_page->get<MaxAge>().is_not_a_date_time() &&_page->get<MaxAge>().total_seconds()
									   ? boost::lexical_cast<string>(_page->get<MaxAge>().total_seconds() / 60)
									   : "" );
					stream << t.cell("Age maximum en cache (minutes)", t.getForm().GetTextInput(WebPageUpdateAction::PARAMETER_MAX_AGE, minutesStr));
					stream << t.cell(
						"Page supérieure",
						t.getForm().getSelectInput(
							WebPageUpdateAction::PARAMETER_UP_ID,
							_page->getRoot()->getPagesList("(racine)"),
							optional<RegistryKeyType>(_page->getParent(true) ? _page->getParent()->getKey() : 0)
					)	);
					stream << t.cell("Chemin URL (facultatif)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_SMART_URL_PATH, _page->get<SmartURLPath>()));
					stream << t.cell("Champ par défaut paramètre (facultatif)", t.getForm().getTextInput(WebPageUpdateAction::PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME, _page->get<SmartURLDefaultParameterName>()));
					stream << t.close();
				}

				stream << "<h1>Sous-pages</h1>";
				{
					AdminActionFunctionRequest<WebPageAddAction, WebPageAdmin> addRequest(request, *this);
					addRequest.getAction()->setParent(const_pointer_cast<Webpage>(_page));

					AdminActionFunctionRequest<RemoveObjectAction, WebPageAdmin> deleteRequest(request, *this);

					AdminActionFunctionRequest<WebPageMoveAction, WebPageAdmin> moveRequest(request, *this);

					AdminActionFunctionRequest<WebpageContentUploadAction, WebPageAdmin> uploadRequest(request, *this);
					uploadRequest.getAction()->setUp(_page);

					WebPageAdmin::DisplaySubPages(stream, _page->getKey(), addRequest, deleteRequest, moveRequest, uploadRequest, request);
				}

				stream << "<h1>Changement de site</h1>";
				{
					AdminActionFunctionRequest<WebPageUpdateAction, WebPageAdmin> siteMoveRequest(request, *this);
					siteMoveRequest.getAction()->setWebPage(const_pointer_cast<Webpage>(_page));
					PropertiesHTMLTable siteMoveForm(siteMoveRequest.getHTMLForm("site_move"));
					stream << siteMoveForm.open();
					stream << siteMoveForm.cell(
						"Site",
						siteMoveForm.getForm().getSelectInput(
							WebPageUpdateAction::PARAMETER_SITE_ID,
							WebsiteTableSync::Search(Env::GetOfficialEnv()),
							optional<boost::shared_ptr<Website> >(
								Env::GetOfficialEnv().getEditableSPtr(_page->getRoot())
							)
						)
					);
					
					stream << siteMoveForm.close();
				}
			}

			////////////////////////////////////////////////////////////////////
			// TAB LINKS
			if (openTabContent(stream, TAB_LINKS))
			{
				stream << "<h1>Liens</h1>";

				AdminFunctionRequest<WebPageAdmin> openRequest(request, *this);

				AdminActionFunctionRequest<WebPageLinkAddAction, WebPageAdmin> addRequest(request, *this);
				addRequest.getAction()->setPage(const_pointer_cast<Webpage>(_page));

				AdminActionFunctionRequest<WebPageLinkRemoveAction, WebPageAdmin> removeRequest(request, *this);
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
				BOOST_FOREACH(const WebpageLinks::Type::value_type& link, _page->get<WebpageLinks>())
				{
					openRequest.getPage()->setPage(Env::GetOfficialEnv().getEditableSPtr(link));
					removeRequest.getAction()->setDestinationPage(Env::GetOfficialEnv().getEditableSPtr(link));

					stream << t.row();
					stream << t.col() << link->getKey();
					stream << t.col() << link->get<Title>();
					stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), ICON);
					stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le lien ?", "/admin/img/page_delete.png");
				}

				stream << t.row();
				stream << t.col() << f.getTextInputAutoCompleteFromTableSync(
					WebPageLinkAddAction::PARAMETER_DESTINATION_ID,
					string(),
					string(),
					lexical_cast<string>(WebPageTableSync::TABLE.ID),
					string(),string(),
					false, true, true, true
				);

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



		AdminInterfaceElement::PageLinks WebPageAdmin::getSubPages( const AdminInterfaceElement& currentPage, const server::Request& request ) const
		{
			AdminInterfaceElement::PageLinks links;

			WebPageTableSync::SearchResult pages(WebPageTableSync::Search(Env::GetOfficialEnv(), _page->getRoot()->getKey(), _page->getKey()));
			BOOST_FOREACH(const boost::shared_ptr<Webpage>& page, pages)
			{
				boost::shared_ptr<WebPageAdmin> p(
					getNewPage<WebPageAdmin>()
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
				boost::shared_ptr<WebPageAdmin> p(
					getNewPage<WebPageAdmin>()
				);
				p->setPage(Env::GetOfficialEnv().getSPtr(_page->getParent()));
				PageLinks links(p->getCurrentTreeBranch());
				links.push_back(p);
				return links;
			}
			else
			{
				boost::shared_ptr<WebsiteAdmin> p(
					getNewPage<WebsiteAdmin>()
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
			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Arborescence", TAB_TREE, true));
			_tabs.push_back(Tab("Liens", TAB_LINKS, true));

			_tabBuilded = true;
		}


		void WebPageAdmin::_displaySubPages(
			std::ostream& stream,
			const WebPageTableSync::SearchResult& pages,
			StaticActionRequest<RemoveObjectAction>& deleteRequest,
			StaticActionRequest<WebPageMoveAction>& moveRequest,
			const server::Request& request,
			HTMLTable& t,
			HTMLForm& f,
			size_t depth
		){

			AdminFunctionRequest<WebPageAdmin> openRequest(request);
			StaticFunctionRequest<WebPageDisplayFunction> viewRequest(request, false);

			for(WebPageTableSync::SearchResult::const_iterator it(pages.begin()); it != pages.end(); ++it)
			{
				boost::shared_ptr<Webpage> page(*it);

				openRequest.getPage()->setPage(const_pointer_cast<const Webpage>(page));
				viewRequest.getFunction()->setPage(page.get());
				moveRequest.getAction()->setPage(page);

				if(	!page->getRoot()->get<ClientURL>().empty()
				){
					viewRequest.setClientURL(page->getRoot()->get<ClientURL>());
				}
				deleteRequest.getAction()->setObjectId(page->getKey());

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
						stream << HTMLModule::getHTMLLink(moveRequest.getURL(), HTMLModule::getHTMLImage("/admin/img/arrow_up.png", "up"));
					}
					stream << t.col();
					if(it+1 != pages.end())
					{
						moveRequest.getAction()->setUp(false);
						stream << HTMLModule::getHTMLLink(moveRequest.getURL(), HTMLModule::getHTMLImage("/admin/img/arrow_down.png", "down"));
					}
				}
				else
				{
					stream << t.col();
					stream << t.col();
				}
				stream << t.col() << page->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + WebPageAdmin::ICON);
				stream << t.col() << HTMLModule::getLinkButton(viewRequest.getURL(), "Voir", string(), "/admin/img/page_go.png");

				WebPageTableSync::SearchResult result(
					WebPageTableSync::Search(
						Env::GetOfficialEnv(),
						optional<RegistryKeyType>(),
						page->getKey()
				)	);

				stream << t.col();
				if(result.empty())
				{
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la page "+ page->getName() +" ?", "/admin/img/page_delete.png");
				}

				_displaySubPages(stream, result, deleteRequest, moveRequest, request, t, f, depth+1);
			}
		}



		void WebPageAdmin::DisplaySubPages(
			std::ostream& stream,
			RegistryKeyType parentId,
			server::StaticActionRequest<WebPageAddAction>& createRequest,
			server::StaticActionRequest<RemoveObjectAction>& deleteRequest,
			server::StaticActionRequest<WebPageMoveAction>& moveRequest,
			server::StaticActionRequest<WebpageContentUploadAction>& uploadRequest,
			const server::Request& request
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

			stream << "<h1>Téléchargement rapide</h1>";

			PropertiesHTMLTable f2(uploadRequest.getHTMLForm("upload_creation"));
			stream << f2.open();
			stream << f2.cell("Fichier", f2.getForm().getFileInput(WebpageContentUploadAction::PARAMETER_CONTENT));
			stream << f2.close();
		}
}	}
