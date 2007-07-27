
/** SubstrValueInterfaceElement class implementation.
	@file SubstrValueInterfaceElement.cpp

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

#include <string>

#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "SubstrValueInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		// template<> const string FactoryTemplate<ValueInterfaceElement, SubstrValueInterfaceElement>::FACTORY_KEY("");
	}

	namespace interfaces
	{
		string SubstrValueInterfaceElement::getValue(const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object, const server::Request* request) const
		{
			string text = _string->getValue(parameters, variables, object, request);
			int start = Conversion::ToInt(_start->getValue(parameters, variables, object, request));
			int length = Conversion::ToInt(_length->getValue(parameters, variables, object, request));

			return text.substr(start, length);
		}

		void SubstrValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 3)
				throw InterfacePageException("3 parameters must be provided");

			_string = vel.front();
			_start = vel.front();
			_length = vel.front();
		}
	}

}
