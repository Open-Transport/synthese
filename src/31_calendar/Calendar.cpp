
/** Calendar class implementation.
	@file Calendar.cpp

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

#include "Calendar.h"

#include "CalendarLink.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	namespace calendar
	{
		Calendar::Calendar(
			util::RegistryKeyType id
		):
			Registrable(id),
			_mutex(new recursive_mutex)
		{}



		Calendar::Calendar(
			const date& firstDate,
			const date& lastDate,
			date_duration step
		):	Registrable(0),
			_mutex(new recursive_mutex),
			_firstActiveDate(firstDate)
			// last active date cannot be constructed at this time
		{
			// Check pre-conditions in debug mode
			assert(!firstDate.is_not_a_date());
			assert(!lastDate.is_not_a_date());
			assert(firstDate <= lastDate);
			assert(step.days() > 0);

			// Lock
			recursive_mutex::scoped_lock lock(*_mutex);

			// Optimized version for all days of the range calendars
			if(step.days() == 1)
			{
				// Loop on years
				for(unsigned short y(firstDate.year()); y<=(lastDate.year()); ++y)
				{
					// Loop on days
					_BitSets::mapped_type bits;
					for(size_t i(y == firstDate.year() ? _BitPos(firstDate) : 0);
						i <= (y == lastDate.year() ? _BitPos(lastDate) : (gregorian_calendar::is_leap_year(y) ? 365 : 364));
						++i
					){
						bits.set(i);
					}

					// Storage
					_BitSets::iterator it(
						_markedDates.insert(
							make_pair(y, bits)
						).first
					);
				}

				// Last active date
				_lastActiveDate = lastDate;
			}
			else
			{
				// Locales
				_BitSets::iterator it(_markedDates.end());
				greg_year y(firstDate.year());
				date d;

				// Loop on days
				for(d = firstDate;
					d <= lastDate;
					d += step
				){
					// Allocate the bitset if necessary
					if(it == _markedDates.end() || d.year() != y)
					{
						it = _markedDates.insert(
							make_pair(d.year(), _BitSets::mapped_type())
						).first;
						y = d.year();
					}

					// Sets the date
					it->second.set(_BitPos(d));
				}

				// Last active date
				_lastActiveDate = d - step;
			}
		}



		Calendar::Calendar(
			const Calendar& other
		):	Registrable(0),
			_markedDates(other._markedDates),
			_mutex(new recursive_mutex),
			_firstActiveDate(other._firstActiveDate),
			_lastActiveDate(other._lastActiveDate)
		{}



		Calendar::Calendar(
			const std::string& serialized
		):	Registrable(0),
			_mutex(new recursive_mutex)
		{
			setFromSerializedString(serialized);
		}



		date Calendar::getFirstActiveDate(
		) const {
			if(!_firstActiveDate)
			{
				if(_markedDates.empty())
				{
					_firstActiveDate = gregorian::date();
				}
				else
				{
					recursive_mutex::scoped_lock lock(*_mutex);
					_BitSets::const_iterator it(_markedDates.begin());

					for(size_t p(0); p != 366; ++p)
					{
						if(it->second.test(p))
						{
							_firstActiveDate = date(it->first, Jan, 1) + days(long(p));
							break;
						}
					}
				}
			}
			return *_firstActiveDate;
		}



		date Calendar::getLastActiveDate(
		) const {
			if(!_lastActiveDate)
			{
				if(empty())
				{
					_lastActiveDate = gregorian::date();
				}
				else
				{
					recursive_mutex::scoped_lock lock(*_mutex);
					_BitSets::const_reverse_iterator it(_markedDates.rbegin());

					for(size_t p(366); p != 0; --p)
					{
						if(it->second.test(p-1))
						{
							_lastActiveDate = date(it->first, Jan, 1) + days(long(p-1));
							break;
						}
					}
				}
			}
			return *_lastActiveDate;
		}



		Calendar::DatesVector Calendar::getActiveDates(
		) const {
			recursive_mutex::scoped_lock lock(*_mutex);
			DatesVector result;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				date d(it.first, Jan, 1);
				for(size_t p(0); p != (gregorian_calendar::is_leap_year(it.first) ? 366 : 365); ++p)
				{
					if(it.second.test(p))
					{
						result.push_back(d + date_duration(long(p)));
					}
				}
			}
			return result;
		}



		bool Calendar::isActive(const date& d) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_BitSets::const_iterator it(_markedDates.find(d.year()));
			if(it == _markedDates.end()) return false;
			return it->second.test(_BitPos(d));
		}



		void Calendar::setActive(const date& d)
		{
			assert(!d.is_not_a_date());

			recursive_mutex::scoped_lock lock(*_mutex);
			_BitSets::iterator it(_markedDates.find(d.year()));
			if(it == _markedDates.end())
			{
				it = _markedDates.insert(
					make_pair(d.year(), _BitSets::mapped_type())
				).first;
			}

			it->second.set(_BitPos(d));

			_firstActiveDate.reset();
			_lastActiveDate.reset();
		}



		void Calendar::clear()
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates.clear();
			_firstActiveDate.reset();
			_lastActiveDate.reset();
		}



		void Calendar::setInactive(const date& d)
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			assert(!d.is_not_a_date());

			_BitSets::iterator it(_markedDates.find(d.year()));
			if(it == _markedDates.end())
				return;

			it->second.set(_BitPos(d), false);

			if(!it->second.any())
				_markedDates.erase(it);

			_firstActiveDate.reset();
			_lastActiveDate.reset();
		}



		Calendar& Calendar::operator&= (const Calendar& op)
		{
			recursive_mutex::scoped_lock lock(*_mutex);

			_BitSets markedDates;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				_BitSets::const_iterator it2(op._markedDates.find(it.first));
				if(it2 == op._markedDates.end())
				{
					continue;
				}

				_BitSets::mapped_type mask(it.second & it2->second);
				if(!mask.none())
				{
					markedDates[it.first] = mask;
				}
			}
			_markedDates = markedDates;

			_firstActiveDate.reset();
			_lastActiveDate.reset();
			return *this;
		}



		Calendar& Calendar::operator|=(
			const Calendar& op
		){
			recursive_mutex::scoped_lock lock(*_mutex);

			set<greg_year> years;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				years.insert(it.first);
			}
			BOOST_FOREACH(const _BitSets::value_type& it, op._markedDates)
			{
				years.insert(it.first);
			}
			BOOST_FOREACH(greg_year y, years)
			{
				_BitSets::const_iterator it1(_markedDates.find(y));
				_BitSets::const_iterator it2(op._markedDates.find(y));
				if(it1 == _markedDates.end())
				{
					_markedDates[y] = it2->second;
				}
				else
				{
					if(it2 != op._markedDates.end())
					{
						_markedDates[y] = it1->second | it2->second;
					}
				}
			}

			_firstActiveDate.reset();
			_lastActiveDate.reset();
			return *this;
		}




		Calendar Calendar::operator& (const Calendar& op2) const
		{
			Calendar dest(*this);
			dest &= op2;
			return dest;
		}



		Calendar Calendar::operator| (const Calendar& op2) const
		{
			Calendar dest(*this);
			dest |= op2;
			return dest;
		}



		bool Calendar::hasAtLeastOneCommonDateWith( const Calendar& op ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				_BitSets::const_iterator it2(op._markedDates.find(it.first));
				if(it2 == op._markedDates.end()) continue;

				if((it.second & it2->second).any()) return true;
			}
			return false;
		}



		bool Calendar::operator==( const Calendar& op ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _markedDates == op._markedDates;
		}



		Calendar& Calendar::operator-=( const Calendar& op )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			set<_BitSets::key_type> toBeRemoved;

			_BitSets markedDates;
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				_BitSets::const_iterator it2(op._markedDates.find(it.first));
				if(it2 == op._markedDates.end())
				{
					markedDates[it.first] = it.second;
					continue;
				}

				_BitSets::mapped_type opMask(it2->second);
				opMask.flip();
				opMask = opMask & it.second;
				if(!opMask.none())
				{
					markedDates[it.first] = opMask;
				}
			}
			_markedDates = markedDates;

			_firstActiveDate.reset();
			_lastActiveDate.reset();

			return *this;
		}



		size_t Calendar::_BitPos( const boost::gregorian::date& d )
		{
			return d.day_of_year() - 1;
		}



		bool Calendar::empty() const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				if(!it.second.none()) return false;
			}
			return true;
		}



		size_t Calendar::size() const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			size_t result(0);
			BOOST_FOREACH(const _BitSets::value_type& it, _markedDates)
			{
				result += it.second.count();
			}
			return result;
		}



		bool Calendar::operator!=( const Calendar& op ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _markedDates != op._markedDates;
		}



		void Calendar::copyDates( const Calendar& op )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates = op._markedDates;
			_firstActiveDate = op._firstActiveDate;
			_lastActiveDate = op._lastActiveDate;
		}



		void Calendar::serialize( std::ostream& stream ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			BOOST_FOREACH(const _BitSets::value_type& yearDates, _markedDates)
			{
				stream << yearDates.first;
				stream << yearDates.second;
			}
		}



		void Calendar::setFromSerializedString( const std::string& value )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates.clear();
			for(size_t p(0); p+369<value.size(); p += 370)
			{
				bitset<366> bits(value.substr(p+4, 366));
				if(bits.none())
				{
					continue;
				}
				_markedDates.insert(
					make_pair(
						greg_year(lexical_cast<unsigned short>(value.substr(p, 4))),
						bits
				)	);
			}
			_firstActiveDate.reset();
			_lastActiveDate.reset();
		}



		Calendar& Calendar::operator<<=( size_t i )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			BOOST_FOREACH(_BitSets::value_type& yearDates, _markedDates)
			{
				yearDates.second <<= i;
			}
			_firstActiveDate.reset();
			_lastActiveDate.reset();
			return *this;
		}



		Calendar Calendar::operator<<( size_t i )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			Calendar obj(*this);
			obj <<= i;
			_firstActiveDate.reset();
			_lastActiveDate.reset();
			return obj;
		}



		bool Calendar::includesDates( const Calendar& other ) const
		{
			return (*this & other) == other;
		}



		void Calendar::setCalendarFromLinks()
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			clear();
			BOOST_FOREACH(const CalendarLinks::value_type& link, _calendarLinks)
			{
				link->addDatesToCalendar();
			}

			// Dates to force
			BOOST_FOREACH(const DatesSet::value_type& d, _datesToForce)
			{
				setActive(d);
			}

			// Dates to bypass
			BOOST_FOREACH(const DatesSet::value_type& d, _datesToBypass)
			{
				setInactive(d);
			}
		}



		void Calendar::removeCalendarLink(
			const CalendarLink& link,
			bool updateCalendar
		){
			recursive_mutex::scoped_lock lock(*_mutex);
			_calendarLinks.erase(const_cast<CalendarLink*>(&link));
			if(updateCalendar)
			{
				setCalendarFromLinks();
			}
		}



		void Calendar::addCalendarLink(
			const CalendarLink& link,
			bool updateCalendar
		){
			recursive_mutex::scoped_lock lock(*_mutex);
			_calendarLinks.insert(const_cast<CalendarLink*>(&link));
			if(updateCalendar)
			{
				setCalendarFromLinks();
			}
		}



		bool Calendar::isLinked() const
		{
			return !_calendarLinks.empty();
		}
}	}
