
/** CalendarTemplateElement class header.
	@file CalendarTemplateElement.h

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

#ifndef SYNTHESE_timetables_CalendarTemplateElement_h__
#define SYNTHESE_timetables_CalendarTemplateElement_h__

#include "Registrable.h"
#include "UId.h"
#include "Date.h"
#include "Calendar.h"
#include "Registry.h"

namespace synthese
{
	namespace timetables
	{
		/** Element of calendar template class.
			@ingroup m55
			
			An element of a calendar describes a range of dates by several ways :
				- single date d : minDate=d, maxDate=d
				- date range from s to e : minDate=s, maxDate=e, interval=1
				- day of a week from s to e : minDate=s, maxDate=e, interval=7
				- the maxDate can be unknown date (unlimited range)
			
			ddd
		*/
		class CalendarTemplateElement
		:	public virtual util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<CalendarTemplateElement>	Registry;
			
		private:
			int			_rank;
			time::Date	_minDate;
			time::Date	_maxDate;
			int			_interval;
			bool		_positive;
			uid			_includeId;

		public:
			CalendarTemplateElement(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);

			//! @name Queries
			//@{
				time::Calendar	getCalendar(const time::Calendar& mask)	const;
			//@}

			//! @name Getters
			//@{
				int					getRank()		const;
				const time::Date&	getMinDate()	const;
				const time::Date&	getMaxDate()	const;
				int					getInterval()	const;
				bool				getPositive()	const;
				uid					getIncludeId()	const;
			//@}

			//! @name Setters
			//@{ 
				void setRank(int text);
				void setMinDate(const time::Date& date);
				void setMaxDate(const time::Date& date);
				void setInterval(int interval);
				void setPositive(bool value);
				void setIncludeId(uid id);
			//@}

			//! @name Modifiers
			//@{
			//@}

		};
	}
}

#endif // SYNTHESE_timetables_CalendarTemplateElement_h__
