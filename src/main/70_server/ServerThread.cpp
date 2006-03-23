#include "ServerThread.h"


#include "Request.h"
#include "RequestDispatcher.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"

#include <boost/iostreams/stream.hpp>


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
    std::cout << "* Server thread started *" << std::endl;
    
    while ( 1 )
    {
	// No need to lock, TcpService methods are thread-safe.
	synthese::tcp::TcpServerSocket& serverSocket =
	    _tcpService->acceptConnection ();
	
	boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
	    tcpStream (serverSocket);
	
	std::string requestString;
	tcpStream >> requestString;
	
	std::cout << "Received request : " << requestString << std::endl;
	
	// Parse request
	Request request (requestString);

	// Send request to proper handler through dispatcher
	RequestDispatcher::getInstance ()->dispatchRequest (request, tcpStream);
	
	_tcpService->closeConnection (serverSocket);
    }
}




}
}

