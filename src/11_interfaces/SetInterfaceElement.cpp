
/** SetInterfaceElement class implementation.
	@file SetInterfaceElement.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "SetInterfaceElement.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

namespace synthese
{
	using namespace interfaces;

	template<> const std::string util::FactorableTemplate<LibraryInterfaceElement,SetInterfaceElement>::FACTORY_KEY("set");

	namespace interfaces
	{
		void SetInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() != 2)
				throw InterfacePageException("Bad parameters for variable setting element");

			_varName = vel.front();
			_varValue = vel.front();
		}

		std::string SetInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			if (!_varName->getValue(parameters, variables, object, request).empty())
				variables[_varName->getValue(parameters, variables, object, request)] = _varValue->getValue(parameters, variables, object, request);
			return "";
		}
	}
}
