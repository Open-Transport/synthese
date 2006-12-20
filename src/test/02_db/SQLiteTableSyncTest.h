#ifndef SYNTHESE_DB_SQLITETABLESYNCTEST_H
#define SYNTHESE_DB_SQLITETABLESYNCTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>


#include <string>

namespace synthese
{
namespace db
{


class SQLiteTableSyncTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE(SQLiteTableSyncTest);
  CPPUNIT_TEST(testGetTableColumnsDb);
  CPPUNIT_TEST(testCreateAndGetSchema);
  CPPUNIT_TEST(testCreateAndGetTriggerNoInsert);
  CPPUNIT_TEST(testCreateAndGetTriggerNoRemove);
  CPPUNIT_TEST(testCreateAndGetTriggerNoUpdate);
  CPPUNIT_TEST(testAdaptTableOnColumnAddition);
  CPPUNIT_TEST(testAdaptTableOnColumnDeletion);
  CPPUNIT_TEST_SUITE_END();

public:

  void setUp(void);
  void tearDown(void);

protected:
  
  void testGetTableColumnsDb ();
  void testCreateAndGetSchema ();
  void testCreateAndGetTriggerNoInsert ();
  void testCreateAndGetTriggerNoRemove ();
  void testCreateAndGetTriggerNoUpdate ();
  void testAdaptTableOnColumnAddition ();
  void testAdaptTableOnColumnDeletion ();

 private:


};




}
}


#endif 

