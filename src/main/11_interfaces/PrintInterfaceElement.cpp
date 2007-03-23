
/** PrintInterfaceElement class implementation.
	@file PrintInterfaceElement.cpp

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

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/PrintInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		std::string PrintInterfaceElement::display(std::ostream& stream, const interfaces::ParametersVector& parameters, VariablesMap& vars, const void* rootObject /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			stream << _toBePrinted->getValue(parameters, vars, rootObject, request);
			return "";
		}

		PrintInterfaceElement::~PrintInterfaceElement()
		{
			delete _toBePrinted;
		}

		void PrintInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed print command");
			_toBePrinted = vel.front();
		}
	}
}

