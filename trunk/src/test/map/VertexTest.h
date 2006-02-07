#ifndef VERTEXTEST_H_
#define VERTEXTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synmap
{

class VertexTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(VertexTest);
  CPPUNIT_TEST(testFindingCloseNeighbors);
  CPPUNIT_TEST(testFindingCloseNeighborsWithLoops);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testFindingCloseNeighbors ();
  void testFindingCloseNeighborsWithLoops ();


};




}



#endif /*VERTEXTEST_H_*/
