#ifndef SYNTHESE_TIME_YEARTEST_H
#define SYNTHESE_TIME_YEARTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace time
  {


class YearTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(YearTest);
  CPPUNIT_TEST(testLeapYears);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testLeapYears();
};




}
}


#endif 
