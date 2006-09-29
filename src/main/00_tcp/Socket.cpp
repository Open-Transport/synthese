/** @file Socket.cpp
    \author Christophe Romain
    \date 2005
*/

#include "Socket.h"
#include <string.h>



namespace synthese
{
namespace tcp
{



/** \brief Constructeur de la classe
    \author Christophe Romain
    \date 2005
*/
Socket::Socket()
{
#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
        throw "WSAStartup";
#endif
    _socket = 0;
}

/** \brief Destructeur de la classe
    \author Christophe Romain
    \date 2005
*/
Socket::~Socket()
{
    closeSocket();
#ifdef WIN32
    WSACleanup();
#endif
}

/** \brief Fermeture de la connection
    \author Christophe Romain
    \date 2005
*/
void 
Socket::closeSocket()
{
    if(_socket) closesocket(_socket);
}


void 
Socket::closeSocket (SOCKET socket)
{
    if(socket) closesocket(socket);
}



/** \brief Ouverture de la connection
    \author Christophe Romain
    \date 2005
*/
void 
Socket::open(const char* hostName, 
	     const int portNumber, 
	     const char* protoName)
{
    name(hostName, portNumber);
    initializeSocket(protoName);
}



/** \brief Nommage de la connection r�seau
    \author Christophe Romain
    \date 2005
*/
void 
Socket::name(const char* hostName, const int portNumber)
{
    _sockAddr.sin_family = AF_INET;
    _sockAddr.sin_port = htons(portNumber);


    if(strcmp(hostName, "*") == 0)
    {
        // Nommage pour toutes les adresses
        _sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else if(strspn(hostName, "0123456789.") == strlen(hostName))
    {
        // Nommage par adresse IP de la machine
        _sockAddr.sin_addr.s_addr = inet_addr(hostName);
    }
    else
    {
        // Nommage par nom de machine
        struct hostent* resolv;
        resolv = gethostbyname(hostName);

        if(resolv == 0) throw "gethostbyname";

        _sockAddr.sin_addr.s_addr = *((unsigned long *) resolv->h_addr_list[0]);
    }
}





/** \brief Initialisation de la socket
    \author Christophe Romain
    \date 2005
*/
void 
Socket::initializeSocket(const char* protoName)
{
    struct protoent* protocol = getprotobyname(protoName);;
    
    if(protocol == 0) throw "getprotobyname";

    // Initialise la socket
    //type = strcmp(transport, "udp")? SOCK_STREAM : SOCK_DGRAM;
    _socket = socket(AF_INET, SOCK_STREAM, protocol->p_proto);
    if(_socket == INVALID_SOCKET) throw "socket";
}



/** \brief Initialisation du mode serveur : �coute sur le port
    \author Christophe Romain
    \date 2005
*/
void 
Socket::server()
{
    int result = 1;

    // Bind la socket avec le nom du service
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &result, sizeof(result));

    result = bind(_socket, (const sockaddr*)& _sockAddr, sizeof(sockaddr));
    if(result == SOCKET_ERROR)
        throw "bind";

    // Demarre le service
    result = listen(_socket, 10);
    if (result == SOCKET_ERROR)
        throw "listen";
}



/** \brief Attente de connection d'un client
    \author Christophe Romain
    \date 2005
*/
SOCKET 
Socket::acceptConnection()
{
    SOCKET client;
    sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(sockaddr);
    client = accept(_socket, (sockaddr*) &clientAddr, &addrSize);
    if(client == INVALID_SOCKET)
    {
#ifdef WIN32
        if(WSAGetLastError() != WSAEINTR) 
            throw "Accept";
#endif
    }
    return client;
}




/** \brief Initialisation du mode client : connection au serveur
    \author Christophe Romain
    \date 2005
*/
SOCKET 
Socket::connectToServer()
{
    SOCKET server = connect(_socket, (const sockaddr*)&_sockAddr, sizeof(sockaddr));
    if(server == INVALID_SOCKET)
        throw "connect";
    return server;
}





/** \brief Ecriture
    \author Christophe Romain
    \date 2005
*/
int 
Socket::write(const char* buffer, const int size, const int timeout)
{
    return write(_socket, buffer, size, timeout);
}




int 
Socket::write(SOCKET socket, const char* buffer, const int size, const int timeout)
{
    int bytesSent;
    bytesSent = send (socket, buffer, size, 0);

    if(bytesSent == SOCKET_ERROR)
        throw "Send";

    return bytesSent;
}




/** \brief Lecture
    \author Christophe Romain
    \date 2005
*/
int 
Socket::read(char* buffer, const int size, const int timeout)
{
    return read(_socket, buffer, size, timeout);
}




int 
Socket::read(SOCKET socket, char* buffer, const int size, const int timeout)
{

    struct timeval tv = {timeout, 0};
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(socket, &fd);

    if(select(socket+1, &fd, 0, 0, &tv) == 0)
        throw "Receive timeout";

    int bytesReceived;
    memset(buffer, 0, size);

    bytesReceived = recv(socket, buffer, size, 0);

    if(bytesReceived == SOCKET_ERROR)
        throw "Receive";

    return bytesReceived;
}



}
}


