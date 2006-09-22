#ifndef SYNTHESE_ENVLSXML_ENVIRONMENTLSTEST_H
#define SYNTHESE_ENVLSXML_ENVIRONMENTLSTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


namespace synthese
{
namespace envlsxml
  {


class EnvironmentLSTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(EnvironmentLSTest);
  CPPUNIT_TEST(testLoadingEnvironment0);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

  void testLoadingEnvironment0 ();


protected:


};




}
}


#endif 
