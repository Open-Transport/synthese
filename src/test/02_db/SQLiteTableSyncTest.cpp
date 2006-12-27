#include "SQLiteTableSyncTest.h"

#include "01_util/Conversion.h"
#include "01_util/Thread.h"

#include "02_db/DBModule.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteTableSync.h"

#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <map>


using synthese::db::SQLite;
using synthese::db::SQLiteResult;

using synthese::util::Conversion;
using synthese::util::Thread;


namespace synthese
{
namespace db
{


  void 
  SQLiteTableSyncTest::setUp () 
  {
  }


    
    void 
    SQLiteTableSyncTest::tearDown() 
    {
    } 
    
    

    void 
    SQLiteTableSyncTest::testGetTableColumnsDb () 
    {
      boost::filesystem::remove ("test_db.s3db");
      
      SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
      sqlite->initialize ();

      SQLiteTableFormat format;
      format.addTableColumn ("col1", "INTEGER", true);
      format.addTableColumn ("col2", "TEXT", true);
     
      std::string sql ("CREATE TABLE test_table (col1 INTEGER UNIQUE PRIMARY KEY, col2 TEXT)");
      sqlite->execUpdate (sql);

      std::vector<std::string> expected;
      expected.push_back ("col1");
      expected.push_back ("col2");
      
      std::vector<std::string> actual = SQLiteTableSync::GetTableColumnsDb (sqlite, "test_table");
      CPPUNIT_ASSERT_EQUAL (2, (int) actual.size ());
      CPPUNIT_ASSERT_EQUAL (std::string ("col1"), actual.at (0));
      CPPUNIT_ASSERT_EQUAL (std::string ("col2"), actual.at (1));
      
      sqlite->finalize ();
      delete sqlite;
      
    }



  void 
  SQLiteTableSyncTest::testCreateAndGetSchema ()
  {
      boost::filesystem::remove ("test_db.s3db");
      
      SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
      sqlite->initialize ();

      SQLiteTableFormat format;
      format.addTableColumn ("col1", "INTEGER", true);
      format.addTableColumn ("col2", "TEXT", true);
     
      std::string expected ("CREATE TABLE test_table (col1 INTEGER UNIQUE PRIMARY KEY, col2 TEXT)");
      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
      sqlite->execUpdate (expected);

      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::GetSQLSchemaDb (sqlite, "test_table"));
      
      sqlite->finalize ();
      delete sqlite;
      
  }
    





  void 
  SQLiteTableSyncTest::testCreateAndGetTriggerNoInsert ()
  {
      boost::filesystem::remove ("test_db.s3db");
      
      SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
      sqlite->initialize ();

      SQLiteTableFormat format;
      format.addTableColumn ("col1", "INTEGER", true);
      format.addTableColumn ("col2", "TEXT", true);

      sqlite->execUpdate (SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
      std::string expected ("CREATE TRIGGER test_table_no_insert BEFORE INSERT ON test_table BEGIN SELECT RAISE (ABORT, 'Insertion in test_table is forbidden.') WHERE 1; END");
      
      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::CreateTriggerNoInsert ("test_table", "1"));

      sqlite->execUpdate (expected);

      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::GetTriggerNoInsertDb (sqlite, "test_table"));
      
      sqlite->finalize ();
      delete sqlite;
      
  }
    

  void 
  SQLiteTableSyncTest::testCreateAndGetTriggerNoRemove ()
  {
      boost::filesystem::remove ("test_db.s3db");
      
      SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
      sqlite->initialize ();

      SQLiteTableFormat format;
      format.addTableColumn ("col1", "INTEGER", true);
      format.addTableColumn ("col2", "TEXT", true);

      sqlite->execUpdate (SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
      std::string expected ("CREATE TRIGGER test_table_no_remove BEFORE DELETE ON test_table BEGIN SELECT RAISE (ABORT, 'Deletion in test_table is forbidden.') WHERE 1; END");
      
      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::CreateTriggerNoRemove ("test_table", "1"));

      sqlite->execUpdate (expected);

      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::GetTriggerNoRemoveDb (sqlite, "test_table"));
      
      sqlite->finalize ();
      delete sqlite;
      
  }
    

  void 
  SQLiteTableSyncTest::testCreateAndGetTriggerNoUpdate ()
  {
      boost::filesystem::remove ("test_db.s3db");
      
      SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
      sqlite->initialize ();

      SQLiteTableFormat format;
      format.addTableColumn ("col1", "INTEGER", true);
      format.addTableColumn ("col2", "TEXT", false);

      sqlite->execUpdate (SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
      std::string expected ("CREATE TRIGGER test_table_no_update BEFORE UPDATE OF col2 ON test_table BEGIN SELECT RAISE (ABORT, 'Update of col2 in test_table is forbidden.') WHERE 1; END");
      
      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::CreateTriggerNoUpdate ("test_table", format, "1"));

      sqlite->execUpdate (expected);

      CPPUNIT_ASSERT_EQUAL (expected, SQLiteTableSync::GetTriggerNoUpdateDb (sqlite, "test_table"));
      
      sqlite->finalize ();
      delete sqlite;
      
  }
    


    class SQLiteTableSyncForTest : public SQLiteTableSync
    {
    public:

	SQLiteTableSyncForTest (const std::string& tableName) 
	    : SQLiteTableSync (tableName) {}

	void rowsAdded (const SQLiteQueueThreadExec* sqlite, 
			SQLiteSync* sync,
			const SQLiteResult& rows) {}
	
	void rowsUpdated (const SQLiteQueueThreadExec* sqlite, 
			  SQLiteSync* sync,
			  const SQLiteResult& rows) {}
	
	void rowsRemoved (const SQLiteQueueThreadExec* sqlite, 
			  SQLiteSync* sync,
			  const SQLiteResult& rows) {}
	
        void addTableColumnForTest (const std::string& name, 
				    const std::string& type,
				    bool updatable)
	{
	    addTableColumn (name, type, updatable);
	}

    };



  void 
  SQLiteTableSyncTest::testAdaptTableOnColumnAddition ()
  {
      boost::filesystem::remove ("test_db.s3db");


      {
	  SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync ();
	  SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	  tabsync->addTableColumnForTest ("col1", "INTEGER", true);
	  tabsync->addTableColumnForTest ("col2", "TEXT", true);

	  syncHook->addTableSynchronizer ("1", tabsync);
	  sqlite->registerUpdateHook (syncHook);
	  sqlite->initialize ();

	  tabsync->firstSync (sqlite, syncHook);

	  // Now the table is created. Fill it with some values
	  sqlite->execUpdate ("INSERT INTO test_table (col1, col2) VALUES (10, 'toto')");
	  sqlite->execUpdate ("INSERT INTO test_table (col1, col2) VALUES (20, 'titi')");
	  sqlite->execUpdate ("INSERT INTO test_table (col1, col2) VALUES (30, 'tata')");

	  sqlite->finalize ();
	  delete sqlite;
	  delete syncHook; 

      }

      {
	  // Add another column 
	  SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync ();
	  SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	  tabsync->addTableColumnForTest ("col1", "INTEGER", true);
	  tabsync->addTableColumnForTest ("col2", "TEXT", true);
	  tabsync->addTableColumnForTest ("col3", "TEXT", true);

	  syncHook->addTableSynchronizer ("1", tabsync);
	  sqlite->registerUpdateHook (syncHook);
	  sqlite->initialize ();
      
	  tabsync->firstSync (sqlite, syncHook);

	  // We must now check that data has been properly migrated to new 
	  // table structure.
	  SQLiteResult result = sqlite->execQuery ("SELECT * FROM test_table");

	  CPPUNIT_ASSERT_EQUAL (3, result.getNbColumns ());

	  CPPUNIT_ASSERT_EQUAL (std::string ("10"),   result.getColumn (0, "col1"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("toto"), result.getColumn (0, "col2"));
	  CPPUNIT_ASSERT_EQUAL (std::string (""),     result.getColumn (0, "col3"));

	  CPPUNIT_ASSERT_EQUAL (std::string ("20"),   result.getColumn (1, "col1"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("titi"), result.getColumn (1, "col2"));
	  CPPUNIT_ASSERT_EQUAL (std::string (""),     result.getColumn (1, "col3"));

	  CPPUNIT_ASSERT_EQUAL (std::string ("30"),   result.getColumn (2, "col1"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("tata"), result.getColumn (2, "col2"));
	  CPPUNIT_ASSERT_EQUAL (std::string (""),     result.getColumn (2, "col3"));

	  sqlite->finalize ();
	  delete sqlite;
	  delete syncHook; 


      }
            
      
  }
    


  void 
  SQLiteTableSyncTest::testAdaptTableOnColumnDeletion ()
  {
      boost::filesystem::remove ("test_db.s3db");


      {
	  SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync ();
	  SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	  tabsync->addTableColumnForTest ("col1", "INTEGER", true);
	  tabsync->addTableColumnForTest ("col2", "TEXT", true);
	  tabsync->addTableColumnForTest ("col3", "TEXT", true);

	  syncHook->addTableSynchronizer ("1", tabsync);
	  sqlite->registerUpdateHook (syncHook);
	  sqlite->initialize ();

	  tabsync->firstSync (sqlite, syncHook);

	  // Now the table is created. Fill it with some values
	  sqlite->execUpdate ("INSERT INTO test_table (col1, col2, col3) VALUES (10, 'toto', 'lolo')");
	  sqlite->execUpdate ("INSERT INTO test_table (col1, col2, col3) VALUES (20, 'titi', 'lili')");
	  sqlite->execUpdate ("INSERT INTO test_table (col1, col2, col3) VALUES (30, 'tata', 'lala')");

	  sqlite->finalize ();
	  delete sqlite;
	  delete syncHook; 

      }

      {
	  // Add another column 
	  SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync ();
	  SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	  tabsync->addTableColumnForTest ("col1", "INTEGER", true);
	  tabsync->addTableColumnForTest ("col3", "TEXT", true);

	  syncHook->addTableSynchronizer ("1", tabsync);
	  sqlite->registerUpdateHook (syncHook);
	  sqlite->initialize ();
      
	  tabsync->firstSync (sqlite, syncHook);

	  // We must now check that data has been properly migrated to new 
	  // table structure.
	  SQLiteResult result = sqlite->execQuery ("SELECT * FROM test_table");

	  CPPUNIT_ASSERT_EQUAL (2, result.getNbColumns ());

	  CPPUNIT_ASSERT_EQUAL (std::string ("10"),   result.getColumn (0, "col1"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("lolo"), result.getColumn (0, "col3"));

	  CPPUNIT_ASSERT_EQUAL (std::string ("20"),   result.getColumn (1, "col1"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("lili"), result.getColumn (1, "col3"));

	  CPPUNIT_ASSERT_EQUAL (std::string ("30"),   result.getColumn (2, "col1"));
	  CPPUNIT_ASSERT_EQUAL (std::string ("lala"), result.getColumn (2, "col3"));

	  sqlite->finalize ();
	  delete sqlite;
	  delete syncHook; 


      }
            
      
  }
    




}
}


