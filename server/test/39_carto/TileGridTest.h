#ifndef SYNTHESE_CARTO_TILEGRIDTEST_H
#define SYNTHESE_CARTO_TILEGRIDTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace carto
{



class TileGridTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(TileGridTest);
  CPPUNIT_TEST(testPointMarking);
  CPPUNIT_TEST(testLineMarking);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);
  void testPointMarking ();
  void testLineMarking ();

protected:


};




}
}


#endif

