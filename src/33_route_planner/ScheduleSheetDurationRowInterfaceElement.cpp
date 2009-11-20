
/** ScheduleSheetDurationRowInterfaceElement class implementation.
	@file ScheduleSheetDurationRowInterfaceElement.cpp

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

#include "ScheduleSheetDurationRowInterfaceElement.h"
#include "Journey.h"
#include "PTRoutePlannerResult.h"
#include "ValueElementList.h"
#include "DurationInterfacePage.h"
#include "Interface.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace ptrouteplanner;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::ScheduleSheetDurationRowInterfaceElement>::FACTORY_KEY("schedules_durations");

	namespace routeplanner
	{
		void ScheduleSheetDurationRowInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_cellHeader = vel.front();
			_cellFooter = vel.front();
		}

		string ScheduleSheetDurationRowInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			
			const PTRoutePlannerResult* jv = static_cast<const PTRoutePlannerResult*>(object);
			const DurationInterfacePage* durationInterfacePage = _page->getInterface()->getPage<DurationInterfacePage>();
			
			BOOST_FOREACH(const PTRoutePlannerResult::Journeys::value_type& journey, jv->getJourneys())
			{
				stream << _cellHeader->getValue(parameters, variables, object, request);
				durationInterfacePage->display(stream, journey.getDuration(), variables, object, request);
				stream << _cellFooter->getValue(parameters, variables, object, request);
			}

			return string();
		}

		ScheduleSheetDurationRowInterfaceElement::~ScheduleSheetDurationRowInterfaceElement()
		{
		}
	}
}
