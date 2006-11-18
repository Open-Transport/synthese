#include "Server.h"

#include "CleanerThreadExec.h"
#include "ServerThreadExec.h"

#include "11_interfaces/RequestException.h"
#include "11_interfaces/Request.h"

#include "00_tcp/TcpService.h"

#include "01_util/Conversion.h"
#include "01_util/Thread.h"
#include "01_util/ThreadGroup.h"
#include "01_util/Log.h"
#include "01_util/Exception.h"
#include "01_util/Factory.h"
#include "01_util/Exception.h"

#include "02_db/SQLite.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "ServerConfigTableSync.h"
#include "ModuleClass.h"
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

#include "11_interfaces/InterfaceTableSync.h"
#include "11_interfaces/InterfacePageTableSync.h"
#include "11_interfaces/SiteTableSync.h"


#include <boost/filesystem/operations.hpp>

using synthese::util::Conversion;
using synthese::util::Log;
using synthese::util::Thread;
using synthese::util::ThreadGroup;
using synthese::util::ThreadExec;
using synthese::util::Factory;

using synthese::env::Environment;

using synthese::interfaces::Interface;

using namespace boost::posix_time;


namespace synthese
{
namespace server
{


Server* Server::_instance = 0;


Server::Server (const boost::filesystem::path& dbFile)
    : _dbFile (dbFile)
{
}



Server::~Server ()
{
}



Server* 
Server::GetInstance ()
{
    if (_instance == 0)
    {
	    throw new synthese::util::Exception ("Server instance has never been set!");
    }
    return _instance;
}



void 
Server::SetInstance (Server* instance)
{
    _instance = instance;
}

    

Environment::Registry& 
Server::getEnvironments ()
{
    return _environments;
}



const Environment::Registry& 
Server::getEnvironments () const
{
    return _environments;
}




Interface::Registry& 
Server::getInterfaces ()
{
    return _interfaces;
}



const Interface::Registry& 
Server::getInterfaces () const
{
    return _interfaces;
}







void 
Server::initialize ()
{
    // Register all synchronizers
    synthese::db::SQLiteThreadExec* sqliteExec = new synthese::db::SQLiteThreadExec (_dbFile);

    // Start the db sync thread right now
    synthese::util::Thread sqliteThread (sqliteExec, "sqlite");
    sqliteThread.start ();
    
    synthese::db::SQLiteSync* syncHook = new synthese::db::SQLiteSync (synthese::envlssql::TABLE_COL_ID);
    
    ServerConfigTableSync* configSync = new ServerConfigTableSync (_config);

    // Note : registration order is important !

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


	synthese::db::InterfaceTableSync* interfaceSync = 
		new synthese::db::InterfaceTableSync (_interfaces, TRIGGERS_ENABLED_CLAUSE);


	synthese::db::InterfacePageTableSync* interfacePageSync = 
		new synthese::db::InterfacePageTableSync (_interfaces, TRIGGERS_ENABLED_CLAUSE);

	synthese::db::SiteTableSync* siteSync = 
		new synthese::db::SiteTableSync (_sites, TRIGGERS_ENABLED_CLAUSE, _interfaces, _environments);

    syncHook->addTableSynchronizer (configSync);
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
	syncHook->addTableSynchronizer (interfaceSync);
	syncHook->addTableSynchronizer (interfacePageSync);

    // Create the env link synchronizer after having added the component synchronizers
    synthese::envlssql::EnvironmentLinkTableSync* envLinkSync = new synthese::envlssql::EnvironmentLinkTableSync 
	(syncHook, _environments);

    syncHook->addTableSynchronizer (envLinkSync);
    
    sqliteExec->registerUpdateHook (syncHook);

    // Environment are populated. Server config is filled.
    sqliteThread.waitForReadyState ();
    
	// Initialize modules
	if (Factory<ModuleClass>::size() == 0)
		throw synthese::util::Exception("No registered module !");

	for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); it != Factory<ModuleClass>::end(); ++it)
	{
		ModuleClass* module = it.getObject();
		module->initialize(this);
		_modules.insert(make_pair(it.getKey(), module));
	}

	

}




void 
Server::run () 
{
    Log::GetInstance ().info ("Starting server...");

    try 
    {
	Log::GetInstance ().info ("");
	Log::GetInstance ().info ("Param datadir  = " + _config.getDataDir ().string ());
	Log::GetInstance ().info ("Param tempdir  = " + _config.getTempDir ().string ());
	Log::GetInstance ().info ("Param loglevel = " + Conversion::ToString (_config.getLogLevel ()));
	Log::GetInstance ().info ("Param port     = " + Conversion::ToString (_config.getPort ()));
	Log::GetInstance ().info ("Param threads  = " + Conversion::ToString (_config.getNbThreads ()));
	Log::GetInstance ().info ("Param httptempdir  = " + _config.getHttpTempDir ().string ());
	Log::GetInstance ().info ("Param httptempurl  = " + _config.getHttpTempUrl ());
	Log::GetInstance ().info ("");

	synthese::tcp::TcpService* service = 
	    synthese::tcp::TcpService::openService (_config.getPort ());
	
	ThreadGroup threadGroup;

	CleanerThreadExec* cleanerExec = new CleanerThreadExec ();

	// Every 4 hours, old files of http temp dir are cleant 
	time_duration checkPeriod = hours(4); 
	cleanerExec->addTempDirectory (_config.getHttpTempDir (), checkPeriod);

	
	if (_config.getNbThreads () == 1) 
	{
	    // Monothread execution ; easier for debugging
	    // Review this to allow going through all loops of each
	    // ThreadExec in the same while loop !
	    Log::GetInstance ().info ("Server ready.");
	    ServerThreadExec serverThreadExec (service);
	    while (1)
	    {
		serverThreadExec.loop ();
	    }
	}
	else
	{
	    
	    for (int i=0; i< _config.getNbThreads (); ++i) 
	    {
		// ServerThreadExec could be shared by all threads (no specific state variable)
		Thread serverThread (new ServerThreadExec (service), "tcp_" + Conversion::ToString (i), 1);
		threadGroup.addThread (serverThread);
		serverThread.start ();
	    }

	    // Create the cleaner thread (check every 5s)
	    Thread cleanerThread (cleanerExec, "cleaner", 5000);
	    threadGroup.addThread (cleanerThread);
	    cleanerThread.start ();

	    threadGroup.waitForAllReady ();

	    Log::GetInstance ().info ("Server ready.");

	    threadGroup.waitForAllStopped ();
	}
	
    }
    catch (std::exception& ex)
    {
	Log::GetInstance ().fatal ("", ex);
    } 


    synthese::tcp::TcpService::closeService (_config.getPort ());
}


const ServerConfig& 
Server::getConfig () const
{
    return _config;
}




}
}
