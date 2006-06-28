#include "CityTableSyncTest.h"

#include "01_util/Conversion.h"

#include "02_db/SQLite.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/Environment.h"

#include "17_env_ls_sql/module.h"
#include "17_env_ls_sql/CityTableSync.h"
#include "17_env_ls_sql/EnvironmentTableSync.h"

#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <map>


using synthese::db::SQLite;
using synthese::db::SQLiteSync;
using synthese::db::SQLiteResult;
using synthese::db::SQLiteThreadExec;

using synthese::env::Environment;
using synthese::util::Conversion;


namespace synthese
{
namespace envlssql
{


  void 
  CityTableSyncTest::setUp () 
  {
  }



  void 
  CityTableSyncTest::tearDown() 
  {
  } 





  void
  CityTableSyncTest::testSingleEnvironmentSync1 ()
  {
      {
	  boost::filesystem::remove ("test_db.s3db");
	  Environment::Registry environments;
	  
	  SQLiteThreadExec* sqliteExec = new SQLiteThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);
	  sqliteExec->registerUpdateHook (syncHook);
	  
	  EnvironmentTableSync* envSync = new EnvironmentTableSync (environments);
	  CityTableSync* citySync = new CityTableSync (environments);

	  syncHook->addTableSynchronizer (envSync);
	  syncHook->addTableSynchronizer (citySync);

	  sqliteExec->initialize ();
      
	  // Everything is ready for the tests
	  CPPUNIT_ASSERT_EQUAL (0, (int) environments.size ());
      
	  // Add a new environment row
	  sqliteExec->execUpdate (
	      "INSERT INTO " + ENVIRONMENTS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + ENVIRONMENTS_TABLE_COL_LINKTABLE + ") " + 
	      "VALUES (0, 'tbl_environment_0')"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) environments.size ());
      
      
	  // Add a new city before linking it to environment
	  Environment* env = environments.get (0);
	  sqliteExec->execUpdate (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (0, 'city_0')"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (0, (int) env->getCities ().size ());

	  sqliteExec->execUpdate (
	      "INSERT INTO tbl_environment_0 (" + 
	      TABLE_COL_ID + ", " + ENVLINKS_TABLE_COL_LINKCLASS + ", " + ENVLINKS_TABLE_COL_LINKID + 
	      ") VALUES (0, " + Conversion::ToString (CITY_CLASS) + ", 0)"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT (env->getCities ().contains (0));
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (0)->getName ());
      
      
	  // Add two other city links and create the objects afterward
	  std::string sql;
	  sql.append ("INSERT INTO tbl_environment_0 (" + TABLE_COL_ID + ", " + 
		      ENVLINKS_TABLE_COL_LINKCLASS + ", " + ENVLINKS_TABLE_COL_LINKID + 
		      ") VALUES (1, " + Conversion::ToString (CITY_CLASS) + ", 1) ;");
	  sql.append ("INSERT INTO tbl_environment_0 (" + TABLE_COL_ID + ", " + 
		      ENVLINKS_TABLE_COL_LINKCLASS + ", " + ENVLINKS_TABLE_COL_LINKID + 
		      ") VALUES (2, " + Conversion::ToString (CITY_CLASS) + ", 2) ;");
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) env->getCities ().size ());

	  sql = "";
	  sql.append ("INSERT INTO " + CITIES_TABLE_NAME + " (" + TABLE_COL_ID + ", " + 
		      CITIES_TABLE_COL_NAME + ") " + "VALUES (2, 'city_2'); ");
	  sql.append ("INSERT INTO " + CITIES_TABLE_NAME + " (" + TABLE_COL_ID + ", " + 
		      CITIES_TABLE_COL_NAME + ") " + "VALUES (1, 'city_1'); ");
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (3, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT (env->getCities ().contains (0));
	  CPPUNIT_ASSERT (env->getCities ().contains (1));
	  CPPUNIT_ASSERT (env->getCities ().contains (2));
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1"), env->getCities ().get (1)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_2"), env->getCities ().get (2)->getName ());
      
	  // Update name of one city
	  sql = "";
	  sql.append ("UPDATE " + CITIES_TABLE_NAME + " SET " + 
		      CITIES_TABLE_COL_NAME + "='city_1_' WHERE " + TABLE_COL_ID + "=1");
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (3, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (1)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_2"), env->getCities ().get (2)->getName ());
      
	  // Delete 2 cities on 3
	  sql = "";
	  sql.append ("DELETE FROM " + CITIES_TABLE_NAME + 
		      " WHERE " + TABLE_COL_ID + "=0 OR " + TABLE_COL_ID + "=2");
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (1)->getName ());


	  // Recreate city with id 0. Added immediately cos the link was not
	  // removed.
	  sqliteExec->execUpdate (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (0, 'city_0')"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (1)->getName ());

	  // Break all links
	  sql = "";
	  sql.append ("DELETE FROM tbl_environment_0 WHERE 1"); 
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (0, (int) env->getCities ().size ());

	  // Recreate links
	  sql = "";
	  sql.append ("INSERT INTO tbl_environment_0 (" + TABLE_COL_ID + ", " + 
		      ENVLINKS_TABLE_COL_LINKCLASS + ", " + ENVLINKS_TABLE_COL_LINKID + 
		      ") VALUES (0, " + Conversion::ToString (CITY_CLASS) + ", 0) ;");
	  sql.append ("INSERT INTO tbl_environment_0 (" + TABLE_COL_ID + ", " + 
		      ENVLINKS_TABLE_COL_LINKCLASS + ", " + ENVLINKS_TABLE_COL_LINKID + 
		      ") VALUES (1, " + Conversion::ToString (CITY_CLASS) + ", 1) ;");
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (1)->getName ());

	  // Clean everything
	  sqliteExec->finalize ();

	  delete sqliteExec;
	  delete syncHook; 
	  delete envSync;
	  delete citySync;
      }
      {
	  // Recreate exactly the same context except that some 
	  // valeus pre exist in db!
	  Environment::Registry environments;
	  
	  SQLiteThreadExec* sqliteExec = new SQLiteThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);
	  sqliteExec->registerUpdateHook (syncHook);
	  
	  EnvironmentTableSync* envSync = new EnvironmentTableSync (environments);
	  CityTableSync* citySync = new CityTableSync (environments);
	  
	  syncHook->addTableSynchronizer (envSync);
	  syncHook->addTableSynchronizer (citySync);
	  
	  sqliteExec->initialize ();

	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) environments.size ());
	  Environment* env = environments.get (0);

	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (1)->getName ());

	  sqliteExec->finalize ();

	  delete sqliteExec;
	  delete syncHook; 
	  delete envSync;
	  delete citySync;
      }


  }





}
}

