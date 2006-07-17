#ifndef SYNTHESE_UTIL_UIDTEST_H
#define SYNTHESE_UTIL_UIDTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class UIdTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(UIdTest);
  CPPUNIT_TEST(testEncodingDecoding);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testEncodingDecoding ();

};




}
}


#endif 
