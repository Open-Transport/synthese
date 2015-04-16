
/** IfThenElseInterfaceElement class implementation.
	@file IfThenElseInterfaceElement.cpp

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

#include "IfThenElseInterfaceElement.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/StaticValueInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, IfThenElseInterfaceElement>::FACTORY_KEY = "if";
	}

	namespace interfaces
	{
		string IfThenElseInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			return _criteria->isFalse(parameters, variables, object, request)
				? _to_return_if_false->display(stream, parameters, variables, object, request)
				: _to_return_if_true->display(stream, parameters, variables, object, request);
		}

		void IfThenElseInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.size() < 2)
				throw InterfacePageException("Conditional element without value to return");

			_criteria = vel.front();
			_to_return_if_true = vel.front();
			if (vel.isEmpty())
				_to_return_if_false.reset(new StaticValueInterfaceElement(""));
			else
				_to_return_if_false = vel.front();
		}
	}
}

