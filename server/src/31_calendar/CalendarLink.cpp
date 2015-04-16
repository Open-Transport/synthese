
/** CalendarLink class implementation.
	@file CalendarLink.cpp

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

#include "CalendarLink.hpp"

#include "Calendar.h"
#include "CalendarLinkTableSync.hpp"
#include "CalendarTemplateTableSync.h"
#include "Fetcher.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace util;

	namespace util
	{
		template<> const string Registry<CalendarLink>::KEY("CalendarLink");
	}

	namespace calendar
	{
		CalendarLink::CalendarLink(
			RegistryKeyType id
		):	Registrable(id),
			_calendar(NULL),
			_calendarTemplate(NULL),
			_calendarTemplate2(NULL)
		{}



		void CalendarLink::addDatesToBitSets(Calendar::BitSets& bitsets) const
		{
			// Do nothing if both calendar templates are null
			if(!_calendarTemplate && !_calendarTemplate2)
			{
				return;
			}

			// Base calendar
			Calendar mask;
			if(!_startDate.is_not_a_date() && !_endDate.is_not_a_date())
			{
				mask = Calendar(_startDate, _endDate);
			}
			else if(_calendarTemplate && _calendarTemplate->isLimited())
			{
				mask = _calendarTemplate->getResult();
			}
			else if(_calendarTemplate2 && _calendarTemplate2->isLimited())
			{
				mask = _calendarTemplate2->getResult();
			}
			else
			{
				return; // Result is not limited : unable to build a calendar
			}

			// Apply templates masks
			if(_calendarTemplate)
			{
				Calendar result(_calendarTemplate->getResult(mask));
				if(_calendarTemplate2)
				{
					bitsets |= _calendarTemplate2->getResult(result)._getDatesCache();
				}
				else
				{
					bitsets |= result._getDatesCache();
				}
			}
			else
			{
				bitsets |= _calendarTemplate2->getResult(mask)._getDatesCache();
			}
		}



		bool CalendarLink::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Start date
			if(record.isDefined(CalendarLinkTableSync::COL_START_DATE))
			{
				date value;
				try
				{
					string text(record.get<string>(CalendarLinkTableSync::COL_START_DATE));
					if(!text.empty())
					{
						value = from_string(text);
					}
				}
				catch(...)
				{
				}
				if(value != getStartDate())
				{
					setStartDate(value);
					result = true;
				}
			}

			// End date
			if(record.isDefined(CalendarLinkTableSync::COL_END_DATE))
			{
				date value;
				try
				{
					string text(record.get<string>(CalendarLinkTableSync::COL_END_DATE));
					if(!text.empty())
					{
						value = from_string(text);
					}
				}
				catch(...)
				{
				}
				if(value != getEndDate())
				{
					setEndDate(value);
					result = true;
				}
			}

//			if (linkLevel == UP_LINKS_LOAD_LEVEL || linkLevel == UP_DOWN_LINKS_LOAD_LEVEL || linkLevel == ALGORITHMS_OPTIMIZATION_LOAD_LEVEL)
			{
				// Calendar template
				if(record.isDefined(CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID))
				{
					CalendarTemplate* value(NULL);
					RegistryKeyType calendarTemplateId(
						record.getDefault<RegistryKeyType>(
							CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID,
							0
					)	);
					if(calendarTemplateId > 0) try
					{
						value = CalendarTemplateTableSync::GetEditable(calendarTemplateId, env).get();
					}
					catch(ObjectNotFoundException<CalendarTemplate>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(calendarTemplateId) + " for calendar in service calendar link " + lexical_cast<string>(getKey()));
					}
					if(value != getCalendarTemplate())
					{
						setCalendarTemplate(value);
						result = true;
					}
				}

				// Calendar template 2
				if(record.isDefined(CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2))
				{
					CalendarTemplate* value(NULL);
					RegistryKeyType calendarTemplateId(
						record.getDefault<RegistryKeyType>(
							CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2,
							0
					)	);
					if(calendarTemplateId > 0) try
					{
						value = CalendarTemplateTableSync::GetEditable(calendarTemplateId, env).get();
					}
					catch(ObjectNotFoundException<CalendarTemplate>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(calendarTemplateId) + " for calendar 2 in service calendar link " + lexical_cast<string>(getKey()));
					}
					if(value != getCalendarTemplate2())
					{
						setCalendarTemplate2(value);
						result = true;
					}
				}

				// Service
				// Must stay at the last position because the service reads the object content
				if(record.isDefined(CalendarLinkTableSync::COL_SERVICE_ID))
				{
					Calendar* value(NULL);
					RegistryKeyType serviceId(
						record.getDefault<RegistryKeyType>(
							CalendarLinkTableSync::COL_SERVICE_ID,
							0
					)	);
					if(serviceId > 0) try
					{
						value = dynamic_cast<Calendar*>(Fetcher<Calendar>::FetchEditable(serviceId, env).get());
					}
					catch(ObjectNotFoundException<Calendar>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(serviceId) + " for calendar in link " + lexical_cast<string>(getKey()));
					}
					if(value != getCalendar())
					{
						setCalendar(value);
						result = true;
					}
				}
			}
			return result;
		}



		void CalendarLink::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(getCalendar())
			{
				getCalendar()->addCalendarLink(*this, true);
			}
		}



		synthese::LinkedObjectsIds CalendarLink::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			RegistryKeyType templateId(
				record.getDefault<RegistryKeyType>(
					CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID,
					0
			)	);
			if(templateId)
			{
				result.push_back(templateId);
			}
			RegistryKeyType templateId2(
				record.getDefault<RegistryKeyType>(
					CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2,
					0
			)	);
			if(templateId2)
			{
				result.push_back(templateId2);
			}
			RegistryKeyType serviceId(
				record.getDefault<RegistryKeyType>(
					CalendarLinkTableSync::COL_SERVICE_ID,
					0
			)	);
			if(serviceId)
			{
				result.push_back(serviceId);
			}
			return result;
		}



		void CalendarLink::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + CalendarLinkTableSync::COL_SERVICE_ID,
				getCalendar() ? getCalendar()->getKey() : 0
			);
			pm.insert(
				prefix + CalendarLinkTableSync::COL_START_DATE,
				(getStartDate().is_special() || getStartDate().is_not_a_date()) ? string() : to_iso_extended_string(getStartDate())
			);
			pm.insert(
				prefix + CalendarLinkTableSync::COL_END_DATE,
				(getEndDate().is_special() || getEndDate().is_not_a_date()) ? string() : to_iso_extended_string(getEndDate())
			);
			pm.insert(
				prefix + CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID,
				getCalendarTemplate() ? getCalendarTemplate()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + CalendarLinkTableSync::COL_CALENDAR_TEMPLATE_ID2,
				getCalendarTemplate2() ? getCalendarTemplate2()->getKey() : RegistryKeyType(0)
			);
		}
}	}
