
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

#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace calendar;

	CLASS_DEFINITION(CalendarTemplateElement, "t055_calendar_template_elements", 55)
	FIELD_DEFINITION_OF_OBJECT(CalendarTemplateElement, "calendar_template_element_id", "calendar_template_element_ids")

	FIELD_DEFINITION_OF_TYPE(CalendarTemplateField, "calendar_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RankField, "rank", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MinDate, "min_date", SQL_DATE)
	FIELD_DEFINITION_OF_TYPE(MaxDate, "max_date", SQL_DATE)
	FIELD_DEFINITION_OF_TYPE(Step, "interval", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Operation, "positive", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(IncludeCalendarTemplate, "include_id", SQL_INTEGER)

	namespace calendar
	{
		CalendarTemplateElement::CalendarTemplateElement(
			RegistryKeyType id
		):	Registrable(id),
			Object<CalendarTemplateElement, CalendarTemplateElementSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(CalendarTemplateField),
					FIELD_VALUE_CONSTRUCTOR(RankField, 0),
					FIELD_VALUE_CONSTRUCTOR(MinDate, neg_infin),
					FIELD_VALUE_CONSTRUCTOR(MaxDate, pos_infin),
					FIELD_VALUE_CONSTRUCTOR(Step, days(1)),
					FIELD_VALUE_CONSTRUCTOR(Operation, ADD),
					FIELD_DEFAULT_CONSTRUCTOR(IncludeCalendarTemplate)
			))
		{
		}



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
			date minDate(getMinDate());
			if(	minDate.is_neg_infinity() ||
				(minDate < mask.getFirstActiveDate() && getStep().days() == 1)
			){
				minDate = mask.getFirstActiveDate();
			}
			date maxDate(getMaxDate());
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
			Calendar elementMask(minDate, maxDate, getStep());
			elementMask &= mask;
			if(getInclude())
			{
				elementMask = getInclude()->getResult(elementMask);
			}

			// Applying the element on the result
			switch(getOperation())
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
			return get<CalendarTemplateField>() ? get<CalendarTemplateField>().get_ptr() : NULL;
		}



		size_t CalendarTemplateElement::getRank() const
		{
			return get<RankField>();
		}



		const date& CalendarTemplateElement::getMinDate() const
		{
			return  get<MinDate>();
		}



		const date& CalendarTemplateElement::getMaxDate() const
		{
			return get<MaxDate>();
		}

		const boost::gregorian::date_duration& CalendarTemplateElement::getStep() const
		{
			return get<Step>();
		}


		CalendarTemplateElementOperation CalendarTemplateElement::getOperation() const
		{
			return get<Operation>();
		}



		const CalendarTemplate* CalendarTemplateElement::getInclude() const
		{
			return get<IncludeCalendarTemplate>() ? get<IncludeCalendarTemplate>().get_ptr() : NULL;
		}



		void CalendarTemplateElement::setRank(size_t text )
		{
			set<RankField>(text);
		}



		void CalendarTemplateElement::setMinDate( const date& d)
		{
			set<MinDate>(d);
		}



		void CalendarTemplateElement::setMaxDate( const date& d)
		{
			set<MaxDate>(d);
		}



		void CalendarTemplateElement::setOperation(
			CalendarTemplateElementOperation value
		){
			set<Operation>(value);
		}


		void CalendarTemplateElement::setStep(const boost::gregorian::date_duration& value)
		{
			set<Step>(value);
		}

		void CalendarTemplateElement::setInclude(const CalendarTemplate* value)
		{
			set<IncludeCalendarTemplate>(value
						? boost::optional<CalendarTemplate&>(*const_cast<CalendarTemplate*>(value))
						: boost::none);
		}



		void CalendarTemplateElement::setCalendar( const CalendarTemplate* value )
		{
			set<CalendarTemplateField>(value
						? boost::optional<CalendarTemplate&>(*const_cast<CalendarTemplate*>(value))
						: boost::none);
		}



		boost::gregorian::date CalendarTemplateElement::getRealMinDate() const
		{
			if(getInclude())
			{
				const date& incMinDate(getInclude()->getMinDate());
				return (incMinDate > getMinDate()) ? incMinDate : getMinDate();
			}
			else
			{
				return getMinDate();
			}
		}



		boost::gregorian::date CalendarTemplateElement::getRealMaxDate() const
		{
			if(getInclude())
			{
				const date& incMaxDate(getInclude()->getMaxDate());
				return (incMaxDate < getMaxDate()) ? incMaxDate : getMaxDate();
			}
			else
			{
				return getMaxDate();
			}
		}



		void CalendarTemplateElement::toParametersMap( util::ParametersMap& map, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			map.insert(TABLE_COL_ID, getKey());
			map.insert(
				CalendarTemplateElementTableSync::COL_CALENDAR_ID,
				getCalendar() ? getCalendar()->getKey() : RegistryKeyType(0)
			);
			map.insert(
				CalendarTemplateElementTableSync::COL_RANK,
				getRank()
			);
			map.insert(
				CalendarTemplateElementTableSync::COL_MIN_DATE,
				getMinDate().is_special() ? string() : to_iso_extended_string(getMinDate())
			);
			map.insert(
				CalendarTemplateElementTableSync::COL_MAX_DATE,
				getMaxDate().is_special() ? string() : to_iso_extended_string(getMaxDate())
			);
			map.insert(
				CalendarTemplateElementTableSync::COL_INTERVAL,
				boost::lexical_cast<std::string>(static_cast<int>(getStep().days()))
			);
			map.insert(
				CalendarTemplateElementTableSync::COL_POSITIVE,
				static_cast<int>(getOperation())
			);
			map.insert(
				CalendarTemplateElementTableSync::COL_INCLUDE_ID,
				getInclude() ? getInclude()->getKey() : RegistryKeyType(0)
			);
		}


		void CalendarTemplateElement::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if (get<MinDate>() == boost::gregorian::date(boost::gregorian::not_a_date_time))
			{
				date mindate(neg_infin);
				set<MinDate>(mindate);
			}

			if (get<MaxDate>() == boost::gregorian::date(boost::gregorian::not_a_date_time))
			{
				date maxdate(pos_infin);
				set<MaxDate>(maxdate);
			}

			if(getCalendar())
			{
				const_cast<CalendarTemplate*>(getCalendar())->addElement(*this);
			}
		}



		synthese::LinkedObjectsIds CalendarTemplateElement::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}



		void CalendarTemplateElement::unlink()
		{
			if(getCalendar())
			{
				const_cast<CalendarTemplate*>(getCalendar())->removeElement(*this);
			}
		}

		bool CalendarTemplateElement::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool CalendarTemplateElement::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool CalendarTemplateElement::allowDelete(const server::Session* session) const
		{
			return true;
		}

}	}
