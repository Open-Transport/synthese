////////////////////////////////////////////////////////////////////////////////
/// AdminTabsInterfaceElement class implementation.
///	@file AdminTabsInterfaceElement.cpp
///	@author Hugues Romain
///	@date 2008-12-24 19:44
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "AdminTabsInterfaceElement.h"
#include "AdminInterfaceElement.h"
#include "ValueElementList.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, admin::AdminTabsInterfaceElement>::FACTORY_KEY("admin_tabs");
	}

	namespace admin
	{
		void AdminTabsInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		string AdminTabsInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			const AdminInterfaceElement* aie(static_cast<const AdminInterfaceElement*>(object));
			const AdminRequest* adminRequest(dynamic_cast<const AdminRequest*>(request));
			assert(aie && adminRequest);

			if(aie && adminRequest)
			{
				aie->displayTabs(
					stream,
					variables,
					*adminRequest
				);
			}
			return string();
		}

		AdminTabsInterfaceElement::~AdminTabsInterfaceElement()
		{
		}
	}
}
