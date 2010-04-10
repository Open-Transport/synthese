
/** SuperiorValueInterfaceElement class implementation.
	@file SuperiorValueInterfaceElement.cpp

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

#include "SuperiorValueInterfaceElement.h"
#include "Interface.h"
#include "ValueElementList.h"
#include "InterfacePageException.h"

#include <string>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<LibraryInterfaceElement, SuperiorValueInterfaceElement>::FACTORY_KEY = ">";
	}

	namespace interfaces
	{
		string SuperiorValueInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object
			, const server::Request* request) const
		{
			stream << (boost::lexical_cast<RegistryKeyType>(_left->getValue(parameters, variables, object, request)) > boost::lexical_cast<RegistryKeyType>(_right->getValue(parameters, variables, object, request))) ? "1" : "0";
			return string();
		}

		void SuperiorValueInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() != 2)
				throw InterfacePageException("2 parameters must be provided");

			_left = vel.front();
			_right = vel.front();
		}

	}

}
