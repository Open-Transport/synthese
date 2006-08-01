#include "Server.h"

#include "CleanerThreadExec.h"
#include "ServerThreadExec.h"

#include "RequestException.h"
#include "Request.h"

#include "00_tcp/TcpService.h"

#include "01_util/Conversion.h"
#include "01_util/Thread.h"
#include "01_util/ThreadGroup.h"
#include "01_util/Log.h"
#include "01_util/Exception.h"

#include "02_db/SQLite.h"
#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "ServerConfigTableSync.h"
#include "17_env_ls_sql/CityTableSync.h"
#include "17_env_ls_sql/EnvironmentTableSync.h"
#include "17_env_ls_sql/EnvironmentLinkTableSync.h"


#include <boost/filesystem/operations.hpp>

using synthese::util::Conversion;
using synthese::util::Log;
using synthese::util::Thread;
using synthese::util::ThreadGroup;
using synthese::util::ThreadExec;

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

    synthese::envlssql::EnvironmentTableSync* envSync = 
	new synthese::envlssql::EnvironmentTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

    synthese::envlssql::CityTableSync* citySync = 
	new synthese::envlssql::CityTableSync (_environments, TRIGGERS_ENABLED_CLAUSE);

    // TODO : add other synchronizers here...

    syncHook->addTableSynchronizer (configSync);
    syncHook->addTableSynchronizer (envSync);
    syncHook->addTableSynchronizer (citySync);

    // Create the env link synchronizer after having added the component synchronizers
    synthese::envlssql::EnvironmentLinkTableSync* envLinkSync = new synthese::envlssql::EnvironmentLinkTableSync 
	(syncHook, _environments);

    syncHook->addTableSynchronizer (envLinkSync);
    
    sqliteExec->registerUpdateHook (syncHook);

    
    // Environment are populated. Server config is filled.
    sqliteThread.waitForReadyState ();
    

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




RequestDispatcher& 
Server::getRequestDispatcher () 
{
    return _requestDispatcher;
}



ServerConfig& 
Server::getConfig ()
{
    return _config;
}




}
}




