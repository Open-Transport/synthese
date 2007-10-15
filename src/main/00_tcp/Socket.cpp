/** @file Socket.cpp
    \author Christophe Romain
    \date 2005
*/

#include "Socket.h"
#include <string>
#include <iostream>



namespace synthese
{
namespace tcp
{



/** \brief Constructeur de la classe
    \author Christophe Romain
    \date 2005
*/
    Socket::Socket (bool nonBlocking, int backlogSize)
    : _nonBlocking (nonBlocking)
    , _backlogSize (backlogSize)
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
	     int portNumber, 
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
Socket::name(const char* hostName, int portNumber)
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
    int rc = 1;

    // Bind la socket avec le nom du service
    setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &result, sizeof(result));

    if (_nonBlocking)
    {
	// Set socket to non-blocking mode; we want a non-blocking accept
	// behaviour
	// ...

#ifdef UNIX
	rc = ioctl (_socket, FIONBIO, (const char*) &result);
#endif
#ifdef WIN32
	rc = ioctlsocket (_socket, FIONBIO, (unsigned long*) &result);
#endif

	if (rc < 0)
	{
	    throw "ioctl";
	}
    }


    result = bind(_socket, (const sockaddr*)& _sockAddr, sizeof(sockaddr));
    if(result == SOCKET_ERROR)
        throw "bind";

    // Demarre le service
    result = listen(_socket, _backlogSize);
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
    
    if (client == INVALID_SOCKET)
    {
	// if (errno != EWOULDBLOCK)
	//    std::cerr << errno;

	/* if (errno == EWOULDBLOCK)
	{
	    std::cerr << "Non-Blocking mode!" << std::endl;
	} */
/* #ifdef WIN32
        if(WSAGetLastError() != WSAEINTR) 
            throw "Accept";
#endif */
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
Socket::write(const char* buffer, int size, int timeout)
{
    return write(_socket, buffer, size, timeout);
}




int 
Socket::write(SOCKET socket, const char* buffer, int size, int timeout)
{
    // timeout in milliseconds.
    struct timeval tv = {0, timeout*1000};
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(socket, &fd);

    int err = select(socket+1, 0, &fd, 0, (timeout > 0) ? &tv : 0);
    if(err == 0)
    {
        throw "Socket send timeout";
    }
    


    int bytesSent;
    bytesSent = send (socket, buffer, size, /*0*/ MSG_NOSIGNAL );

    if(bytesSent == SOCKET_ERROR)
	throw "Send";

    // std::cerr << "(" << bytesSent << ")" << std::endl;
    return bytesSent;
}




/** \brief Lecture
    \author Christophe Romain
    \date 2005
*/
int 
Socket::read(char* buffer, int size, int timeout)
{
    return read(_socket, buffer, size, timeout);
}




int 
Socket::read(SOCKET socket, char* buffer, int size, int timeout)
{
    // timeout in milliseconds.
    struct timeval tv = {0, timeout*1000};
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(socket, &fd);

    if(select(socket+1, &fd, 0, 0, (timeout > 0) ? &tv : 0) == 0)
    {
        throw "Socket recv timeout";
    }

    int bytesReceived;
    memset(buffer, 0, size);

    bytesReceived = recv(socket, buffer, size, 0);

    if(bytesReceived == SOCKET_ERROR)
	throw "Receive";

    return bytesReceived;
}



}
}



