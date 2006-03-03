
#ifdef LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <io.h>
#include <windows.h>
#endif

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <time.h>
#include <fstream>
#include <iostream>

#include "module.h"

#ifdef LINUX
#define Sleep(x) if(x>=1000) sleep(x/1000);
#endif

#ifdef LINUX
extern int errno;
#else
//#define errno WSAGetLastError()
#endif

using std::endl;


/** @addtogroup m72
    @{
*/

namespace synthese
{
namespace client
{
/** Message de bienvenue de synthese */
#define WELCOME_MSG "Welcome to SYNTHESE"
/** Taille maximale d'une requête ou d'un résultat de synthese */
#define MAX_QUERY_SIZE 4096
namespace rs485
{
/** Requete de base pour synthese */
#define QUERY_BASE "fonction=tdg&date=A&tb="



void rs485::init(char *comm)
{
#ifdef WIN32
    DCB dcb;
    HANDLE _com; // à mettre dans la classe

    _com = CreateFile(comm,
                    GENERIC_READ | GENERIC_WRITE,
                    0,    // must be opened with exclusive-access
                    NULL, // no security attributes
                    OPEN_EXISTING, // must use OPEN_EXISTING
                    0,    // not overlapped I/O
                    NULL  // hTemplate must be NULL for comm devices
                    );
    if (_com == INVALID_HANDLE_VALUE) 
        throw "create";
    if(!GetCommState(_com, &dcb))
        throw "getstate";
    dcb.BaudRate = CBR_9600;      // set the baud rate
    dcb.ByteSize = 8;             // data size, xmit, and rcv
    dcb.Parity = NOPARITY;        // parity bit
    dcb.StopBits = ONESTOPBIT;    // one stop bit
    if(!SetCommState(_com, &dcb))
        throw "setstate";
#endif
#ifdef LINUX
    // _com = open("/dev/ttyS?",O_RDWR);
    // setserial
#endif
} 

void rs485::write(char byte)
{
#ifdef WIN32
    TransmitCommChar(_com, byte);
#endif
#ifdef LINUX
    //write(_com, &byte, 1);
#endif
}

}
}
}

/** @} */



/*! \brief Point d'entrée du client
*/
int main(int argc, char* argv[])
{
    static char buffer[MAX_QUERY_SIZE];
    char *codes[MAX_CLIENTS];
    int outdate[MAX_CLIENTS];
    int nbclients;
    
#ifdef WIN32
    std::ofstream logfile("C:\\CLIENT_RS485.LOG", std::ios_base::app);
#endif
#ifdef LINUX
    std::ofstream logfile("/tmp/client_rs485.log", std::ios_base::app);
#endif

    /* get parameters */
    if(argc > MAX_CLIENTS+4 || argc < 4) exit(-1);
    memset(codes, MAX_CLIENTS, sizeof(char*));
    for(nbclients=0; nbclients<argc-4; nbclients++) {
        codes[nbclients] = argv[nbclients+4];
        outdate[nbclients] = 0;
    }
    useRS485 = (strncmp(argv[3],"none",4) != 0);
    if(useRS485)
    {
        try
        {
            rs485.init(argv[3]);
        }
        catch (const char *err)
        {
            logfile << "rs485 init error: " << err << endl;
            exit(-2);
        }
    }
    
    while(1)
    {
        time_t now;
        struct tm *hms;
        int stamp;
        time(&now);
        hms = localtime(&now);
        stamp = hms->tm_min;
        for(int client=0; client<nbclients; client++)
        {
            if(outdate[client] != stamp)
            {
                try
                {
                    sock.open(argv[1],argv[2]);
                    sock.read(buffer, sizeof(buffer), 2);
                }
                catch (const char *err)
                {
                    logfile << "connection init error: " << err << endl;
                    break;
                }
                if(strncmp(buffer, WELCOME_MSG, strlen(WELCOME_MSG)))
                {
                    logfile << "no synthese service" << endl;
                    break;
                }
                try
                {
                    sprintf(buffer, "%s%s\n", QUERY_BASE, codes[client]);
                    sock.write(buffer, strlen(buffer), 0);
                    sock.read(buffer, sizeof(buffer), 2);
                }
                catch (const char *err)
                {
                    logfile << "connection stream error: " << err << endl;
                    break;
                }
                time(&now);
                hms = localtime(&now);
                logfile << "Date: " << asctime(hms) << "Message: " << buffer << endl;
                if(useRS485)
                {
                    for(char *bufptr = buffer; *bufptr; bufptr++)
                        TransmitCommChar(hCom, *bufptr);
                    logfile << "Refresh OK" << endl;
                }
                outdate[client] = hms->tm_min;
                sock.close();
                Sleep(100);
            } 
        }
        Sleep(1000);
    }
}
