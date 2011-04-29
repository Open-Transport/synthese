
/** DebugInterfaceElement class implementation.
	@file DebugInterfaceElement.cpp
	@author Hugues Romain
	@date 2007

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

#include "DebugInterfaceElement.h"
#include "ValueElementList.h"

#ifdef WIN32
	#include <CRTDBG.h>
#endif

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, interfaces::DebugInterfaceElement>::FACTORY_KEY("debug");
	}

	namespace interfaces
	{
		void DebugInterfaceElement::storeParameters(ValueElementList& vel)
		{
			while(!vel.isEmpty())
				_parameters.push_back(vel.front());
		}

		string DebugInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

#ifdef WIN32
			_CrtDbgBreak();
#endif

			return string();
		}

		DebugInterfaceElement::~DebugInterfaceElement()
		{
		}
	}
}
