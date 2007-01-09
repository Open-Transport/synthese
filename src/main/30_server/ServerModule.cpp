
#include "00_tcp/TcpService.h"

#include "01_util/Log.h"
#include "01_util/Thread.h"
#include "01_util/ThreadManager.h"

#include "30_server/ServerModule.h"
#include "30_server/CleanerThreadExec.h"
#include "30_server/ServerThreadExec.h"

using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace tcp;

	namespace server
	{
		Site::Registry				ServerModule::_sites;
		ServerConfig				ServerModule::_config;
		ServerModule::SessionMap	ServerModule::_sessionMap;

		void ServerModule::initialize()
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

				    for (int i=0; i<_config.getNbThreads (); ++i) 
				    {
					// ServerThreadExec could be shared by all 
                                        // threads (no specific state variable)
					ThreadSPtr serverThread (
					    new Thread (
						new ServerThreadExec (service), 
						"tcp_" + Conversion::ToString (i), 1));
					ThreadManager::Instance ()->addThread (serverThread);

					serverThread->start ();
					serverThread->waitForReadyState ();
					
				    }
				    
				    // Create the cleaner thread (check every 5s)
				    ThreadSPtr cleanerThread (new Thread (cleanerExec, "cleaner", 5000));
				    ThreadManager::Instance ()->addThread (cleanerThread);
				    
				    cleanerThread->start ();
				    cleanerThread->waitForReadyState ();
				    
				    Log::GetInstance ().info ("Server ready.");
				}

			}
			catch (std::exception& ex)
			{
				Log::GetInstance ().fatal ("", ex);
			} 

			// @todo : decide when to clse the service ??
			// synthese::tcp::TcpService::closeService (_config.getPort ());

		}



		Site::Registry& ServerModule::getSites()
		{
			return _sites;
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
