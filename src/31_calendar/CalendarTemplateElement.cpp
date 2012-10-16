
/** CalendarTemplateElement class implementation.
	@file CalendarTemplateElement.cpp

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
			_step(days(1)),
			_minDate(neg_infin),
			_maxDate(pos_infin),
			_operation(ADD),
			_include(NULL)
		{}



		void CalendarTemplateElement::apply(
			Calendar& result,
			const Calendar& mask
		) const	{

			// Empty mask : exit
			if(mask.empty())
			{
				return;
			}

			// Min and max dates of the update
			date minDate(_minDate);
			if(	minDate.is_neg_infinity() ||
				(minDate < mask.getFirstActiveDate() && _step.days() == 1)
			){
				minDate = mask.getFirstActiveDate();
			}
			date maxDate(_maxDate);
			if (maxDate > mask.getLastActiveDate())
			{
				maxDate = mask.getLastActiveDate();
			}

			// Abort if no date
			if(minDate > maxDate)
			{
				return;
			}

			// Base mask
			Calendar elementMask(minDate, maxDate, _step);
			elementMask &= mask;
			if(_include)
			{
				elementMask = _include->getResult(elementMask);
			}

			// Applying the element on the result
			switch(_operation)
			{
			case ADD:
				result |= elementMask;
				break;

			case SUB:
				result -= elementMask;
				break;

			case AND:
				result &= elementMask;
				break;
			}
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
