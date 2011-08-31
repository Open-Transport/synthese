
/** ServiceCalendarLink class header.
	@file ServiceCalendarLink.hpp

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

#ifndef SYNTHESE_pt_ServiceCalendarLink_h__
#define SYNTHESE_pt_ServiceCalendarLink_h__

#include "Registry.h"
#include "Registrable.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{
		class Calendar;
		class CalendarTemplate;
	}

	namespace pt
	{
		class NonPermanentService;
		class CommercialLine;

		//////////////////////////////////////////////////////////////////////////
		/// Service calendar link.
		///	@ingroup m35
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		class ServiceCalendarLink:
			public util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ServiceCalendarLink>	Registry;

		private:

			//! @name Content
			//@{
				NonPermanentService* _service;
				calendar::CalendarTemplate* _calendarTemplate;
				calendar::CalendarTemplate* _calendarTemplate2;
				boost::gregorian::date _startDate;
				boost::gregorian::date _endDate;
			//@}


		public:

			ServiceCalendarLink(
				util::RegistryKeyType id = 0
			);

			//! @name Getters
			//@{
				NonPermanentService* getService() const { return _service; }
				calendar::CalendarTemplate* getCalendarTemplate() const { return _calendarTemplate; }
				calendar::CalendarTemplate* getCalendarTemplate2() const { return _calendarTemplate2; }
				const boost::gregorian::date& getStartDate() const { return _startDate; }
				const boost::gregorian::date& getEndDate() const { return _endDate; }
			//@}

			//! @name Setters
			//@{
				void setService(NonPermanentService* value){ _service = value; }
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
				void addDatesToCalendar(calendar::Calendar& cal) const;
			//@}

		};
}	}

#endif
