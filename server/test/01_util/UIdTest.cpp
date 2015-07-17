
/** UId Test.
	@file UIdTest.cpp

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

#include "UtilTypes.h"

#include <iostream>
#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;


BOOST_AUTO_TEST_CASE (testEncodingDecoding)
{

    RegistryTableType tableId = 4; // 1125899906842624
    RegistryNodeType gridNodeId = 7; // 30064771072
    RegistryObjectType objectId = 256788;

    RegistryKeyType id = encodeUId (tableId, gridNodeId, objectId);

	BOOST_REQUIRE_EQUAL (id, 1125929971870484ULL);
    BOOST_REQUIRE_EQUAL (tableId, decodeTableId (id));
    BOOST_REQUIRE_EQUAL (gridNodeId, decodeGridNodeId (id));
    BOOST_REQUIRE_EQUAL (objectId, decodeObjectId (id));

}

