
/** Journey Test implementation.
	@file JourneyTest.cpp

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

#include "FakeGraphImplementation.hpp"
#include "Journey.h"
#include "ServicePointer.h"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::graph;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;


BOOST_AUTO_TEST_CASE (TransferNumber)
{
	{	// Empty journey
		{
			Journey j1;
			BOOST_CHECK_EQUAL(j1.getTransportConnectionsCount(), 0ULL);
			BOOST_CHECK_EQUAL(j1.getDistance(), 0);
			BOOST_CHECK_EQUAL(j1.getEffectiveDuration().total_seconds(), 0);
		}

		// Journey with a road in it
		{
			FakePath L(true);
			FakeService S;
			S.setPath(&L);
			FakeHub h1;
			FakeVertex v3(&h1);
			FakeEdge e3;
			e3.setFromVertex(&v3);
			e3.setParentPath(&L);
			FakeEdge e31;
			e31.setFromVertex(&v3);
			e31.setParentPath(&L);
			e31.setMetricOffset(1);
			ptime d3_0s(date(2000,1,1), minutes(0));
			ptime d3_0e(date(2000,1,1), minutes(1));
			ServicePointer sp3_0(true, false, 0, S, d3_0s);
			sp3_0.setDepartureInformations(e3, d3_0s, d3_0s, *e3.getFromVertex());
			sp3_0.setArrivalInformations(e31, d3_0e, d3_0e, *e31.getFromVertex());
			Journey j4;
			j4.append(sp3_0);

			BOOST_CHECK_EQUAL(j4.getTransportConnectionsCount(), 0);
			BOOST_CHECK_EQUAL(j4.getDistance(), 1);
			BOOST_CHECK_EQUAL(j4.getEffectiveDuration().total_seconds(), 60);

			// Journey with a transport in it
			FakePath LF(false);
			FakeService SF;
			SF.setPath(&LF);
			FakeEdge e3F;
			e3F.setFromVertex(&v3);
			e3F.setParentPath(&LF);
			FakeEdge e3F1;
			e3F1.setFromVertex(&v3);
			e3F1.setParentPath(&LF);
			e3F1.setMetricOffset(1);
			ServicePointer sp3_0F(true, false, 0, SF, d3_0s);
			sp3_0F.setDepartureInformations(e3F, d3_0s, d3_0s, *e3F.getFromVertex());
			sp3_0F.setArrivalInformations(e3F1, d3_0e, d3_0e, *e3F1.getFromVertex());
			Journey j3;
			j3.append(sp3_0F);

			BOOST_CHECK_EQUAL(j3.getTransportConnectionsCount(), 1);
			BOOST_CHECK_EQUAL(j3.getDistance(), 1);
			BOOST_CHECK_EQUAL(j3.getEffectiveDuration().total_seconds(), 60);

			// Copy of transport
			Journey j5(j3);
			BOOST_CHECK_EQUAL(j5.getTransportConnectionsCount(), 1);
			BOOST_CHECK_EQUAL(j5.getDistance(), 1);
			BOOST_CHECK_EQUAL(j5.getEffectiveDuration().total_seconds(), 60);

			// Copy of road
			Journey j2(j4);
			BOOST_CHECK_EQUAL(j2.getTransportConnectionsCount(), 0);
			BOOST_CHECK_EQUAL(j2.getDistance(), 1);
			BOOST_CHECK_EQUAL(j2.getEffectiveDuration().total_seconds(), 60);

			// Merge of road and transport
			Journey j6(j4);
			j6.append(j3);
			BOOST_CHECK_EQUAL(j6.getTransportConnectionsCount(), 1);
			BOOST_CHECK_EQUAL(j6.getDistance(), 2);
			BOOST_CHECK_EQUAL(j6.getEffectiveDuration().total_seconds(), 120);

			// Merge of twice road
			Journey j7(j4);
			j7.append(j4);
			BOOST_CHECK_EQUAL(j7.getTransportConnectionsCount(), 0);
			BOOST_CHECK_EQUAL(j7.getDistance(), 2);
			BOOST_CHECK_EQUAL(j7.getEffectiveDuration().total_seconds(), 120);

			// Merge of twice transport
			Journey j8(j3);
			j8.append(j3);
			BOOST_CHECK_EQUAL(j8.getTransportConnectionsCount(), 2);
			BOOST_CHECK_EQUAL(j8.getDistance(), 2);
			BOOST_CHECK_EQUAL(j8.getEffectiveDuration().total_seconds(), 120);

			// Merge of road, transport, road
			Journey j9(j4);
			j9.append(j3);
			j9.append(j4);
			BOOST_CHECK_EQUAL(j9.getTransportConnectionsCount(), 1);
			BOOST_CHECK_EQUAL(j9.getDistance(), 3);
			BOOST_CHECK_EQUAL(j9.getEffectiveDuration().total_seconds(), 180);

			// Merge of transport, road, transport
			Journey j10(j3);
			j10.append(j4);
			j10.append(j3);
			BOOST_CHECK_EQUAL(j10.getTransportConnectionsCount(), 2);
			BOOST_CHECK_EQUAL(j10.getDistance(), 3);
			BOOST_CHECK_EQUAL(j10.getEffectiveDuration().total_seconds(), 180);

		}
	}
}
