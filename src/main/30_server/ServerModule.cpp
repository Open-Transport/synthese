
/** ServerModule class implementation.
	@file ServerModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "00_tcp/TcpService.h"

#include "01_util/Log.h"
#include "01_util/ManagedThread.h"
#include "01_util/Conversion.h"

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
				    ServerThreadExec serverThreadExec (service);
				    // Monothread execution ; easier for debugging
				    // Review this to allow going through all loops of each
				    // ThreadExec in the same while loop !
					Log::GetInstance ().info ("Server ready.");
					while (1)
					{
					    serverThreadExec.loop ();
					}
				}
				else
				{
				    // Can be shared between threads because no state variables
				    ServerThreadExec* serverThreadExec = 
					new ServerThreadExec (service);

				    for (int i=0; i<_config.getNbThreads (); ++i) 
				    {
					ManagedThread* serverThread = 
					    new ManagedThread (serverThreadExec,
							       "tcp_" + Conversion::ToString (i), 
							       1, true);

					// serverThread->start ();
					// serverThread->waitForReadyState ();
					
				    }
				    
				    // Create the cleaner thread (check every 5s)
				    /* buggé !! ManagedThread* cleanerThread = 
					new ManagedThread (cleanerExec,
							   "cleaner",
							   5000, true);
				    */

				    // cleanerThread->start ();
				    // cleanerThread->waitForReadyState ();
				    
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
