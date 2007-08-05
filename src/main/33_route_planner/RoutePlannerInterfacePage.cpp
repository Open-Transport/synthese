
/** RoutePlannerInterfacePage class implementation.
	@file RoutePlannerInterfacePage.cpp

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

#include "RoutePlannerInterfacePage.h"

#include "04_time/Date.h"

namespace synthese
{
	using namespace interfaces;
	using namespace env;

	namespace routeplanner
	{

		void RoutePlannerInterfacePage::display(
			std::ostream& stream
			, VariablesMap& variables
			, const JourneyBoardJourneys& object /*= NULL*/
			, const time::Date& date
			, const server::Request* request /*= NULL*/
			) const
		{
			const void* vobj(static_cast<const void*>(&object));
			ParametersVector pv;
			pv.push_back(date.toInternalString());

			InterfacePage::display(stream, pv, variables, vobj, request);
		}
	}
}
