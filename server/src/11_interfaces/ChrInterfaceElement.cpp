
/** ChrInterfaceElement class implementation.
	@file ChrInterfaceElement.cpp

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

#include "ChrInterfaceElement.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, ChrInterfaceElement>::FACTORY_KEY("chr");
	}

	namespace interfaces
	{
		string ChrInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters,
              interfaces::VariablesMap& vars,
              const void* rootObject,
			const server::Request* request
		) const	{
			stream << static_cast<char>(atoi (_asciiCode->getValue (parameters, vars, rootObject, request).c_str ()));
            return string();
		}



		void ChrInterfaceElement::storeParameters(ValueElementList& vel )
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed chr command");

			_asciiCode = vel.front();
		}
	}
}

