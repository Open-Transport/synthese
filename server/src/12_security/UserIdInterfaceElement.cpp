
/** UserIdInterfaceElement class implementation.
	@file UserIdInterfaceElement.cpp
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

#include "UserIdInterfaceElement.h"

#include "Request.h"
#include "User.h"
#include "ValueElementList.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, security::UserIdInterfaceElement>::FACTORY_KEY("user_id");
	}

	namespace security
	{
		void UserIdInterfaceElement::storeParameters(ValueElementList& vel)
		{
		}

		string UserIdInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			boost::shared_ptr<const User> user(request->getUser());
			if (user)
				stream << user->getKey();

			return string();
		}

		UserIdInterfaceElement::~UserIdInterfaceElement()
		{
		}
	}
}
