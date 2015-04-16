#ifndef SYNTHESE_CARTO_MAPTEST_H
#define SYNTHESE_CARTO_MAPTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace carto
{



class MapTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(MapTest);
  CPPUNIT_TEST(testMap1);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

  void testMap1 ();

protected:


};




}
}


#endif

