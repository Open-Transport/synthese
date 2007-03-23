
/** ParameterValueInterfaceElement class implementation.
	@file ParameterValueInterfaceElement.cpp

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

#include "01_util/Conversion.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/ParameterValueInterfaceElement.h"

namespace synthese
{
	using namespace std;
	using namespace util;

	namespace interfaces
	{
		string ParameterValueInterfaceElement::getValue( const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object, const server::Request* request) const
		{
			return parameters[Conversion::ToInt(_rank->getValue(parameters, variables, object, request))] ;
		}

		void ParameterValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed parameter interface element");
			_rank = vel.front();
		}

	}
}

