
/** CalendarTemplateElement class implementation.
	@file CalendarTemplateElement.cpp

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

#include "CalendarTemplateElement.h"

namespace synthese
{
	using namespace util;
	using namespace time;
	using namespace env;

	namespace timetables
	{
		CalendarTemplateElement::CalendarTemplateElement()
			: Registrable<uid, CalendarTemplateElement>()
			, _minDate(TIME_UNKNOWN)
			, _maxDate(TIME_UNKNOWN)
			, _interval(0)
			, _positive(true)
			, _includeId(0)
		{

		}



		env::Calendar CalendarTemplateElement::getCalendar( const env::Calendar& mask ) const
		{
			Calendar result;

			if (_includeId == UNKNOWN_VALUE)
			{
				Date minDate(_minDate);
				if(minDate.isUnknown() || minDate < mask.getFirstMarkedDate())
					minDate = mask.getFirstMarkedDate();
				Date maxDate(_maxDate);
				if (maxDate.isUnknown() || maxDate > mask.getLastMarkedDate())
					maxDate = mask.getLastMarkedDate();

				for (Date date = minDate; date <= maxDate; date += _interval)
					if (mask.isMarked(date))
						result.mark(date);
			}
			return result;
		}



		int CalendarTemplateElement::getRank() const
		{
			return _rank;
		}



		const time::Date& CalendarTemplateElement::getMinDate() const
		{
			return _minDate;
		}



		const time::Date& CalendarTemplateElement::getMaxDate() const
		{
			return _maxDate;
		}



		int CalendarTemplateElement::getInterval() const
		{
			return _interval;
		}



		bool CalendarTemplateElement::getPositive() const
		{
			return _positive;
		}



		uid CalendarTemplateElement::getIncludeId() const
		{
			return _includeId;
		}



		void CalendarTemplateElement::setRank( int text )
		{
			_rank = text;
		}



		void CalendarTemplateElement::setMinDate( const time::Date& date )
		{
			_minDate = date;
		}



		void CalendarTemplateElement::setMaxDate( const time::Date& date )
		{
			_maxDate = date;
		}



		void CalendarTemplateElement::setInterval( int interval )
		{
			_interval = interval;
		}



		void CalendarTemplateElement::setPositive( bool value )
		{
			_positive =value;
		}



		void CalendarTemplateElement::setIncludeId( uid id )
		{
			_includeId = id;
		}
	}
}
