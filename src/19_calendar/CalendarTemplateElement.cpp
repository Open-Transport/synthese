
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

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const std::string Registry<calendar::CalendarTemplateElement>::KEY("CalendarTemplateElement");
	}

	namespace calendar
	{
		CalendarTemplateElement::CalendarTemplateElement(
			RegistryKeyType id
		):	Registrable(id),
			_interval(days(1)),
			_positive(true),
			_period(date(not_a_date_time), date(not_a_date_time))
		{

		}



		Calendar CalendarTemplateElement::getCalendar( const Calendar& mask ) const
		{
			Calendar result;

			if (!_includeId)
			{
				date minDate(_period.begin());
				if(minDate.is_not_a_date() || minDate < mask.getFirstActiveDate())
					minDate = mask.getFirstActiveDate();
				date maxDate(_period.last());
				if (maxDate.is_not_a_date() || maxDate > mask.getLastActiveDate())
					maxDate = mask.getLastActiveDate();

				for (date d = minDate; d <= maxDate; d += _interval)
					if (mask.isActive(d))
						result.setActive(d);
			}
			return result;
		}



		size_t CalendarTemplateElement::getRank() const
		{
			return _rank;
		}



		date CalendarTemplateElement::getMinDate() const
		{
			return _period.begin();
		}



		date CalendarTemplateElement::getMaxDate() const
		{
			return _period.last();
		}



		date_duration CalendarTemplateElement::getInterval() const
		{
			return _interval;
		}



		bool CalendarTemplateElement::getPositive() const
		{
			return _positive;
		}



		optional<RegistryKeyType> CalendarTemplateElement::getIncludeId() const
		{
			return _includeId;
		}



		void CalendarTemplateElement::setRank(size_t text )
		{
			_rank = text;
		}



		void CalendarTemplateElement::setMinDate( const date& d)
		{
			_period = _period.is_null() ? date_period(d, d + days(1)) : date_period(d, _period.end());
		}



		void CalendarTemplateElement::setMaxDate( const date& d)
		{
			_period = _period.is_null() ? date_period(d, d + days(1)) : date_period(_period.begin(), d + days(1));
		}



		void CalendarTemplateElement::setInterval( date_duration interval )
		{
			_interval = interval;
		}



		void CalendarTemplateElement::setPositive( bool value )
		{
			_positive =value;
		}



		void CalendarTemplateElement::setIncludeId( optional<RegistryKeyType> id )
		{
			_includeId = id;
		}
	}
}
