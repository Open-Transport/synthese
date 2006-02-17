#ifndef DBLCOMPARATORTEST_H_
#define DBLCOMPARATORTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synmap
{

class DBLComparatorTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(DBLComparatorTest);
  CPPUNIT_TEST(testVariousComparisons);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testVariousComparisons ();

};




}



#endif /*DBLCOMPARATORTEST_H_*/
