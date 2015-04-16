
/** ConnectionInfoTest class implementation.
	@file ConnectionInfoTest.cpp
	@author Sylvain Pasche
	@date 2011

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

#include "10_db/DB.hpp"
#include "10_db/101_sqlite/SQLiteDB.h"

#include <boost/test/unit_test.hpp>

typedef synthese::db::DB::ConnectionInfo ConnectionInfo;
typedef synthese::db::DB::InvalidConnectionStringException InvalidConnectionStringException;

struct RegisterFactories {
	RegisterFactories()
	{
		synthese::db::SQLiteDB::integrate();
	}
};

BOOST_GLOBAL_FIXTURE(RegisterFactories);

BOOST_AUTO_TEST_CASE(InvalidBackend)
{
	BOOST_CHECK_THROW(ConnectionInfo ci(""), InvalidConnectionStringException);
	BOOST_CHECK_THROW(ConnectionInfo ci("sqlite"), InvalidConnectionStringException);
	BOOST_CHECK_THROW(ConnectionInfo ci("sqlite:/"), InvalidConnectionStringException);
	BOOST_CHECK_THROW(ConnectionInfo ci("://"), InvalidConnectionStringException);
}

BOOST_AUTO_TEST_CASE(InvalidParams)
{
	BOOST_CHECK_THROW(ConnectionInfo ci("sqlite://unknown_param=foo"), InvalidConnectionStringException);
	BOOST_CHECK_THROW(ConnectionInfo ci("sqlite://host"), InvalidConnectionStringException);
	BOOST_CHECK_THROW(ConnectionInfo ci("sqlite://host,db"), InvalidConnectionStringException);
}

BOOST_AUTO_TEST_CASE(ValidNoParams)
{
	ConnectionInfo ci("sqlite://");
	BOOST_CHECK_EQUAL("sqlite", ci.backend);
	BOOST_CHECK_EQUAL("", ci.path);
	BOOST_CHECK_EQUAL("", ci.host);
	BOOST_CHECK_EQUAL("", ci.user);
	BOOST_CHECK_EQUAL("", ci.passwd);
	BOOST_CHECK_EQUAL("", ci.db);
	BOOST_CHECK_EQUAL("", ci.triggerHost);
	BOOST_CHECK_EQUAL(0, ci.port);
	BOOST_CHECK_EQUAL(false, ci.debug);
	BOOST_CHECK_EQUAL(true, ci.triggerCheck);
}

BOOST_AUTO_TEST_CASE(ValidParams0)
{
	ConnectionInfo ci("sqlite://path=/tmp/test.db,host=localhost,triggerHost=example.com,debug=0");
	BOOST_CHECK_EQUAL("sqlite", ci.backend);
	BOOST_CHECK_EQUAL("/tmp/test.db", ci.path);
	BOOST_CHECK_EQUAL("localhost", ci.host);
	BOOST_CHECK_EQUAL("", ci.user);
	BOOST_CHECK_EQUAL("", ci.passwd);
	BOOST_CHECK_EQUAL("", ci.db);
	BOOST_CHECK_EQUAL("example.com", ci.triggerHost);
	BOOST_CHECK_EQUAL(0, ci.port);
	BOOST_CHECK_EQUAL(false, ci.debug);
	BOOST_CHECK_EQUAL(true, ci.triggerCheck);
}

BOOST_AUTO_TEST_CASE(ValidParams1)
{
	ConnectionInfo ci("sqlite://path=test.db,host=localhost,user=joe,passwd=secret,db=myDb,triggerHost=foo.org,port=9999,debug=1,triggerCheck=0");
	BOOST_CHECK_EQUAL("sqlite", ci.backend);
	BOOST_CHECK_EQUAL("test.db", ci.path);
	BOOST_CHECK_EQUAL("localhost", ci.host);
	BOOST_CHECK_EQUAL("joe", ci.user);
	BOOST_CHECK_EQUAL("secret", ci.passwd);
	BOOST_CHECK_EQUAL("myDb", ci.db);
	BOOST_CHECK_EQUAL("foo.org", ci.triggerHost);
	BOOST_CHECK_EQUAL(9999, ci.port);
	BOOST_CHECK_EQUAL(true, ci.debug);
	BOOST_CHECK_EQUAL(false, ci.triggerCheck);
}
