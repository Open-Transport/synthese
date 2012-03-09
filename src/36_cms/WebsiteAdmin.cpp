
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
#include "WebsiteTableSync.hpp"
#include "Website.hpp"
#include "WebsiteUpdateAction.hpp"
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
			const admin::AdminRequest& _request
		) const	{

			////////////////////////////////////////////////////////////////////
			// TAB PROPERTIES
			if (openTabContent(stream, TAB_PROPERTIES))
			{

				// Requests
				AdminActionFunctionRequest<WebsiteUpdateAction, WebsiteAdmin> updateRequest(
					_request
				);
				updateRequest.getAction()->setSiteId(_site->getKey());

				// Display
				stream << "<h1>Propri�t�s</h1>";
				PropertiesHTMLTable pt(updateRequest.getHTMLForm());
				stream << pt.open();
				stream << pt.title("Identification");
				stream << pt.cell("Nom", pt.getForm().getTextInput(WebsiteUpdateAction::PARAMETER_NAME, _site->get<Name>()));
				stream << pt.cell("D�but validit�", pt.getForm().getCalendarInput(WebsiteUpdateAction::PARAMETER_START_DATE, _site->get<StartDate>()));
				stream << pt.cell("Fin validit�", pt.getForm().getCalendarInput(WebsiteUpdateAction::PARAMETER_END_DATE, _site->get<EndDate>()));
				stream << pt.cell("URL", pt.getForm().getTextInput(WebsiteUpdateAction::PARAMETER_CLIENT_URL, _site->get<ClientURL>()));
				stream << pt.cell(
					"Mod�le de page par d�faut",
					pt.getForm().getTextInput(
						WebsiteUpdateAction::PARAMETER_DEFAULT_PAGE_TEMPLATE_ID,
						lexical_cast<string>(_site->get<DefaultTemplate>() ? _site->get<DefaultTemplate>()->getKey() : RegistryKeyType(0))
				)	);
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

			_tabs.push_back(Tab("Propri�t�s", TAB_PROPERTIES, true));
			_tabs.push_back(Tab("Pages web", TAB_WEB_PAGES, true));

			_tabBuilded = true;
		}
	}
}
