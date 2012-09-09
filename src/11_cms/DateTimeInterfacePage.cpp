
/** DateTimeInterfacePage class implementation.
	@file DateTimeInterfacePage.cpp

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

#include "DateTimeInterfacePage.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace cms
	{
		const string DateTimeInterfacePage::DATA_DAY("day");
		const string DateTimeInterfacePage::DATA_DAY_OF_WEEK("day_of_week");
		const string DateTimeInterfacePage::DATA_HOURS("hours");
		const string DateTimeInterfacePage::DATA_MINUTES("minutes");
		const string DateTimeInterfacePage::DATA_MONTH("month");
		const string DateTimeInterfacePage::DATA_YEAR("year");
		const string DateTimeInterfacePage::DATA_TOTAL_MINUTES("total_minutes");

		void DateTimeInterfacePage::fillParametersMap(
			ParametersMap& pm,
			const boost::posix_time::ptime& dateTime
		){
			pm.insert(DATA_YEAR, dateTime.date().year());
			pm.insert(DATA_MONTH, dateTime.date().month());
			pm.insert(DATA_DAY, dateTime.date().day());
			pm.insert(DATA_HOURS, dateTime.time_of_day().hours());
			pm.insert(DATA_MINUTES, dateTime.time_of_day().minutes());
			pm.insert(DATA_DAY_OF_WEEK, dateTime.date().day_of_week());
		}

		void DateTimeInterfacePage::fillParametersMap(
			ParametersMap& pm,
			const boost::gregorian::date& date
		){
			pm.insert(DATA_YEAR, date.year());
			pm.insert(DATA_MONTH, date.month());
			pm.insert(DATA_DAY, date.day());
			pm.insert(DATA_DAY_OF_WEEK, date.day_of_week());
		}

		void DateTimeInterfacePage::fillParametersMap(
			ParametersMap& pm,
			const boost::posix_time::time_duration& duration
		){
			pm.insert(DATA_HOURS, duration.hours());
			pm.insert(DATA_MINUTES, duration.minutes());
			pm.insert(DATA_TOTAL_MINUTES, duration.total_seconds() / 60);
		}

		void DateTimeInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const boost::posix_time::ptime& dateTime
		){
			Display<boost::posix_time::ptime>(stream,page,request,dateTime);
		}

		void DateTimeInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const boost::gregorian::date& date
		){
			Display<boost::gregorian::date>(stream,page,request,date);
		}

		void DateTimeInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const boost::posix_time::time_duration& duration
		){
			Display<boost::posix_time::time_duration>(stream,page,request,duration);
		}
}	}
