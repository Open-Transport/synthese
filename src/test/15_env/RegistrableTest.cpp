#include "RegistrableTest.h"

#include "15_env/RegistryKeyException.h"

#include "15_env/Registrable.h"

#include <iostream>


namespace synthese
{
namespace env
{


  void 
  RegistrableTest::setUp () 
  {

  }


  void 
  RegistrableTest::tearDown() 
  {

  } 

  
    class RegistrableForTest : public Registrable<int, RegistrableForTest>
    {
    public:

	RegistrableForTest (int key) 
	    : Registrable<int,RegistrableForTest>::Registrable (key)
	{
	}
	    
    };


  void
  RegistrableTest::testConstruction ()
  {
      RegistrableForTest reg (1);

      CPPUNIT_ASSERT_EQUAL (1, reg.getKey ());
  }


  
  void
  RegistrableTest::testRegistryOperations ()
  {
      bool exceptionThrown (false);
      RegistrableForTest::Registry reg;

      CPPUNIT_ASSERT_EQUAL ((size_t) 0, reg.size ());

      RegistrableForTest* reg1 = new RegistrableForTest (1);
      reg.add (reg1);

      CPPUNIT_ASSERT_EQUAL ((size_t)1, reg.size ());
      CPPUNIT_ASSERT (reg.contains (1));
      CPPUNIT_ASSERT_EQUAL (reg1, reg.get (1));
      
      RegistrableForTest* reg2 = new RegistrableForTest (2);
      reg.add (reg2);

      CPPUNIT_ASSERT_EQUAL ((size_t)2, reg.size ());
      CPPUNIT_ASSERT (reg.contains (1));
      CPPUNIT_ASSERT (reg.contains (2));
      CPPUNIT_ASSERT_EQUAL (reg1, reg.get (1));
      CPPUNIT_ASSERT_EQUAL (reg2, reg.get (2));

      exceptionThrown = false;
      try
      {
	  reg.get (3);
      } 
      catch (RegistrableForTest::RegistryKeyException& rke)
      {
	  exceptionThrown = true;
      }
      CPPUNIT_ASSERT (exceptionThrown);

      RegistrableForTest* reg3 = new RegistrableForTest (1);
      exceptionThrown = false;
      try
      {
	  reg.add (reg3);
      } 
      catch (RegistrableForTest::RegistryKeyException& rke)
      {
	  exceptionThrown = true;
      }
      CPPUNIT_ASSERT (exceptionThrown);

      delete reg3;

      CPPUNIT_ASSERT_EQUAL ((size_t) 2, reg.size ());
      CPPUNIT_ASSERT (reg.contains (1));
      CPPUNIT_ASSERT (reg.contains (2));
      CPPUNIT_ASSERT_EQUAL (reg1, reg.get (1));
      CPPUNIT_ASSERT_EQUAL (reg2, reg.get (2));

      exceptionThrown = false;
      try
      {
	  reg.remove (3);
      } 
      catch (RegistrableForTest::RegistryKeyException& rke)
      {
	  exceptionThrown = true;
      }
      CPPUNIT_ASSERT (exceptionThrown);
      
      reg.remove (1);
      CPPUNIT_ASSERT_EQUAL ((size_t) 1, reg.size ());
      CPPUNIT_ASSERT (reg.contains (2));
      CPPUNIT_ASSERT_EQUAL (reg2, reg.get (2));
      
      reg.clear ();
      CPPUNIT_ASSERT_EQUAL ((size_t) 0, reg.size ());
  }





}
}

