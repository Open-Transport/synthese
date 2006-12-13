#ifndef SYNTHESE_UTIL_HTMLFILTERTEST_H
#define SYNTHESE_UTIL_HTMLFILTERTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class HtmlFilterTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(HtmlFilterTest);
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

