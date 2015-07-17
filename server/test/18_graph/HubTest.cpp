
/** Hub Test implementation.
	@file HubTest.cpp

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
#include "Hub.h"

#pragma GCC diagmostic ignored "-Werror=sign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

using namespace synthese::graph;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;


BOOST_AUTO_TEST_CASE (UsefulForTransfer)
{
	// No edge
	FakeHub h1(true);
	FakeHub h2(true);
	FakeHub h3(true);

	// No vertex
//	BOOST_CHECK(!h1.isUsefulForTransfer());

	// One vertex no edge
	FakeVertex v1(&h1);
//	BOOST_CHECK(!h.isUsefulForTransfer());

	// One vertex one edge
	FakePathGroup pg;
	FakePath p1(false);
	p1.setPathGroup(&pg);
	FakeEdge e1;
	e1.setFromVertex(&v1);
}
