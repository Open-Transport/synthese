
/** ParametersMap Test.
	@file ParametersMapTest.cpp

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

#include "ParametersMap.h"

#include <iostream>
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

using namespace boost;
using namespace std;
using namespace synthese::util;

BOOST_AUTO_TEST_CASE (testParametersMap)
{
	{	// Default constructor
		ParametersMap pm;
		BOOST_CHECK_EQUAL(pm.getFormat(), ParametersMap::FORMAT_INTERNAL);
		BOOST_CHECK(pm.getMap().empty());
		BOOST_CHECK_THROW(pm.get<int>("a"), ParametersMap::MissingParameterException);
	}

	{	// URI constructor
		ParametersMap pm("a=1&b=toto");
		BOOST_CHECK_EQUAL(pm.getMap().size(), 2);
		BOOST_CHECK_EQUAL(pm.get<int>("a"), 1);
		BOOST_CHECK_EQUAL(pm.get<string>("b"), "toto");
	}

	{	// Multipart constructor
		ParametersMap pm(
			"-----------------------------2921238217421\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\n2097152\r\n-----------------------------2921238217421\r\nContent-Disposition: form-data; name=\"b\"; filename=\"notes.txt\"\r\nContent-Type: text/plain\r\n\r\nTablette : GTP1000\r\n\r\n-----------------------------2921238217421--",
			"---------------------------2921238217421"
		);
		BOOST_CHECK_EQUAL(pm.getMap().size(), 1);
		BOOST_CHECK_EQUAL(pm.get<string>("a", false), "2097152");
		ParametersMap::File f(pm.getFile("b"));
		try
		{
			BOOST_CHECK_EQUAL(f.content, "Tablette : GTP1000\r\n");
			BOOST_CHECK_EQUAL(f.filename, "notes.txt");
			BOOST_CHECK_EQUAL(string(f.mimeType), "text/plain");
		}
		catch(ParametersMap::MissingParameterException&)
		{
			BOOST_CHECK_MESSAGE(false, "b not present in files");
		}

		// Copy operator
		ParametersMap pm2;
		BOOST_CHECK_EQUAL(pm2.getFormat(), ParametersMap::FORMAT_INTERNAL);
		BOOST_CHECK(pm2.getMap().empty());

		pm2 = pm;
		BOOST_CHECK_EQUAL(pm2.getMap().size(), 1);
		BOOST_CHECK_EQUAL(pm2.get<string>("a", false), "2097152");
		try
		{
			ParametersMap::File f2(pm2.getFile("b"));
			BOOST_CHECK_EQUAL(f2.content, "Tablette : GTP1000\r\n");
			BOOST_CHECK_EQUAL(f2.filename, "notes.txt");
			BOOST_CHECK_EQUAL(string(f2.mimeType), "text/plain");
		}
		catch(ParametersMap::MissingParameterException&)
		{
			BOOST_CHECK_MESSAGE(false, "b not present in files");
		}
	}

	{	// Inline insertions
		ParametersMap pm;
		pm.insert("a", 23);
		pm.insert("b", string("toto"));
		BOOST_CHECK_EQUAL(pm.getMap().size(), 2);
		BOOST_CHECK_EQUAL(pm.get<int>("a"), 23);
		BOOST_CHECK_EQUAL(pm.get<string>("b"), "toto");

		// Inline removal
		pm.remove("a");
		BOOST_CHECK_EQUAL(pm.getMap().size(), 1);
		BOOST_CHECK_THROW(pm.get<int>("a"), ParametersMap::MissingParameterException);
		BOOST_CHECK_EQUAL(pm.get<string>("b"), "toto");
	}

	{	// Extract
		ParametersMap pm;
		pm.insert("ab12", 23);
		pm.insert("ab16", string("toto"));
		pm.insert("a", string("toto"));
		pm.insert("b16", string("tutu"));

		ParametersMap pm2(pm.getExtract("a"));
		BOOST_CHECK_EQUAL(pm2.getMap().size(), 2);
		BOOST_CHECK_EQUAL(pm2.get<int>("b12"), 23);
		BOOST_CHECK_EQUAL(pm2.get<string>("b16"), "toto");
	}

	{	// Sub map
		ParametersMap pm;
		pm.insert("a", 23);
		pm.insert("b", string("toto"));
		boost::shared_ptr<ParametersMap> subMap1(new ParametersMap);
		subMap1->insert("c", string("test1"));
		subMap1->insert("d", string("test1d"));
		pm.insert("d", subMap1);
		boost::shared_ptr<ParametersMap> subMap2(new ParametersMap);
		subMap2->insert("c", string("test2"));
		subMap2->insert("d", string("test2d"));
		pm.insert("d", subMap2);

		BOOST_CHECK_EQUAL(pm.get<int>("a"), 23);
		BOOST_CHECK_EQUAL(pm.get<string>("b"), "toto");
		BOOST_CHECK(pm.hasSubMaps("d"));
		const ParametersMap::SubParametersMap::mapped_type& subMaps(
			pm.getSubMaps("d")
		);

		BOOST_CHECK_EQUAL(subMaps.size(), 2);
		BOOST_CHECK_EQUAL(subMaps[0]->get<string>("c"), "test1");
		BOOST_CHECK_EQUAL(subMaps[0]->get<string>("d"), "test1d");
		BOOST_CHECK_EQUAL(subMaps[1]->get<string>("c"), "test2");
		BOOST_CHECK_EQUAL(subMaps[1]->get<string>("d"), "test2d");

		{	// Export URI
			stringstream s;
			pm.outputURI(s);
			BOOST_CHECK_EQUAL(s.str(), "a=23&b=toto&d_c=test1&d_d=test1d&d_c=test2&d_d=test2d");
		}

		{	// Export CSV
			stringstream s;
			pm.outputCSV(s, "d");
			stringstream s2;
			s2	<< "\"c\",\"d\"" << endl
				<< "\"test1\",\"test1d\"" << endl
				<< "\"test2\",\"test2d\"" << endl;
			BOOST_CHECK_EQUAL(s.str(), s2.str());
		}

		{	// Export JSON
			stringstream s;
			pm.outputJSON(s, "e");
			BOOST_CHECK_EQUAL(s.str(), "{\"e\":{\"a\":\"23\",\"b\":\"toto\",\"d\":[{\"c\":\"test1\",\"d\":\"test1d\"},{\"c\":\"test2\",\"d\":\"test2d\"}]}}");
		}

		{	// Export XML
			stringstream s;
			pm.outputXML(s, "e");
			BOOST_CHECK_EQUAL(s.str(), "<e a=\"23\" b=\"toto\"><d c=\"test1\" d=\"test1d\" /><d c=\"test2\" d=\"test2d\" /></e>");
		}
	}
    
}


BOOST_AUTO_TEST_CASE (testFromJsonWithTwoChildren)
{
    stringstream s;
    s << "{ \"a\": 1, \"b\": 2 }";
    boost::shared_ptr<ParametersMap> pm(ParametersMap::FromJson(s.str()));
    BOOST_CHECK_EQUAL("1", pm->getValue("a"));
    BOOST_CHECK_EQUAL("2", pm->getValue("b"));    
    
}

BOOST_AUTO_TEST_CASE (testFromJsonWithOneChildAndSubChild)
{
    stringstream s;
    s << "{ \"a\": { \"b\": 2 }}";
    boost::shared_ptr<ParametersMap> pm(ParametersMap::FromJson(s.str()));
    BOOST_CHECK_EQUAL("2", pm->getSubMaps("a").at(0)->getValue("b"));
    
}



