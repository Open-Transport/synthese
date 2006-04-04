#ifndef SYNTHESE_TCP_TCPSERVICE_H
#define SYNTHESE_TCP_TCPSERVICE_H

#include <map>

#include "module.h"

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

    Socket* _socket;

private:

    TcpService (int portNumber,
		bool tcpProtocol = true);

    ~TcpService ();

public:

    
    /** Opens a new TCP service on the given port.
	If a service is already listening on the given port,
	it is returned.
	@return A service listening on the given port.
     */
    static TcpService* openService (int portNumber);
    
    /** Closes the TCP service listening on the given port.
     * All the active connection to this service are closed.
     */
    static void closeService (int portNumber);


    /** Waits for incoming TCP connection (blocking).
	@return The TCP server socket.
     */
    TcpServerSocket& acceptConnection () throw (SocketException);

    
    /** Closes an existing TCP connection
       @param The socket id.
     */
    void closeConnection (TcpServerSocket& socket) throw (SocketException);


    /** Gets active connection count on this service.
     */
    int getConnectionCount () const;
    

private:

    void initialize () throw (SocketException);


    friend class TcpServerSocket;

};




}
}
#endif
