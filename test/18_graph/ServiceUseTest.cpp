
/** ServiceUse Test implementation.
	@file VertexAccessMapTest.cpp

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

#include "ServiceUse.h"
#include "FakeGraphImplementation.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::graph;
using namespace synthese::geometry;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;


BOOST_AUTO_TEST_CASE (Reverse)
{
	{ // Reversing scheduled service based ServiceUse
		FakePath L;
		FakeService S;
		FakeHub h1;

		FakeVertex v2(&h1);
		FakeEdge e2;
		e2.setFromVertex(&v2);
		e2.setParentPath(&L);

		FakeVertex v3(&h1);
		FakeEdge e3;
		e3.setFromVertex(&v3);
		e3.setParentPath(&L);

		ServicePointer sp(false, DEPARTURE_TO_ARRIVAL, 0, &e2);
		sp.setService(&S);
		ptime ds(date(2000,1,1), minutes(0));
		sp.setActualTime(ds);
		ptime de(date(2000,1,1), minutes(5));
		ServiceUse su(sp,&e3,de);

		su.reverse();

		BOOST_CHECK_EQUAL(su.getMethod(), ARRIVAL_TO_DEPARTURE);
		BOOST_CHECK_EQUAL(su.getEdge(), &e3);
		BOOST_CHECK_EQUAL(su.getSecondEdge(), &e2);
		BOOST_CHECK_EQUAL(su.getRealTimeVertex(), &v3);
		BOOST_CHECK_EQUAL(su.getSecondVertex(), &v2);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(ds));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(de));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getSecondActualDateTime()), boost::posix_time::to_simple_string(ds));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getActualDateTime()), boost::posix_time::to_simple_string(de));

		su.reverse();

		BOOST_CHECK_EQUAL(su.getMethod(), DEPARTURE_TO_ARRIVAL);
		BOOST_CHECK_EQUAL(su.getEdge(), &e2);
		BOOST_CHECK_EQUAL(su.getSecondEdge(), &e3);
		BOOST_CHECK_EQUAL(su.getRealTimeVertex(), &v2);
		BOOST_CHECK_EQUAL(su.getSecondVertex(), &v3);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(ds));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(de));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getActualDateTime()), boost::posix_time::to_simple_string(ds));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getSecondActualDateTime()), boost::posix_time::to_simple_string(de));
	}

	{ // 7799 Reversing continuous service based ServiceUse with range greater than journey duration
		FakePath L;
		FakeService S;
		FakeHub h1;

		FakeVertex v2(&h1);
		FakeEdge e2;
		e2.setFromVertex(&v2);
		e2.setParentPath(&L);

		FakeVertex v3(&h1);
		FakeEdge e3;
		e3.setFromVertex(&v3);
		e3.setParentPath(&L);

		ServicePointer sp(false, DEPARTURE_TO_ARRIVAL, 0, &e2);
		sp.setService(&S);
		ptime ds(date(2000,1,1), minutes(0));
		sp.setActualTime(ds);
		time_duration range(minutes(10));
		sp.setServiceRange(range);
		ptime de(date(2000,1,1), minutes(5));
		ServiceUse su(sp,&e3,de);
		
		su.reverse();

		BOOST_CHECK_EQUAL(su.getMethod(), ARRIVAL_TO_DEPARTURE);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(ds + range));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(de + range));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getSecondActualDateTime()), boost::posix_time::to_simple_string(ds + range));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getActualDateTime()), boost::posix_time::to_simple_string(de + range));

		su.reverse();

		BOOST_CHECK_EQUAL(su.getMethod(), DEPARTURE_TO_ARRIVAL);
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getDepartureDateTime()), boost::posix_time::to_simple_string(ds));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getArrivalDateTime()), boost::posix_time::to_simple_string(de));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getActualDateTime()), boost::posix_time::to_simple_string(ds));
		BOOST_CHECK_EQUAL(boost::posix_time::to_simple_string(su.getSecondActualDateTime()), boost::posix_time::to_simple_string(de));
	}
}
