
#include "01_util/Conversion.h"
#include "01_util/threads/Thread.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteTableSync.h"

#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <map>

#include <boost/test/auto_unit_test.hpp>


using namespace synthese::db;
using namespace synthese::util;


/*
    
BOOST_AUTO_TEST_CASE (testGetTableColumnsDb)
{
    boost::filesystem::remove ("test_db.s3db");
      
    SQLite* sqlite = new SQLite ("test_db.s3db");
    sqlite->initialize ();

    SQLiteTableSync::Formatformat;
    format.addTableColumn ("col1", "SQL_INTEGER", true);
    format.addTableColumn ("col2", "SQL_TEXT", true);
     
    std::string sql ("CREATE TABLE test_table (col1 SQL_INTEGER UNIQUE PRIMARY KEY, col2 SQL_TEXT)");
    sqlite->execUpdate (sql);

    std::vector<std::string> expected;
    expected.push_back ("col1");
    expected.push_back ("col2");
      
    std::vector<std::string> actual = SQLiteTableSync::GetTableColumnsDb (sqlite, "test_table");
    BOOST_CHECK_EQUAL (2, (int) actual.size ());
    BOOST_CHECK_EQUAL (std::string ("col1"), actual.at (0));
    BOOST_CHECK_EQUAL (std::string ("col2"), actual.at (1));
      
    sqlite->finalize ();
    delete sqlite;
      
}



BOOST_AUTO_TEST_CASE (testCreateAndGetSchema)
{
    boost::filesystem::remove ("test_db.s3db");
      
    SQLite* sqlite = new SQLite ("test_db.s3db");
    sqlite->initialize ();

    SQLiteTableSync::Formatformat;
    format.addTableColumn ("col1", "SQL_INTEGER", true);
    format.addTableColumn ("col2", "SQL_TEXT", true);
     
    std::string expected ("CREATE TABLE test_table (col1 SQL_INTEGER UNIQUE PRIMARY KEY, col2 SQL_TEXT)");
    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
    sqlite->execUpdate (expected);

    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::GetSQLSchemaDb (sqlite, "test_table"));
      
    sqlite->finalize ();
    delete sqlite;
      
}
    





BOOST_AUTO_TEST_CASE (testCreateAndGetTriggerNoInsert)
{
    boost::filesystem::remove ("test_db.s3db");
      
    SQLite* sqlite = new SQLite ("test_db.s3db");
    sqlite->initialize ();

    SQLiteTableSync::Formatformat;
    format.addTableColumn ("col1", "SQL_INTEGER", true);
    format.addTableColumn ("col2", "SQL_TEXT", true);

    sqlite->execUpdate (SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
    std::string expected ("CREATE TRIGGER test_table_no_insert BEFORE INSERT ON test_table BEGIN SELECT RAISE (ABORT, 'Insertion in test_table is forbidden.') WHERE 1; END");
      
    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::CreateTriggerNoInsert ("test_table", "1"));

    sqlite->execUpdate (expected);

    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::GetTriggerNoInsertDb (sqlite, "test_table"));
      
    sqlite->finalize ();
    delete sqlite;
      
}
    

BOOST_AUTO_TEST_CASE (testCreateAndGetTriggerNoRemove)
{
    boost::filesystem::remove ("test_db.s3db");
      
    SQLite* sqlite = new SQLite ("test_db.s3db");
    sqlite->initialize ();

    SQLiteTableSync::Formatformat;
    format.addTableColumn ("col1", "SQL_INTEGER", true);
    format.addTableColumn ("col2", "SQL_TEXT", true);

    sqlite->execUpdate (SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
    std::string expected ("CREATE TRIGGER test_table_no_remove BEFORE DELETE ON test_table BEGIN SELECT RAISE (ABORT, 'Deletion in test_table is forbidden.') WHERE 1; END");
      
    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::CreateTriggerNoRemove ("test_table", "1"));

    sqlite->execUpdate (expected);

    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::GetTriggerNoRemoveDb (sqlite, "test_table"));
      
    sqlite->finalize ();
    delete sqlite;
      
}
    

BOOST_AUTO_TEST_CASE (testCreateAndGetTriggerNoUpdate)
{
    boost::filesystem::remove ("test_db.s3db");
      
    SQLite* sqlite = new SQLite ("test_db.s3db");
    sqlite->initialize ();

    SQLiteTableSync::Formatformat;
    format.addTableColumn ("col1", "SQL_INTEGER", true);
    format.addTableColumn ("col2", "SQL_TEXT", false);

    sqlite->execUpdate (SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
    std::string expected ("CREATE TRIGGER test_table_no_update BEFORE UPDATE OF col2 ON test_table BEGIN SELECT RAISE (ABORT, 'Update of col2 in test_table is forbidden.') WHERE 1; END");
      
    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::CreateTriggerNoUpdate ("test_table", format, "1"));

    sqlite->execUpdate (expected);

    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::GetTriggerNoUpdateDb (sqlite, "test_table"));
      
    sqlite->finalize ();
    delete sqlite;
      
}
    


class SQLiteTableSyncForTest : public SQLiteTableSync
{
public:

    SQLiteTableSyncForTest (const std::string& tableName) 
	: SQLiteTableSync (tableName) {}

    void rowsAdded (SQLite* sqlite, 
		    SQLiteSync* sync,
		    const SQLiteResult& rows,
	            bool isFirstSync = false) {}
	
    void rowsUpdated (SQLite* sqlite, 
		      SQLiteSync* sync,
		      const SQLiteResult& rows) {}
	
    void rowsRemoved (SQLite* sqlite, 
		      SQLiteSync* sync,
		      const SQLiteResult& rows) {}
	
    void addTableColumnForTest (const std::string& name, 
				const std::string& type,
				bool updatable)
	{
	    addTableColumn (name, type, updatable);
	}

};



BOOST_AUTO_TEST_CASE (testAdaptTableOnColumnAddition)
{
    boost::filesystem::remove ("test_db.s3db");

    {
	SQLite* sqlite = new SQLite ("test_db.s3db");
	SQLiteSync* syncHook = new SQLiteSync ("col1");
	SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	tabsync->addTableColumnForTest ("col1", "SQL_INTEGER", true);
	tabsync->addTableColumnForTest ("col2", "SQL_TEXT", true);

	syncHook->addTableSynchronizer ("1", boost::shared_ptr<SQLiteTableSync> (tabsync));
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
	SQLite* sqlite = new SQLite ("test_db.s3db");
	SQLiteSync* syncHook = new SQLiteSync ("col1");
	SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	tabsync->addTableColumnForTest ("col1", "SQL_INTEGER", true);
	tabsync->addTableColumnForTest ("col2", "SQL_TEXT", true);
	tabsync->addTableColumnForTest ("col3", "SQL_TEXT", true);

	syncHook->addTableSynchronizer ("1", boost::shared_ptr<SQLiteTableSync> (tabsync));
	sqlite->registerUpdateHook (syncHook);
	sqlite->initialize ();
      
	tabsync->firstSync (sqlite, syncHook);

	// We must now check that data has been properly migrated to new 
	// table structure.
	SQLiteResult result = sqlite->execQuery ("SELECT * FROM test_table");

	BOOST_CHECK_EQUAL (3, result.getNbColumns ());

	BOOST_CHECK_EQUAL (std::string ("10"),   result.getColumn (0, "col1"));
	BOOST_CHECK_EQUAL (std::string ("toto"), result.getColumn (0, "col2"));
	BOOST_CHECK_EQUAL (std::string (""),     result.getColumn (0, "col3"));

	BOOST_CHECK_EQUAL (std::string ("20"),   result.getColumn (1, "col1"));
	BOOST_CHECK_EQUAL (std::string ("titi"), result.getColumn (1, "col2"));
	BOOST_CHECK_EQUAL (std::string (""),     result.getColumn (1, "col3"));

	BOOST_CHECK_EQUAL (std::string ("30"),   result.getColumn (2, "col1"));
	BOOST_CHECK_EQUAL (std::string ("tata"), result.getColumn (2, "col2"));
	BOOST_CHECK_EQUAL (std::string (""),     result.getColumn (2, "col3"));

	sqlite->finalize ();
	delete sqlite;
	delete syncHook; 


    }
            
      
}
    


BOOST_AUTO_TEST_CASE (testAdaptTableOnColumnDeletion)
{
    boost::filesystem::remove ("test_db.s3db");


    {
	SQLite* sqlite = new SQLite ("test_db.s3db");
	SQLiteSync* syncHook = new SQLiteSync ("col1");
	SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	tabsync->addTableColumnForTest ("col1", "SQL_INTEGER", true);
	tabsync->addTableColumnForTest ("col2", "SQL_TEXT", true);
	tabsync->addTableColumnForTest ("col3", "SQL_TEXT", true);

	syncHook->addTableSynchronizer ("1", boost::shared_ptr<SQLiteTableSync> (tabsync));
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
	SQLite* sqlite = new SQLite ("test_db.s3db");
	SQLiteSync* syncHook = new SQLiteSync ("col1");
	SQLiteTableSyncForTest* tabsync = new SQLiteTableSyncForTest ("test_table");
	tabsync->addTableColumnForTest ("col1", "SQL_INTEGER", true);
	tabsync->addTableColumnForTest ("col3", "SQL_TEXT", true);

	syncHook->addTableSynchronizer ("1", boost::shared_ptr<SQLiteTableSync> (tabsync));
	sqlite->registerUpdateHook (syncHook);
	sqlite->initialize ();
      
	tabsync->firstSync (sqlite, syncHook);

	// We must now check that data has been properly migrated to new 
	// table structure.
	SQLiteResult result = sqlite->execQuery ("SELECT * FROM test_table");

	BOOST_CHECK_EQUAL (2, result.getNbColumns ());

	BOOST_CHECK_EQUAL (std::string ("10"),   result.getColumn (0, "col1"));
	BOOST_CHECK_EQUAL (std::string ("lolo"), result.getColumn (0, "col3"));

	BOOST_CHECK_EQUAL (std::string ("20"),   result.getColumn (1, "col1"));
	BOOST_CHECK_EQUAL (std::string ("lili"), result.getColumn (1, "col3"));

	BOOST_CHECK_EQUAL (std::string ("30"),   result.getColumn (2, "col1"));
	BOOST_CHECK_EQUAL (std::string ("lala"), result.getColumn (2, "col3"));

	sqlite->finalize ();
	delete sqlite;
	delete syncHook; 


    }
            
      
}
    

*/

