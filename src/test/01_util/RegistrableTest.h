#ifndef SYNTHESE_UTIL_REGISTRABLETEST_H
#define SYNTHESE_UTIL_REGISTRABLETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
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

