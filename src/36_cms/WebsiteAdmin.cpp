
/** WebsiteAdmin class implementation.
	@file WebsiteAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "WebsiteAdmin.hpp"

#include "CMSModule.hpp"
#include "ObjectUpdateAction.hpp"
#include "WebsiteTableSync.hpp"
#include "Website.hpp"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "ResultHTMLTable.h"
#include "PropertiesHTMLTable.h"
#include "HTMLModule.h"
#include "Interface.h"
#include "InterfaceTableSync.h"
#include "StaticFunctionRequest.h"
#include "Profile.h"
#include "SearchFormHTMLTable.h"
#include "HTMLForm.h"
#include "WebPageAdmin.h"
#include "ActionResultHTMLTable.h"
#include "WebPageAddAction.h"
#include "AdminFunctionRequest.hpp"
#include "WebPageDisplayFunction.h"
#include "WebPageMoveAction.hpp"
#include "RemoveObjectAction.hpp"
#include "ServerModule.h"

#include <geos/geom/Point.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace impex;
	using namespace security;
	using namespace html;
	using namespace cms;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, WebsiteAdmin>::FACTORY_KEY("Website");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<WebsiteAdmin>::ICON("layout.png");
		template<> const string AdminInterfaceElementTemplate<WebsiteAdmin>::DEFAULT_TITLE("Site inconnu");
	}

	namespace cms
	{
		const string WebsiteAdmin::PARAMETER_SEARCH_PAGE = "pp";
		const string WebsiteAdmin::PARAMETER_SEARCH_RANK = "pr";

		const string WebsiteAdmin::TAB_PROPERTIES("pr");
		const string WebsiteAdmin::TAB_WEB_PAGES("wp");
		const string WebsiteAdmin::TAB_SVN_STORAGE("sr");



		WebsiteAdmin::WebsiteAdmin():
			AdminInterfaceElementTemplate<WebsiteAdmin>()
		{}



		void WebsiteAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Site
			try
			{
				_site = WebsiteTableSync::Get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
					Env::GetOfficialEnv(),
					UP_LINKS_LOAD_LEVEL
				);
			}
			catch (...)
			{
				throw AdminParametersException("No such site");
			}

			// Search page
			_searchPage = map.getDefault<string>(PARAMETER_SEARCH_PAGE);
			_pageSearchParameter.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_RANK, optional<size_t>());

			// SVN storage
			_svnStorage._setFromParametersMap(map);
		}



		util::ParametersMap WebsiteAdmin::getParametersMap() const
		{
			ParametersMap m(_pageSearchParameter.getParametersMap());

			m.insert(PARAMETER_SEARCH_PAGE, _searchPage);
			if(_site.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _site->getKey());
			}

			m.merge(_svnStorage._getParametersMap());
			return m;
		}



		void WebsiteAdmin::display(
			ostream& stream,
			const admin::AdminRequest& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<ObjectUpdateAction, WebsiteAdmin> updateRequest(
					_request
				);
				updateRequest.getAction()->setObject(*_site);

				// Display
				stream << "<h1>Propriétés</h1>";
				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.title("Identification");
				stream << pt.cell(
					"Nom",
					pt.getForm().getTextInput(ObjectUpdateAction::GetInputName<Name>(), _site->get<Name>())
				);
				stream << pt.cell(
					"Début validité",
					pt.getForm().getCalendarInput(ObjectUpdateAction::GetInputName<StartDate>(), _site->get<StartDate>())
				);
				stream << pt.cell(
					"Fin validité",
					pt.getForm().getCalendarInput(ObjectUpdateAction::GetInputName<EndDate>(), _site->get<EndDate>())
				);
				stream << pt.cell(
					"URL",
					pt.getForm().getTextInput(ObjectUpdateAction::GetInputName<ClientURL>(), _site->get<ClientURL>())
				);
				stream << pt.cell(
					"Modèle de page par défaut",
					pt.getForm().getTextInput(
						ObjectUpdateAction::GetInputName<DefaultTemplate>(),
						lexical_cast<string>(_site->get<DefaultTemplate>() ? _site->get<DefaultTemplate>()->getKey() : RegistryKeyType(0))
				)	);
				stream << pt.title("SVN");
				stream << pt.cell(
					"URL",
					pt.getForm().getTextInput(ObjectUpdateAction::GetInputName<SVNURL>(), _site->get<SVNURL>())
				);
				stream << pt.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB WEB PAGES
			if (openTabContent(stream, TAB_WEB_PAGES))
			{
				stream << "<h1>Pages</h1>";
				AdminActionFunctionRequest<WebPageAddAction, WebsiteAdmin> addRequest(_request);
				addRequest.getAction()->setSite(const_pointer_cast<Website>(_site));

				AdminActionFunctionRequest<RemoveObjectAction, WebsiteAdmin> deleteRequest(_request);

				AdminActionFunctionRequest<WebPageMoveAction, WebsiteAdmin> moveRequest(_request);

				WebPageAdmin::DisplaySubPages(stream, _site->getKey(), addRequest, deleteRequest, moveRequest, _request);
			}


			////////////////////////////////////////////////////////////////////
			// TAB SVN STORAGE
			if (openTabContent(stream, TAB_SVN_STORAGE))
			{
				AdminFunctionRequest<WebsiteAdmin> svnStorageRequest(_request);

				stream << "<h1>SVN Storage</h1>";
				HTMLForm svnStorageForm = svnStorageRequest.getHTMLForm("svnStorage");
				
				PropertiesHTMLTable pt(svnStorageForm);
				stream << pt.open();
				stream << pt.title("SVN update ou commit");

				stream << pt.cell("ID datasource import", pt.getForm().getTextInput(SVNStorageFunction::PARAMETER_DATASOURCE_ID, ""));
				stream << pt.cell("ID d'objet", pt.getForm().getTextInput(SVNStorageFunction::PARAMETER_OBJECT_ID, lexical_cast<string>(_site->getKey())));

				string exportPath("");
				if(!ServerModule::GetSitesStoragePath().empty())
				{
					exportPath = ServerModule::GetSitesStoragePath() + "/" + lexical_cast<string>(_site->getKey());
				}

				stream << pt.cell("Chemin d'export", pt.getForm().getTextInput(SVNStorageFunction::PARAMETER_EXPORT_PATH, exportPath));
				stream << pt.cell("URL", pt.getForm().getTextInput(SVNStorageFunction::PARAMETER_URL, _site->get<SVNURL>()));
				stream << pt.cell("Utilisateur", pt.getForm().getTextInput(SVNStorageFunction::PARAMETER_USERNAME, _request.getUser()->getSVNUsername()));
				stream << pt.cell("Mot de passe", pt.getForm().getPasswordInput(SVNStorageFunction::PARAMETER_PASSWORD, _request.getUser()->getSVNPassword()));

				stream << pt.cell("Update", pt.getForm().getOuiNonRadioInput(SVNStorageFunction::PARAMETER_UPDATE, false));
				stream << pt.cell("Commit", pt.getForm().getOuiNonRadioInput(SVNStorageFunction::PARAMETER_COMMIT, false));
				stream << pt.cell("Message (si commit)", pt.getForm().getTextInput(SVNStorageFunction::PARAMETER_COMMIT_MESSAGE, ""));
				stream << pt.cell("Verbeux", pt.getForm().getOuiNonRadioInput(SVNStorageFunction::PARAMETER_VERBOSE, false));

				stream << pt.close();

				stream << "<h1>Résultat de la commande</h1>";

				stream << "<h2>Sortie</h2>";
				stream << "<pre style='font-size: 1.2em'>";
				ParametersMap result = _svnStorage.run(stream, _request);
				stream << "</pre>";

				if(result.isDefined(SVNStorageFunction::VALUE_SUCCESS))
				{
					stream << "<h2>Etat</h2>";
					if(result.get<bool>(SVNStorageFunction::VALUE_SUCCESS))
					{
						stream << "<p style='color: green'>Succès</p>";
					}
					else
					{
						stream << "<p style='color: red'>Echec</p>";
					}
				}
			}


			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		bool WebsiteAdmin::isAuthorized(
			const security::User& user
		) const	{
return true;
//			return user.getProfile()->isAuthorized<WebsiteRight>(READ);
		}



		AdminInterfaceElement::PageLinks WebsiteAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const CMSModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				WebsiteTableSync::SearchResult sites(
					WebsiteTableSync::Search(Env::GetOfficialEnv())
				);
				BOOST_FOREACH(const shared_ptr<Website>& site, sites)
				{
					shared_ptr<WebsiteAdmin> p(
						getNewPage<WebsiteAdmin>()
					);
					p->_site = const_pointer_cast<const Website>(site);
					links.push_back(p);
				}
			}
			return links;
		}


		AdminInterfaceElement::PageLinks WebsiteAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				dynamic_cast<const WebPageAdmin*>(&currentPage)
			){
				WebPageTableSync::SearchResult pages(WebPageTableSync::Search(Env::GetOfficialEnv(), _site->getKey(), RegistryKeyType(0)));
				BOOST_FOREACH(const shared_ptr<Webpage>& page, pages)
				{
					shared_ptr<WebPageAdmin> p(
						getNewPage<WebPageAdmin>()
					);
					p->setPage(const_pointer_cast<const Webpage>(page));
					links.push_back(p);
				}	}

			return links;
		}



		std::string WebsiteAdmin::getTitle() const
		{
			return _site.get() ? _site->getName() : DEFAULT_TITLE;
		}




		bool WebsiteAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _site->getKey() == static_cast<const WebsiteAdmin&>(other)._site->getKey();
		}



		void WebsiteAdmin::_buildTabs( const security::Profile& profile ) const
		{
			_tabs.clear();

			_tabs.push_back(Tab("Propriétés", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Pages web", TAB_WEB_PAGES, true));
			_tabs.push_back(Tab("SVN", TAB_SVN_STORAGE, true));

			_tabBuilded = true;
		}
	}
}
