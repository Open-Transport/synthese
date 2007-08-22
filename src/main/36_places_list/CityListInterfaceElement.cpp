
/** CityListInterfaceElement class implementation.
	@file CityListInterfaceElement.cpp

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

#include "CityListInterfaceElement.h"

#include <string>
#include <vector>

#include "36_places_list/Types.h"
#include "36_places_list/PlacesListItemInterfacePage.h"

#include "01_util/Conversion.h"

#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/Interface.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace interfaces;

	namespace transportwebsite
	{
		string CityListInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request
		) const	{
			const PlacesList& tbCommunes = *static_cast<const PlacesList*>(object);
			const PlacesListItemInterfacePage* page(_page->getInterface()->getPage<PlacesListItemInterfacePage>());
			
			int i(0);
			for (PlacesList::const_iterator it(tbCommunes.begin()); it != tbCommunes.end(); ++it, ++i)
				page->display(
					stream
					, variables
					, i
					, it->second
					, it->first
				);
			return string();
		}

		void CityListInterfaceElement::storeParameters( interfaces::ValueElementList& pv)
		{
		}
	}
}

