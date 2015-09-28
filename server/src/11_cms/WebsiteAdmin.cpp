
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

#include "ActionResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"
#include "AjaxForm.hpp"
#include "CMSModule.hpp"
#include "CMSRight.hpp"
#include "EditArea.hpp"
#include "HTMLForm.h"
#include "HTMLList.h"
#include "HTMLModule.h"
#include "ModuleAdmin.h"
#include "ObjectUpdateAction.hpp"
#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "RemoveObjectAction.hpp"
#include "ResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "ServerModule.h"
#include "Session.h"
#include "StaticFunctionRequest.h"
#include "ObjectUpdateAction.hpp"
#include "User.h"
#include "WebPageAdmin.h"
#include "WebPageAddAction.h"
#include "Website.hpp"
#include "WebsiteConfigTableSync.hpp"
#include "WebsiteTableSync.hpp"
#include "WebPageDisplayFunction.h"
#include "WebPageMoveAction.hpp"
#include "WebpageContentUploadAction.hpp"

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
	using namespace cms;
	using namespace db;
	using namespace html;
	using namespace security;
	using namespace server;
	using namespace util;

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
		const string WebsiteAdmin::TAB_CONFIG("config");



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
			_pageSearchParameter.setFromParametersMap(map, PARAMETER_SEARCH_RANK, optional<size_t>());
		}



		util::ParametersMap WebsiteAdmin::getParametersMap() const
		{
			ParametersMap m(_pageSearchParameter.getParametersMap());

			m.insert(PARAMETER_SEARCH_PAGE, _searchPage);
			if(_site.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _site->getKey());
			}

			return m;
		}



		void WebsiteAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<ObjectUpdateAction, WebsiteAdmin> updateRequest(
					request,
					*this
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
					"Host name",
					pt.getForm().getTextInput(ObjectUpdateAction::GetInputName<HostName>(), _site->get<HostName>())
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
				stream << pt.close();
			}


			////////////////////////////////////////////////////////////////////
			// TAB WEB PAGES
			if (openTabContent(stream, TAB_WEB_PAGES))
			{
				stream << "<h1>Pages</h1>";
				AdminActionFunctionRequest<WebPageAddAction, WebsiteAdmin> addRequest(request, *this);
				addRequest.getAction()->setSite(const_pointer_cast<Website>(_site));

				AdminActionFunctionRequest<RemoveObjectAction, WebsiteAdmin> deleteRequest(request, *this);

				AdminActionFunctionRequest<WebPageMoveAction, WebsiteAdmin> moveRequest(request, *this);

				AdminActionFunctionRequest<WebpageContentUploadAction, WebsiteAdmin> uploadRequest(request, *this);
				uploadRequest.getAction()->setSite(_site);

				WebPageAdmin::DisplaySubPages(stream, _site->getKey(), addRequest, deleteRequest, moveRequest, uploadRequest, request);
			}

			////////////////////////////////////////////////////////////////////
			// TAB CONFIG
			if (openTabContent(stream, TAB_CONFIG))
			{
				stream << "<h1>Configuration du site</h1>";

				// Creation of a config object if necessary
				if(!_site->getConfig())
				{
					WebsiteConfig c;
					c.set<Website>(optional<Website&>(const_cast<Website&>(*_site)));
					WebsiteConfigTableSync::Save(&c);
					assert(_site->getConfig());
				}

				AdminActionFunctionRequest<ObjectUpdateAction, WebsiteAdmin> updateConfigRequest(request, *this);
				updateConfigRequest.getAction()->setObject(*_site->getConfig());
				
				AjaxForm f(updateConfigRequest.getAjaxForm("update_config"));
				stream << f.open();
				EditArea editArea(stream);
				editArea.getAjaxForm(
					stream,
					updateConfigRequest.getURL(),
					ObjectUpdateAction::PARAMETER_FIELD_PREFIX + ConfigScript::FIELD.name,
					_site->getConfig()->get<ConfigScript>().getCode(),
					20,
					80
				);
				stream << f.getSubmitButton("Sauvegarder");
				stream << f.close();
			}

			////////////////////////////////////////////////////////////////////
			/// END TABS
			closeTabContent(stream);
		}



		bool WebsiteAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<CMSRight>(READ);
		}



		AdminInterfaceElement::PageLinks WebsiteAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
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
				BOOST_FOREACH(const boost::shared_ptr<Website>& site, sites)
				{
					boost::shared_ptr<WebsiteAdmin> p(
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
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	currentPage == *this ||
				dynamic_cast<const WebPageAdmin*>(&currentPage)
			){
				WebPageTableSync::SearchResult pages(WebPageTableSync::Search(Env::GetOfficialEnv(), _site->getKey(), RegistryKeyType(0)));
				BOOST_FOREACH(const boost::shared_ptr<Webpage>& page, pages)
				{
					boost::shared_ptr<WebPageAdmin> p(
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
			_tabs.push_back(Tab("Configuration", TAB_CONFIG, true));

			_tabBuilded = true;
		}
	}
}
