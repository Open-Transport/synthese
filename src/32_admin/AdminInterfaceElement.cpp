
/// AdminInterfaceElement class implementation.
///	@file AdminInterfaceElement.cpp
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

#include <sstream>
#include <boost/foreach.hpp>
#include <assert.h>

#include "HTMLModule.h"
#include "FunctionRequest.h"
#include "AdminRequest.h"
#include "HomeAdmin.h"
#include "Interface.h"
#include "Profile.h"
#include "User.h"
#include "Session.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace html;
	using namespace util;
	using namespace security;
	

	namespace admin
	{
		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPages(
			const AdminInterfaceElement& currentPage
		) const	{
			AdminInterfaceElement::PageLinks links;
			if (!currentPage.isAuthorized()) return links;

			PageLink currentPageLink(getPageLink());
			Factory<AdminInterfaceElement>::ObjectsCollection pages(Factory<AdminInterfaceElement>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<AdminInterfaceElement> page, pages)
			{
				page->setRequest(_request);
				if (page->isAuthorized())
				{
					PageLinks l(page->getSubPagesOfParent(currentPageLink, currentPage));
					links.insert(links.end(), l.begin(), l.end());
				}
				else if(currentPage.getFactoryKey() == page->getFactoryKey())
				{
					PageLinks l(currentPage.getSubPagesOfParent(currentPageLink, currentPage));
					links.insert(links.end(), l.begin(), l.end());
				}
			}
			return links;
		}


		AdminInterfaceElement::PageLink AdminInterfaceElement::getPageLink() const
		{
			PageLink link;
			link.factoryKey = getFactoryKey();
			link.icon = getIcon();
			link.name = getTitle();
			link.parameterName = getParameterName();
			link.parameterValue = getParameterValue();
			link.request = _request;
			return link;
		}
		
		
		
		void AdminInterfaceElement::_buildTree(
		) const {
			// Cleaning
			_tree.subPages.clear();
			_treePosition.clear();

			// Initialisation
			auto_ptr<HomeAdmin> homeAdmin(new HomeAdmin);
			homeAdmin->setRequest(_request);
			PageLinks position;
			PageLink homeLink(homeAdmin->getPageLink());
			position.push_back(homeLink);
			_tree = _buildTreeRecursion(homeAdmin.get(), position);
			if (homeLink == getPageLink())
				_treePosition = position;
		}



		AdminInterfaceElement::PageLinksTree AdminInterfaceElement::_buildTreeRecursion(
			const AdminInterfaceElement* adminPage,
			PageLinks position
		) const {

			// Local variables
			AdminInterfaceElement::PageLinksTree	tree;
			
			// Tree
			tree.pageLink = adminPage->getPageLink();
			PageLink currentLink = getPageLink();
			
			// Recursion
			PageLinks pages = adminPage->getSubPages(*this);
			BOOST_FOREACH(AdminInterfaceElement::PageLink link, pages)
			{
				position.push_back(link);
				if (link == currentLink)
				{
					_treePosition = position;
					tree.isNodeOpened = true;
				}
				
				try
				{
					auto_ptr<AdminInterfaceElement>			subPage(link.getAdminPage());
					AdminInterfaceElement::PageLinksTree	subTree(_buildTreeRecursion(subPage.get(), position));

					if (link == currentLink)
						subTree.isNodeOpened = true;

					tree.subPages.push_back(subTree);
					if (!tree.isNodeOpened)
						tree.isNodeOpened = subPage->isPageVisibleInTree(*this) || subTree.isNodeOpened;
				}
				catch(...)
				{
					continue;
				}

				position.pop_back();
			}

			return tree;
		}

		const AdminInterfaceElement::PageLinks& AdminInterfaceElement::getTreePosition() const
		{
			if (_treePosition.empty())
			{
				_buildTree();
			}
			return _treePosition;
		}

		const AdminInterfaceElement::PageLinksTree& AdminInterfaceElement::getTree() const
		{
			if (_treePosition.empty())
			{
				_buildTree();
			}
			return _tree;
		}

		std::string AdminInterfaceElement::getParameterName() const
		{
			return std::string();
		}



		std::string AdminInterfaceElement::getParameterValue() const
		{
			return std::string();
		}



		bool AdminInterfaceElement::isPageVisibleInTree(const AdminInterfaceElement& currentPage) const
		{
			return false;
		}



		void AdminInterfaceElement::setRequest(
			const server::FunctionRequest<admin::AdminRequest>* value
		) {
			_request = value;
		}



		void AdminInterfaceElement::displayTabs(
			std::ostream& stream, interfaces::VariablesMap& variables
		) const {
			assert(_request != NULL);

			if (!_tabBuilded) _buildTabs();

			if (_tabs.empty()) return;

			bool first(true);
			stream << "<div id=\"admin_tabs\">";
			BOOST_FOREACH(const Tab& tab, _tabs)
			{
				stream << "<span id=\"tab_" << tab.getId() << "\"";
				if(_activeTab.empty() && first || tab.getId() == _activeTab)
				{
					stream << " class=\"active_tab\"";
				}
				stream << " onclick=\"activateTab(this);\">";
				if (!tab.getIcon().empty())
				{
					stream  << HTMLModule::getHTMLImage(tab.getIcon(), tab.getTitle()) << "&nbsp;";
				}
				stream << tab.getTitle() << "</span>";
				first = false;
			}
			stream << "</div>";
			stream << HTMLModule::GetHTMLJavascriptOpen();
			stream << "function activateTab(tab) {";
			stream << "var nodes = document.getElementById('admin_tabs').childNodes;";
			stream << "for(var i=0; i<nodes.length; ++i) {";
			stream << "if(nodes[i] == tab) { nodes[i].className = 'active_tab';";
			stream << "document.getElementById(nodes[i].id+'_content').style.display='block'; }";
			stream << "else { nodes[i].className = '';";
			stream << "document.getElementById(nodes[i].id+'_content').style.display='none'; }";
			stream << "}";
			stream << "}" << HTMLModule::GetHTMLJavascriptClose();
		}



		AdminInterfaceElement::AdminInterfaceElement(
		):	_request(NULL),
			_tabBuilded(false),
			_currentTab(NULL)
		{
		}



		void AdminInterfaceElement::_buildTabs(
		) const {
			_tabBuilded = true;
		}



		bool AdminInterfaceElement::openTabContent(
			ostream& stream,
			const std::string& key
		) const {
			closeTabContent(stream);

			if (!_tabBuilded)
			{
				_buildTabs();
			}

			bool first(true);
			BOOST_FOREACH(const Tab& rtab, _tabs)
			{
				if(rtab.getId() == key)
				{
					_currentTab = &rtab;
					break;
				}
				first = false;
			}
			if (_currentTab == NULL)
			{
				return false;
			}

			stream << "<div class=\"tabdiv ";
			if (_activeTab.empty() && first || _activeTab == _currentTab->getId())
			{
				stream << "active_tab_content";
			}
			stream << "\" id=\"tab_" << _currentTab->getId() << "_content\">";
			return true;
		}



		void AdminInterfaceElement::closeTabContent(
			std::ostream& stream
		) const {
			if (_currentTab != NULL)
			{
				stream << "</div>";
				_currentTab = NULL;
			}
		}



		bool AdminInterfaceElement::tabHasWritePermissions(
		) const {
			assert(_currentTab != NULL);
			return _currentTab->getWritePermission();
		}



		void AdminInterfaceElement::setActiveTab(
			const std::string& value
		){
			_activeTab = value;
		}


		bool AdminInterfaceElement::PageLink::operator==(const AdminInterfaceElement::PageLink& other) const
		{
			return
				factoryKey == other.factoryKey &&
				parameterName == other.parameterName &&
				parameterValue == other.parameterValue;
		}



		std::string AdminInterfaceElement::PageLink::getURL(
		) const	{
			assert(request);
			assert(request->getSession());
			assert(request->getFunction());
			assert(request->getFunction()->getInterface());

			Request r;
			r.getInternalParameters().insert(Request::PARAMETER_FUNCTION, AdminRequest::FACTORY_KEY);
			r.getInternalParameters().insert(Request::PARAMETER_SESSION, request->getSession()->getKey());
			r.getInternalParameters().insert(AdminRequest::PARAMETER_PAGE, factoryKey);
			r.getInternalParameters().insert(AdminRequest::PARAMETER_INTERFACE, request->getFunction()->getInterface()->getKey());
			if (!parameterName.empty())
				r.getInternalParameters().insert(parameterName, parameterValue);
			return r.getURL();
		}



		AdminInterfaceElement* AdminInterfaceElement::PageLink::getAdminPage(
		) const {
			AdminInterfaceElement* page(Factory<AdminInterfaceElement>::create(factoryKey));
			page->setRequest(request);
			if (!parameterName.empty())
			{
				ParametersMap parameter;
				parameter.insert(parameterName, parameterValue);
				page->setFromParametersMap(parameter, false);
			}
			return page;
		}



		AdminInterfaceElement::Tab::Tab(
			std::string title,
			std::string id,
			bool writePermission,
			string icon
		):	_title(title),
			_id(id),
			_writePermission(writePermission),
			_icon(icon)
		{
		}



		const std::string& AdminInterfaceElement::Tab::getTitle(
		) const {
			return _title;
		}



		const std::string& AdminInterfaceElement::Tab::getId(
		) const {
			return _id;
		}



		bool AdminInterfaceElement::Tab::getWritePermission(
		) const {
			return _writePermission;
		}



		std::string AdminInterfaceElement::Tab::getIcon(
		) const {
			return _icon;
		}



		std::string AdminInterfaceElement::getTabLinkButton(
			const string& tab
		) const {
			if (!_tabBuilded)
			{
				_buildTabs();
			}

			BOOST_FOREACH(const Tab& rtab, _tabs)
			{
				if(rtab.getId() == tab)
				{
					return HTMLModule::getLinkButton("activateTab(document.getElementById('tab_"+ rtab.getId() +"'));", rtab.getTitle(), string(), rtab.getIcon(), true);
				}
			}
			return string();
		}



		std::string AdminInterfaceElement::getCurrentTab(
		) const {
			return (_currentTab == NULL) ? string() : _currentTab->getId();
		}



		const std::string& AdminInterfaceElement::getActiveTab(
		) const {
			return _activeTab;
		}



		const Request* AdminInterfaceElement::getRequest(
		) const {
			return _request;
		}
	}
}
