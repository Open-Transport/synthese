
/** StopPointAccessibilityCalendar class header.
	@file StopPointAccessibilityCalendar.hpp

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

#ifndef SYNTHESE_StopPointAccessibilityCalendar_H__
#define SYNTHESE_StopPointAccessibilityCalendar_H__

#include "Object.hpp"

#include "Calendar.h"
#include "CalendarTemplate.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTemplate.hpp"
#include "PointerField.hpp"
#include "SchemaMacros.hpp"
#include "StopPoint.hpp"
#include "TimeField.hpp"

namespace synthese
{
	namespace pt
	{
		FIELD_POINTER(StopPointer, StopPoint)
		FIELD_POINTER(CalendarTemplatePointer, calendar::CalendarTemplate)
		FIELD_INT(UserClass)
		FIELD_BOOL(DailyTimeFilter)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(StopPointer),
			FIELD(CalendarTemplatePointer),
			FIELD(UserClass),
			FIELD(StartHour),
			FIELD(EndHour),
			FIELD(DailyTimeFilter),
			FIELD(impex::DataSourceLinks)
		> StopPointAccessibilityCalendarSchema;

		class StopPointInaccessibilityCalendar:
			public Object<StopPointInaccessibilityCalendar, StopPointAccessibilityCalendarSchema>,
			public impex::ImportableTemplate<StopPointInaccessibilityCalendar>
		{
		public:
			typedef util::Registry<StopPointInaccessibilityCalendar> Registry;

		private:
			calendar::Calendar _inaccessibilityCalendar;


		public:
			StopPointInaccessibilityCalendar(util::RegistryKeyType id = 0);

			bool isAccessibleAtTime(
				const graph::AccessParameters& accessParameters,
				const boost::posix_time::ptime& time
			) const;

			virtual void link(
				util::Env& env,
				bool withAlgorithmOptimizations = false
			);

			virtual void unlink();
		};
	}
}

#endif // SYNTHESE_StopPointAccessibilityCalendar_H__
