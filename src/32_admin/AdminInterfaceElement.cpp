
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

#include "AdminInterfaceElement.h"
#include "HTMLModule.h"
#include "FunctionRequest.h"
#include "HomeAdmin.h"
#include "Interface.h"
#include "Profile.h"
#include "User.h"
#include "Session.h"
#include "AdminRequest.h"

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
		AdminInterfaceElement::AdminInterfaceElement(
		):	_tabBuilded(false),
			_currentTab(NULL)
		{
		}
		
		
		
		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPages(
			boost::shared_ptr<const AdminInterfaceElement> currentPage,
					const server::FunctionRequest<admin::AdminRequest>& _request
		) const	{
			return PageLinks();
		}


		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPagesOfModule(
			const std::string& moduleKey,
			boost::shared_ptr<const AdminInterfaceElement> currentPage,
					const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			return PageLinks();
		}



		void AdminInterfaceElement::_buildTree(
			const server::FunctionRequest<admin::AdminRequest>& request
		) const {
			// Cleaning
			_tree.subPages.clear();
			_treePosition.clear();

			// Initialisation
			shared_ptr<HomeAdmin> homeAdmin;
			if(dynamic_cast<const HomeAdmin*>(this))
			{
				homeAdmin = const_pointer_cast<HomeAdmin>(
					static_pointer_cast<const HomeAdmin, const AdminInterfaceElement>(
						shared_from_this()
				)	);
			}
			else
			{
				homeAdmin.reset(new HomeAdmin);
			}
			PageLinks position;
			AddToLinks(position, homeAdmin);
			_tree = _buildTreeRecursion(homeAdmin, position, request);
			if (homeAdmin == shared_from_this())
				_treePosition = position;
		}



		AdminInterfaceElement::PageLinksTree AdminInterfaceElement::_buildTreeRecursion(
			shared_ptr<const AdminInterfaceElement> adminPage,
			PageLinks position,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const {

			// Local variables
			AdminInterfaceElement::PageLinksTree tree(adminPage);
			shared_ptr<const AdminInterfaceElement> currentLink(shared_from_this());
			
			// Recursion
			PageLinks pages = adminPage->getSubPages(currentLink, request);
			BOOST_FOREACH(shared_ptr<const AdminInterfaceElement> link, pages)
			{
				AddToLinks(position, link);
				if (link == currentLink)
				{
					_treePosition = position;
					tree.isNodeOpened = true;
				}
				
				try
				{
					AdminInterfaceElement::PageLinksTree subTree(
						_buildTreeRecursion(link, position, request)
					);

					if (link == currentLink)
						subTree.isNodeOpened = true;

					tree.subPages.push_back(subTree);
					if (!tree.isNodeOpened)
					{
						tree.isNodeOpened =
							link->isPageVisibleInTree(*this) ||
							subTree.isNodeOpened
						;
					}
				}
				catch(...)
				{
					continue;
				}

				position.pop_back();
			}

			return tree;
		}

		const AdminInterfaceElement::PageLinks& AdminInterfaceElement::getTreePosition(
			const server::FunctionRequest<admin::AdminRequest>& request
		) const
		{
			if (_treePosition.empty())
			{
				_buildTree(request);
			}
			return _treePosition;
		}

		const AdminInterfaceElement::PageLinksTree& AdminInterfaceElement::getTree(
			const server::FunctionRequest<admin::AdminRequest>& request
		) const
		{
			if (_treePosition.empty())
			{
				_buildTree(request);
			}
			return _tree;
		}



		bool AdminInterfaceElement::isPageVisibleInTree(const AdminInterfaceElement& currentPage) const
		{
			return false;
		}



		void AdminInterfaceElement::displayTabs(
			std::ostream& stream,
			interfaces::VariablesMap& variables,
			const server::FunctionRequest<admin::AdminRequest>& request
		) const {
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



		void AdminInterfaceElement::_buildTabs(
			const server::FunctionRequest<admin::AdminRequest>& _request
		) const {
			_tabBuilded = true;
		}


		void AdminInterfaceElement::setEnv(
			boost::shared_ptr<util::Env> value
		){
			_env = value;
		}


		bool AdminInterfaceElement::openTabContent(
			ostream& stream,
			const std::string& key
		) const {
			closeTabContent(stream);

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

			stream << "<div class=\"tabdiv";
			if (_activeTab.empty() && first || _activeTab == _currentTab->getId())
			{
				stream << " active_tab_content";
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



		util::Env& AdminInterfaceElement::_getEnv() const
		{
			assert(_env.get());
			return *_env;
		}
		
		boost::shared_ptr<AdminInterfaceElement> AdminInterfaceElement::getNewPage() const
		{
			shared_ptr<AdminInterfaceElement> page(
				Factory<AdminInterfaceElement>::create(getFactoryKey())
			);
			page->setEnv(_env);
			page->setActiveTab(getCurrentTab());
			page->setFromParametersMap(
				getParametersMap(),
				false,
				false
			);
			return page;
		}

	}
}
