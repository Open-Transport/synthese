
/** IncludePageInterfaceElement class implementation.
	@file IncludePageInterfaceElement.cpp

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

#include "IncludePageInterfaceElement.h"

#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"

using namespace boost;
using namespace std;

namespace synthese
{
	namespace interfaces
	{
		std::string IncludePageInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object
			, const server::Request* request) const
		{
			shared_ptr<const InterfacePage> page_to_include = _page->getInterface()->getPage(_page_code->getValue(parameters, variables, object, request));
			page_to_include->display(stream, _parameters.fillParameters( parameters, variables, object, request ), variables, object, request);
			return string();
		}

		void IncludePageInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.isEmpty())
				throw InterfacePageException("Included page not specified");

			_page_code = vel.front();
			_parameters.takeFrom(vel, _page);
		}
	}
}
