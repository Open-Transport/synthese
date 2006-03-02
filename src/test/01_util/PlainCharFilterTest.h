#ifndef SYNTHESE_UTIL_PLAINCHARFILTERTEST_H
#define SYNTHESE_UTIL_PLAINCHARFILTERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class PlainCharFilterTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(PlainCharFilterTest);
  CPPUNIT_TEST(testFiltering);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testFiltering ();

};




}
}


#endif 
