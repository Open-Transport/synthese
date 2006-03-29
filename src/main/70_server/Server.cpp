#include "Server.h"

#include "ServerThread.h"


#include "RequestException.h"
#include "Request.h"
#include "RequestDispatcher.h"

#include "01_util/Log.h"
#include "01_util/Exception.h"

#include <boost/filesystem/operations.hpp>
#include <boost/thread/thread.hpp>

#ifdef MODULE_39
#include "39_carto/MapRequestHandler.h"
#include "39_carto/MapBackgroundManager.h"
#endif

using synthese::util::Log;


namespace synthese
{
namespace server
{


Server* Server::_instance = 0;



Server::Server (int port, 
		int nbThreads,
		const std::string& dataDir,
		const std::string& tempDir)
    : _port (port)
    , _nbThreads (nbThreads)
    , _dataDir (dataDir)
    , _tempDir (tempDir)
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
	_instance = new Server ();
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

#ifdef MODULE_39
    
    // Initialize map background manager
    synthese::carto::MapBackgroundManager::SetBackgroundsDir (_dataDir / "backgrounds");

    synthese::server::RequestDispatcher::getInstance ()->
	registerHandler (new synthese::carto::MapRequestHandler ());
#endif
    

}




void 
Server::run () 
{
    Log::GetInstance ().info ("Starting server...");

    initialize ();

    try 
    {
	synthese::tcp::TcpService* service = 
	    synthese::tcp::TcpService::openService (_port);
	
	ServerThread serverThread (service);
	
	if (_nbThreads == 1) 
	{
	    // Monothread execution ; easier for debugging
	    Log::GetInstance ().info ("Server ready.");
	    serverThread ();
	}
	{
	    // Create the thread group.
	    boost::thread_group threads;
	    
	    // Creates all server threads.
	    for (int i=0; i< _nbThreads; ++i) 
	    {
		threads.create_thread (serverThread);
	    }
	    Log::GetInstance ().info ("Server ready.");
	    threads.join_all();
	}
	
    }
    catch (synthese::util::Exception& ex)
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



    




}
}




