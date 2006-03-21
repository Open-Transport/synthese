#include "Server.h"

#include "RequestException.h"

#include <boost/iostreams/stream.hpp>
#include <boost/thread/thread.hpp>


namespace synthese
{
namespace server
{



Server::Server (int port, int nbThreads)
    : _port (port)
    , _nbThreads (nbThreads)
{
}



Server::~Server ()
{
}








    
    class ServerThread
    {
    private:
	synthese::tcp::TcpService* _tcpService;
    public:

	ServerThread (synthese::tcp::TcpService* tcpService) 
	    : _tcpService (tcpService)
	{
	}

	
	void operator()()
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
		    
		    // Echo...
		    tcpStream << requestString << std::endl;

		    _tcpService->closeConnection (serverSocket);
		}
	    }
    };
    





void 
Server::run () 
{
    synthese::tcp::TcpService* service = 
	synthese::tcp::TcpService::openService (_port);
    
    // Create the thread group.
    boost::thread_group threads;

    ServerThread serverThread (service);

    // Creates all server threads.
    for (int i=0; i<_nbThreads; ++i) 
    {
	threads.create_thread (serverThread);
    }
      
    threads.join_all();

    synthese::tcp::TcpService::closeService (_port);
}






}
}




