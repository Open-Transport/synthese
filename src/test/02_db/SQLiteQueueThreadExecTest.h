#ifndef SYNTHESE_DB_SQLITETHREADEXECTEST_H
#define SYNTHESE_DB_SQLITETHREADEXECTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


#include <string>

namespace synthese
{
namespace db
{


class SQLiteQueueThreadExecTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(SQLiteQueueThreadExecTest);
  CPPUNIT_TEST(testSingleUpdateHook1);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:

  /** An empty database is created with the following schema :
      test_table (id INTEGER, label VARCHAR(20), primary key (id))
  
      Insertion of rows ; Modification of rows ; Deletion of rows.
      Verification that test hook is activated and reflects db events.
   */
  void testSingleUpdateHook1 ();

 private:

  void createTestDb (const std::string& dbFile, bool prefilled);

};




}
}


#endif 

