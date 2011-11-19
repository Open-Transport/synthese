
/** CalendarDateInterfacePage class implementation.
	@file CalendarDateInterfacePage.cpp
	@author Hugues Romain
	@date 2010

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

#include "CalendarDateInterfacePage.hpp"
#include "Webpage.h"
#include "Request.h"
#include "Function.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace cms;


	namespace calendar
	{
		const string CalendarDateInterfacePage::DATA_DAY("day");
		const string CalendarDateInterfacePage::DATA_WEEK_DAY("week_day");
		const string CalendarDateInterfacePage::DATA_MONTH("month");
		const string CalendarDateInterfacePage::DATA_YEAR("year");
		const string CalendarDateInterfacePage::DATA_IS_ACTIVE("is_active");


		void CalendarDateInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const Webpage> page,
			const server::Request& request,
			const util::ParametersMap& templateParametersMap,
			boost::gregorian::date value,
			bool isActive
		){
			ParametersMap pm(templateParametersMap);

			pm.insert(DATA_DAY, value.day());
			pm.insert(DATA_MONTH, value.month());
			pm.insert(DATA_YEAR, value.year());
			pm.insert(DATA_WEEK_DAY, value.day_of_week());
			pm.insert(DATA_IS_ACTIVE, isActive);

			page->display(stream, request, pm);
		}
}	}
