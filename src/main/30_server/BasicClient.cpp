#include "BasicClient.h"


#include "00_tcp/TcpClientSocket.h"
#include "01_util/Conversion.h"

#include "QueryString.h"
#include "ClientException.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>

#include <iostream>
#include <sstream>


using namespace synthese::server;
using namespace synthese::util;
using namespace synthese::tcp;


namespace synthese
{

namespace server
{


    BasicClient::BasicClient (const std::string& serverHost,
			      const int serverPort,
			      int timeOut)
	: _serverHost (serverHost)
	, _serverPort (serverPort)
	, _timeOut (timeOut)
    {
    }



    
    void 
    BasicClient::request (std::ostream& out, const std::string& request, const std::string& ip, const std::string& clientUrl)
    {
	std::stringstream req;
	req << request ;

	// Append client IP
	req << QueryString::PARAMETER_SEPARATOR 
	    << QueryString::PARAMETER_IP << QueryString::PARAMETER_ASSIGNMENT << ip;
	
	if (clientUrl.size () > 0)
	{
	    // Append client URL
	    req << QueryString::PARAMETER_SEPARATOR 
		<< QueryString::PARAMETER_CLIENT_URL << QueryString::PARAMETER_ASSIGNMENT << clientUrl; 
	}

	TcpClientSocket clientSock (_serverHost, _serverPort, _timeOut);
	boost::iostreams::stream<TcpClientSocket> cliSocketStream;

	clientSock.tryToConnect ();
	
	if (clientSock.isConnected () == false)
	{
	    throw ClientException ("Cannot connect to server " + _serverHost + ":" + Conversion::ToString (_serverPort));
	}
	
	cliSocketStream.open (clientSock);
	
	// Send request to server
	cliSocketStream << req.str () << std::flush;
	
	// This is valid because server closes stream as soon as it has replied.
	boost::iostreams::copy (cliSocketStream, out);
	
	cliSocketStream.close ();
    }


}
}
