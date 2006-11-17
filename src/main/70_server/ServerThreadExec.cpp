#include "ServerThreadExec.h"


#include "Server.h"

#include "11_interfaces/Request.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"

#include "01_util/Conversion.h"

#include <boost/iostreams/stream.hpp>

#include "01_util/Log.h"

using synthese::util::Log;
using synthese::util::Conversion;
using synthese::interfaces::Request;
using synthese::util::Factory;

namespace synthese
{
namespace server
{

ServerThreadExec::ServerThreadExec (synthese::tcp::TcpService* tcpService) 
    : _tcpService (tcpService)
{
}


	
void 
ServerThreadExec::loop ()
{
    // No need to lock, TcpService methods are thread-safe.
    synthese::tcp::TcpServerSocket& serverSocket =
	_tcpService->acceptConnection ();
    
    boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
	tcpStream (serverSocket);
    
    
    char buffer[1024*512]; // 512K buffer max
    tcpStream.getline (buffer, 1024*512);
    
    try
    {
		std::string requestString (buffer);
		// tcpStream >> requestString;
		
		Log::GetInstance ().debug ("Received request : " + 
					requestString + " (" + Conversion::ToString (requestString.size ()) + 
					" bytes)");
		
		// Parse request
		Request* request = Factory<Request>::create(requestString);
		request->run(tcpStream);
		delete request;
	
    }
    catch (std::exception& ex)
    {
		Log::GetInstance ().error ("", ex);
    } 
    _tcpService->closeConnection (serverSocket);
}




}
}

