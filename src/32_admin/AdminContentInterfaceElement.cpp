
/** AdminContentInterfaceElement class implementation.
	@file AdminContentInterfaceElement.cpp

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

#include "ValueElementList.h"
#include "StaticFunctionRequest.h"
#include "AdminInterfaceElement.h"
#include "AdminInterfaceElement.h"
#include "AdminContentInterfaceElement.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace server;

	template<> const string util::FactorableTemplate<interfaces::LibraryInterfaceElement,admin::AdminContentInterfaceElement>::FACTORY_KEY("admincontent");

	namespace admin
	{
		void AdminContentInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		string AdminContentInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* rootObject /*= NULL*/,
			const server::Request* request /*= NULL*/ ) const
		{
			const AdminInterfaceElement* aie = static_cast<const AdminInterfaceElement*>(rootObject);
			const AdminRequest* adminRequest(
				dynamic_cast<const AdminRequest*>(request)
			);
			assert(aie && adminRequest);
			
			if(aie && adminRequest)
			{
				aie->display(
					stream,
					variables,
					*adminRequest
				);
			}
			return string();
		}
	}
}

