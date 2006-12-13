#ifndef SYNTHESE_TCP_TCPSERVERSOCKETTEST_H
#define SYNTHESE_TCP_TCPSERVERSOCKETTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace tcp
  {


class TcpServerSocketTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(TcpServerSocketTest);
  CPPUNIT_TEST(testSimpleConnection);
  CPPUNIT_TEST(testSimpleConnectionWhitespaceTransfer);
  CPPUNIT_TEST(testMultipleConnections);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testSimpleConnection ();
  void testSimpleConnectionWhitespaceTransfer ();
  void testMultipleConnections ();

};




}
}


#endif 

