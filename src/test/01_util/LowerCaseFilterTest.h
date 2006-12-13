#ifndef SYNTHESE_UTIL_LOWERCASEFILTERTEST_H
#define SYNTHESE_UTIL_LOWERCASEFILTERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class LowerCaseFilterTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(LowerCaseFilterTest);
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

