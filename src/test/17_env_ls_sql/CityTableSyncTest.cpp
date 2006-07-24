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
#include "17_env_ls_sql/EnvironmentLinkTableSync.h"

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

	  // Create the env link synchronizer after having added the component synchronizers
	  EnvironmentLinkTableSync* envLinkSync = new EnvironmentLinkTableSync (syncHook,
										environments);

	  syncHook->addTableSynchronizer (envLinkSync);

	  sqliteExec->initialize ();
      
	  // Everything is ready for the tests
	  CPPUNIT_ASSERT_EQUAL (0, (int) environments.size ());
      
	  // Add a new environment row
	  
	  uid environmentId = envSync->encodeUId (0, 0, 0);

	  sqliteExec->execUpdate (
	      "INSERT INTO " + ENVIRONMENTS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ") " + 
	      "VALUES (" + Conversion::ToString (environmentId) + ")"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) environments.size ());
      
      
	  // Add a new city before linking it to environment
	  Environment* env = environments.get (0);
	  uid cityId0 = citySync->encodeUId (0, 0, 0);

	  sqliteExec->execUpdate (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (" + Conversion::ToString (cityId0) + ", 'city_0')"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (0, (int) env->getCities ().size ());
	  
	  uid environmentLinkId = envLinkSync->encodeUId (0, 0, 0);

	  sqliteExec->execUpdate (
	      "INSERT INTO " + ENVIRONMENT_LINKS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + ", " + 
	      ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID + 
	      ") VALUES (" + Conversion::ToString (environmentLinkId) + ", " + 
	      Conversion::ToString (environmentId) + ", " +
	      Conversion::ToString (cityId0) + ")"
	      );

	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT (env->getCities ().contains (cityId0));
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
      
      
	  // Add two other city links and create the objects afterward
	  std::string sql;

	  uid cityId1 = citySync->encodeUId (0, 0, 1);
	  uid cityId2 = citySync->encodeUId (0, 0, 2);

	  sql.append (
	      "INSERT INTO " + ENVIRONMENT_LINKS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + ", " + 
	      ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID + 
	      ") VALUES (" + Conversion::ToString (++environmentLinkId) + ", " + 
	      Conversion::ToString (environmentId) + ", " +
	      Conversion::ToString (cityId1) + ") ;"
	      );
	  sql.append (
	      "INSERT INTO " + ENVIRONMENT_LINKS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + ", " + 
	      ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID + 
	      ") VALUES (" + Conversion::ToString (++environmentLinkId) + ", " + 
	      Conversion::ToString (environmentId) + ", " +
	      Conversion::ToString (cityId2) + ") ;"
	      );

	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) env->getCities ().size ());

	  sql = "";
	  sql.append (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (" + Conversion::ToString (cityId1) + ", 'city_1') ;"
	      );
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT (env->getCities ().contains (cityId0));
	  CPPUNIT_ASSERT (env->getCities ().contains (cityId1));
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1"), env->getCities ().get (cityId1)->getName ());
      
	  // Update name of one city
	  sql = "";
	  sql.append ("UPDATE " + CITIES_TABLE_NAME + " SET " + 
		      CITIES_TABLE_COL_NAME + "='city_1_' WHERE " + TABLE_COL_ID + "=" + Conversion::ToString (cityId1));
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (cityId1)->getName ());
      
	  // Break all links
	  sql = "";
	  sql.append ("DELETE FROM " + ENVIRONMENT_LINKS_TABLE_NAME + " WHERE 1"); // WHERE 1 cos of SQLite optim.
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (0, (int) env->getCities ().size ());

	  // Recreate links
	  sql = "";
	  sql.append (
	      "INSERT INTO " + ENVIRONMENT_LINKS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + ", " + 
	      ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID + 
	      ") VALUES (" + Conversion::ToString (++environmentLinkId) + ", " + 
	      Conversion::ToString (environmentId) + ", " +
	      Conversion::ToString (cityId0) + ") ;"
	      );
	  sql.append (
	      "INSERT INTO " + ENVIRONMENT_LINKS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + ENVIRONMENT_LINKS_TABLE_COL_ENVIRONMENTID + ", " + 
	      ENVIRONMENT_LINKS_TABLE_COL_LINKTARGETID + 
	      ") VALUES (" + Conversion::ToString (++environmentLinkId) + ", " + 
	      Conversion::ToString (environmentId) + ", " +
	      Conversion::ToString (cityId1) + ") ;"
	      );

	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (cityId1)->getName ());

	  // Clean everything
	  sqliteExec->finalize ();

	  delete sqliteExec;
	  delete syncHook; 
	  delete envSync;
	  delete envLinkSync;
      }
      {
	  // Recreate exactly the same context except that some 
	  // values pre exist in db!
	  Environment::Registry environments;
	  
	  SQLiteThreadExec* sqliteExec = new SQLiteThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);
	  sqliteExec->registerUpdateHook (syncHook);
	  
	  EnvironmentTableSync* envSync = new EnvironmentTableSync (environments);
	  CityTableSync* citySync = new CityTableSync (environments);
	  syncHook->addTableSynchronizer (envSync);
	  syncHook->addTableSynchronizer (citySync);

	  // Create the env link synchronizer after having added the component synchronizers
	  EnvironmentLinkTableSync* envLinkSync = new EnvironmentLinkTableSync (syncHook,
										environments);

	  syncHook->addTableSynchronizer (envLinkSync);
	  
	  uid environmentId = envSync->encodeUId (0, 0, 0);
	  uid cityId0 = citySync->encodeUId (0, 0, 0);
	  uid cityId1 = citySync->encodeUId (0, 0, 1);

	  sqliteExec->initialize ();

	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) environments.size ());
	  Environment* env = environments.get (environmentId);

	  CPPUNIT_ASSERT_EQUAL (2, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (cityId1)->getName ());

	  sqliteExec->finalize ();

	  delete sqliteExec;
	  delete syncHook; 
	  delete envSync;
	  delete envLinkSync;
      }


  }





}
}

