#ifndef ROADTEST_H_
#define ROADTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synmap
{

class RoadTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(RoadTest);
  CPPUNIT_TEST(testMostPlausibleAddressNumber1);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testMostPlausibleAddressNumber1();



};




}



#endif /*ROAD_H_*/
