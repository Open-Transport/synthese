
/** TcpService class header.
	@file TcpService.h

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

#ifndef SYNTHESE_TCP_TCPSERVICE_H
#define SYNTHESE_TCP_TCPSERVICE_H

#include <map>

#include "SocketException.h"
#include "TcpServerSocket.h"

#include <boost/thread/mutex.hpp>


namespace synthese
{
namespace tcp
{
    class Socket;


/** TCP Service.

A TCP service is waiting for incoming connection and delivers socket ids.
Connection methods are thread safe.

@ingroup m00
*/

class TcpService 
{
public:

private:

    static std::map<int, TcpService*> _activeServices;

    mutable boost::mutex _serviceMutex; 

    std::map<int, TcpServerSocket*> _activeConnections;

    const int _portNumber;
    const std::string _protocol;
    const bool _nonBlocking;
    int _backlogSize;

    Socket* _socket;

private:

    TcpService (int portNumber,
		bool tcpProtocol = true,
		bool nonBlocking = true,
		int backlogSize = 20);

    ~TcpService ();

public:

    
    /** Opens a new TCP service on the given port.
	If a service is already listening on the given port,
	it is returned.
	@return A service listening on the given port.
     */
    static TcpService* openService (int portNumber,
	                            bool tcpProtocol = true,
				    bool nonBlocking = true,
				    int backlogSize = 20);
    
    /** Closes the TCP service listening on the given port.
     * All the active connection to this service are closed.
     */
    static void closeService (int portNumber);


    /** Waits for incoming TCP connection (non-blocking).
	@return The TCP server socket, or 0 if connection failed.
     */
    TcpServerSocket* acceptConnection () ;

    
    /** Closes an existing TCP connection
       @param socket The socket id.
     */
    void closeConnection (TcpServerSocket* socket);


    /** Gets active connection count on this service.
     */
    int getConnectionCount () const;
    
    int getPort () const { return _portNumber; }

private:

    void initialize () ;


    friend class TcpServerSocket;

};




}
}
#endif

