#ifndef ROADCHUNKTEST_H_
#define ROADCHUNKTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synmap
{

class RoadChunkTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(RoadChunkTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testAddressNumbers1);
  CPPUNIT_TEST(testAddressNumbers2);
  CPPUNIT_TEST(testAddressNumbers3);
  CPPUNIT_TEST(testAddressNumbers4);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testConstruction ();
  void testAddressNumbers1();
  void testAddressNumbers2();
  void testAddressNumbers3();
  void testAddressNumbers4();



};




}



#endif /*ROADCHUNKTEST_H_*/
