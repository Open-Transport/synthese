
/** ParameterValueInterfaceElement class implementation.
	@file ParameterValueInterfaceElement.cpp

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

#include "ParameterValueInterfaceElement.h"
#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "StaticValueInterfaceElement.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, ParameterValueInterfaceElement>::FACTORY_KEY = "param";
	}

	namespace interfaces
	{
		string ParameterValueInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object, const server::Request* request) const
		{
			unsigned int value(lexical_cast<unsigned int>(_rank->getValue(parameters, variables, object, request)));
			if (value >= parameters.size())
				Log::GetInstance().warn("Invalid parameter rank in interface module");
			else
				stream << parameters[value];
			return string();
		}

		void ParameterValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() != 1)
				throw InterfacePageException("Malformed parameter interface element");
			_rank = vel.front();
		}

		ParameterValueInterfaceElement::ParameterValueInterfaceElement(
			const std::string& key
		):	_rank(new StaticValueInterfaceElement(key))
		{

		}

		ParameterValueInterfaceElement::ParameterValueInterfaceElement()
			: _rank()
		{

		}
	}
}

