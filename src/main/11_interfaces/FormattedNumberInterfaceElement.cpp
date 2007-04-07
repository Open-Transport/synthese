
/** DeparturesTableDisplayNumberContentInterfaceElement class implementation.
@file DeparturesTableDisplayNumberContentInterfaceElement.cpp

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

#include <sstream>
#include <complex>

#include "01_util/Conversion.h"

#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/FormattedNumberInterfaceElement.h"
#include "11_interfaces/InterfacePageException.h"
#include "11_interfaces/StaticValueInterfaceElement.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace interfaces
	{
		const std::string FormattedNumberInterfaceElement::TYPE_CHAR_2 = "char(2)";
		const std::string FormattedNumberInterfaceElement::TYPE_IDENTICAL = "identical";

		FormattedNumberInterfaceElement::FormattedNumberInterfaceElement()
			: _numberVIE(NULL)
			, _formatVIE(NULL)
			, _numberToAdd(NULL)
		{

		}

		FormattedNumberInterfaceElement::~FormattedNumberInterfaceElement()
		{
			delete _numberVIE;
			delete _formatVIE;
			delete _numberToAdd;
		}

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
				_formatVIE = new StaticValueInterfaceElement(TYPE_IDENTICAL);
            }

			if (vel.size ())
			{
				_numberToAdd = vel.front();
			}
		}

		std::string FormattedNumberInterfaceElement::getValue(const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			int __Nombre = Conversion::ToInt(_numberVIE->getValue(parameters, variables, object, request));
			if (_numberToAdd != NULL)
			{
				__Nombre += Conversion::ToInt(_numberToAdd->getValue(parameters, variables, object, request));
			}
			std::string __Format = _formatVIE->getValue(parameters, variables, object, request);
			stringstream s;

			int numbers = Conversion::ToInt(__Format);
			if (numbers > 0)
			{
				for (; numbers; numbers--)
				{
					if (__Nombre < pow(10.0, numbers - 1))
						s << "0";
				}
				s << __Nombre;
				return s.str();
			}

			if (__Format == TYPE_CHAR_2)
			{
				__Nombre = __Nombre % 100;
				s << __Nombre / 10;
				s << __Nombre % 10;
				return s.str();
			}

			s << __Nombre;
			return s.str();
		}
	}
}
