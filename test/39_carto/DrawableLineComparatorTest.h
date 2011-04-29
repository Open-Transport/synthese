#ifndef SYNTHESE_CARTO_DRAWABLELINECOMPARATORTEST_H
#define SYNTHESE_CARTO_DRAWABLELINECOMPARATORTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace carto
  {



class DrawableLineComparatorTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(DrawableLineComparatorTest);
  CPPUNIT_TEST(testSameLines);
  CPPUNIT_TEST(testTiniestLines0);
  CPPUNIT_TEST(testTiniestLines1);
  CPPUNIT_TEST(testVariousComparisons);
  CPPUNIT_TEST(testLinesWithSharedBeginEnd);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);
  void testSameLines ();
  void testTiniestLines0 ();
  void testTiniestLines1 ();
  void testTiniestLines2 ();
  void testVariousComparisons ();
  void testLinesWithSharedBeginEnd ();

protected:


};




}
}


#endif

