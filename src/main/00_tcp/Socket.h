/** @file Socket.h
    \author Christophe Romain
    \date 2005
    \brief Implémentation d'un serveur sur socket tcp ou udp
*/

#ifndef SYNTHESE_TCP_SOCKET_H
#define SYNTHESE_TCP_SOCKET_H

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

/** ATTENTION
  Pour compiler sous MacOS X, il faut définir UNIX comme sous Linux.
  __APPLE_CC__ est défini implicitement.
*/
#ifdef __APPLE_CC__
#include <sys/types.h>
#include <machine/types.h>
#endif

#ifdef UNIX
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif


/** @addtogroup m00
    @{
*/


#ifdef UNIX
#define closesocket(s) close(s)
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif



namespace synthese
{
namespace tcp
{



/** Implémetation de la couche réseau TCP/IP
    \author Christophe Romain
    \date 2005

Cette classe permet la mise en place d'un serveur tcp ou udp
Elle assure aussi la fonction de client.

La connection est surveillée par timer ce qui permet de remonter 
des exceptions lors d'innactivité réseaux.
Les exceptions sont également utilisées pour remonter les erreurs
de connections ou de dialogue.

Utilisation en mode serveur:
Socket socket;
socket.open(hostmask,port,proto);
socket.server();
while(1) {
    SOCKET client = socket.acceptConnection();
    socket.read(client, buffer, size, timeout);
    socket.write(client, buffer, size, timeout);
    socket.close(client);
}

Utilisation en mode client:
Socket socket;
socket.open(hostname,port,proto);
socket.connectToServer();
socket.read(Client,Buffer,Size,Timeout);
socket.write(Client,Buffer,Size,Timeout);

*/
class Socket
{
private:
    sockaddr_in _sockAddr;
    SOCKET _socket;

protected:
    void name(const char* hostName, const int portNumber);
    void initializeSocket(const char* protoName);
    
public:
    Socket();
    ~Socket();
    void closeSocket();
    void closeSocket (SOCKET socket);
    void open(const char* hostName, 
	      const int portNumber, 
	      const char* protoName);

    void server();
    SOCKET acceptConnection();
    SOCKET connectToServer();
    int write(const char* buffer, const int size, const int timeout);
    int write(SOCKET socket, const char* buffer, const int size, const int timeout);
    int read(char* buffer, const int size, const int timeout);
    int read(SOCKET socket, char* buffer, const int size, const int timeout);
};





}
}

/** @} */


#endif

