#include "SQLiteThreadExecTest.h"

#include "01_util/Conversion.h"
#include "01_util/Thread.h"

#include "02_db/SQLite.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteUpdateHook.h"

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
  SQLiteThreadExecTest::setUp () 
  {
  }



  void 
  SQLiteThreadExecTest::tearDown() 
  {
  } 



    void 
    SQLiteThreadExecTest::createTestDb (const std::string& dbFile, bool prefilled)
    {
      boost::filesystem::path testDbPath (dbFile);
      
      // Removes the db file if exists.
      boost::filesystem::remove (testDbPath);
      
      // Creation of the test_db
      sqlite3* db = SQLite::OpenConnection (testDbPath);
      
      // Create test_table
      SQLite::ExecUpdate (db, 
	"CREATE TABLE IF NOT EXISTS test_table (id INTEGER, label VARCHAR(20), primary key (id))"
	  );

      if (prefilled)
      {
	  // Insert some values
	  SQLite::ExecUpdate (db, 
	"INSERT INTO test_table VALUES (0, 'label0');\
         INSERT INTO test_table VALUES (1, 'label1');\
         INSERT INTO test_table VALUES (2, 'label2');\
        ");
      }

      SQLite::CloseConnection (db);

	
    }


    class TestUpdateHook : public SQLiteUpdateHook
    {
    private:

	bool _registerCalled;
	std::map<sqlite_int64, std::string> _labels;

    public:
	
	TestUpdateHook () 
	    : _registerCalled (false)
	{
	}

	~TestUpdateHook () 
	{
	}


	bool wasRegisterCalled () {
	    return _registerCalled;
	}


	std::map<sqlite_int64, std::string>& getLabels ()
	{
	    return _labels;
	}

	void registerCallback (const SQLiteThreadExec* emitter)
	{
	    _registerCalled = true;
	}
	

	
	void eventCallback (const SQLiteThreadExec* emitter,
			    const SQLiteEvent& event)
	{
	    if ((event.opType == SQLITE_INSERT) 
		|| (event.opType == SQLITE_UPDATE))
	    {
		// Query for row values
		SQLiteResult result = 
		    emitter->execQuery ("SELECT * FROM test_table WHERE id=" + 
					Conversion::ToString (event.rowId));

		for (int i=0; i<result.getNbRows (); ++i)
		{
		    std::string label (result.getColumn (i, "label"));
		    _labels[event.rowId] = label;
		}

	    }
	    else if (event.opType == SQLITE_DELETE)
	    {
		_labels.erase (event.rowId);
	    }

	}


    };
    
    



  void
  SQLiteThreadExecTest::testSingleUpdateHook1 ()
  {
      // Create empty test table
      createTestDb ("test_db.s3db", false); 

      TestUpdateHook* hook = new TestUpdateHook ();
      SQLiteThreadExec* sqliteExec = new SQLiteThreadExec ("test_db.s3db");

      // Register hook before launching thread
      sqliteExec->registerUpdateHook (hook);
      
      Thread sqliteThread (sqliteExec);

      sqliteThread.start ();

      
      while (sqliteThread.getState () != Thread::READY) Thread::Sleep (5);

      CPPUNIT_ASSERT (hook->wasRegisterCalled ());

      // Insert a row in test db
      sqliteExec->execUpdate ("INSERT INTO test_table VALUES (0, 'label0')");

      Thread::Sleep (100);

      CPPUNIT_ASSERT_EQUAL (1, (int) hook->getLabels().size ());
      CPPUNIT_ASSERT_EQUAL (std::string ("label0"), hook->getLabels()[0]);

      // Insert other rows in test db
      sqliteExec->execUpdate ("INSERT INTO test_table VALUES (2, 'label2')");
      sqliteExec->execUpdate ("INSERT INTO test_table VALUES (3, 'label3')");

      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (3, (int) hook->getLabels().size ());
      CPPUNIT_ASSERT_EQUAL (std::string ("label0"), hook->getLabels()[0]);
      CPPUNIT_ASSERT_EQUAL (std::string ("label2"), hook->getLabels()[2]);
      CPPUNIT_ASSERT_EQUAL (std::string ("label3"), hook->getLabels()[3]);

      sqliteExec->execUpdate ("UPDATE test_table SET label='modifiedLabel2' WHERE id=2");

      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (3, (int) hook->getLabels().size ());
      CPPUNIT_ASSERT_EQUAL (std::string ("label0"), hook->getLabels()[0]);
      CPPUNIT_ASSERT_EQUAL (std::string ("modifiedLabel2"), hook->getLabels()[2]);
      CPPUNIT_ASSERT_EQUAL (std::string ("label3"), hook->getLabels()[3]);

      sqliteExec->execUpdate ("DELETE FROM test_table WHERE id=0");

      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (2, (int) hook->getLabels().size ());
      CPPUNIT_ASSERT_EQUAL (std::string ("modifiedLabel2"), hook->getLabels()[2]);
      CPPUNIT_ASSERT_EQUAL (std::string ("label3"), hook->getLabels()[3]);

      // Be aware of special implementation of SQLite DELETE FROM table!
      // It drops and re-creates the table and does not generate update event!
      // Use DELETE FROM table WHERE 1 instead.
      sqliteExec->execUpdate ("DELETE FROM test_table WHERE 1");

      Thread::Sleep (100);
      CPPUNIT_ASSERT_EQUAL (0, (int) hook->getLabels().size ());

      sqliteThread.stop ();
      while (sqliteThread.getState () != Thread::STOPPED) Thread::Sleep (5);

      // Temporization to wait for the thread to die
      Thread::Sleep (300);

      delete hook;
      boost::filesystem::remove ("test_db.s3db");


  }



}
}

