#ifndef SYNTHESE_UTIL_LOGTEST_H
#define SYNTHESE_UTIL_LOGTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class LogTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(LogTest);
  CPPUNIT_TEST(testBasicLogging);
  CPPUNIT_TEST(testConcurrentLogging);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testBasicLogging ();
  void testConcurrentLogging ();

};




}
}


#endif 
