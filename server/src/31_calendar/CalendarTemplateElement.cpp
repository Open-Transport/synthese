
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

#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
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
			_minDate(neg_infin),
			_maxDate(pos_infin),
			_step(days(1)),
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



		bool CalendarTemplateElement::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			if(record.isDefined(TABLE_COL_ID))
			{
				RegistryKeyType value(record.getDefault<RegistryKeyType>(TABLE_COL_ID, 0));
				if(value != getKey())
				{
					result = true;
					setKey(value);
				}
			}

			// Rank
			if(record.isDefined(CalendarTemplateElementTableSync::COL_RANK))
			{
				size_t value(
					record.getDefault<size_t>(CalendarTemplateElementTableSync::COL_RANK, 0)
				);
				if(value != getRank())
				{
					result = true;
					setRank(value);
				}
			}

			// Min date
			if(record.isDefined(CalendarTemplateElementTableSync::COL_MIN_DATE))
			{
				date value(neg_infin);
				if(!record.get<string>(CalendarTemplateElementTableSync::COL_MIN_DATE).empty())
				{
					try
					{
						value = from_string(record.get<string>(CalendarTemplateElementTableSync::COL_MIN_DATE));
					}
					catch(...)
					{
					}
				}
				if(value != getMinDate())
				{
					result = true;
					setMinDate(value);
				}
			}

			// Max date
			if(record.isDefined(CalendarTemplateElementTableSync::COL_MAX_DATE))
			{
				date value(pos_infin);
				if(!record.get<string>(CalendarTemplateElementTableSync::COL_MAX_DATE).empty())
				{
					try
					{
						value = from_string(record.get<string>(CalendarTemplateElementTableSync::COL_MAX_DATE));
					}
					catch(...)
					{
					}
				}
				if(value != getMaxDate())
				{
					result = true;
					setMaxDate(value);
				}
			}

			// Days modulo
			if(record.isDefined(CalendarTemplateElementTableSync::COL_INTERVAL))
			{
				days value(
					record.getDefault<long>(CalendarTemplateElementTableSync::COL_INTERVAL, 0)
				);
				if(value != getStep())
				{
					result = true;
					setStep(value);
				}
			}

			// Operation
			if(record.isDefined(CalendarTemplateElementTableSync::COL_POSITIVE))
			{
				Operation value(
					static_cast<Operation>(
						record.getDefault<int>(CalendarTemplateElementTableSync::COL_POSITIVE, 0)
				)	);
				if(value != getOperation())
				{
					result = true;
					setOperation(value);
				}
			}

			// Included calendar
//			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				if(record.isDefined(CalendarTemplateElementTableSync::COL_INCLUDE_ID))
				{
					CalendarTemplate* value(NULL);
					RegistryKeyType iid(
						record.getDefault<RegistryKeyType>(
							CalendarTemplateElementTableSync::COL_INCLUDE_ID,
							0
					)	);
					if(iid > 0)	try
					{
						value = CalendarTemplateTableSync::GetEditable(iid, env).get();
					}
					catch (ObjectNotFoundException<CalendarTemplate> e)
					{
						Log::GetInstance().warn("Data corrupted in " + CalendarTemplateElementTableSync::TABLE.NAME + "/" + CalendarTemplateElementTableSync::COL_INCLUDE_ID, e);
					}
					if(value != getInclude())
					{
						result = true;
						setInclude(value);
					}
				}
			}

			// Link with calendar template
//			if(linkLevel == UP_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				if(record.isDefined(CalendarTemplateElementTableSync::COL_CALENDAR_ID))
				{
					CalendarTemplate* value(NULL);
					RegistryKeyType id(
						record.getDefault<RegistryKeyType>(
							CalendarTemplateElementTableSync::COL_CALENDAR_ID,
							0
					)	);
					if(id > 0) try
					{
						value = CalendarTemplateTableSync::GetEditable(id, env).get();
					}
					catch (ObjectNotFoundException<CalendarTemplate> e)
					{
						Log::GetInstance().warn("Data corrupted in " + CalendarTemplateElementTableSync::TABLE.NAME + "/" + CalendarTemplateElementTableSync::COL_CALENDAR_ID, e);
					}
					if(value != getCalendar())
					{
						result = true;
						setCalendar(value);
					}
				}
			}

			return result;
		}



		void CalendarTemplateElement::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
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
}	}
