
/** Website unit test.
	@file WebsiteTest.cpp

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

#include "CMSModule.hpp"
#include "DynamicRequest.h"
#include "ParametersMap.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "Website.hpp"

#include <boost/test/auto_unit_test.hpp>

using namespace boost;
using namespace std;

using namespace synthese::util;
using namespace synthese::cms;
using namespace synthese::server;
using namespace synthese;

BOOST_AUTO_TEST_CASE (WebsiteTest)
{
	WebPageDisplayFunction::integrate();

	Website s2;
	s2.set<HostName>("www.toto.com");
	CMSModule::AddSite(s2);
	BOOST_CHECK_EQUAL(s2.get<HostName>(), "www.toto.com");
	BOOST_CHECK(s2.get<ClientURL>().empty());
	
	Website s3;
	s3.set<ClientURL>("/synthese");
	CMSModule::AddSite(s3);
	BOOST_CHECK(s3.get<HostName>().empty());
	BOOST_CHECK_EQUAL(s3.get<ClientURL>(), "/synthese");
	
	Website s4;
	s4.set<HostName>("www.toto.com");
	s4.set<ClientURL>("/synthese");
	CMSModule::AddSite(s4);
	BOOST_CHECK_EQUAL(s4.get<HostName>(), "www.toto.com");
	BOOST_CHECK_EQUAL(s4.get<ClientURL>(), "/synthese");

	Webpage p1s4;
	p1s4.setRoot(&s4);
	p1s4.set<SmartURLPath>("/test.html");
	s4.addPage(p1s4);

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", ""));
		BOOST_CHECK(!s);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", ""));
		BOOST_CHECK_EQUAL(s, &s2);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/toto"));
		BOOST_CHECK_EQUAL(s, &s2);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/toto/test.html"));
		BOOST_CHECK_EQUAL(s, &s2);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese/"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese/test.html"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/synthese"));
		BOOST_CHECK_EQUAL(s, &s4);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/synthese/test.html"));
		BOOST_CHECK_EQUAL(s, &s4);

		Webpage* p(s4.getPageBySmartURL("/test.html"));
		BOOST_CHECK_EQUAL(p, &p1s4);
	}

	Website s1;
	CMSModule::AddSite(s1);
	BOOST_CHECK(s1.get<HostName>().empty());
	BOOST_CHECK(s1.get<ClientURL>().empty());

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", ""));
		BOOST_CHECK_EQUAL(s, &s1);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/"));
		BOOST_CHECK_EQUAL(s, &s2);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/toto"));
		BOOST_CHECK_EQUAL(s, &s2);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/toto/test.html"));
		BOOST_CHECK_EQUAL(s, &s2);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese/"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese/test.html"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.toto.com", "/synthese"));
		BOOST_CHECK_EQUAL(s, &s4);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/synthese/test.html"));
		BOOST_CHECK_EQUAL(s, &s3);
	}

	{
		Website* s(CMSModule::GetSiteByURL("www.tutu.com", "/toto"));
		BOOST_CHECK_EQUAL(s, &s1);
	}
}

