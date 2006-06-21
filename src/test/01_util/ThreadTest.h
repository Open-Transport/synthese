#ifndef SYNTHESE_UTIL_THREADTEST_H
#define SYNTHESE_UTIL_THREADTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace util
  {


class ThreadTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(ThreadTest);
  CPPUNIT_TEST(testStartPauseResumeStop);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testStartPauseResumeStop ();

};




}
}


#endif 
