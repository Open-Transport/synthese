
/** AdminInterfaceElement class implementation.
	@file AdminInterfaceElement.cpp

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

#include <sstream>

#include "05_html/HTMLModule.h"

#include "30_server/FunctionRequest.h"
#include "30_server/ParametersMap.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/HomeAdmin.h"

#include "11_interfaces/Interface.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace server;
	using namespace html;
	using namespace util;

	namespace admin
	{

		AdminInterfaceElement::AdminInterfaceElement(const Args& args)
			: _pageLink(args.factoryKey, args.defaultIcon, args.defaultName)
		{
		}

		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			for (Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); it != Factory<AdminInterfaceElement>::end(); ++it)
			{
				if (it->isAuthorized(static_cast<const FunctionRequest<AdminRequest>* >(request)))
				{
					PageLinks l(it->getSubPagesOfParent(_pageLink, currentPage, static_cast<const FunctionRequest<AdminRequest>* >(request)));
					links.insert(links.end(), l.begin(), l.end());
				}
			}
			return links;
		}

		const AdminInterfaceElement::PageLink& AdminInterfaceElement::getPageLink() const
		{
			return _pageLink;
		}

		shared_ptr<AdminInterfaceElement> AdminInterfaceElement::GetAdminPage( const PageLink& pageLink )
		{
			boost::shared_ptr<AdminInterfaceElement> page(Factory<AdminInterfaceElement>::create(pageLink.factoryKey));
			if (!pageLink.parameterName.empty())
			{
				ParametersMap parameter;
				parameter.insert(pageLink.parameterName, pageLink.parameterValue);
				page->setFromParametersMap(parameter);
			}
			return page;
		}

		void AdminInterfaceElement::_buildTree(
			const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			// Cleaning
			_tree.subPages.clear();
			_treePosition.clear();

			// Initialisation
			auto_ptr<HomeAdmin> homeAdmin(new HomeAdmin);
			_tree.pageLink = homeAdmin->getPageLink();
			_tree.subPages = _buildTreeRecursion(_tree.pageLink, request, PageLinks());
		}

		vector<AdminInterfaceElement::PageLinksTree> AdminInterfaceElement::_buildTreeRecursion(
			const PageLink page
			, const server::FunctionRequest<admin::AdminRequest>* request
			, PageLinks position
		) const {
			vector<AdminInterfaceElement::PageLinksTree> trees;

			// Position
			position.push_back(page);
			if (page == _pageLink)
				_treePosition = position;

			// Tree
			PageLinks pages = GetAdminPage(page)->getSubPages(*this, request);

			// Recursion
			for (AdminInterfaceElement::PageLinks::const_iterator it = pages.begin(); it != pages.end(); ++it)
			{
				PageLinksTree tree;
				tree.pageLink = *it;
				tree.subPages = _buildTreeRecursion(*it, request, position);
				trees.push_back(tree);
			}
			return trees;
		}

		const AdminInterfaceElement::PageLinks& AdminInterfaceElement::getTreePosition(const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			if (_treePosition.empty())
				_buildTree(request);
			return _treePosition;
		}

		const AdminInterfaceElement::PageLinksTree& AdminInterfaceElement::getTree(const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			if (_treePosition.empty())
				_buildTree(request);
			return _tree;
		}

		AdminInterfaceElement::PageLink::PageLink( const std::string& _factoryKey  , const std::string& _defaultIcon  , const std::string& _defaultName )
			: name(_defaultName)
			, icon(_defaultIcon)
			, factoryKey(_factoryKey)
		{

		}

		AdminInterfaceElement::PageLink::PageLink()
		{

		}

		bool AdminInterfaceElement::PageLink::operator==(const AdminInterfaceElement::PageLink& other) const
		{
			return factoryKey == other.factoryKey && parameterName == other.parameterName && parameterValue == other.parameterValue;
		}

		std::string AdminInterfaceElement::PageLink::getURL( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			Request r;
			r.getInternalParameters().insert(QueryString::PARAMETER_FUNCTION, AdminRequest::FACTORY_KEY);
			r.getInternalParameters().insert(QueryString::PARAMETER_SESSION, request->getSession()->getKey());
			r.getInternalParameters().insert(AdminRequest::PARAMETER_PAGE, factoryKey);
			r.getInternalParameters().insert(AdminRequest::PARAMETER_INTERFACE, request->getFunction()->getInterface()->getKey());
			if (!parameterName.empty())
				r.getInternalParameters().insert(parameterName, parameterValue);
			return r.getURL();
		}

		AdminInterfaceElement::Args::Args(const std::string& _factoryKey, const std::string& _defaultIcon, const std::string& _defaultName )
			: defaultIcon(_defaultIcon)
			, defaultName(_defaultName)
			, factoryKey(_factoryKey)
		{
		
		}

		AdminInterfaceElement::Args::Args()

		{

		}
	}
}
