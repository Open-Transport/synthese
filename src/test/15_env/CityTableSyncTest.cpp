
#include "CityTableSyncTest.h"

#include "01_util/Conversion.h"
#include "01_util/UId.h"

#include "02_db/DBModule.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/EnvModule.h"
#include "15_env/Environment.h"

#include "15_env/CityTableSync.h"
#include "15_env/EnvironmentTableSync.h"
#include "15_env/EnvironmentLinkTableSync.h"

#include <boost/filesystem/operations.hpp>

#include <iostream>
#include <sstream>
#include <map>



using namespace synthese::db;

using synthese::util::Conversion;





namespace synthese
{
namespace env
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
      Environment::Registry& environments = EnvModule::getEnvironments ();
      {
	  environments.clear ();
	  
	  boost::filesystem::remove ("test_db.s3db");
	  
	  SQLiteThreadExec* sqliteExec = new SQLiteThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);



	  
	  EnvironmentTableSync* envSync = new EnvironmentTableSync ();
	  CityTableSync* citySync = new CityTableSync ();  
          citySync->setEnableTriggers (false); // Override triggers for testing

	  syncHook->addTableSynchronizer (envSync);
	  syncHook->addTableSynchronizer (citySync);

	  // Create the env link synchronizer after having added the component synchronizers
	  EnvironmentLinkTableSync* envLinkSync = new EnvironmentLinkTableSync ();

	  syncHook->addTableSynchronizer (envLinkSync);

	  sqliteExec->registerUpdateHook (syncHook);
	  sqliteExec->initialize ();
      
	  // Everything is ready for the tests
	  CPPUNIT_ASSERT_EQUAL (0, (int) environments.size ());
      
	  // Add a new environment row
	  
	  uid environmentId = synthese::util::encodeUId (envSync->getTableId (), 0, 0, 0);

	  sqliteExec->execUpdate (
	      "INSERT INTO " + ENVIRONMENTS_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ") " + 
	      "VALUES (" + Conversion::ToString (environmentId) + ")"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) environments.size ());
      
      
	  // Add a new city before linking it to environment
	  Environment* env = environments.get (0);
	  uid cityId0 = synthese::util::encodeUId (citySync->getTableId (), 0, 0, 0);

	  sqliteExec->execUpdate (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (" + Conversion::ToString (cityId0) + ", 'city_0')"
	      );
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (0, (int) env->getCities ().size ());
	  
	  uid environmentLinkId = synthese::util::encodeUId (envLinkSync->getTableId (), 0, 0, 0);

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

	  uid cityId1 = synthese::util::encodeUId (citySync->getTableId (), 0, 0, 1);
	  uid cityId2 = synthese::util::encodeUId (citySync->getTableId (), 0, 0, 2);

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
	      "VALUES (" + Conversion::ToString (cityId2) + ", 'city_2') ;"
	      );
	  sql.append (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (" + Conversion::ToString (cityId1) + ", 'city_1') ;"
	      );
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (3, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT (env->getCities ().contains (cityId0));
	  CPPUNIT_ASSERT (env->getCities ().contains (cityId1));
	  CPPUNIT_ASSERT (env->getCities ().contains (cityId2));
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1"), env->getCities ().get (cityId1)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_2"), env->getCities ().get (cityId2)->getName ());
      
	  // Update name of one city
	  sql = "";
	  sql.append ("UPDATE " + CITIES_TABLE_NAME + " SET " + 
		      CITIES_TABLE_COL_NAME + "='city_1_' WHERE " + TABLE_COL_ID + "=" + Conversion::ToString (cityId1));
	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (3, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_0"), env->getCities ().get (cityId0)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (cityId1)->getName ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_2"), env->getCities ().get (cityId2)->getName ());
      
	  // Delete 2 cities on 3
	  sql = "";
	  sql.append ("DELETE FROM " + CITIES_TABLE_NAME + 
		      " WHERE " + TABLE_COL_ID + "=" + Conversion::ToString (cityId0) + 
		      " OR " + TABLE_COL_ID + "=" + Conversion::ToString (cityId2));

	  sqliteExec->execUpdate (sql);
	  sqliteExec->loop ();
	  CPPUNIT_ASSERT_EQUAL (1, (int) env->getCities ().size ());
	  CPPUNIT_ASSERT_EQUAL (std::string ("city_1_"), env->getCities ().get (cityId1)->getName ());


	  // Recreate city with id 0. Added immediately cos the link was not
	  // removed.
	  sqliteExec->execUpdate (
	      "INSERT INTO " + CITIES_TABLE_NAME + " (" + 
	      TABLE_COL_ID + ", " + CITIES_TABLE_COL_NAME + ") " + 
	      "VALUES (" + Conversion::ToString (cityId0) + ", 'city_0')"
	      );
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
	  environments.clear ();
	  
	  SQLiteThreadExec* sqliteExec = new SQLiteThreadExec ("test_db.s3db");
	  SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);
	  sqliteExec->registerUpdateHook (syncHook);
	  
	  EnvironmentTableSync* envSync = new EnvironmentTableSync ();
	  CityTableSync* citySync = new CityTableSync ();
          citySync->setEnableTriggers (false); // Override triggers for testing

	  syncHook->addTableSynchronizer (envSync);
	  syncHook->addTableSynchronizer (citySync);

	  // Create the env link synchronizer after having added the component synchronizers
	  EnvironmentLinkTableSync* envLinkSync = new EnvironmentLinkTableSync ();

	  syncHook->addTableSynchronizer (envLinkSync);
	  
	  uid environmentId = synthese::util::encodeUId (envSync->getTableId (), 0, 0, 0);
	  uid cityId0 = synthese::util::encodeUId (citySync->getTableId (), 0, 0, 0);
	  uid cityId1 = synthese::util::encodeUId (citySync->getTableId (), 0, 0, 1);
	  
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

      environments.clear ();


  }





}
}


