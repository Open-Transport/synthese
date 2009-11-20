
/** schedulesheetlineslistlineinterfaceelement class header.
	@file schedulesheetlineslistlineinterfaceelement.cpp

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

#include "ScheduleSheetLinesListLineInterfaceElement.h"
#include "PTRoutePlannerResult.h"
#include "RoutePlannerSheetLinesCellInterfacePage.h"

#include "Request.h"

#include "Interface.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace graph;
	using namespace ptrouteplanner;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::ScheduleSheetLinesListLineInterfaceElement>::FACTORY_KEY("schedules_lines");
	
	namespace routeplanner
	{
		string ScheduleSheetLinesListLineInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const PTRoutePlannerResult* jv = static_cast<const PTRoutePlannerResult*>(object);
			const RoutePlannerSheetLinesCellInterfacePage* linesInterfacePage = _page->getInterface()->getPage<RoutePlannerSheetLinesCellInterfacePage>();

			int n = 1;
			BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, jv->getJourneys())
			{
				linesInterfacePage->display( stream, n, variables, &journey, request );
				++n;
			}

			return string();
		}

		void ScheduleSheetLinesListLineInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{

		}
	}
}
