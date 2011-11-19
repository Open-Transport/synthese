
/** CalendarTemplateElement class header.
	@file CalendarTemplateElement.h

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

#ifndef SYNTHESE_timetables_CalendarTemplateElement_h__
#define SYNTHESE_timetables_CalendarTemplateElement_h__

#include "Registrable.h"
#include "Calendar.h"
#include "Registry.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;

		/** Element of calendar template class.
			@ingroup m19

			An element of a calendar describes a range of dates by several ways :
				- single date d : minDate=d, maxDate=d
				- date range from s to e : minDate=s, maxDate=e, interval=1
				- day of a week from s to e : minDate=s, maxDate=e, interval=7
				- the maxDate can be unknown date (unlimited range)

		*/
		class CalendarTemplateElement
		:	public virtual util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<CalendarTemplateElement>	Registry;

			enum Operation
			{
				ADD = '+',
				SUB = '-',
				AND = '*'
			};

		private:
			//! @name Key
			//@{
				const CalendarTemplate*	_calendar;
				std::size_t			_rank;
			//@}

			//! @name Data
			//@{
				boost::gregorian::date	_minDate;
				boost::gregorian::date	_maxDate;
				boost::gregorian::date_duration	_interval;
				Operation		_operation;
				const CalendarTemplate* _include;
			//@}

		public:
			CalendarTemplateElement(
				util::RegistryKeyType id = 0
			);

			//! @name Queries
			//@{
				Calendar	getResult(const Calendar& mask)	const;

				//////////////////////////////////////////////////////////////////////////
				/// Min date getter which takes into account of included calendar.
				boost::gregorian::date	getRealMinDate()	const;

				//////////////////////////////////////////////////////////////////////////
				/// Max date getter which takes into account of included calendar.
				boost::gregorian::date	getRealMaxDate()	const;

			//@}

			//! @name Getters
			//@{
				size_t					getRank()		const;
				const boost::gregorian::date&	getMinDate()	const;
				const boost::gregorian::date&	getMaxDate()	const;
				boost::gregorian::date_duration	getInterval()	const;
				Operation				getOperation()	const;
				const CalendarTemplate* getInclude()	const;
				const CalendarTemplate*	getCalendar() const;
			//@}

			//! @name Setters
			//@{
				void setCalendar(const CalendarTemplate* value);
				void setRank(size_t text);
				void setMinDate(const boost::gregorian::date& date);
				void setMaxDate(const boost::gregorian::date& date);
				void setInterval(boost::gregorian::date_duration interval);
				void setOperation(Operation value);
				void setInclude(const CalendarTemplate* value);
			//@}

			//! @name Modifiers
			//@{
			//@}

		};
	}
}

#endif // SYNTHESE_timetables_CalendarTemplateElement_h__
