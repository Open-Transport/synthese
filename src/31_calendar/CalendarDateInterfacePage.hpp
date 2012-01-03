
/** CalendarDateInterfacePage class header.
	@file CalendarDateInterfacePage.hpp
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

#ifndef SYNTHESE_CalendarDateInterfacePage_H__
#define SYNTHESE_CalendarDateInterfacePage_H__

#include <boost/shared_ptr.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace calendar
	{
		//////////////////////////////////////////////////////////////////////////
		/// 31.11 Interface : calendar date.
		///	@ingroup m31Pages refPages
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Values sent to the web page :
		///	<ul>
		///		<li>week_day</li>
		///		<li>day</li>
		///		<li>month</li>
		///		<li>year</li>
		///		<li>is_active</li>
		///	</ul>
		class CalendarDateInterfacePage
		{
		public:
			static const std::string DATA_WEEK_DAY;
			static const std::string DATA_DAY;
			static const std::string DATA_MONTH;
			static const std::string DATA_YEAR;
			static const std::string DATA_IS_ACTIVE;

			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...
				@param variables Execution variables
				@param request Source request
			*/
			static void Display(
				std::ostream& stream,
				boost::shared_ptr<const cms::Webpage> page,
				const server::Request& request,
				const util::ParametersMap& templateParametersMap,
				boost::gregorian::date value,
				bool isActive
			);
		};
	}
}

#endif // SYNTHESE_CalendarDateInterfacePage_H__
