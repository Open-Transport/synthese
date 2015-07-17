
/** Registrable Test implementation.
	@file RegistrableTest.cpp

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

#include "RegistryKeyException.h"
#include "Registrable.h"
#include "Registry.h"

#include <iostream>
#include <boost/shared_ptr.hpp>

#pragma GCC diagmostic ignored "-Werror=sign-compare"
#include <boost/test/auto_unit_test.hpp>
#pragma GCC diagnostic pop

using namespace synthese::util;

class RegistrableForTest:
	public Registrable
{
public:
	typedef synthese::util::Registry<RegistrableForTest> Registry;

	RegistrableForTest(
		RegistryKeyType key
	):	Registrable(key)
    {}
};


typedef boost::shared_ptr<RegistrableForTest> SPtr;


BOOST_AUTO_TEST_CASE (testRegistryOperations)
{
      {
	  RegistrableForTest reg (1);
	  BOOST_REQUIRE_EQUAL (1, reg.getKey ());
      }

      bool exceptionThrown (false);
      RegistrableForTest::Registry reg;

      BOOST_REQUIRE_EQUAL ((size_t) 0, reg.size ());

      RegistrableForTest* reg1 = new RegistrableForTest (1);
      reg.add (SPtr (reg1));

      BOOST_REQUIRE_EQUAL ((size_t)1, reg.size ());
      BOOST_REQUIRE (reg.contains (1));
      BOOST_REQUIRE_EQUAL (reg1, reg.get (1).get ());

      RegistrableForTest* reg2 = new RegistrableForTest (2);
      reg.add (SPtr (reg2));

      BOOST_REQUIRE_EQUAL ((size_t)2, reg.size ());
      BOOST_REQUIRE (reg.contains (1));
      BOOST_REQUIRE (reg.contains (2));
      BOOST_REQUIRE_EQUAL (reg1, reg.get (1).get ());
      BOOST_REQUIRE_EQUAL (reg2, reg.get (2).get ());

      exceptionThrown = false;
      try
      {
		  reg.get (3);
      }
      catch (ObjectNotFoundException<RegistrableForTest>&)
      {
		  exceptionThrown = true;
      }
      BOOST_REQUIRE (exceptionThrown);

      RegistrableForTest* reg3 = new RegistrableForTest (1);
      exceptionThrown = false;
      try
      {
		  reg.add (SPtr (reg3));
      }
      catch (RegistryKeyException<RegistrableForTest>&)
      {
		  exceptionThrown = true;
      }
      BOOST_REQUIRE (exceptionThrown);

      BOOST_REQUIRE_EQUAL ((size_t) 2, reg.size ());
      BOOST_REQUIRE (reg.contains (1));
      BOOST_REQUIRE (reg.contains (2));
      BOOST_REQUIRE_EQUAL (reg1, reg.get (1).get ());
      BOOST_REQUIRE_EQUAL (reg2, reg.get (2).get ());

      exceptionThrown = false;
      try
      {
		  reg.remove (3);
      }
      catch (ObjectNotFoundException<RegistrableForTest>&)
      {
		  exceptionThrown = true;
      }
      BOOST_REQUIRE (exceptionThrown);

      reg.remove (1);
      BOOST_REQUIRE_EQUAL ((size_t) 1, reg.size ());
      BOOST_REQUIRE (reg.contains (2));
      BOOST_REQUIRE_EQUAL (reg2, reg.get (2).get ());

      reg.clear ();
      BOOST_REQUIRE_EQUAL ((size_t) 0, reg.size ());
}
