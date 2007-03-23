
/** LineLabelInterfaceElement class implementation.
	@file LineLabelInterfaceElement.cpp

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
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/StaticValueInterfaceElement.h"
#include "11_interfaces/LineLabelInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		std::string LineLabelInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, VariablesMap& vars, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			return "";
		}

		void LineLabelInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed line label declaration");
			ValueInterfaceElement* vie = vel.front();
            if (dynamic_cast<StaticValueInterfaceElement*>(vie) == NULL)
				throw InterfacePageException("Line label must be statically defined");
			ParametersVector pv;
			VariablesMap vars;
			_label = vie->getValue(pv, vars);
		}
		const std::string& synthese::interfaces::LineLabelInterfaceElement::getLabel() const
		{
			return _label;
		}
	}
}

