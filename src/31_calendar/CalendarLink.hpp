
/** CalendarLink class header.
	@file CalendarLink.hpp

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

#ifndef SYNTHESE_pt_ServiceCalendarLink_h__
#define SYNTHESE_pt_ServiceCalendarLink_h__

#include "Registry.h"
#include "Registrable.h"

#include "Calendar.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{
		class Calendar;
		class CalendarTemplate;

		//////////////////////////////////////////////////////////////////////////
		/// Calendar link.
		///	@ingroup m31
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		class CalendarLink:
			public util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<CalendarLink>	Registry;

		private:

			//! @name Content
			//@{
				Calendar* _calendar;
				calendar::CalendarTemplate* _calendarTemplate;
				calendar::CalendarTemplate* _calendarTemplate2;
				boost::gregorian::date _startDate;
				boost::gregorian::date _endDate;
			//@}


		public:

			CalendarLink(
				util::RegistryKeyType id = 0
			);

			//! @name Getters
			//@{
				Calendar* getCalendar() const { return _calendar; }
				calendar::CalendarTemplate* getCalendarTemplate() const { return _calendarTemplate; }
				calendar::CalendarTemplate* getCalendarTemplate2() const { return _calendarTemplate2; }
				const boost::gregorian::date& getStartDate() const { return _startDate; }
				const boost::gregorian::date& getEndDate() const { return _endDate; }
			//@}

			//! @name Setters
			//@{
				void setCalendar(Calendar* value){ _calendar = value; }
				void setCalendarTemplate(calendar::CalendarTemplate* value){ _calendarTemplate = value; }
				void setCalendarTemplate2(calendar::CalendarTemplate* value){ _calendarTemplate2 = value; }
				void setStartDate(const boost::gregorian::date& value){ _startDate = value; }
				void setEndDate(const boost::gregorian::date& value){ _endDate = value; }
			//@}

			//! @name Update methods.
			//@{
			//@}

			//! @name Queries
			//@{
				void addDatesToBitSets(Calendar::BitSets& bitsets) const;
			//@}
		};
}	}

#endif
