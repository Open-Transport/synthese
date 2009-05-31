
/** DateTimeInterfacePage class implementation.
	@file DateTimeInterfacePage.cpp

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

#include "DateTimeInterfacePage.h"

#include "DateTime.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace time;

	template<> const string util::FactorableTemplate<InterfacePage,DateTimeInterfacePage>::FACTORY_KEY("datetime");

	namespace interfaces
	{
		const string DateTimeInterfacePage::DATA_DAY("day");
		const string DateTimeInterfacePage::DATA_DAY_OF_WEEK("day_of_week");
		const string DateTimeInterfacePage::DATA_HOURS("hours");
		const string DateTimeInterfacePage::DATA_MINUTES("minutes");
		const string DateTimeInterfacePage::DATA_MONTH("month");
		const string DateTimeInterfacePage::DATA_YEAR("year");

		void DateTimeInterfacePage::display(std::ostream& stream, VariablesMap& variables	, const time::DateTime& dateTime
			, const server::Request* request /*= NULL*/) const
		{
			ParametersVector pv;

			pv.push_back(Conversion::ToString(dateTime.getYear()));
			pv.push_back(Conversion::ToString(dateTime.getMonth()));
			pv.push_back(Conversion::ToString(dateTime.getDay()));
			pv.push_back(Conversion::ToString(dateTime.getHours()));
			pv.push_back(Conversion::ToString(dateTime.getMinutes()));
			pv.push_back(Conversion::ToString(dateTime.getDate().getWeekDay()));

			InterfacePage::_display(stream, pv, variables, NULL, request);
		}

		void DateTimeInterfacePage::display( std::ostream& stream , interfaces::VariablesMap& variables , const time::Date& date , const server::Request* request /*= NULL  */ ) const
		{
			ParametersVector pv;

			pv.push_back(Conversion::ToString(date.getYear()));
			pv.push_back(Conversion::ToString(date.getMonth()));
			pv.push_back(Conversion::ToString(date.getDay()));
			pv.push_back(Conversion::ToString(UNKNOWN_VALUE));
			pv.push_back(Conversion::ToString(UNKNOWN_VALUE));
			pv.push_back(Conversion::ToString(date.getWeekDay()));

			InterfacePage::_display(stream, pv, variables, NULL, request);
		}



		DateTimeInterfacePage::DateTimeInterfacePage()
			: Registrable(UNKNOWN_VALUE)
		{

		}
	}
}
