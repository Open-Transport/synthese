
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

#include "HTMLModule.h"

#include "FunctionRequest.h"
#include "QueryString.h"

#include "AdminRequest.h"
#include "HomeAdmin.h"

#include "Interface.h"

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
		:	FactoryBase<AdminInterfaceElement>()
		{
		}

		AdminInterfaceElement::PageLinks AdminInterfaceElement::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			PageLink currentPageLink(getPageLink());
			for (Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); it != Factory<AdminInterfaceElement>::end(); ++it)
			{
				if (it->isAuthorized(static_cast<const FunctionRequest<AdminRequest>* >(request)))
				{
					PageLinks l(it->getSubPagesOfParent(currentPageLink, currentPage, static_cast<const FunctionRequest<AdminRequest>* >(request)));
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
			return link;
		}
		
		
		
		AdminInterfaceElement* AdminInterfaceElement::GetAdminPage( const PageLink& pageLink )
		{
			AdminInterfaceElement* page(Factory<AdminInterfaceElement>::create(pageLink.factoryKey));
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
			PageLinks position;
			PageLink homeLink(homeAdmin->getPageLink());
			position.push_back(homeLink);
			_tree = _buildTreeRecursion(homeAdmin.get(), request, position);
			if (homeLink == getPageLink())
				_treePosition = position;
		}



		AdminInterfaceElement::PageLinksTree AdminInterfaceElement::_buildTreeRecursion(
			const AdminInterfaceElement* adminPage
			, const server::FunctionRequest<admin::AdminRequest>* request
			, PageLinks position
		) const {

			// Local variables
			AdminInterfaceElement::PageLinksTree	tree;
			
			// Tree
			tree.pageLink = adminPage->getPageLink();
			
			// Recursion
			PageLinks pages = adminPage->getSubPages(*this, request);
			for (AdminInterfaceElement::PageLinks::const_iterator it = pages.begin(); it != pages.end(); ++it)
			{
				position.push_back(*it);
				if (*it == getPageLink())
				{
					_treePosition = position;
					tree.isNodeOpened = true;
				}
				
				auto_ptr<AdminInterfaceElement>			subPage(GetAdminPage(*it));
				AdminInterfaceElement::PageLinksTree	subTree(_buildTreeRecursion(subPage.get(), request, position));

				if (*it == getPageLink())
					subTree.isNodeOpened = true;

				tree.subPages.push_back(subTree);
				if (!tree.isNodeOpened)
					tree.isNodeOpened = subPage->isPageVisibleInTree(*this) || subTree.isNodeOpened;

				position.pop_back();
			}

			return tree;
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


		bool AdminInterfaceElement::PageLink::operator==(const AdminInterfaceElement::PageLink& other) const
		{
			return factoryKey == other.factoryKey && parameterName == other.parameterName && parameterValue == other.parameterValue;
		}



		std::string AdminInterfaceElement::PageLink::getURL(
			server::FunctionRequest<admin::AdminRequest> const* request
		) const	{
			Request r;
			r.getInternalParameters().insert(QueryString::PARAMETER_FUNCTION, AdminRequest::FACTORY_KEY);
			r.getInternalParameters().insert(QueryString::PARAMETER_SESSION, request->getSession()->getKey());
			r.getInternalParameters().insert(AdminRequest::PARAMETER_PAGE, factoryKey);
			r.getInternalParameters().insert(AdminRequest::PARAMETER_INTERFACE, request->getFunction()->getInterface()->getKey());
			if (!parameterName.empty())
				r.getInternalParameters().insert(parameterName, parameterValue);
			return r.getURL();
		}



		AdminInterfaceElement::Args::Args(const std::string& _defaultIcon, const std::string& _defaultName )
			: defaultIcon(_defaultIcon)
			, defaultName(_defaultName)
		{
		
		}

		AdminInterfaceElement::Args::Args()

		{

		}
	}
}
