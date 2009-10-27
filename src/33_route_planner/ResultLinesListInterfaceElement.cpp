
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


#include "ResultLinesListInterfaceElement.h"
#include "PTRoutePlannerResult.h"
#include "LineMarkerInterfacePage.h"
#include "RoutePlanner.h"
#include "ServiceUse.h"
#include "Service.h"
#include "Line.h"
#include "CommercialLine.h"
#include "Journey.h"
#include "Request.h"
#include "ValueElementList.h"
#include "Interface.h"

#include <set>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace graph;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::ResultLinesListInterfaceElement>::FACTORY_KEY("result_lines_list");

	namespace routeplanner
	{
		string ResultLinesListInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const	{
			// Read the result
			const PTRoutePlannerResult* jv = static_cast<const PTRoutePlannerResult*>(object);
			if(jv == NULL) return string();

			// Fetch the line display page
			const LineMarkerInterfacePage* lineInterfacePage(
				_page->getInterface()->getPage<LineMarkerInterfacePage>(
					_pageCode.get() ? _pageCode->getValue(parameters, variables, object, request) : string()
			)	);

			// Selection of the lines to display
			set<const CommercialLine*> lines;
			BOOST_FOREACH(shared_ptr<Journey> journey, jv->getJourneys())
			{
				BOOST_FOREACH(const ServiceUse& service, journey->getServiceUses())
				{
					if(dynamic_cast<const Line*>(service.getService()->getPath()))
					{
						lines.insert(static_cast<const Line*>(service.getService()->getPath())->getCommercialLine());
					}
				}
			}

			// Display of each line
			BOOST_FOREACH(const CommercialLine* line, lines)
			{
				lineInterfacePage->display(stream, variables, string(), string(), 0, 0, *line, request);
			}

			return string();
		}

		void ResultLinesListInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			if(!vel.isEmpty())
			{
				_pageCode = vel.front();
			}
		}
	}
}
