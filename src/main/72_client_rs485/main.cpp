
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

/** @addtogroup m72
	@{
*/

extern int errno;

using namespace std;

namespace synthese
{
	namespace rs485_client
	{

		#ifdef LINUX
		#define SOCKET int
		void closesocket(SOCKET sock)
		{
			close(sock);
		}
		#endif

		static SOCKET sock;

		#ifndef INADDR_NONE
		#define INADDR_NONE 0xFFFFFFFF
		#endif

		#define WELCOME_MSG "Welcome to SYNTHESE"

		/** Taille maximale d'une requête ou d'un résultat de synthese */
		#define MAX_QUERY_SIZE 4096
		/** Nombre maximal de postes clients */
		#define MAX_CLIENTS 128
		/** Requete de base pour synthese */
		#define QUERY_BASE "fonction=tdg&date=A&tb="

		SOCKET connectsock(const char *host, const char *service, const char *transport)
		{
			struct hostent *phe;
			struct servent *pse;
			struct protoent *ppe;
			struct sockaddr_in sin;
			int type;
			SOCKET s;

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			type = strcmp(transport, "udp")? SOCK_STREAM : SOCK_DGRAM;

			/**** Look for host */
			if((phe = gethostbyname(host)) != NULL)
				memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
			else if((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
			{
				fprintf(stderr, "can't get %s host entry\n", host);
				return -1;
			}

			/**** Look for service */
			if((pse = getservbyname(service, transport)) != NULL)
				sin.sin_port = pse->s_port;
			else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
			{
				fprintf(stderr, "can't get %s service entry\n", service);
				return -1;
			} 

			/**** Look for protocol */
			if((ppe = getprotobyname(transport)) == NULL)
			{
				fprintf(stderr, "can't get %s protocol entry\n", transport);
				return -1;
			}

			/**** Create the socket */
			if((s = socket(PF_INET, type, ppe->p_proto)) < 0)
			//if((s = socket(PF_INET, type, 6)) < 0) // tcp->6
			{
				fprintf(stderr, "can't create socket: %s\n", strerror(errno));
				return -1;
			}

			/**** Connect to host */
			if(connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
			{
				closesocket(s);
				fprintf(stderr, "can't connect to %s.%s: %s\n", host, service, strerror(errno));
				return -1;
			}
			return s;
		}

		void server_disconnect()
		{
			if( sock >= 0 )
			{
				closesocket(sock);
			}
			sock = -1;
		}

		int server_connect(const char *host, const char *service)
		{
		#ifdef WIN32
			/* initialize win32 layer */
			WSADATA wsaData;
			if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
			{
				fprintf(stderr,"WSAStartup");
				return -1;
			}
		#endif
			sock = connectsock(host, service, "tcp");
			if(sock == SOCKET_ERROR)
			{
		#ifdef WIN32
			WSACleanup();
		#endif
			}
			return (int)sock;
		}


		/*! \brief Point d'entrée du client
		*/
		int main(int argc, char* argv[])
		{
			static char buffer[MAX_QUERY_SIZE];
			char *codes[MAX_CLIENTS], *server, *port;
			int outdate[MAX_CLIENTS];
			int nbread, nbclients;
			DCB dcb;
			HANDLE hCom;
			BOOL fSuccess;
			char *pcCommPort = "COM3";

			std::ofstream fichier("C:\\CLIENT_RS485.LOG", std::ios_base::app);

			/* get parameters */
			if(argc > MAX_CLIENTS+3 || argc < 3) exit(-1);
			server = argv[1];
			port = argv[2];
			memset(codes, MAX_CLIENTS, sizeof(char*));
			for(nbclients=0; nbclients<argc-3; nbclients++) {
				codes[nbclients] = argv[nbclients+3];
				outdate[nbclients] = 0;
			}

			hCom = CreateFile( pcCommPort,
							GENERIC_READ | GENERIC_WRITE,
							0,    // must be opened with exclusive-access
							NULL, // no security attributes
							OPEN_EXISTING, // must use OPEN_EXISTING
							0,    // not overlapped I/O
							NULL  // hTemplate must be NULL for comm devices
							);
			if (hCom == INVALID_HANDLE_VALUE) 
			{
				fprintf(stderr, "CreateFile failed with error %d.\n", GetLastError());
				exit(1);
			}
			// Build on the current configuration, and skip setting the size
			// of the input and output buffers with SetupComm.
			fSuccess = GetCommState(hCom, &dcb);
			if (!fSuccess) 
			{
				// Handle the error.
				fprintf(stderr, "GetCommState failed with error %d.\n", GetLastError());
				exit(2);
			}
			// Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.
			dcb.BaudRate = CBR_9600;      // set the baud rate
			dcb.ByteSize = 8;             // data size, xmit, and rcv
			dcb.Parity = NOPARITY;      // parity bit
			dcb.StopBits = ONESTOPBIT;    // one stop bit
			fSuccess = SetCommState(hCom, &dcb);
			if (!fSuccess) 
			{
				// Handle the error.
				fprintf(stderr, "SetCommState failed with error %d.\n", GetLastError());
				exit(3);
			}
			printf("Serial port %s successfully reconfigured.\n", pcCommPort);
			printf("Starting probing for %d clients...\n", nbclients);

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
						if(!SetCommState(hCom, &dcb))
							fichier << "erreur reinit port com" << endl;
						if(server_connect(server, port)!=-1)
						{
							int pos = 0;
							memset(buffer, 0, sizeof(buffer));
							do {
								int read = recv(sock, buffer+pos, sizeof(buffer)-pos, 0);
								if(read > 0) pos += read;
							} while(*(buffer+pos-1) != '\r' && *(buffer+pos-1) != '\n');
							if( strncmp(buffer, WELCOME_MSG, strlen(WELCOME_MSG)) )
							{
								fichier << "no synthese server found" << endl;
								server_disconnect();
							} else {
								sprintf(buffer, "%s%s\n", QUERY_BASE, codes[client]);
								send(sock, buffer, strlen(buffer), 0);
								//fprintf(stdout, buffer);
								nbread = 0;
								pos = 0;
								memset(buffer, 0, sizeof(buffer));
								do {
									nbread = recv(sock, buffer+pos, sizeof(buffer)-pos, 0);
									if (nbread > 0) pos += nbread;
								} while(nbread > 0);
								//printf("sending %s\n",buffer);
								time_t heure;
								struct tm* timeinfo;
								time(&heure);
								timeinfo = localtime(&heure);
								fichier << "Date: " << asctime(timeinfo) << "Message: " << buffer << std::endl;
								for(char *bufptr = buffer; *bufptr; bufptr++)
									TransmitCommChar(hCom, *bufptr);
								server_disconnect();
								outdate[client] = stamp;
							}
						} else {
							fichier << "can not connect to synthese" << endl;
						}
					} 
				}

		#ifdef WIN32
			Sleep(1000);
		#endif
		#ifdef LINUX
			sleep(1);
		#endif

			}
		}
	}
}



/** @} */
