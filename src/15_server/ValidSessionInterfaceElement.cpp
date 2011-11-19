
/** ValidSessionInterfaceElement class implementation.
	@file ValidSessionInterfaceElement.cpp
	@author Hugues Romain
	@date 2007

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

#include "ValidSessionInterfaceElement.h"

#include "Request.h"
#include "Session.h"
#include "ValueElementList.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, ValidSessionInterfaceElement>::FACTORY_KEY("valid_session");
	}

	namespace server
	{
		void ValidSessionInterfaceElement::storeParameters(ValueElementList& vel)
		{
			//_parameter1 = vel.front();
			/// @todo control and Fill the parameters init
		}

		string ValidSessionInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			if (request->getSession())
				stream << request->getSession()->getKey();
			return string();
		}

		ValidSessionInterfaceElement::~ValidSessionInterfaceElement()
		{
		}
	}
}
