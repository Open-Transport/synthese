
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
#include "32_admin/AdminParametersException.h"

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
		void AdminPagePositionInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			if (vel.size() < 2)
				throw AdminParametersException("2 arguments needed");
			_normalSeparator = vel.front();
			_lastSeparator = vel.front();
		}

		std::string AdminPagePositionInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables, const void* object /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			const shared_ptr<const AdminInterfaceElement>* page = (const shared_ptr<const AdminInterfaceElement>*) object;
			string normalSeparator(_normalSeparator->getValue(parameters, variables, object, request));
			string lastSeparator(_lastSeparator->getValue(parameters, variables, object, request));

			const AdminInterfaceElement::PageLinks& links((*page)->getTreePosition(static_cast<const server::FunctionRequest<AdminRequest>*>(request)));

			for (AdminInterfaceElement::PageLinks::const_iterator it(links.begin()); it != links.end(); ++it)
			{
				if (it == links.end()-1)
					stream << lastSeparator;
				else if (it != links.begin())
					stream << normalSeparator;

				stream << HTMLModule::getHTMLImage(it->icon, it->name);
				if (it != (links.end() -1))
					stream << HTMLModule::getHTMLLink(it->getURL(static_cast<const server::FunctionRequest<AdminRequest>*>(request)), it->name);
				else
					stream << it->name;
			}
			return string();
		}
	}
}

