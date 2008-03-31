
/** TcpService class implementation.
	@file TcpService.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include "00_tcp/Socket.h"

#include "00_tcp/TcpService.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/Constants.h"


namespace synthese
{
namespace tcp
{



std::map<int, TcpService*> TcpService::_activeServices;



TcpService::TcpService (int portNumber,
			bool tcpProtocol,
			bool nonBlocking,
			int backlogSize
    )
    : _portNumber (portNumber)
    , _protocol (tcpProtocol ? PROTOCOL_TYPE_TCP 
                  : PROTOCOL_TYPE_UDP )
    , _nonBlocking (nonBlocking)
    , _backlogSize (backlogSize)
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
TcpService::initialize () 
{
    Socket* socket = new Socket (_nonBlocking, _backlogSize);
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
TcpService::openService (int portNumber, 
			 bool tcpProtocol,
			 bool nonBlocking,
			 int backlogSize)
{
    std::map<int, TcpService*>::iterator it = 
	_activeServices.find (portNumber);
    
    if (it != _activeServices.end ()) return it->second;

    //std::cerr << "**************************************  NEW TCP SERVICE " << portNumber << std::endl;

    TcpService* newService = new TcpService (portNumber, tcpProtocol, nonBlocking);
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
TcpService::acceptConnection () 
{
	boost::mutex::scoped_lock lock (_serviceMutex);
    if (_socket == 0) throw SocketException ("Socket not ready !");

    try 
    {
	int socketId = _socket->acceptConnection ();

	if (socketId == INVALID_SOCKET) return 0;

	// Note : the lock must be done here; otherwise another take the lock, waits
        // for client connection and no other client connection will ever be closed!
        // This is valid because the acceptConnection method does not modify Socket object.

//	boost::mutex::scoped_lock lock (_serviceMutex);

	TcpServerSocket* serverSocket = new TcpServerSocket (*this, socketId);

	_activeConnections.insert (std::make_pair (socketId, serverSocket));

	// std::cerr << " nb conn = " << _activeConnections.size () << std::endl;
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
TcpService::closeConnection (TcpServerSocket* socket) 
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

