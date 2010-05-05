
/** CalendarHTMLViewer class header.
	@file CalendarHTMLViewer.h

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

#ifndef SYNTHESE_calendar_CalendarHTMLViewer_h__
#define SYNTHESE_calendar_CalendarHTMLViewer_h__

#include "Calendar.h"
#include "ResultHTMLTable.h"
#include "HTMLModule.h"
#include "StaticActionRequest.h"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <ostream>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace calendar
	{
		//////////////////////////////////////////////////////////////////////////
		/// Generic calendar viewer for administration purpose.
		/// @ingroup m19
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		template<class DateChangeRequest>
		class CalendarHTMLViewer
		{
		public:

		private:
			const Calendar& _calendar;
			DateChangeRequest* _dateChangeRequest;


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param calendar calendar to view
			/// @param dateChangeRequest request to link to allow the user to change the status of the calendar at a date
			CalendarHTMLViewer(
				const Calendar& calendar,
				DateChangeRequest* dateChangeRequest = NULL
			);


			//////////////////////////////////////////////////////////////////////////
			/// Displays the result on a stream
			/// @param stream stream to write the result on
			void display(
				std::ostream& stream
			) const;
		};



		template<class DateChangeRequest>
		CalendarHTMLViewer<DateChangeRequest>::CalendarHTMLViewer(
			const Calendar& calendar,
			DateChangeRequest* dateChangeRequest
		):	_calendar(calendar),
			_dateChangeRequest(dateChangeRequest)
		{
		}


		template<class DateChangeRequest>
		void CalendarHTMLViewer<DateChangeRequest>::display( std::ostream& stream ) const
		{
			boost::gregorian::date minDate;
			boost::gregorian::date maxDate;
			if(	_calendar.empty()
				){
					minDate = boost::gregorian::day_clock::local_day();
					maxDate = minDate;
			}
			else
			{
				minDate = _calendar.getFirstActiveDate();
				maxDate = _calendar.getLastActiveDate();
			}

			boost::gregorian::date firstDate(minDate - boost::gregorian::date_duration(minDate.day_of_week() == 0 ? 13 : minDate.day_of_week() + 6));
			boost::gregorian::date lastDate(maxDate + boost::gregorian::date_duration(maxDate.day_of_week() == 0 ? 7 : 14 - maxDate.day_of_week()));

			html::HTMLTable::ColsVector c;
			c.push_back("&nbsp;");
			c.push_back("Lu");
			c.push_back("Ma");
			c.push_back("Me");
			c.push_back("Je");
			c.push_back("Ve");
			c.push_back("Sa");
			c.push_back("Di");
			html::HTMLTable t(c, html::ResultHTMLTable::CSS_CLASS);
			int white(7);
			bool first(true);
			bool leftWhite(false);
			stream << t.open();

			for(boost::gregorian::date curDate(firstDate);
				curDate <= lastDate;
				curDate += boost::gregorian::date_duration(1)
				){
					// Value of the calendar at this date
					bool value(_calendar.isActive(curDate));

					if(	curDate.day_of_week() == 1)
					{
						stream << t.row();
						stream << t.col(1, std::string(), true);
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
					std::stringstream style;
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
					stream << t.col(1, std::string(), false, style.str());

					if(_dateChangeRequest)
					{
						_dateChangeRequest->getAction()->setDate(curDate);
						stream << html::HTMLModule::getHTMLLink(_dateChangeRequest->getURL(), boost::lexical_cast<std::string>(curDate.day()));
					}
					else
					{
						stream << curDate.day();
					}
			}

			stream << t.close();
		}
	}
}

#endif // SYNTHESE_calendar_CalendarHTMLViewer_h__
