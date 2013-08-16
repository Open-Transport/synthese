
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
			_markedDates(firstDate, lastDate, step),
			_firstActiveDate(firstDate),
			// last active date cannot be constructed at this time
			_mutex(new recursive_mutex)
		{
		}



		Calendar::Calendar(
			const Calendar& other
		):	Registrable(0),
			_markedDates(other._markedDates),
			_firstActiveDate(other._firstActiveDate),
			_lastActiveDate(other._lastActiveDate),
			_calendarLinks(other._calendarLinks),
			_mutex(new recursive_mutex)
		{}



		Calendar::Calendar(
			const std::string& serialized
		):	Registrable(0),
			_mutex(new recursive_mutex)
		{
			setFromSerializedString(serialized);
		}


		
		boost::gregorian::date Calendar::BitSets::getFirstActiveDate() const
		{
			if(_value.empty())
			{
				return gregorian::date();
			}
			else
			{
				_BitSets::const_iterator it(_value.begin());

				for(size_t p(0); p != 366; ++p)
				{
					if(it->second.test(p))
					{
						return date(it->first, Jan, 1) + days(long(p));
					}
				}
			}

			// Should never happen
			return gregorian::date();
		}


		date Calendar::getFirstActiveDate(
		) const {
			if(!_firstActiveDate)
			{
				recursive_mutex::scoped_lock lock(*_mutex);
				_firstActiveDate = _getDatesCache().getFirstActiveDate();
			}
			return *_firstActiveDate;
		}



		boost::gregorian::date Calendar::BitSets::getLastActiveDate() const
		{
			if(_value.empty())
			{
				return gregorian::date();
			}
			else
			{
				_BitSets::const_reverse_iterator it(_value.rbegin());

				for(size_t p(366); p != 0; --p)
				{
					if(it->second.test(p-1))
					{
						return date(it->first, Jan, 1) + days(long(p-1));
					}
				}
			}

			// Should never happen
			return gregorian::date();
		}



		date Calendar::getLastActiveDate(
		) const {
			if(!_lastActiveDate)
			{
				_lastActiveDate = _getDatesCache().getLastActiveDate();
			}
			return *_lastActiveDate;
		}



		
		Calendar::DatesVector Calendar::BitSets::getActiveDates() const
		{
			DatesVector result;
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
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



		Calendar::DatesVector Calendar::getActiveDates(
		) const {
			recursive_mutex::scoped_lock lock(*_mutex);
			return _getDatesCache().getActiveDates();
		}


		
		bool Calendar::BitSets::isActive( const boost::gregorian::date& d ) const
		{
			_BitSets::const_iterator it(_value.find(d.year()));
			if(it == _value.end()) return false;
			return it->second.test(_BitPos(d));
		}


		bool Calendar::isActive(const date& d) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _getDatesCache().isActive(d);
		}



		void Calendar::BitSets::setActive( const boost::gregorian::date& d )
		{
			_BitSets::iterator it(_value.find(d.year()));
			if(it == _value.end())
			{
				it = _value.insert(
					make_pair(d.year(), _BitSets::mapped_type())
				).first;
			}

			it->second.set(_BitPos(d));
		}



		void Calendar::setActive(const date& d)
		{
			assert(!d.is_not_a_date());

			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates.setActive(d);
		
			_resetDatesCache();
		}



		void Calendar::clear()
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates.clear();
			_calendarLinks.clear();
			_datesToBypass.clear();
			_datesToForce.clear();
			_resetDatesCache();
		}



		void Calendar::BitSets::setInactive( const boost::gregorian::date& d )
		{
			_BitSets::iterator it(_value.find(d.year()));
			if(it == _value.end())
			{
				return;
			}

			it->second.set(_BitPos(d), false);

			if(!it->second.any())
			{
				_value.erase(it);
			}
		}



		void Calendar::setInactive(const date& d)
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			assert(!d.is_not_a_date());

			_markedDates.setInactive(d);

			_resetDatesCache();
		}



		Calendar::BitSets& Calendar::BitSets::operator&=( const BitSets& op )
		{
			_BitSets markedDates;
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
			{
				_BitSets::const_iterator it2(op._value.find(it.first));
				if(it2 == op._value.end())
				{
					continue;
				}

				_BitSets::mapped_type mask(it.second & it2->second);
				if(!mask.none())
				{
					markedDates[it.first] = mask;
				}
			}
			_value = markedDates;

			return *this;
		}



		Calendar& Calendar::operator&= (const Calendar& op)
		{
			recursive_mutex::scoped_lock lock(*_mutex);

			_markedDates &= op._getDatesCache();

			_resetDatesCache();
			return *this;
		}



		Calendar& Calendar::operator|=(
			const Calendar& op
		){
			recursive_mutex::scoped_lock lock(*_mutex);

			_markedDates.operator |=(op._getDatesCache());

			_resetDatesCache();
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



		
		bool Calendar::BitSets::hasAtLeastOneCommonDateWith( const BitSets& op ) const
		{
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
			{
				_BitSets::const_iterator it2(op._value.find(it.first));
				if(it2 == op._value.end()) continue;

				if((it.second & it2->second).any()) return true;
			}
			return false;
		}


		bool Calendar::hasAtLeastOneCommonDateWith( const Calendar& op ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _getDatesCache().hasAtLeastOneCommonDateWith(op._getDatesCache());
		}



		bool Calendar::operator==( const Calendar& op ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _getDatesCache() == op._getDatesCache();
		}

	


		Calendar::BitSets& Calendar::BitSets::operator-=( const BitSets& op )
		{
			set<_BitSets::key_type> toBeRemoved;

			_BitSets markedDates;
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
			{
				_BitSets::const_iterator it2(op._value.find(it.first));
				if(it2 == op._value.end())
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
			_value= markedDates;

			return *this;
		}



		Calendar& Calendar::operator-=( const Calendar& op )
		{
			recursive_mutex::scoped_lock lock(*_mutex);

			_markedDates -= op._getDatesCache();
			_resetDatesCache();

			return *this;
		}



		size_t Calendar::BitSets::_BitPos( const boost::gregorian::date& d )
		{
			return d.day_of_year() - 1;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Tests if the calendar has no activated date.
		/// @return true if the calendar has no activated date.
		bool Calendar::BitSets::empty() const
		{
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
			{
				if(!it.second.none()) return false;
			}
			return true;
		}


		
		bool Calendar::empty() const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _getDatesCache().empty();
		}


		size_t Calendar::BitSets::size() const
		{
			size_t result(0);
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
			{
				result += it.second.count();
			}
			return result;
		}



		size_t Calendar::size() const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return _getDatesCache().size();
		}



		bool Calendar::operator!=( const Calendar& op ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			return !(_getDatesCache() == op._getDatesCache());
		}



		
		void Calendar::BitSets::copyDates( const BitSets& calendar )
		{
			_value = calendar._value;
		}



		void Calendar::copyDates( const Calendar& op )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates = op._getDatesCache();
			_resetDatesCache();
		}



		void Calendar::BitSets::serialize( std::ostream& stream ) const
		{
			BOOST_FOREACH(const _BitSets::value_type& yearDates, _value)
			{
				stream << yearDates.first;
				stream << yearDates.second;
			}
		}



		void Calendar::serialize( std::ostream& stream ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates.serialize(stream);
		}



		
		void Calendar::BitSets::setFromSerializedString( const std::string& value )
		{
			_value.clear();
			for(size_t p(0); p+369<value.size(); p += 370)
			{
				bitset<366> bits(value.substr(p+4, 366));
				if(bits.none())
				{
					continue;
				}
				_value.insert(
					make_pair(
						greg_year(lexical_cast<unsigned short>(value.substr(p, 4))),
						bits
				)	);
			}
		}



		void Calendar::setFromSerializedString( const std::string& value )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates.setFromSerializedString(value);
			_resetDatesCache();
		}



		Calendar::BitSets& Calendar::BitSets::operator<<=( size_t i )
		{
			BOOST_FOREACH(_BitSets::value_type& yearDates, _value)
			{
				yearDates.second <<= i;
			}
			return *this;
		}



		Calendar& Calendar::operator<<=( size_t i )
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			_markedDates <<= i;
			_resetDatesCache();
			return *this;
		}



		Calendar Calendar::operator<<( size_t i ) const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			Calendar obj(*this);
			obj <<= i;
			return obj;
		}



		bool Calendar::includesDates( const Calendar& other ) const
		{
			return (*this & other) == other;
		}



		void Calendar::removeCalendarLink(
			const CalendarLink& link,
			bool updateCalendar
		){
			recursive_mutex::scoped_lock lock(*_mutex);
			_calendarLinks.erase(const_cast<CalendarLink*>(&link));
			_resetDatesCache();
		}



		void Calendar::addCalendarLink(
			const CalendarLink& link,
			bool updateCalendar
		){
			recursive_mutex::scoped_lock lock(*_mutex);
			_calendarLinks.insert(const_cast<CalendarLink*>(&link));
			_resetDatesCache();
		}



		bool Calendar::isLinked() const
		{
			return !_calendarLinks.empty();
		}



		void Calendar::_resetDatesCache() const
		{
			_firstActiveDate.reset();
			_lastActiveDate.reset();
			_datesCache.reset();
		}



		const Calendar::BitSets& Calendar::_getDatesCache() const
		{
			recursive_mutex::scoped_lock lock(*_mutex);
			if(!_datesCache)
			{
				if(_calendarLinks.empty())
				{
					_datesCache = _markedDates;
				}
				else
				{
					BitSets cache;
					BOOST_FOREACH(const CalendarLinks::value_type& link, _calendarLinks)
					{
						link->addDatesToBitSets(cache);
					}

					// Dates to force
					BOOST_FOREACH(const DatesSet::value_type& d, _datesToForce)
					{
						cache.setActive(d);
					}

					// Dates to bypass
					BOOST_FOREACH(const DatesSet::value_type& d, _datesToBypass)
					{
						cache.setInactive(d);
					}

					_datesCache = cache;
				}
			}
			return *_datesCache;
		}



		Calendar::BitSets& Calendar::BitSets::operator|=( const Calendar::BitSets& op )
		{
			set<greg_year> years;
			BOOST_FOREACH(const _BitSets::value_type& it, _value)
			{
				years.insert(it.first);
			}
			BOOST_FOREACH(const _BitSets::value_type& it, op._value)
			{
				years.insert(it.first);
			}
			BOOST_FOREACH(greg_year y, years)
			{
				_BitSets::const_iterator it1(_value.find(y));
				_BitSets::const_iterator it2(op._value.find(y));
				if(it1 == _value.end())
				{
					_value[y] = it2->second;
				}
				else
				{
					if(it2 != op._value.end())
					{
						_value[y] = it1->second | it2->second;
					}
				}
			}
			return *this;
		}



		Calendar::BitSets::BitSets(
			const boost::gregorian::date& firstDate,
			const boost::gregorian::date& lastDate,
			boost::gregorian::date_duration step /*= boost::gregorian::days(1) */
		){
			// Check pre-conditions in debug mode
			assert(!firstDate.is_not_a_date());
			assert(!lastDate.is_not_a_date());
			assert(firstDate <= lastDate);
			assert(step.days() > 0);

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
						_value.insert(
							make_pair(y, bits)
						).first
					);
				}
			}
			else
			{
				// Locales
				_BitSets::iterator it(_value.end());
				greg_year y(firstDate.year());
				date d;

				// Loop on days
				for(d = firstDate;
					d <= lastDate;
					d += step
				){
					// Allocate the bitset if necessary
					if(it == _value.end() || d.year() != y)
					{
						it = _value.insert(
							make_pair(d.year(), _BitSets::mapped_type())
						).first;
						y = d.year();
					}

					// Sets the date
					it->second.set(_BitPos(d));
				}
			}
		}



		Calendar::BitSets::BitSets()
		{

		}

		void Calendar::BitSets::clear()
		{
			_value.clear();
		}



		bool Calendar::BitSets::operator==( const BitSets& op ) const
		{
			return _value == op._value;
		}
}	}
