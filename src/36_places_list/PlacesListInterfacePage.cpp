
/** PlacesListInterfacePage class implementation.
	@file PlacesListInterfacePage.cpp

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

#include "PlacesListInterfacePage.h"
#include "City.h"
#include "Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace transportwebsite;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, PlacesListInterfacePage>::FACTORY_KEY("places_list");
	}

	namespace transportwebsite
	{
		const std::string PlacesListInterfacePage::PARAMETER_IS_CITY_LIST("is_city_list");
		const std::string PlacesListInterfacePage::PARAMETER_IS_FOR_ORIGIN("is_for_origin");

		const std::string PlacesListInterfacePage::DATA_CITY_ID("city_id");
		const std::string PlacesListInterfacePage::DATA_CITY_NAME("city_name");
		const std::string PlacesListInterfacePage::DATA_RESULTS_SIZE("size");

		void PlacesListInterfacePage::display(
			std::ostream& stream
			, VariablesMap& variables
			, const PlacesList& results
			, bool isCities
			, bool isForOrigin
			, const City* city
			, const server::Request* request /*= NULL*/) const
		{
			ParametersVector pv;
			pv.push_back(Conversion::ToString(isCities));
			pv.push_back(Conversion::ToString(isForOrigin));
			pv.push_back(Conversion::ToString(results.size()));
			pv.push_back((isCities || !city) ? string() : Conversion::ToString(city->getKey()));
			pv.push_back((isCities || !city) ? string() : Conversion::ToString(city->getName()));

			InterfacePage::_display(stream, pv, variables, static_cast<const void*>(&results), request);
		}



		PlacesListInterfacePage::PlacesListInterfacePage()
			: Registrable(0)
		{

		}
	}
}
