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
	
	std::string requestString;
	tcpStream >> requestString;
	
	Log::GetInstance ().debug ("Received request : " + requestString);

	// TODO : add a BIG try/catch here...
	
	// Parse request
	Request request (requestString);

	// Send request to proper handler through dispatcher
	RequestDispatcher::getInstance ()->dispatchRequest (request, tcpStream);
	
	_tcpService->closeConnection (serverSocket);
    }
}




}
}

