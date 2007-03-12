#include "TcpService.h"

#include "TcpServerSocket.h"
#include "Socket.h"


namespace synthese
{
namespace tcp
{



std::map<int, TcpService*> TcpService::_activeServices;



TcpService::TcpService (int portNumber,
			bool tcpProtocol,
			bool nonBlocking)
    : _portNumber (portNumber)
    , _protocol (tcpProtocol ? PROTOCOL_TYPE_TCP 
                  : PROTOCOL_TYPE_UDP )
    , _nonBlocking (nonBlocking)
    , _socket (0)
{
    initialize ();
}



TcpService::~TcpService ()
{
    // Close all active connections
    while (_activeConnections.size () > 0)
    {
	closeConnection (_activeConnections.begin ()->second);
    }

    // Clean up is done in _socket destructor.
    // Nothing to do.
    if (_socket) delete _socket;
}



void 
TcpService::initialize () throw (SocketException)
{
    Socket* socket = new Socket (_nonBlocking);
    try 
    {
	socket->open ("*",
		     _portNumber, 
		     _protocol.c_str () );
	socket->server();
    } 
    catch (const char* msg)
    {
	delete socket;
	throw SocketException (msg);
    }
    _socket = socket;

}



TcpService* 
TcpService::openService (int portNumber)
{
    std::map<int, TcpService*>::iterator it = 
	_activeServices.find (portNumber);
    
    if (it != _activeServices.end ()) return it->second;

    TcpService* newService = new TcpService (portNumber);
    _activeServices.insert (std::make_pair (portNumber, newService));
    return newService;
}



void 
TcpService::closeService (int portNumber)
{
    std::map<int, TcpService*>::iterator it = 
	_activeServices.find (portNumber);
    
    if (it == _activeServices.end ()) return;

    delete it->second;
    _activeServices.erase (portNumber);
    
}





TcpServerSocket*
TcpService::acceptConnection () throw (SocketException)
{
    try 
    {
	int socketId = _socket->acceptConnection ();
	if (socketId == INVALID_SOCKET) return 0;

	// Note : the lock must be done here; otherwise another take the lock, waits
        // for client connection and no other client connection will ever be closed!
        // This is valid because the acceptConnection method does not modify Socket object.

	boost::mutex::scoped_lock lock (_serviceMutex);

	TcpServerSocket* serverSocket = new TcpServerSocket (*this, socketId);

	_activeConnections.insert (std::make_pair (socketId, serverSocket));
	return serverSocket;
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}




int 
TcpService::getConnectionCount () const
{
    boost::mutex::scoped_lock lock (_serviceMutex);

    return _activeConnections.size ();
}



void 
TcpService::closeConnection (TcpServerSocket* socket) throw (SocketException)
{
    boost::mutex::scoped_lock lock (_serviceMutex);

    int socketId = socket->getSocketId ();

    std::map<int, TcpServerSocket*>::iterator iter = 
	_activeConnections.find (socketId);

    if (iter == _activeConnections.end ()) 
    {
	return;
    }

    try 
    {
	_socket->closeSocket (socketId);
	delete iter->second;
	_activeConnections.erase (socketId);
    }
    catch (const char* msg)
    {
	throw SocketException (msg);
    }
}




}
}

