
/** DeparturesTableRoutePlanningRowKeyInterfacePage class implementation.
	@file DeparturesTableRoutePlanningRowKeyInterfacePage.cpp
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

#include "DeparturesTableRoutePlanningRowKeyInterfacePage.h"
#include "StopArea.hpp"
#include "City.h"
#include "StopPoint.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace departure_boards;

	namespace util
	{
		template<> const string FactorableTemplate<InterfacePage, departure_boards::DeparturesTableRoutePlanningRowKeyInterfacePage>::FACTORY_KEY("departures_table_route_planning_row_key");
	}

	namespace departure_boards
	{
		DeparturesTableRoutePlanningRowKeyInterfacePage::DeparturesTableRoutePlanningRowKeyInterfacePage()
			: Registrable(0),
			  FactorableTemplate<interfaces::InterfacePage, DeparturesTableRoutePlanningRowKeyInterfacePage>()
		{
		}



		void DeparturesTableRoutePlanningRowKeyInterfacePage::display(
			std::ostream& stream,
			const pt::StopArea& origin,
			const RoutePlanningRow& row ,
			VariablesMap& variables,
			const server::Request* request /*= NULL*/
		) const	{
			ParametersVector v;
			v.push_back(row.first->getCity()->getName());
			v.push_back(lexical_cast<string>(row.first->getCity()->getKey() != origin.getCity()->getKey()));
			v.push_back(row.first->getName());
			v.push_back(row.first->getName13());
			v.push_back(row.first->getName26());

			InterfacePage::_display(
				stream
				, v
				, variables
				, static_cast<const void*>(&row)
				, request
			);
		}
	}
}
