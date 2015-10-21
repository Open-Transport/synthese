
/** Calendar Test implementation.
	@file CalendarTest.cpp

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
#include "CalendarTemplate.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/foreach.hpp>

using namespace synthese::calendar;
using namespace boost::gregorian;
using namespace boost;


BOOST_AUTO_TEST_CASE(CalendarTest)
{
	date d1(2009, Dec, 1);
	date d2(2011, Mar, 31);
	Calendar c(d1, d2, days(1));

	BOOST_CHECK(!c.empty());
	BOOST_CHECK(!c.isLinked());
	BOOST_CHECK_EQUAL(to_simple_string(c.getFirstActiveDate()), to_simple_string(d1));
	BOOST_CHECK_EQUAL(to_simple_string(c.getLastActiveDate()), to_simple_string(d2));
	for(date d(d1); d<=d2; d += days(1))
	{
		BOOST_CHECK(c.isActive(d));
	}
	for(date d(2009, Jan, 2); d<d1; d += days(1))
	{
		BOOST_CHECK(!c.isActive(d));
	}
	for(date d(d2 + days(1)); d<date(2011,Dec,31); d += days(1))
	{
		BOOST_CHECK(!c.isActive(d));
	}

	Calendar::DatesVector v(c.getActiveDates());
	BOOST_CHECK_EQUAL(v.size(), (size_t)d2.day_of_year() + 365 + 366 - d1.day_of_year());
	BOOST_CHECK_EQUAL(v.size(), c.size());
	date dtest(d1);
	BOOST_FOREACH(const Calendar::DatesVector::value_type& d, v)
	{
		BOOST_CHECK_EQUAL(to_simple_string(d), to_simple_string(dtest));
		dtest += days(1);
	}
	BOOST_CHECK(c.hasAtLeastOneCommonDateWith(c));
	BOOST_CHECK(c.includesDates(c));
	BOOST_CHECK(c == c);
	BOOST_CHECK(!(c != c));
}

BOOST_AUTO_TEST_CASE(CalendarTemplateTest)
{
	date d1(2009, Jan, 1);
	date d2(2009, Mar, 1);

	CalendarTemplate ct1;
	CalendarTemplateElement ct11;
	ct11.setMinDate(d1);
	ct11.setMaxDate(d2);
	ct11.setRank(0);
	ct11.setOperation(ADD);
	ct1.addElement(CalendarTemplateElement(ct11));

	Calendar r1(ct1.getResult());
	BOOST_CHECK_EQUAL(r1.size(), d2.day_of_year());
	for(date d(d1); d<=d2; d += days(1))
	{
		BOOST_CHECK(r1.isActive(d));
	}

	CalendarTemplate ct2;
	date d3(2009, Apr, 1);
	CalendarTemplateElement ct21;
	ct21.setMinDate(d2);
	ct21.setMaxDate(d3);
	ct21.setOperation(ADD);
	ct21.setRank(0);
	ct2.addElement(CalendarTemplateElement(ct21));

	Calendar r2(ct2.getResult());
	BOOST_CHECK_EQUAL(r2.size(), d3.day_of_year() - d2.day_of_year() + 1);
	for(date d(d2); d<=d3; d += days(1))
	{
		BOOST_CHECK(r2.isActive(d));
	}

	CalendarTemplateElement ct12;
	ct12.setInclude(&ct2);
	ct12.setOperation(ADD);
	ct12.setRank(1);
	ct1.addElement(ct12);

	Calendar r3(ct1.getResult());
	BOOST_CHECK_EQUAL(r3.size(), d3.day_of_year());
	for(date d(d1); d<=d3; d += days(1))
	{
		BOOST_CHECK(r3.isActive(d));
	}

	int i(0);
}

