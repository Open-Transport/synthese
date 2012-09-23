
/** WebpageContent unit test.
	@file WebpageContentTest.cpp

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

#include "IfFunction.hpp"
#include "ParametersMap.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "Website.hpp"
#include "WebpageContent.hpp"
#include "StaticFunctionRequest.h"

#include <boost/test/auto_unit_test.hpp>

using namespace boost;
using namespace std;
using namespace synthese::util;
using namespace synthese::cms;
using namespace synthese::server;
using namespace synthese;

BOOST_AUTO_TEST_CASE (WebpageContentTest)
{
	// Factory initialization
	IfFunction::integrate();

	StaticFunctionRequest<WebPageDisplayFunction> request;
	ParametersMap additionalParametersMap;
	ParametersMap variables;
	Website site;
	Webpage page;
	page.setRoot(&site);

	{ // Empty content
		string code;
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), true);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, code);
	}

	{ // Only a space
		string code(" ");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, code);
	}

	{ // A space to trim
		string code(" ");
		WebpageContent wpc(code, true);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), true);
		BOOST_CHECK_EQUAL(wpc.empty(), true);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, string());
	}

	{ // A word
		string code("test");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, code);
	}

	{ // 2 lines
		string code("test\ntest");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, code);
	}

	{ // Numeric expression
		string code("test<@23@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test23");
	}

	{ // Addition expression
		string code("test<@23+12@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test35");
	}

	{ // Subtraction expression
		string code("test<@23-12@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test11");
	}

	{ // Multiplication expression
		string code("test<@23*12@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test276");
	}

	{ // Division expression
		string code("test<@24/12@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2");
	}

	{ // Modulo expression
		string code("test<@25%12@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}

	{ // Not expression
		string code("test<@!0@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}

	{ // Not expression
		string code("test<@!1@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test0");
	}

	{ // Not expression
		string code("test<@!111@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test0");
	}

	{ // Not expression
		string code("test<@!(0)@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}

	{ // Not expression
		string code("test<@~not(0)@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}

	{ // Sub expression
		string code("test<@-3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test-3");
	}

	{ // Combined expression
		string code("test<@-3+23@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test20");
	}

	{ // And expression
		string code("test<@1&&1@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}

	{ // And expression with spaces
		string code("test<@ 1 && 1 @>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}

	{ // And expression
		string code("test<@1&&0@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test0");
	}

	{ // Or expression
		string code("test<@1||0@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test1");
	}


	{ // Bitand expression
		string code("test<@3&2@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2");
	}

	{ // Bitor expression
		string code("test<@3|2@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test3");
	}

	{ // Ternary operator
		string code("test<@1?2:3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2");
	}

	{ // Ternary operator
		string code("test<@0?2:3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test3");
	}

	{ // Variable set
		string code("test<@variable=2@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 1);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 2);
	}

	{ // Variable get
		string code("test<@variable=2@><@variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 1);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 2);
	}

	{ // Variable + operation get
		string code("test<@variable=2@><@variable+2@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test4");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 1);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 2);
	}

	{ // 2 variable with numbers + operation get
		string code("test<@variable1=2@><@variable2=3@><@variable1+variable2@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test5");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<int>("variable1"), 2);
		BOOST_CHECK_EQUAL(variables.get<int>("variable2"), 3);
	}

	{ // Fake operation in variable set
		string code("test<@variable=2+5@><@variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2+5");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "2+5");
	}

	{ // Real operation in variable set
		string code("test<@variable=<@2+5@>@><@variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test7");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "7");
	}

	{ // Variable in variable
		string code("test<@variable=<@variable1@>@><@variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "2");
	}

	{ // Variable and operation and concatenation
		string code("test<@variable=<@variable1+8@>-@><@variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test10-");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "10-");
	}

	{ // String in expression
		string code("test<@\"test\"+3+\"test\"@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testtest3test");
	}

	{ // String in expression with escaped character
		string code("test<@\"test\\\"\"+3+\"\\\"test\"@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testtest\"3\"test");
	}

	{ // Single quoted string in expression
		string code("test<@'test'+3+'test'@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testtest3test");
	}

	{ // Double quotes in single quoted expression
		string code("test<@'test\"'+3+'\"test'@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testtest\"3\"test");
	}

	{ // Parentheses
		string code("test<@2+(4*6)@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test26");
	}

	{ // Multiple parentheses
		string code("test<@2+((4*6)/(8-5))@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test10");
	}

	{ // Encapsulated expression
		string code("test<@2+<@(4*6)/(8-5)@>@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test10");
	}

	{ // Foreach
		shared_ptr<ParametersMap> pm1(new ParametersMap);
		pm1->insert("test", string("OK1"));
		additionalParametersMap.insert("spm", pm1);
		shared_ptr<ParametersMap> pm2(new ParametersMap);
		pm2->insert("test", string("OK2"));
		additionalParametersMap.insert("spm", pm2);
		string code("test<{spm&template=<@test@>}>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testOK1OK2");
	}

	{ // Foreach in expression
		string code("test<@\"test\"+<{spm&template=<@test@>}>@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testtestOK1OK2");
	}

	{ // Foreach in variable
		string code("test<@variable=<{spm&template=<@test@>}>@><@variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "testOK1OK2");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "OK1OK2");
	}

	{ // Variable printer
		string code("<@@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "<table class=\"table table-striped table-condensed sortable\"><thead><tr><th>name</th><th>value</th></tr></thead><tbody><tr class=\"r1\"><td>variable</td><td>OK1OK2</td></tr><tr class=\"r2\"><td>variable1</td><td>2</td></tr><tr class=\"r1\"><td>variable2</td><td>3</td></tr><tr class=\"r2\"><td>SERVICE</td><td>page</td></tr><tr class=\"r1\"><td>host_name</td><td></td></tr><tr class=\"r2\"><td>client_url</td><td></td></tr><tr class=\"r1\"><td>site</td><td>0</td></tr><tr class=\"r2\"><td>spm</td><td>(submap)</td></tr></tbody></table>");
	}

	{ // Empty expression
		string code("<@ @>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), true);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "");
	}

	{ // Modulo operator
		string code("<@4%3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "1");
	}

	{ // & operator
		string code("<@6&3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "2");
	}

	{ // & operator with long long
		string code("<@25769803776&12884901888@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "8589934592");
	}

	{ // & operator
		string code("<@variable=6@><@variable&3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "2");
	}

	{ // Modulo operator
		string code("<@3%4@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "3");
	}

	{ // Modulo operator
		string code("<@5%3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "2");
	}

	{ // Equal test
		string code("<@4==3@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "0");
	}

	{ // Equal test
		string code("<@variable=OK1OK2@><@variable==\"OK1OK2\"@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "1");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "OK1OK2");
	}

	{ // Equal test
		string code("<@variable!=\"OK1OK2\"@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "0");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "OK1OK2");
	}

	{ // Equal test
		string code("<@variable!=\"K1OK2\"@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "1");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 3);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "OK1OK2");
	}

	{ // Combined ternary operators
		string code("<@1?<@0?2:3@>:4@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "3");
	}

	{ // Variable with _
		string code("test<@_variable=4@><@_variable@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test4");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<string>("variable"), "OK1OK2");
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 4);
	}

	{ // Variable with spaces before and after the name
		string code("test<@    variable  =6@><@  variable   @>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test6");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 6);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 4);
	}

	{ // Service call
		string code("test<?if&cond=1&then=2&else=3?>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test2");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 6);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 4);
	}

	{ // Service call
		string code("test<?if&cond=0&then=2&else=3?>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test3");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 6);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 4);
	}

	{ // Delayed evaluation parameters
		string code("test<?if&cond=1&then=<@variable=8@>&else=<@_variable=12@>?>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 4);
	}

	{ // Delayed evaluation parameters
		string code("test<?if&cond=0&then=<@variable=22@>&else=<@_variable=37@>?>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Access to additional parameter map
		string code("test<@param@>");
		additionalParametersMap.insert("param", 8);
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test8");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Access to sub map
		string code("test<{item&template=<@value@><{item&template=<@value@>}>}>");
		shared_ptr<ParametersMap> subMap1(new ParametersMap);
		subMap1->insert("id", 24);
		subMap1->insert("value", 21);
		additionalParametersMap.insert("item", subMap1);
		shared_ptr<ParametersMap> subMap2(new ParametersMap);
		subMap2->insert("id", 26);
		subMap2->insert("value", 32);
		subMap2->insert("other", 56);
		shared_ptr<ParametersMap> subMap3(new ParametersMap);
		subMap3->insert("id", 21);
		subMap3->insert("value", 33);
		subMap3->insert("other", 57);
		subMap2->insert("item", subMap3);
		additionalParametersMap.insert("item", subMap2);
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test213233");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Unique access to sub map
		string code("test<@item[26]@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test32");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Unique access to sub map
		string code("test<@item[26].other@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test56");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Unique access to sub map
		string code("test<@item[26].item[21]@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test33");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Unique access to sub map
		string code("test<@item[26].item[21].other@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "test57");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
	}

	{ // Create submap in CMS
		string code("<@submap[\"idx\"]=test1@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
		BOOST_CHECK_EQUAL(variables.hasSubMaps("submap"), true);
		BOOST_CHECK_EQUAL((*variables.getSubMaps("submap").begin())->getDefault<string>("id"), "idx");
		BOOST_CHECK_EQUAL((*variables.getSubMaps("submap").begin())->getDefault<string>("value"), "test1");
	}

	{ // Create submap in CMS
		string code("<@submap[\"idx\"].variable=test2@>");
		WebpageContent wpc(code);
		BOOST_CHECK_EQUAL(wpc.getCode(), code);
		BOOST_CHECK_EQUAL(wpc.getIgnoreWhiteChars(), false);
		BOOST_CHECK_EQUAL(wpc.empty(), false);
		string eval(wpc.eval(request, additionalParametersMap, page, variables));
		BOOST_CHECK_EQUAL(eval, "");
		BOOST_CHECK_EQUAL(variables.getMap().size(), 4);
		BOOST_CHECK_EQUAL(variables.get<int>("variable"), 8);
		BOOST_CHECK_EQUAL(variables.get<int>("_variable"), 37);
		BOOST_CHECK_EQUAL(variables.hasSubMaps("submap"), true);
		BOOST_CHECK_EQUAL((*variables.getSubMaps("submap").begin())->getDefault<string>("id"), "idx");
		BOOST_CHECK_EQUAL((*variables.getSubMaps("submap").begin())->getDefault<string>("value"), "test1");
		BOOST_CHECK_EQUAL((*variables.getSubMaps("submap").begin())->getDefault<string>("variable"), "test2");
	}
}

