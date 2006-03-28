#ifndef SYNTHESE_CARTO_CITYTEST_H
#define SYNTHESE_CARTO_CITYTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace carto
  {



class DrawableLineComparatorTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(DrawableLineComparatorTest);
  CPPUNIT_TEST(testVariousComparisons);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);
  void testVariousComparisons ();

protected:


};




}
}


#endif 
