
/** CalendarHTMLViewer class implementation.
	@file CalendarHTMLViewer.cpp

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

#include "CalendarHTMLViewer.h"
#include "Calendar.h"
#include "ResultHTMLTable.h"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace html;

	namespace calendar
	{
		CalendarHTMLViewer::CalendarHTMLViewer(
			const Calendar& calendar
		):	_calendar(calendar)
		{
		}



		void CalendarHTMLViewer::display( std::ostream& stream ) const
		{
			date minDate;
			date maxDate;
			if(	_calendar.empty()
			){
				minDate = day_clock::local_day();
				maxDate = minDate;
			}
			else
			{
				minDate = _calendar.getFirstActiveDate();
				maxDate = _calendar.getLastActiveDate();
			}

			date firstDate(minDate - date_duration(minDate.day_of_week() == 0 ? 13 : minDate.day_of_week() + 6));
			date lastDate(maxDate + date_duration(maxDate.day_of_week() == 0 ? 7 : 14 - maxDate.day_of_week()));

			HTMLTable::ColsVector c;
			c.push_back("&nbsp;");
			c.push_back("Lu");
			c.push_back("Ma");
			c.push_back("Me");
			c.push_back("Je");
			c.push_back("Ve");
			c.push_back("Sa");
			c.push_back("Di");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
			int white(7);
			bool first(true);
			bool leftWhite(false);
			stream << t.open();

			for(date curDate(firstDate);
				curDate <= lastDate;
				curDate += date_duration(1)
			){
				// Value of the calendar at this date
				bool value(_calendar.isActive(curDate));

				if(	curDate.day_of_week() == 1)
				{
					stream << t.row();
					stream << t.col(1, string(), true);
					if (curDate.day()<8 || first)
					{
						stream  << "<a name=\"" << curDate.year() << "-" << curDate.month() << "\"></a>" << curDate.month() << " " << curDate.year();
					}
					else
					{
						stream << "&nbsp;";
					}	
					if(first)
						first = false;
				}

				leftWhite = false;
				stringstream style;
				style << "background-color:" << (value ? "green" : "red") << ";";
				if( curDate.day() == 1)
				{
					white = 7;
					if (curDate.day() == 1)
						leftWhite = true;
				}
				if (white || leftWhite)
				{
					style << "border-style:solid; border-color:#C0C0A0; border-width:";
					if (white > 0)
					{
						style << "1";
						--white;
					}
					else
					{
						style << "0";
					}
					style << "px 0px 0px " << leftWhite << "px;";
				}
				else
				{
					style << "border:none;";
				}
				stream << t.col(1, string(), false, style.str());

				stream << curDate.day();
			}

			stream << t.close();
		}
	}
}
