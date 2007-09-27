#include "03_db_ring/RecvTokenThreadExec.h"

#include "03_db_ring/Constants.h"

#include "01_util/Log.h"
#include "01_util/iostreams/Compression.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"
#include "00_tcp/Constants.h"

#include <boost/iostreams/stream.hpp>
#include <sstream>


using namespace synthese::tcp;
using namespace synthese::util;



namespace synthese
{
    namespace dbring
    {



	RecvTokenThreadExec::RecvTokenThreadExec (int port, util::SynchronizedQueue<TokenSPtr>& tokenQueue) 
	    : _port (port)
	    , _tcpService (0)
	    , _tokenQueue (tokenQueue)
	{
	    
	}
	
	
	void
	RecvTokenThreadExec::initialize () 
	{
	    _tcpService = TcpService::openService (_port);
	}
	
	

	void
	RecvTokenThreadExec::finalize () 
	{
	    TcpService::closeService (_port);
	}
	
	
	



	    
	void 
	RecvTokenThreadExec::loop ()
	{
	    synthese::tcp::TcpServerSocket* serverSocket =
		_tcpService->acceptConnection ();
	    
	    TokenSPtr token;

	    // Non-blocking mode (default)
	    if (serverSocket != 0) 
	    {
		serverSocket->setTimeOut (5*10*TCP_TOKEN_TIMEOUT);
		token.reset (new Token ());
		
		boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
		    tcpStream (*serverSocket);
		
		try
		{
		    std::stringstream tmp;
		    Compression::ZlibDecompress (tcpStream, tmp);
		    tmp >> (*(token.get ()));

		    std::stringstream logstr;
		    logstr << "Node " << " recv [" << tmp.str ().size () << " bytes" <<
			/*<<  (*token)  <<*/ "] fm node " << token->getEmitterNodeId () << std::endl;
		    
		    Log::GetInstance ().debug (logstr.str ());

		    _tokenQueue.push (token);
		}
		catch (std::exception& e)
		{
		    Log::GetInstance ().error ("Error parsing token message", e);
		}
		
		_tcpService->closeConnection (serverSocket);
		
	    }
	}


    }
    
}

