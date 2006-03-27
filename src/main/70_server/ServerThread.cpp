#include "ServerThread.h"


#include "Request.h"
#include "RequestDispatcher.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"

#include <boost/iostreams/stream.hpp>

#include "01_util/Log.h"

using synthese::util::Log;

namespace synthese
{
namespace server
{

ServerThread::ServerThread (synthese::tcp::TcpService* tcpService) 
    : _tcpService (tcpService)
{
}


	
void 
ServerThread::operator()()
{
    while ( 1 )
    {
	// No need to lock, TcpService methods are thread-safe.
	synthese::tcp::TcpServerSocket& serverSocket =
	    _tcpService->acceptConnection ();
	
	boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
	    tcpStream (serverSocket);
	
	char buffer[1024*64]; // 64K buffer max
	tcpStream.getline (buffer, 1024*64);
	    
	try
	{
	    std::string requestString (buffer);
	    // tcpStream >> requestString;
	    
	    Log::GetInstance ().debug ("Received request : " + requestString);
	    
	    // Parse request
	    Request request (requestString);
	    
	    // Send request to proper handler through dispatcher
	    RequestDispatcher::getInstance ()->dispatchRequest (request, tcpStream);

	    _tcpService->closeConnection (serverSocket);
	}
	catch (synthese::util::Exception& ex)
	{
	    Log::GetInstance ().error ("Error while executing request", ex);
	} 
	
    }
}




}
}

