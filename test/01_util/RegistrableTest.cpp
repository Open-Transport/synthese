#include "01_util/RegistryKeyException.h"
#include "01_util/Registrable.h"

#include <iostream>
#include <boost/shared_ptr.hpp>


#include <boost/test/auto_unit_test.hpp>

using namespace synthese::util;


    class RegistrableForTest : public Registrable<int, RegistrableForTest>
    {
    public:

	RegistrableForTest (int key)
	    : Registrable<int,RegistrableForTest> (key)
	    {
	    }
	    
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
      catch (RegistrableForTest::RegistryKeyException& rke)
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
      catch (RegistrableForTest::RegistryKeyException& rke)
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
      catch (RegistrableForTest::RegistryKeyException& rke)
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


