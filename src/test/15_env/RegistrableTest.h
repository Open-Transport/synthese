#ifndef SYNTHESE_ENV_REGISTRABLETEST_H
#define SYNTHESE_ENV_REGISTRABLETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace env
  {


class RegistrableTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(RegistrableTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testRegistryOperations);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testConstruction ();
  void testRegistryOperations ();

};




}
}


#endif 
