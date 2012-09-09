
/** BaseCalendarAdmin class header.
	@file BaseCalendarAdmin.hpp

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

#ifndef SYNTHESE_calendar_CalendarHTMLViewer_h__
#define SYNTHESE_calendar_CalendarHTMLViewer_h__

#include "UtilTypes.h"
#include "StaticFunctionRequest.h"

#include <ostream>
#include <boost/optional.hpp>

namespace synthese
{
	namespace admin
	{
		class BaseAdminFunctionRequest;
	}

	namespace server
	{
		class Request;
	}

	namespace calendar
	{
		class Calendar;

		//////////////////////////////////////////////////////////////////////////
		/// Generic calendar viewer for administration purpose.
		/// @ingroup m31
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		class BaseCalendarAdmin
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Displays the result on a stream
			/// @param stream stream to write the result on
			/// @pre If defined, updateRequest action must inherit from BaseCalendarUpdateAction
			static void Display(
				std::ostream& stream,
				const Calendar& object,
				boost::optional<const admin::BaseAdminFunctionRequest&> updateRequest =	boost::optional<const admin::BaseAdminFunctionRequest&>(),
				boost::optional<util::RegistryKeyType> parentTemplateId = boost::optional<util::RegistryKeyType>(),
				boost::optional<util::RegistryKeyType> parentTemplateId2 = boost::optional<util::RegistryKeyType>()
			);
		};
}	}

#endif // SYNTHESE_calendar_CalendarHTMLViewer_h__
