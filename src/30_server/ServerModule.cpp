
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

#include "HTTPServer.hpp"

#include "Log.h"
//#include "threads/ManagedThread.h"
//#include "Conversion.h"

#include "ServerModule.h"
//#include "CleanerThreadExec.h"
//#include "ServerThreadExec.h"

#include <boost/lexical_cast.hpp>

#ifdef UNIX
  #define DEFAULT_TEMP_DIR "/tmp"
#endif
#ifdef WIN32
  #define DEFAULT_TEMP_DIR "c:/temp"
#endif

using namespace boost;
using namespace std;


namespace synthese
{
	using namespace util;
	
	template<> const std::string util::FactorableTemplate<db::DbModuleClass, server::ServerModule>::FACTORY_KEY("999_server");

    namespace server
    {
		ServerModule::SessionMap	ServerModule::_sessionMap;

		const std::string ServerModule::MODULE_PARAM_PORT ("port");
		const std::string ServerModule::MODULE_PARAM_NB_THREADS ("nb_threads");
		const std::string ServerModule::MODULE_PARAM_LOG_LEVEL ("log_level");
		const std::string ServerModule::MODULE_PARAM_TMP_DIR ("tmp_dir");

		void ServerModule::preInit ()
		{
			RegisterParameter (MODULE_PARAM_PORT, "8080", &ParameterCallback);
			RegisterParameter (MODULE_PARAM_NB_THREADS, "5", &ParameterCallback);
			RegisterParameter (MODULE_PARAM_LOG_LEVEL, "1", &ParameterCallback);
			RegisterParameter (MODULE_PARAM_TMP_DIR, DEFAULT_TEMP_DIR, &ParameterCallback);
		}



		void ServerModule::initialize()
		{

			Log::GetInstance ().info ("HTTP Server is listening on port " + GetParameter (MODULE_PARAM_PORT) +"...");
			
			try 
			{
				// Initialize server.
				std::size_t num_threads = boost::lexical_cast<std::size_t>(GetParameter(MODULE_PARAM_NB_THREADS));
				
				HTTPServer s("0.0.0.0", GetParameter (MODULE_PARAM_PORT), num_threads);
				s.run();



	/*		    synthese::tcp::TcpService* service = 
				synthese::tcp::TcpService::openService (port);
			    
				// CleanerThreadExec* cleanerExec = new CleanerThreadExec ();
			    
				// Every 4 hours, old files of http temp dir are cleant 
				// time_duration checkPeriod = hours(4); 
				// cleanerExec->addTempDirectory (_config.getHttpTempDir (), checkPeriod);

				// Can be shared between threads because no state variables
				ServerThreadExec* serverThreadExec = 
				new ServerThreadExec (service);
			    
				for (int i=0; i<nb_threads; ++i) 
				{
				ManagedThread* serverThread = 
					new ManagedThread (serverThreadExec,
							   "tcp_" + Conversion::ToString (i), 
							   100, true);
				}
					    
				// Create the cleaner thread (check every 5s)
				/* buggé !! ManagedThread* cleanerThread = 
				   new ManagedThread (cleanerExec,
				   "cleaner",
				   5000, true);
				*/

				// cleanerThread->start ();
				// cleanerThread->waitForReadyState ();
			    
			}
			
			catch (std::exception& ex)
			{
				Log::GetInstance ().fatal ("Unexpected exception", ex);
			} 
			catch (...)
			{
				Log::GetInstance ().fatal ("Unexpected exception");
			}

			// @todo : decide when to clse the service ??
			// synthese::tcp::TcpService::closeService (_config.getPort ());

			}



			ServerModule::SessionMap& ServerModule::getSessions()
			{
			return _sessionMap;
			}


			void 
			ServerModule::ParameterCallback (const std::string& name, 
							 const std::string& value)
			{
			if (name == "port") 
			{
				// TODO : close and reopen service on the new port
			}
			if (name == "log_level") 
			{
				Log::GetInstance ().setLevel (static_cast<Log::Level>(lexical_cast<int>(value)));
			}

	    }

		std::string ServerModule::getName() const
		{
			return "Serveur TCP";
		}

	}
}

