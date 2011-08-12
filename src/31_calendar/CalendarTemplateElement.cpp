
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
#include "CalendarTemplate.h"

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
			_calendar(NULL),
			_rank(0),
			_interval(days(1)),
			_operation(ADD),
			_minDate(neg_infin),
			_maxDate(pos_infin),
			_include(NULL)
		{
		}



		Calendar CalendarTemplateElement::getResult( const Calendar& mask ) const
		{
			Calendar result;

			if (!_include)
			{
				date minDate(_minDate);
				if(minDate.is_neg_infinity())
					minDate = mask.getFirstActiveDate();
				date maxDate(_maxDate);
				if (maxDate > mask.getLastActiveDate())
					maxDate = mask.getLastActiveDate();

				for (date d = minDate; d <= maxDate; d += _interval)
					if (mask.isActive(d))
						result.setActive(d);
			}
			else
			{
				Calendar included(_include->getResult(mask));
				date minDate(_minDate);
				if(minDate.is_neg_infinity())
					minDate = mask.getFirstActiveDate();
				date maxDate(_maxDate);
				if (maxDate > mask.getLastActiveDate())
					maxDate = mask.getLastActiveDate();

				for (date d = minDate; d <= maxDate; d += _interval)
					if (mask.isActive(d) && included.isActive(d))
						result.setActive(d);
			}
			return result;
		}



		const CalendarTemplate* CalendarTemplateElement::getCalendar() const
		{
			return _calendar;
		}



		size_t CalendarTemplateElement::getRank() const
		{
			return _rank;
		}



		const date& CalendarTemplateElement::getMinDate() const
		{
			return  _minDate;
		}



		const date& CalendarTemplateElement::getMaxDate() const
		{
			return _maxDate;
		}



		date_duration CalendarTemplateElement::getInterval() const
		{
			return _interval;
		}



		CalendarTemplateElement::Operation CalendarTemplateElement::getOperation() const
		{
			return _operation;
		}



		const CalendarTemplate* CalendarTemplateElement::getInclude() const
		{
			return _include;
		}



		void CalendarTemplateElement::setRank(size_t text )
		{
			_rank = text;
		}



		void CalendarTemplateElement::setMinDate( const date& d)
		{
			_minDate = d;
		}



		void CalendarTemplateElement::setMaxDate( const date& d)
		{
			_maxDate = d;
		}



		void CalendarTemplateElement::setInterval( date_duration interval )
		{
			_interval = interval;
		}



		void CalendarTemplateElement::setOperation(
			CalendarTemplateElement::Operation value
		){
			_operation = value;
		}



		void CalendarTemplateElement::setInclude(const CalendarTemplate* value)
		{
			_include = value;
		}



		void CalendarTemplateElement::setCalendar( const CalendarTemplate* value )
		{
			_calendar = value;
		}



		boost::gregorian::date CalendarTemplateElement::getRealMinDate() const
		{
			if(_include)
			{
				const date& incMinDate(_include->getMinDate());
				return (incMinDate > _minDate) ? incMinDate : _minDate;
			}
			else
			{
				return _minDate;
			}
		}



		boost::gregorian::date CalendarTemplateElement::getRealMaxDate() const
		{
			if(_include)
			{
				const date& incMaxDate(_include->getMaxDate());
				return (incMaxDate < _maxDate) ? incMaxDate : _maxDate;
			}
			else
			{
				return _maxDate;
			}
		}
}	}
