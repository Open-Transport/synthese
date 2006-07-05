#ifndef SYNTHESE_ENV_ENVIRONMENTTEST_H
#define SYNTHESE_ENV_ENVIRONMENTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace env
  {


class EnvironmentTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(EnvironmentTest);
  CPPUNIT_TEST(testEnvironmentConstruction0);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

  void testEnvironmentConstruction0 ();

protected:


};




}
}


#endif 
