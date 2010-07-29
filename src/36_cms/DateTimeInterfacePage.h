
/** DateTimeInterfacePage class header.
	@file DateTimeInterfacePage.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_DateTimeInterfacePage_H__
#define SYNTHESE_DateTimeInterfacePage_H__

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace cms
	{
		class WebPage;

		/** DateTimeInterfacePage Interface Page Class.
			@ingroup m56Pages refPages

			Date :
			 - year
			 - month
			 - day
			 - hours (TIME_UNKNOWN = do not display hour)
			 - minutes
			 - day of week  : 0 = sunday, 1 = monday, ..., 6 = saturday 
		*/
		class DateTimeInterfacePage
		{
		public:
			static const std::string DATA_YEAR;
			static const std::string DATA_MONTH;
			static const std::string DATA_DAY;
			static const std::string DATA_HOURS;
			static const std::string DATA_MINUTES;
			static const std::string DATA_DAY_OF_WEEK;
			static const std::string DATA_TOTAL_MINUTES;

			//////////////////////////////////////////////////////////////////////////
			/// Date time display.
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::WebPage> page,
				const server::Request& request,
				const boost::posix_time::ptime& dateTime
			);


			//////////////////////////////////////////////////////////////////////////
			/// Date display.
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::WebPage> page,
				const server::Request& request,
				const boost::gregorian::date& date
			);


			//////////////////////////////////////////////////////////////////////////
			/// Time or duration display.
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::WebPage> page,
				const server::Request& request,
				const boost::posix_time::time_duration& duration
			);
		};
	}
}

#endif // SYNTHESE_DateTimeInterfacePage_H__
