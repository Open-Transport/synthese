
#include "01_util/Conversion.h"
#include "02_db/SQLiteValue.h"
#include "02_db/SQLite.h"


#include <iostream>
#include <sstream>

#include <boost/filesystem/operations.hpp>

#include <boost/test/auto_unit_test.hpp>


using namespace synthese::db;
using namespace synthese::util;




    
BOOST_AUTO_TEST_CASE (testSQLiteValueWrapping)
{
    boost::filesystem::path testDbPath ("test_db.s3db");
      
    // Removes the db file if exists.
    boost::filesystem::remove (testDbPath);
      
    // Creation of the test_db
    sqlite3* db = SQLite::OpenHandle (testDbPath);
      
    // Create test_table
    SQLite::ExecUpdate (db, 
			"CREATE TABLE IF NOT EXISTS test_table (col1 INT64 PRIMARY KEY, col2 INTEGER, col3 DOUBLE, col4 TEXT, col5 BLOB)"
	);

    // Insert some values
    // Note : SQLite browser does not show blob properly (truncated to null char)
    std::string sampleText ("this is a sample text");
    std::string sampleBlob ("0102030405060708090A0B0C000D0E0F101112131415161718191A1B");
    std::stringstream query;
    query << "INSERT INTO test_table VALUES (123456789123456789, 123456, 12345.6789, '" << sampleText
	  << "', x'" << sampleBlob << "');";

    SQLite::ExecUpdate (db, query.str ()); 

    
    {
	SQLiteResultSPtr result (SQLite::ExecQuery (db, "SELECT col1,col2,col3,col4,hex(col5),col5 FROM test_table"));
	BOOST_CHECK (result->next ());
	
	SQLiteValueSPtr col1 (result->getValue (0));
	BOOST_CHECK_EQUAL ("123456789123456789", col1->getText ());
	BOOST_CHECK_EQUAL (((long long) 123456789123456789LL), col1->getLongLong ());
	
	SQLiteValueSPtr col2 (result->getValue (1));
	BOOST_CHECK_EQUAL (((int) 123456), col2->getInt ());

	SQLiteValueSPtr col3 (result->getValue (2));
	BOOST_CHECK_EQUAL (((double) 12345.6789), col3->getDouble ());

	SQLiteValueSPtr col4 (result->getValue (3));
	BOOST_CHECK_EQUAL (sampleText, col4->getText ());

	SQLiteValueSPtr col5hex (result->getValue (4));
	BOOST_CHECK_EQUAL (sampleBlob, col5hex->getText ());

	SQLiteValueSPtr col5 (result->getValue (5));
	BOOST_CHECK_EQUAL (28, col5->getBlob ().size ());

	

    }
    
    // SQLiteResult must me destroyed so that close handle can work ! (otherwise databse locked error)
    SQLite::CloseHandle (db);
}



BOOST_AUTO_TEST_CASE (testCreateAndGetSchema)
{
/*    boost::filesystem::remove ("test_db.s3db");
      
    SQLiteQueueThreadExec* sqlite = new SQLiteQueueThreadExec ("test_db.s3db");
    sqlite->initialize ();

    SQLiteTableFormat format;
    format.addTableColumn ("col1", "INTEGER", true);
    format.addTableColumn ("col2", "TEXT", true);
     
    std::string expected ("CREATE TABLE test_table (col1 INTEGER UNIQUE PRIMARY KEY, col2 TEXT)");
    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::CreateSQLSchema ("test_table", format));
      
    sqlite->execUpdate (expected);

    BOOST_CHECK_EQUAL (expected, SQLiteTableSync::GetSQLSchemaDb (sqlite, "test_table"));
      
    sqlite->finalize ();
    delete sqlite;
*/    
}
    

