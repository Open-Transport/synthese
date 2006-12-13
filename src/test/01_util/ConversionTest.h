#ifndef SYNTHESE_UTIL_CONVERSIONTEST_H
#define SYNTHESE_UTIL_CONVERSIONTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class ConversionTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(ConversionTest);
  CPPUNIT_TEST(testLongLongConversion);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testLongLongConversion ();

};




}
}


#endif 

