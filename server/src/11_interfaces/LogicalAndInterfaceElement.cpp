
/** LogicalAndInterfaceElement class implementation.
	@file LogicalAndInterfaceElement.cpp
	@author Hugues
	@date 2009

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

#include "LogicalAndInterfaceElement.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, interfaces::LogicalAndInterfaceElement>::FACTORY_KEY("&");
	}

	namespace interfaces
	{
		void LogicalAndInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_parameters = vel;
		}

		string LogicalAndInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			ValueElementList vel(_parameters);
			try
			{
				int number(lexical_cast<int>(vel.front()->getValue(parameters, variables, object, request)));

				while (!vel.isEmpty())
				{
					number &= lexical_cast<int>(vel.front()->getValue(parameters, variables, object, request));
				}
				stream << number;
			}
			catch(bad_lexical_cast)
			{
				stream << "0";
			}

			return string();
		}

		LogicalAndInterfaceElement::~LogicalAndInterfaceElement()
		{
		}
	}
}
