#ifndef SYNTHESE_ENVLSSQL_CITYTABLESYNCTEST_H
#define SYNTHESE_ENVLSSQL_CITYTABLESYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


#include <string>

namespace synthese
{
namespace envlssql
{


class CityTableSyncTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(CityTableSyncTest);
  CPPUNIT_TEST(testSingleEnvironmentSync1);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  /** 
   */
  void testSingleEnvironmentSync1 ();

 private:

//  void createTestDb (const std::string& dbFile, bool prefilled);

};




}
}


#endif 

