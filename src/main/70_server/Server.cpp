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


Server::Server (int port, 
		int nbThreads,
		const std::string& dataDir,
		const std::string& tempDir,
        const std::string& httpTempDir, 
        const std::string& httpTempUrl)
    : _port (port)
    , _nbThreads (nbThreads)
	, _dataDir (dataDir, boost::filesystem::native)
    , _tempDir (tempDir, boost::filesystem::native)
    , _httpTempDir (httpTempDir, boost::filesystem::native)
    , _httpTempUrl (httpTempUrl)
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
    _dataDir.normalize ();
    if (boost::filesystem::exists (_dataDir) == false)
    {
	throw synthese::util::Exception ("Cannot find data directory '" + _dataDir.string () + "'");
    }
    _tempDir.normalize ();
    if (boost::filesystem::exists (_tempDir) == false)
    {
	throw synthese::util::Exception ("Cannot find temp directory '" + _tempDir.string () + "'");
    }


}




void 
Server::run () 
{
    Log::GetInstance ().info ("Starting server...");

    try 
    {
	initialize ();

	synthese::tcp::TcpService* service = 
	    synthese::tcp::TcpService::openService (_port);
	
	ThreadGroup threadGroup;

	CleanerThreadExec* cleanerExec = new CleanerThreadExec ();

	// Every 4 hours, old files of http temp dir are cleant 
	time_duration checkPeriod = hours(4); 
	cleanerExec->addTempDirectory (_httpTempDir, checkPeriod);

	
	if (_nbThreads == 1) 
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
	    
	    for (int i=0; i< _nbThreads; ++i) 
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


    synthese::tcp::TcpService::closeService (_port);
}




int 
Server::getPort () const
{
    return _port;
}




RequestDispatcher& 
Server::getRequestDispatcher () 
{
    return _requestDispatcher;
}



int 
Server::getNbThreads () const
{
    return _nbThreads;
}




const boost::filesystem::path& 
Server::getDataDir () const
{
    return _dataDir;
}



const boost::filesystem::path& 
Server::getTempDir () const
{
    return _tempDir;
}


const boost::filesystem::path& 
Server::getHttpTempDir () const
{
    return _httpTempDir;
}


const std::string& 
Server::getHttpTempUrl () const
{
    return _httpTempUrl;
}

    




}
}




