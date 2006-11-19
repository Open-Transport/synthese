
#include "01_util/Thread.h"

#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteSync.h"

#include "17_env_ls_sql/AddressTableSync.h"
#include "17_env_ls_sql/AlarmTableSync.h"
#include "17_env_ls_sql/AxisTableSync.h"
#include "17_env_ls_sql/BikeComplianceTableSync.h"
#include "17_env_ls_sql/CityTableSync.h"
#include "17_env_ls_sql/ConnectionPlaceTableSync.h"
#include "17_env_ls_sql/ContinuousServiceTableSync.h"
#include "17_env_ls_sql/FareTableSync.h"
#include "17_env_ls_sql/HandicappedComplianceTableSync.h"
#include "17_env_ls_sql/LineStopTableSync.h"
#include "17_env_ls_sql/LineTableSync.h"
#include "17_env_ls_sql/PedestrianComplianceTableSync.h"
#include "17_env_ls_sql/PhysicalStopTableSync.h"
#include "17_env_ls_sql/PlaceAliasTableSync.h"
#include "17_env_ls_sql/PublicPlaceTableSync.h"
#include "17_env_ls_sql/ReservationRuleTableSync.h"
#include "17_env_ls_sql/RoadChunkTableSync.h"
#include "17_env_ls_sql/RoadTableSync.h"
#include "17_env_ls_sql/ScheduledServiceTableSync.h"
#include "17_env_ls_sql/ServiceDateTableSync.h"
#include "17_env_ls_sql/TransportNetworkTableSync.h"
#include "17_env_ls_sql/EnvironmentTableSync.h"
#include "17_env_ls_sql/EnvironmentLinkTableSync.h"
#include "15_env/EnvModule.h"

namespace synthese
{
	using namespace db;
	using namespace util;

	namespace env
	{
		void EnvModule::initialize()
		{
			// Register all synchronizers
			synthese::db::SQLiteThreadExec* sqliteExec = new synthese::db::SQLiteThreadExec (_databasePath);

			// Start the db sync thread right now
			synthese::util::Thread sqliteThread (sqliteExec, "sqlite");
			sqliteThread.start ();

			synthese::db::SQLiteSync* syncHook = new synthese::db::SQLiteSync (TABLE_COL_ID);


			synthese::envlssql::EnvironmentTableSync* envSync = 
				new synthese::envlssql::EnvironmentTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);


			synthese::envlssql::AlarmTableSync* alarmSync = 
				new synthese::envlssql::AlarmTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::FareTableSync* fareSync = 
				new synthese::envlssql::FareTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::BikeComplianceTableSync* bikeComplianceSync = 
				new synthese::envlssql::BikeComplianceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::HandicappedComplianceTableSync* handicappedComplianceSync = 
				new synthese::envlssql::HandicappedComplianceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::PedestrianComplianceTableSync* pedestrianComplianceSync = 
				new synthese::envlssql::PedestrianComplianceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::ReservationRuleTableSync* reservationRuleSync = 
				new synthese::envlssql::ReservationRuleTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::CityTableSync* citySync = 
				new synthese::envlssql::CityTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::TransportNetworkTableSync* transportNetworkSync = 
				new synthese::envlssql::TransportNetworkTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::AxisTableSync* axisSync = 
				new synthese::envlssql::AxisTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::LineTableSync* lineSync = 
				new synthese::envlssql::LineTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::RoadTableSync* roadSync = 
				new synthese::envlssql::RoadTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::ConnectionPlaceTableSync* connectionPlaceSync = 
				new synthese::envlssql::ConnectionPlaceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::PublicPlaceTableSync* publicPlaceSync = 
				new synthese::envlssql::PublicPlaceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::PlaceAliasTableSync* placeAliasSync = 
				new synthese::envlssql::PlaceAliasTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::AddressTableSync* addressSync = 
				new synthese::envlssql::AddressTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::RoadChunkTableSync* roadChunkSync = 
				new synthese::envlssql::RoadChunkTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::ContinuousServiceTableSync* continuousServiceSync = 
				new synthese::envlssql::ContinuousServiceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::ScheduledServiceTableSync* scheduledServiceSync = 
				new synthese::envlssql::ScheduledServiceTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::ServiceDateTableSync* serviceDateSync = 
				new synthese::envlssql::ServiceDateTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::PhysicalStopTableSync* physicalStopSync = 
				new synthese::envlssql::PhysicalStopTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

			synthese::envlssql::LineStopTableSync* lineStopSync = 
				new synthese::envlssql::LineStopTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);


			syncHook->addTableSynchronizer (envSync);
			syncHook->addTableSynchronizer (alarmSync);
			syncHook->addTableSynchronizer (fareSync);
			syncHook->addTableSynchronizer (bikeComplianceSync);
			syncHook->addTableSynchronizer (handicappedComplianceSync);
			syncHook->addTableSynchronizer (pedestrianComplianceSync);
			syncHook->addTableSynchronizer (reservationRuleSync);
			syncHook->addTableSynchronizer (citySync);
			syncHook->addTableSynchronizer (transportNetworkSync);
			syncHook->addTableSynchronizer (axisSync);
			syncHook->addTableSynchronizer (lineSync);
			syncHook->addTableSynchronizer (roadSync);
			syncHook->addTableSynchronizer (connectionPlaceSync);
			syncHook->addTableSynchronizer (publicPlaceSync);
			syncHook->addTableSynchronizer (placeAliasSync);
			syncHook->addTableSynchronizer (addressSync);
			syncHook->addTableSynchronizer (roadChunkSync);
			syncHook->addTableSynchronizer (continuousServiceSync);
			syncHook->addTableSynchronizer (scheduledServiceSync);
			syncHook->addTableSynchronizer (serviceDateSync);
			syncHook->addTableSynchronizer (physicalStopSync);
			syncHook->addTableSynchronizer (lineStopSync);

			// Create the env link synchronizer after having added the component synchronizers
			synthese::envlssql::EnvironmentLinkTableSync* envLinkSync = new synthese::envlssql::EnvironmentLinkTableSync 
				(syncHook, _environments);

			syncHook->addTableSynchronizer (envLinkSync);

			sqliteExec->registerUpdateHook (syncHook);

			// Environment are populated. Server config is filled.
			sqliteThread.waitForReadyState ();
		}


		Environment::Registry& 
			EnvModule::getEnvironments ()
		{
			return _environments;
		}



		const Environment::Registry& 
			EnvModule::getEnvironments () const
		{
			return _environments;
		}




	}
}