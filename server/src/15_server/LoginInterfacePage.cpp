
/** LoginInterfacePage class implementation.
	@file LoginInterfacePage.cpp
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

#include "LoginInterfacePage.h"

#include "Function.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, server::LoginInterfacePage>::FACTORY_KEY("login");
	}

	namespace server
	{
		const std::string LoginInterfacePage::DATA_FUNCTION_KEY_IF_SUCCESS("function_if_success");
		const std::string LoginInterfacePage::DATA_FUNCTION_PARAMETER("function_parameters");
		const std::string LoginInterfacePage::DATA_TITLE("title");

		void LoginInterfacePage::display(
			std::ostream& stream
			, const Function* functionIfSucceed
			, const string& title
			, VariablesMap& variables
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;

			pv.push_back(functionIfSucceed->getFactoryKey());

			// URI
			stringstream uri;
			functionIfSucceed->_getParametersMap().outputURI(uri);
			pv.push_back(uri.str());

			// Title
			pv.push_back(title);

			InterfacePage::_display(
				stream
				, pv
				, variables
				, NULL
				, request
			);
		}

		LoginInterfacePage::LoginInterfacePage()
		:	Registrable(0)
		{

		}
	}
}
