
/** RoutePlannerFunction class implementation.
	@file RoutePlannerFunction.cpp

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

#include "01_util/Conversion.h"

#include "30_server/RequestException.h"

#include "RoutePlannerFunction.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace routeplanner
	{
		std::string AdminRequest::PARAMETER_SITE = "site";
		
		RoutePlannerFunction::RoutePlannerFunction()
			: Function(), _site(NULL)
		{}

		ParametersMap RoutePlannerFunction::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}

		void RoutePlannerFunction::_setFromParametersMap(const ParametersMap& map)
		{
			ParametersMap::const_iterator it;

			/// @todo Parameters parsing
			// eg
			// it = map.find(PARAMETER_PAGE);
			//try
			//{
			//	AdminInterfaceElement* page = (it == map.end())
			//		? Factory<AdminInterfaceElement>::create<HomeAdmin>()
			//		: Factory<AdminInterfaceElement>::create(it->second);
			//	page->setFromParametersMap(map);
			//	_page = page;
			//}
			//catch (FactoryException<AdminInterfaceElement> e)
			//{
			//	throw RequestException("Admin page " + it->second + " not found");
			//}
		}

		void RoutePlannerFunction::_run( std::ostream& stream )
		{
			RoutePlanner r(_origin, _destination, _accessParameters, _planningOrder, _journeySheetStartTime, _journeySheetEndTime);
			JourneyVector jv = r.computeJourneySheetDepartureArrival();
			_page->display(jv, _site, this);
		}

		RoutePlannerFunction::~RoutePlannerFunction()
		{
		}
	}
}
