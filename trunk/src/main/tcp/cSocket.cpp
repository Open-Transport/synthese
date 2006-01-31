/** @file cSocket.cpp
    \author Christophe Romain
    \date 2005
    \brief Impl�mentation d'un serveur sur socket tcp ou udp
*/

#include "cSocket.h"
#include <string.h>


/** \brief Constructeur de la classe
    \author Christophe Romain
    \date 2005
*/
cSocket::cSocket()
{
#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
        throw "WSAStartup";
#endif
    _Socket = 0;
}

/** \brief Destructeur de la classe
    \author Christophe Romain
    \date 2005
*/
cSocket::~cSocket()
{
    Close();
#ifdef WIN32
    WSACleanup();
#endif
}

/** \brief Fermeture de la connection
    \author Christophe Romain
    \date 2005
*/
void cSocket::Close()
{
    if(_Socket) closesocket(_Socket);
}
void cSocket::Close(SOCKET __Socket)
{
    if(__Socket) closesocket(__Socket);
}

/** \brief Ouverture de la connection
    \author Christophe Romain
    \date 2005
*/
void cSocket::Open(const char *__HostName, const int __PortNumber, const char *__ProtoName)
{
    Name(__HostName, __PortNumber);
    Socket(__ProtoName);
}

/** \brief Nommage de la connection r�seau
    \author Christophe Romain
    \date 2005
*/
void cSocket::Name(const char *__HostName, const int __PortNumber)
{
    _SockAddr.sin_family = AF_INET;
    _SockAddr.sin_port = htons(__PortNumber);
    if(strcmp(__HostName, "*") == 0)
    {
        // Nommage pour toutes les adresses
        _SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    } 
    else if(strspn(__HostName, "0123456789.") == strlen(__HostName))
    {
        // Nommage par adresse IP de la machine
        _SockAddr.sin_addr.s_addr = inet_addr(__HostName);
    }
    else
    {
        // Nommage par nom de machine
        struct hostent *__Resolv;
        __Resolv = gethostbyname(__HostName);
        if(__Resolv == NULL) throw "gethostbyname";
        _SockAddr.sin_addr.s_addr = *((unsigned long *) __Resolv->h_addr_list[0]);
    }
}

/** \brief Initialisation de la socket
    \author Christophe Romain
    \date 2005
*/
void cSocket::Socket(const char *__ProtoName)
{
    struct protoent *__Protocol;
    
    // D�fini le protocole
    __Protocol = getprotobyname(__ProtoName);
    if(__Protocol == NULL) throw "getprotobyname";

    // Initialise la socket
    //type = strcmp(transport, "udp")? SOCK_STREAM : SOCK_DGRAM;
    _Socket = socket(AF_INET, SOCK_STREAM, __Protocol->p_proto);
    if(_Socket == INVALID_SOCKET) throw "socket";
}

/** \brief Initialisation du mode serveur : �coute sur le port
    \author Christophe Romain
    \date 2005
*/
void cSocket::Server()
{
    int __Result = 1;
    // Bind la socket avec le nom du service
    setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &__Result, sizeof(__Result));
    __Result = bind(_Socket, (const sockaddr*)&_SockAddr, sizeof(sockaddr));
    if(__Result == SOCKET_ERROR)
        throw "bind";

    // D�marre le service
    __Result = listen(_Socket, 10);
    if(__Result == SOCKET_ERROR)
        throw "listen";
}

/** \brief Attente de connection d'un client
    \author Christophe Romain
    \date 2005
*/
SOCKET cSocket::Accept()
{
    SOCKET __Client;
    sockaddr_in __ClientAddr;
    socklen_t __AddrSize = sizeof(sockaddr);
    __Client = accept(_Socket, (sockaddr*)&__ClientAddr, &__AddrSize);
    if(__Client == INVALID_SOCKET)
    {
#ifdef WIN32
        if(WSAGetLastError() != WSAEINTR) 
            throw "Accept";
#endif
    }
    return __Client;
}

/** \brief Initialisation du mode client : connection au serveur
    \author Christophe Romain
    \date 2005
*/
SOCKET cSocket::Connect()
{
    SOCKET __Server;
    __Server = connect(_Socket, (const sockaddr*)&_SockAddr, sizeof(sockaddr));
    if(__Server == INVALID_SOCKET)
        throw "connect";
    return __Server;
}

/** \brief Ecriture
    \author Christophe Romain
    \date 2005
int cSocket::Write(const char* __Buffer, const int __Size, const int __Timeout)
{
    int __BytesSent = 0;
    int __BytesThisTime;
    const char* __Cursor = __Buffer;
    do {
        __BytesThisTime = Write(_Peer, __Cursor, __Size - __BytesSent, __Timeout);
        __BytesSent += __BytesThisTime;
        __Cursor += __BytesThisTime;
    } while(__BytesSent < __Size);
    return __BytesSent;
}
*/

/** \brief Ecriture
    \author Christophe Romain
    \date 2005
*/
int cSocket::Write(const char* __Buffer, const int __Size, const int __Timeout)
{
    return Write(_Socket, __Buffer, __Size, __Timeout);
}
int cSocket::Write(SOCKET __Socket, const char* __Buffer, const int __Size, const int __Timeout)
{
    //assert(_Socket != NULL);
    int __BytesSent;
    __BytesSent = send(__Socket, __Buffer, __Size, 0);
    if(__BytesSent == SOCKET_ERROR)
        throw "Send";
    return __BytesSent;
}

/** \brief Lecture
    \author Christophe Romain
    \date 2005
int cSocket::Read(char* __Buffer, const int __Size, const int __Timeout)
{
    int __BytesRead = 0;
    int __BytesThisTime;
    char* __Cursor = __Buffer;
    do {
        __BytesThisTime = Read(_Peer, __Cursor, __Size - __BytesRead, __Timeout);
        __BytesRead += __BytesThisTime;
        __Cursor += __BytesThisTime;
    } while(__BytesRead < __Size);
    return __BytesRead;
}
*/

/** \brief Lecture
    \author Christophe Romain
    \date 2005
*/
int cSocket::Read(char* __Buffer, const int __Size, const int __Timeout)
{
    return Read(_Socket, __Buffer, __Size, __Timeout);
}
int cSocket::Read(SOCKET __Socket, char* __Buffer, const int __Size, const int __Timeout)
{
    //assert(_Socket != NULL);
    struct timeval tv = {__Timeout, 0};
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(__Socket, &fd);
    if(select(__Socket+1, &fd, NULL, NULL, &tv) == 0)
        throw "Receive timeout";
    int __BytesReceived;
    memset(__Buffer, 0, __Size);
    __BytesReceived = recv(__Socket, __Buffer, __Size, 0);
    if(__BytesReceived == SOCKET_ERROR)
        throw "Receive";
    return __BytesReceived;
}

