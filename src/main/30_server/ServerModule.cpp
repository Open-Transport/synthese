
#include <stdlib.h>
#include <time.h>

#include "00_tcp/TcpService.h"

#include "01_util/Log.h"
#include "01_util/Thread.h"
#include "01_util/ThreadGroup.h"

#include "02_db/SQLiteSync.h"
#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteThreadExec.h"

#include "11_interfaces/InterfaceModule.h"

#include "15_env/EnvModule.h"

#include "30_server/ServerModule.h"
#include "30_server/CleanerThreadExec.h"
#include "30_server/ServerThreadExec.h"

using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace tcp;

	namespace server
	{
		Site::Registry				ServerModule::_sites;
		ServerConfig				ServerModule::_config;
		ServerModule::SessionMap	ServerModule::_sessionMap;

		void ServerModule::initialize()
		{
			srand( (unsigned) std::time( NULL ) );
		}

		Site::Registry& ServerModule::getSites()
		{
			return _sites;
		}

		void ServerModule::startServer()
		{
			// Initialize permanent ram loaded data
			Log::GetInstance().info("Loading live data...");
			SQLiteThreadExec* sqliteExec = new SQLiteThreadExec (_databasePath);
			Thread sqliteThread (sqliteExec, "sqlite");
			sqliteThread.start ();
			SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);
			sqliteExec->registerUpdateHook (syncHook);
			sqliteThread.waitForReadyState ();


			// Initialize modules
			if (Factory<ModuleClass>::size() == 0)
				throw Exception("No registered module !");

			for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); it != Factory<ModuleClass>::end(); ++it)
			{
				Log::GetInstance ().info ("Initializing module " + it.getKey() + "...");
				it->setDatabasePath(_databasePath);
				it->initialize();
			}


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

		ServerConfig& ServerModule::getConfig()
		{
			return _config;
		}

		ServerModule::SessionMap& ServerModule::getSessions()
		{
			return _sessionMap;
		}
	}
}