
/** NonPermanentService class header.
	@file NonPermanentService.h

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

#ifndef SYNTHESE_env_NonPermanentService_h__
#define SYNTHESE_env_NonPermanentService_h__

#include "Service.h"
#include "Calendar.h"

#include <set>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace pt
	{
		class PTUseRule;
		class ServiceCalendarLink;

		/** NonPermanentService class.
			@ingroup m35

			Use isActive to determinate if this service is provided a given day.
			The date parameter corresponds to the departure date of the service from its origin
			(warning: do not test the customer departure date which can be one or more days later;
			use getOriginDateTime to compute the origin date)
		*/
		class NonPermanentService:
			public graph::Service,
			public calendar::Calendar
		{
		public:
			typedef std::set<ServiceCalendarLink*> CalendarLinks;

		protected:
			NonPermanentService(
				util::RegistryKeyType id = 0
			);
			NonPermanentService(
				std::string serviceNumber,
				graph::Path* path
			);

			CalendarLinks _calendarLinks;
			mutable boost::recursive_mutex _calendarLinksMutex;
	
		public:
			//! @name Getters
			//@{
				const CalendarLinks& getCalendarLinks() const { return _calendarLinks; }
			//@}

			//! @name Setters
			//@{
				void setCalendarLinks(const CalendarLinks& value){ _calendarLinks = value; }
			//@}


			void setCalendarFromLinks();

			void removeCalendarLink(const ServiceCalendarLink& link, bool updateCalendar);
			void addCalendarLink(const ServiceCalendarLink& link, bool updateCalendar);

			/** Latest schedule of the service : the last arrival at the last vertex.
				@return The latest schedule of the service
			*/
			virtual const boost::posix_time::time_duration& getLastArrivalSchedule(
				bool RTData
			) const = 0;

			/** Marks a date on the calendar of the service.
			 * Updates the calendar of the path too.
			 * @param date the date to mark
			 */
			virtual void setActive(const boost::gregorian::date& d);

			/** Unmarks a date on the calendar of the service.
			 * Updates the calendar of the path too.
			 * @param date the date to unmark
			 */
			virtual void setInactive(const boost::gregorian::date& d);

			void updatePathCalendar();
		};
}	}

#endif // SYNTHESE_env_NonPermanentService_h__
