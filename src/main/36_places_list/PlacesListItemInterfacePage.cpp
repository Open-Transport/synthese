
/** PlacesListItemInterfacePage class implementation.
	@file PlacesListItemInterfacePage.cpp

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

#include "PlacesListItemInterfacePage.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace transportwebsite;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, PlacesListItemInterfacePage>::FACTORY_KEY("places_list_item");
	}

	namespace transportwebsite
	{

		void PlacesListItemInterfacePage::display(
			std::ostream& stream
			, VariablesMap& variables
			, int n
			, const std::string& name
			, uid id
			, const server::Request* request /*= NULL*/
		) const	{
			ParametersVector pv;
			pv.push_back(Conversion::ToString(n));
			pv.push_back(name);
			pv.push_back(Conversion::ToString(id));

			InterfacePage::display(stream, pv, variables, NULL, request);
		}
	}
}
