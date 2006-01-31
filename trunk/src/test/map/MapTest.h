#ifndef MAPTEST_H_
#define MAPTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synmap
{

class MapTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(MapTest);
  CPPUNIT_TEST(testBidon);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testBidon();
};




}



#endif /*MAPTEST_H_*/
