/** @file cSocketServer.h
    \author Christophe Romain
    \date 2005
    \brief Implémentation d'un serveur sur socket tcp ou udp
*/

#ifndef SYNTHESE_CSOCKET_H
#define SYNTHESE_CSOCKET_H

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

/** @defgroup m00 00 Serveur socket tcp ou udp
    @{
*/

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
cSocket Socket;
Socket.Open(hostmask,port,proto);
Socket.Server();
while(1) {
    SOCKET Client = Socket.Accept();
    Socket.Read(Client,Buffer,Size,Timeout);
    Socket.Write(Client,Buffer,Size,Timeout);
    Socket.Close(Client);
}

Utilisation en mode client:
cSocket Socket;
Socket.Open(hostname,port,proto);
Socket.Connect();
Socket.Read(Client,Buffer,Size,Timeout);
Socket.Write(Client,Buffer,Size,Timeout);

*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef UNIX
#define closesocket(s) close(s)
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif
using
class cSocket
{
private:
    sockaddr_in _SockAddr;
    SOCKET _Socket;

protected:
    void Name(const char *__HostName, const int __PortNumber);
    void Socket(const char *__ProtoName);
    
public:
    cSocket();
    ~cSocket();
    void Close();
    void Close(SOCKET __Socket);
    void Open(const char *__HostName, const int __PortNumber, const char *__ProtoName);
    void Server();
    SOCKET Accept();
    SOCKET Connect();
    int Write(const char* __Buffer, const int __Size, const int __Timeout);
    int Write(SOCKET __Socket, const char* __Buffer, const int __Size, const int __Timeout);
    int Read(char* __Buffer, const int __Size, const int __Timeout);
    int Read(SOCKET __Socket, char* __Buffer, const int __Size, const int __Timeout);
};

#endif
