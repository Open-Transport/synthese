
/** AdminPagesTreeInterfaceElement class implementation.
	@file AdminPagesTreeInterfaceElement.cpp

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

#include "11_interfaces/ValueElementList.h"

#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminPagesTreeInterfaceElement.h"

namespace synthese
{
	using namespace std;
	using namespace interfaces;
	using namespace util;
	using namespace server;

	namespace admin
	{
		void AdminPagesTreeInterfaceElement::storeParameters(ValueElementList& vel)
		{
			//_parameter1 = vel.front();
			/// @todo control and Fill the parameters init
		}

		std::string AdminPagesTreeInterfaceElement::getValue( const ParametersVector&, interfaces::VariablesMap& variables, const void* object /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			return getSubPages("", (const AdminInterfaceElement*) object, (const AdminRequest*) request);
		}

		AdminPagesTreeInterfaceElement::~AdminPagesTreeInterfaceElement()
		{
			/// @todo Destroy all the parameters
			//delete _parameter1;
		}

		/** @todo Put the html code as parameters */
		std::string AdminPagesTreeInterfaceElement::getSubPages( const std::string& page, const AdminInterfaceElement* currentPage, const AdminRequest* request)
		{
			stringstream str;
			for (Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); it != Factory<AdminInterfaceElement>::end(); ++it)
			{
				if (it->getSuperior() == page 
					&& ((it->getDisplayMode() == AdminInterfaceElement::EVER_DISPLAYED)
					|| (it->getDisplayMode() == AdminInterfaceElement::DISPLAYED_IF_CURRENT && it.getKey() == currentPage->getFactoryKey()))
				){
					str << "<li>";
					if (it.getKey() == currentPage->getFactoryKey())
					{
						str << currentPage->getTitle();
					}
					else
					{
						str << it->getHTMLLink(request);	
					}
					str << "</li>";

					string sp = getSubPages(it->getFactoryKey(), currentPage, request);

					if (sp.size() > 0)
					{
						str << "<ul style=\"margin:0px 0px 0px 15px\">" << sp << "</ul>";
					}
				}
			}
			return str.str();
		}
	}
}

