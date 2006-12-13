#ifndef SYNTHESE_ENV_CALENDARTEST_H
#define SYNTHESE_ENV_CALENDARTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace env
  {


class CalendarTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(CalendarTest);
  CPPUNIT_TEST(testCalendarSimpleMarking);
  CPPUNIT_TEST(testCalendarLogicalAnd);
  CPPUNIT_TEST(testCalendarLogicalOr);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

  void testCalendarSimpleMarking ();
  void testCalendarLogicalAnd ();
  void testCalendarLogicalOr ();

protected:


};




}
}


#endif 

