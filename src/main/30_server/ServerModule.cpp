
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
	ServerModule::SessionMap	ServerModule::_sessionMap;


	void ServerModule::preInit ()
	{
		RegisterParameter ("port", "3591", &ParameterCallback);
		RegisterParameter ("nb_threads", "5", &ParameterCallback);
		RegisterParameter ("log_level", "1", &ParameterCallback);
	}

	void ServerModule::initialize()
	{

		Log::GetInstance ().info ("Starting server...");
		
		try 
		{
		    int port = Conversion::ToInt (GetParameter ("port"));
		    int nb_threads = Conversion::ToInt (GetParameter ("nb_threads"));

		    synthese::tcp::TcpService* service = 
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
		    
		    Log::GetInstance ().info ("Server ready.");
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
		    synthese::util::Log::GetInstance ().setLevel ((Log::Level) Conversion::ToInt (value));
		}

	    }

	}
	
	
	
    }
    
