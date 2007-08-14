
/** AdminPagePositionInterfaceElement class implementation.
	@file AdminPagePositionInterfaceElement.cpp

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

#include "11_interfaces/ValueElementList.h"

#include "30_server/FunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminPagePositionInterfaceElement.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace server;
	using namespace html;
	using namespace admin;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, AdminPagePositionInterfaceElement>::FACTORY_KEY("adminpos");
	}

	namespace admin
	{
		std::string AdminPagePositionInterfaceElement::getUpPages(shared_ptr<const AdminInterfaceElement> page, const server::FunctionRequest<admin::AdminRequest>* request, bool isFirst)
		{
			stringstream str;
			Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); 
			for (; it != Factory<AdminInterfaceElement>::end() && it->getFactoryKey() != page->getSuperiorVirtual(); ++it);
			
			if (it != Factory<AdminInterfaceElement>::end())
			{
				str << getUpPages(*it, request, false);
				str << "&nbsp;&gt;&nbsp;";
			}
			str << HTMLModule::getHTMLImage(page->getIcon(), page->getTitle());
			if (!isFirst)
			{
				FunctionRequest<AdminRequest> r(request);
				r.getFunction()->setPage(page);
				str << HTMLModule::getHTMLLink(r.getURL(), page->getTitle());
			}
			else
			{
				str << page->getTitle();
			}			
			return str.str();
		}

		void AdminPagePositionInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{

		}

		std::string AdminPagePositionInterfaceElement::display(
			ostream& stream
			, const ParametersVector&
			, interfaces::VariablesMap& variables, const void* object /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			const shared_ptr<const AdminInterfaceElement>* page = (const shared_ptr<const AdminInterfaceElement>*) object;
			stream << getUpPages(
				*page
				, (const server::FunctionRequest<admin::AdminRequest>*) request
			);
			return string();
		}
	}
}

