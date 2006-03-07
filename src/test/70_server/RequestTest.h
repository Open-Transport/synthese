#ifndef SYNTHESE_SERVER_REQUESTTEST_H
#define SYNTHESE_SERVER_REQUESTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace server
  {


class RequestTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(RequestTest);
  CPPUNIT_TEST(testParametersParsing);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  void testParametersParsing();

};




}
}


#endif 
