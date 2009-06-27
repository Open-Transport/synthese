
/** PeriodsListInterfaceElement class implementation.
	@file PeriodsListInterfaceElement.cpp

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

#include "PeriodsListInterfaceElement.h"

#include "36_places_list/Site.h"
#include "36_places_list/HourPeriod.h"

#include "33_route_planner/RoutePlannerFunction.h"

#include "30_server/Request.h"

#include "11_interfaces/ValueElementList.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace transportwebsite;
	using namespace util;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::PeriodsListInterfaceElement>::FACTORY_KEY("route_planner_periods");

	namespace routeplanner
	{
		void PeriodsListInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_current = vel.front();
		}

		string PeriodsListInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			shared_ptr<const RoutePlannerFunction> function(request->getFunction<RoutePlannerFunction>());

			assert(function.get());

			const Site::Periods& periods(function->getSite()->getPeriods());

			int current(Conversion::ToInt(_current->getValue(parameters, variables, object, request)));

			stream << "<select name=\"" << RoutePlannerFunction::PARAMETER_PERIOD_ID << "\">";
			for(size_t i(0); i<periods.size(); ++i)
			{
				stream << "<option value=\"" << i << "\"";
				if (i == current)
					stream << " selected=\"selected\"";
				stream << ">" << periods.at(i).getCaption() << "</option>";
			}
			stream << "</select>";

			return string();
		}

		PeriodsListInterfaceElement::~PeriodsListInterfaceElement()
		{
		}
	}
}
