
/** PegaseFileFormatTest class implementation.
	@file PegaseFileFormatTest.cpp
	@author Sylvain Pasche

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

// scons workaround
#ifdef PEGASE_TEST_SQL

#include "PegaseFileFormat.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace synthese::pt;
using namespace synthese::data_exchange;
using namespace std;
using synthese::Exception;

BOOST_AUTO_TEST_CASE (testJourneyPatternCalendarScheduledService)
{

	ifstream sqlStream(PEGASE_TEST_SQL);
	PegaseFileFormat::SQLDumpParser parser(sqlStream);

	parser.setTableToParse("TABLE1");
	BOOST_REQUIRE(parser.getRow());
	BOOST_CHECK_EQUAL(parser.getCell("string_col"), "foo'bar\non several lines");
	BOOST_CHECK_EQUAL(parser.getCell("null_col"), "NULL");
	BOOST_CHECK_EQUAL(parser.getCell("int1"), "123");
	BOOST_REQUIRE(parser.getRow());
	BOOST_CHECK_EQUAL(parser.getCell("string_col"), "another\nstring");
	BOOST_REQUIRE(!parser.getRow());

	parser.setTableToParse("TABLE2");
	BOOST_REQUIRE(parser.getRow());
	BOOST_CHECK_EQUAL(parser.getCell("string_col"), "double'quotes");
	BOOST_CHECK_THROW(parser.getCell("invalid_column"), Exception);
	BOOST_CHECK_EQUAL(parser.getCell("null_col"), "NULL");
	BOOST_REQUIRE(parser.getRow());
	BOOST_CHECK_EQUAL(parser.getCell("string_col"), "another");
	BOOST_CHECK_EQUAL(parser.getCell("null_col"), "NULL");
	BOOST_REQUIRE(!parser.getRow());

	parser.setTableToParse("ITINERAIRE");
	BOOST_REQUIRE(parser.getRow());
	BOOST_REQUIRE(parser.getRow());
	BOOST_REQUIRE(parser.getRow());
	BOOST_REQUIRE(!parser.getRow());

	parser.setTableToParse("POINT_D_ARRET");
	BOOST_REQUIRE(parser.getRow());
	BOOST_REQUIRE(parser.getRow());
	BOOST_REQUIRE(!parser.getRow());

}
#else
int main() {
	return 0;
}
#endif
