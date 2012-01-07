
/// AdminInterfaceElement class implementation.
///	@file AdminInterfaceElement.cpp
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

#include <sstream>
#include <boost/foreach.hpp>
#include <assert.h>

#include "AdminInterfaceElement.h"
#include "HTMLModule.h"
#include "StaticFunctionRequest.h"
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
		AdminInterfaceElement::AdminInterfaceElement(
		):	_tabBuilded(false),
			_currentTab(NULL)
		{}



		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& _request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	currentPage.getCurrentTreeBranch().find(*this))
			{
				links.push_back(currentPage.getCurrentTreeBranch().getNextSubPage(*this));
			}

			return links;
		}



		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPagesOfModule(
			const server::ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& _request
		) const {
			return PageLinks();
		}



		void AdminInterfaceElement::_buildTree(
			const admin::AdminRequest& request
		) const {
			// Cleaning
			_tree.subPages.clear();

			// Initialisation
			shared_ptr<HomeAdmin> homeAdmin(
				getNewPage<HomeAdmin>()
			);

			PageLinks position;
			position.push_back(homeAdmin);
			_tree = _buildTreeRecursion(homeAdmin, position, request);
			if (dynamic_cast<const HomeAdmin*>(this))
			{
				_treePosition = position;
			}
		}



		bool AdminInterfaceElement::operator==(const AdminInterfaceElement& other) const
		{
			return getFactoryKey() == other.getFactoryKey() && _hasSameContent(other);
		}


		bool AdminInterfaceElement::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return true;
		}


		AdminInterfaceElement::PageLinksTree AdminInterfaceElement::_buildTreeRecursion(
			shared_ptr<const AdminInterfaceElement> adminPage,
			PageLinks position,
			const admin::AdminRequest& request
		) const {

			// Local variables
			AdminInterfaceElement::PageLinksTree tree(adminPage);

			// Recursion
			PageLinks pages(adminPage->getSubPages(*this, request));
			BOOST_FOREACH(const shared_ptr<const AdminInterfaceElement>& link, pages)
			{
				position.push_back(link);
				if (*link == *this)
				{
					_treePosition = position;
					tree.isNodeOpened = true;
				}

				try
				{
					AdminInterfaceElement::PageLinksTree subTree(
						_buildTreeRecursion(link, position, request)
					);

					if (*link == *this)
						subTree.isNodeOpened = true;

					tree.subPages.push_back(subTree);
					if (!tree.isNodeOpened)
					{
						tree.isNodeOpened =
							(link)->isPageVisibleInTree(*this, request) ||
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
			const admin::AdminRequest& request
		) const	{
			if (_treePosition.empty())
			{
				_buildTree(request);
			}
			return _treePosition;
		}

		const AdminInterfaceElement::PageLinksTree& AdminInterfaceElement::getTree(
			const admin::AdminRequest& request
		) const
		{
			if (_treePosition.empty())
			{
				_buildTree(request);
			}
			return _tree;
		}



		bool AdminInterfaceElement::isPageVisibleInTree(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			return false;
		}



		void AdminInterfaceElement::displayTabs(
			std::ostream& stream,
			const admin::AdminRequest& request
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
			const security::Profile&
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



		const AdminInterfaceElement::PageLinks& AdminInterfaceElement::getCurrentTreeBranch() const
		{
			if(!_theoreticalTreePosition)
			{
				_theoreticalTreePosition = _getCurrentTreeBranch();
			}
			return *_theoreticalTreePosition;
		}




		bool AdminInterfaceElement::PageLinks::find( const AdminInterfaceElement& page ) const
		{
			BOOST_FOREACH(const Links::value_type& link, _links)
			{
				if(*link == page) return true;
			}
			return false;
		}



		AdminInterfaceElement::PageLinks::const_iterator AdminInterfaceElement::PageLinks::begin() const
		{
			return _links.begin();
		}



		AdminInterfaceElement::PageLinks::iterator AdminInterfaceElement::PageLinks::begin()
		{
			return _links.begin();
		}



		AdminInterfaceElement::PageLinks::const_iterator AdminInterfaceElement::PageLinks::end() const
		{
			return _links.end();
		}



		AdminInterfaceElement::PageLinks::iterator AdminInterfaceElement::PageLinks::end()
		{
			return _links.end();
		}



		void AdminInterfaceElement::PageLinks::pop_back()
		{
			_links.pop_back();
		}



		bool AdminInterfaceElement::PageLinks::empty() const
		{
			return _links.empty();
		}



		boost::shared_ptr<const AdminInterfaceElement> AdminInterfaceElement::PageLinks::getNextSubPage( const AdminInterfaceElement& page ) const
		{
			for(Links::const_iterator it(_links.begin()); it+1 != _links.end(); ++it)
			{
				if(**it == page) return *(it+1);
			}
			return shared_ptr<const AdminInterfaceElement>();
		}
	}
}
