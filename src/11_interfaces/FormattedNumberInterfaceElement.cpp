
/** FormattedNumberInterfaceElement class implementation.
	@file FormattedNumberInterfaceElement.cpp

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

#include "FormattedNumberInterfaceElement.h"


#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/StaticValueInterfaceElement.h"

#include "01_util/Conversion.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const std::string FactorableTemplate<LibraryInterfaceElement, FormattedNumberInterfaceElement>::FACTORY_KEY = "format";
	}

	namespace interfaces
	{
		const std::string FormattedNumberInterfaceElement::TYPE_CHAR_2 = "char(2)";
		const std::string FormattedNumberInterfaceElement::TYPE_IDENTICAL = "identical";

		void FormattedNumberInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 1)
				throw InterfacePageException("Malformed number declaration");

			_numberVIE = vel.front();

			if (vel.size ())
			{
				_formatVIE = vel.front();
			}
			else
            {
				_formatVIE.reset(new StaticValueInterfaceElement(TYPE_IDENTICAL));
            }

			if (vel.size ())
			{
				_numberToAdd = vel.front();
			}
		}

		string FormattedNumberInterfaceElement::display(
			ostream& s
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			int __Nombre = lexical_cast<int>(_numberVIE->getValue(parameters, variables, object, request));
			if (_numberToAdd != NULL)
			{
				__Nombre += lexical_cast<int>(_numberToAdd->getValue(parameters, variables, object, request));
			}
			std::string __Format = _formatVIE->getValue(parameters, variables, object, request);

			int numbers = lexical_cast<int>(__Format);
			if (numbers > 0)
			{
				s << Conversion::ToFixedSizeString(__Nombre, numbers);
				return string();
			}

			if (__Format == TYPE_CHAR_2)
			{
				__Nombre = __Nombre % 100;
				s << __Nombre / 10;
				s << __Nombre % 10;
				return string();
			}

			s << __Nombre;
			return string();
		}
}	}
