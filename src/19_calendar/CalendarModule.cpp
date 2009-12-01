
/** CalendarModule class implementation.
	@file CalendarModule.cpp

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

#include "CalendarModule.h"
#include "Calendar.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/formatters.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,CalendarModule>::FACTORY_KEY("19_calendar");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<CalendarModule>::NAME("Calendriers");

		template<> void ModuleClassTemplate<CalendarModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<CalendarModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<CalendarModule>::End()
		{
		}
	}

	namespace calendar
	{
		std::string CalendarModule::GetBestCalendarTitle( const Calendar& calendar )
		{
			stringstream result;
			bool first(true);
			Calendar::DatesVector dates(calendar.getActiveDates());
			BOOST_FOREACH(const Calendar::DatesVector::value_type& date, dates)
			{
				result << (first ? string() : ",") << to_simple_string(date);
				first=false;
			}
			return result.str();
		}
	}
}
