
/** ParseDisplayReturnInterfacePage class implementation.
	@file ParseDisplayReturnInterfacePage.cpp

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

#include "ParseDisplayReturnInterfacePage.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace departure_boards;

	namespace util
	{
		 template<> const string FactorableTemplate<InterfacePage, ParseDisplayReturnInterfacePage>::FACTORY_KEY("parse_display_return");
	}

	namespace departure_boards
	{
		const string ParseDisplayReturnInterfacePage::DATA_STRING("string");

		void ParseDisplayReturnInterfacePage::display(std::ostream& stream, const std::string& text, VariablesMap& variables, const server::Request* request /*= NULL*/) const
		{
			ParametersVector pv;
			pv.push_back(text);

			InterfacePage::_display(stream, pv, variables, NULL, request);
		}



		ParseDisplayReturnInterfacePage::ParseDisplayReturnInterfacePage()
			: Registrable(0)
		{

		}
	}
}
