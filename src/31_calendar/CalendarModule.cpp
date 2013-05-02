
/** CalendarModule class implementation.
	@file CalendarModule.cpp

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

#include "CalendarModule.h"
#include "Calendar.h"
#include "CalendarTemplate.h"
#include "Env.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/date_time/gregorian/formatters.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<ModuleClass,CalendarModule>::FACTORY_KEY("19_calendar");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<CalendarModule>::NAME("Calendriers");

		template<> void ModuleClassTemplate<CalendarModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<CalendarModule>::Init()
		{
		}

		template<> void ModuleClassTemplate<CalendarModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<CalendarModule>::End()
		{
		}



		template<> void ModuleClassTemplate<CalendarModule>::InitThread(
			
			){
		}



		template<> void ModuleClassTemplate<CalendarModule>::CloseThread(
			
			){
		}
	}

	namespace calendar
	{
		CalendarModule::BaseCalendar CalendarModule::GetBestCalendarTitle(
			const Calendar& calendar,
			const Calendar& mask
		){
			CalendarTitlesGenerator generator(mask);
			Calendar maskedCalendar(calendar & mask);
			return generator.getBestCalendarTitle(maskedCalendar);
		}



		CalendarModule::CalendarTitlesGenerator::CalendarTitlesGenerator( const Calendar& mask )
		{
			_value.clear();
			BOOST_FOREACH(const Registry<CalendarTemplate>::value_type& calendarTpl, Env::GetOfficialEnv().getRegistry<CalendarTemplate>())
			{
				Calendar maskedCandidate(calendarTpl.second->getResult(mask));
				if(maskedCandidate.empty())
				{
					continue;
				}
				_value.push_back(make_pair(maskedCandidate, calendarTpl.second.get()));
			}
		}



		CalendarModule::BaseCalendar CalendarModule::CalendarTitlesGenerator::getBestCalendarTitle(
			const Calendar& calendar
		){
			Value::const_iterator result(_value.end());

			// Search for exact match
			for(Value::const_iterator itCal(_value.begin()); itCal != _value.end(); ++itCal)
			{
				if(itCal->first != calendar)
				{
					continue;
				}

				if(	result == _value.end() ||
					itCal->second->getCategory() < result->second->getCategory() ||
					(itCal->second->getCategory() == result->second->getCategory() &&
					 itCal->second->getName().size() < result->second->getName().size())
				){
					result = itCal;
				}
			}

			if(result != _value.end())
			{
				return
					make_pair(
						result->second,
						result->second->getName()
					);
			}

			// Search for match with max 10 dates of difference 
			size_t bestDifference(11);
			Calendar bestAddings;
			Calendar bestRemovals;
			for(Value::const_iterator itCal(_value.begin()); itCal != _value.end(); ++itCal)
			{
				// Difference
				Calendar commonDates(calendar);
				commonDates &= itCal->first;
				Calendar addings(calendar);
				addings -= commonDates;
				size_t addingsSize(addings.size());
				if(addingsSize > 10)
				{
					continue;
				}
				Calendar removals(itCal->first);
				removals -= commonDates;
				size_t removalsSize(removals.size());
				if(removalsSize + addingsSize > bestDifference)
				{
					continue;
				}

				if(	result == _value.end() ||
					removalsSize + addingsSize < bestDifference ||
					(	removalsSize + addingsSize == bestDifference &&
						(	itCal->second->getCategory() < result->second->getCategory() ||
							(	itCal->second->getCategory() == result->second->getCategory() &&
								itCal->second->getName().size() < result->second->getName().size()
				)	)	)	){
					result = itCal;
					bestDifference = removalsSize + addingsSize;
					bestAddings = addings;
					bestRemovals = removals;
				}
			}

			if(result != _value.end())
			{
				stringstream strresult;
				strresult << result->second->getName();
				if(bestAddings.size())
				{
					strresult << ", ainsi que le";
					if(bestAddings.size() > 1)
					{
						strresult << "s";
					}

					bool first(true);
					BOOST_FOREACH(const Calendar::DatesVector::value_type& date, bestAddings.getActiveDates())
					{
						strresult << (first ? string() : ",") << " " << int(date.day()) << "/" << int(date.month()) << "/" << date.year();
						first=false;
					}
				}
				if(bestRemovals.size())
				{
					strresult << ", sauf le";
					if(bestRemovals.size() > 1)
					{
						strresult << "s";
					}

					bool first(true);
					BOOST_FOREACH(const Calendar::DatesVector::value_type& date, bestRemovals.getActiveDates())
					{
						strresult << (first ? string() : ",") << " " << int(date.day()) << "/" << int(date.month()) << "/" << date.year();
						first=false;
					}
				}
				CalendarTemplate* nullCalendar(NULL);
				return
					make_pair(
						nullCalendar,
						strresult.str()
					);
			}


			// If not template found, generation of a generic description text
			stringstream strresult;
			bool first(true);
			Calendar::DatesVector dates(calendar.getActiveDates());
			BOOST_FOREACH(const Calendar::DatesVector::value_type& date, dates)
			{
				strresult << (first ? string() : ",") << " " << int(date.day()) << "/" << int(date.month()) << "/" << date.year();
				first=false;
			}
			CalendarTemplate* nullCalendar(NULL);
			return
				make_pair(
					nullCalendar,
					strresult.str()
				);
		}
}	}
