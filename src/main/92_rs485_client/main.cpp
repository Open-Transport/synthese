
/*
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
#include <sstream>


using std::endl;



static SOCKET sock;

#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFF
#endif


// Taille maximale d'une requête ou d'un résultat de synthese 
#define MAX_QUERY_SIZE 4096

// Nombre maximal de postes clients 
#define MAX_CLIENTS 128

// Requete de base pour synthese 
#define QUERY_BASE "fonction=tdg&date=A&tb="




int main(int argc, char* argv[])
{
    static char buffer[MAX_QUERY_SIZE];
    char *codes[MAX_CLIENTS], *server, *port, *comm;
    int outdate[MAX_CLIENTS];
    int nbclients;
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;

    // get parameters 
    if(argc > MAX_CLIENTS+4 || argc < 4) exit(-1);
    server = argv[1];
    port = argv[2];
    comm = argv[3];

    std::stringstream fileName;
    fileName << "CLIENT_RS485_" << comm << ".LOG";
    std::ofstream fichier(fileName.str().c_str(), std::ios_base::app);


    memset(codes, MAX_CLIENTS, sizeof(char*));
    for(nbclients=0; nbclients<argc-4; nbclients++) {
	codes[nbclients] = argv[nbclients+4];
	outdate[nbclients] = 0;
    }

    hCom = CreateFile( comm,
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
    
    printf("Serial port %s successfully reconfigured.\n", comm);

    
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
                //if(!SetCommState(hCom, &dcb))
                //    fichier << "erreur reinit port com" << endl;

                if(server_connect(server, port)>0)
                {
                    int pos = 0;
                    fd_set infd;
                    struct timeval timeout; 
                    FD_ZERO(&infd); // Winsock2.h. Link to Ws2_32.lib.
                    FD_SET(sock,&infd);
                    timeout.tv_sec = 2;
                    timeout.tv_usec = 0;
                    fichier << "server connection OK" << endl;
		    
		    
		    int read = 0;
		    sprintf(buffer, "%s%s%s\n", QUERY_BASE, codes[client], "&ipaddr=127.0.0.1");
		    send(sock, buffer, strlen(buffer), 0);
		    pos = 0;
		    memset(buffer, 0, sizeof(buffer));
		    FD_ZERO(&infd);
		    FD_SET(sock,&infd);
		    timeout.tv_sec = 2;
		    timeout.tv_usec = 0;

		    if(select(sock+1,&infd,NULL,NULL,&timeout)>0)
		    {
			do {
			    read = recv(sock, buffer+pos, sizeof(buffer)-pos, 0);
			    if (read > 0) pos += read;
			} while((read > 0) && (read != SOCKET_ERROR));
			
			if(read != SOCKET_ERROR)
			{
			    time(&now);
			    hms = localtime(&now);
			    fichier << "Date: " << asctime(hms) << "Message: " << buffer << endl;

			    for(char *bufptr = buffer; *bufptr; bufptr++)
			    {
				TransmitCommChar(hCom, *bufptr);
			    }

			    outdate[client] = hms->tm_min;
			    
			    char* buf = (char*) malloc(17);
			    DWORD readComm = 0;
			    ReadFile(hCom,buf,17,&readComm,NULL); // read is updated with the number of bytes read

			    free(buf);
			}
			
			
			fichier << "emission OK" << endl;
		    } 
		    else 
		    {
			fichier << "answer read error" << endl;
		    }
		} 
		else 
		{
		    fichier << "answer timeout" << endl;
		}
		
		server_disconnect();
	    } 
	    else 
	    {
		fichier << "can not connect to synthese" << endl;
	    }
	} 
	Sleep(100);
    }

    Sleep(1000);
}



*/


#include "00_tcp/TcpService.h"
#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/SocketException.h"

#include "01_util/Thread.h"
#include "01_util/Log.h"


#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/thread/thread.hpp>
#include <boost/iostreams/stream.hpp>


#define MAX_QUERY_SIZE 4096
#define MAX_CLIENTS 128


using namespace synthese::util;
using namespace synthese::tcp;



 
int main(int argc, char* argv[])
{

    char *codes[MAX_CLIENTS], *server, *port, *comm;
    int outdate[MAX_CLIENTS];
    int nbclients;
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;

    // get parameters 
    if(argc > MAX_CLIENTS+4 || argc < 4) exit(-1);
    server = argv[1];
    port = argv[2];
    comm = argv[3];

    memset(codes, MAX_CLIENTS, sizeof(char*));
    for(nbclients=0; nbclients<argc-4; nbclients++) {
	codes[nbclients] = argv[nbclients+4];
	outdate[nbclients] = 0;
    }

    hCom = CreateFile( comm,
		       GENERIC_READ | GENERIC_WRITE,
		       0,    // must be opened with exclusive-access
		       NULL, // no security attributes
		       OPEN_EXISTING, // must use OPEN_EXISTING
		       0,    // not overlapped I/O
		       NULL  // hTemplate must be NULL for comm devices
	);
    
    if (hCom == INVALID_HANDLE_VALUE) 
    {
	Log::GetInstance ().fatal ("Error while creating comm file " + Conversion::ToString (GetLastError()));
	exit(1);
    }
    
    // Build on the current configuration, and skip setting the size
    // of the input and output buffers with SetupComm.
    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess) 
    {
	// Handle the error.
	Log::GetInstance ().fatal ("Error while getting comm state " + Conversion::ToString (GetLastError()));
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
	Log::GetInstance ().fatal ("SetCommState failed with error " + Conversion::ToString (GetLastError()));
	exit(3);
    }
    
    // printf("Serial port %s successfully reconfigured.\n", comm);
    // printf("Starting probing for %d clients...\n", nbclients);

    char buf[4096];
    DWORD readComm = 0;

    while (true)
    {
        time_t now;
        struct tm *hms;
        int stamp;
        time(&now);
        hms = localtime(&now);
        stamp = hms->tm_min;


        for(int client=0; client<nbclients; client++)
        {
	    try 
	    {

		if(outdate[client] == stamp) continue;
		
		//if(!SetCommState(hCom, &dcb))
		//    fichier << "erreur reinit port com" << endl;
		
		Log::GetInstance ().info ("Connecting " + server + ":" + Conversion::ToString (port));
		
		// No timeout !
		int timeout = 2; // 2 seconds timeout
		TcpClientSocket clientSock (host, port, timeout);
		
		while (clientSock.isConnected () == false)
		{
		    clientSock.tryToConnect ();
		    Thread::Sleep (500);
		}
		
		// The client is connected.
		Log::GetInstance ().info ("Connected.");
		
		
		clientSock << "fonction=tdg&date=A&tb=" << codes[client] << "&ipaddr=127.0.0.1" << std::endl;
		clientSock.flush ();
		
		clientSock.read (buf, sizeof (buf), (char) 0);
		
		Log::GetInstance ().info ("Received message : " + buf);
		
		for (char* bufptr=buf ; *bufptr ; bufptr++)
		{
		    TransmitCommChar (hCom, *bufptr);
		}
		
		outdate[client] = hms->tm_min;
		
		// Read is updated with the number of bytes read
		ReadFile (hCom, buf, sizeof (buf), &readComm, NULL); 
		
		// We do nothing with response right now...
	    } 
	    catch (std::exception e) 
	    {
		Log::GetInstance ().error ("Error while updating client " + Conversion.ToString (client) + e.what ());
	    }
	    catch (...) 
	    {
		Log::GetInstance ().error ("Unexpected error  : " + e.what ());
	    }
	    
	    Thread::Sleep (100);
	}
	
	Thread::Sleep (1000);
	


/*


   struct TcpClientThread
    {
	std::string _replyToSend;

	TcpClientThread (const std::string& replyToSend) 
	    : _replyToSend (replyToSend)
	    {
	    }
	
	void operator()()
	    {
		TcpClientSocket clientSock ("localhost", 8899);
		
		while (clientSock.isConnected () == false)
		{
		    clientSock.tryToConnect ();
		    boost::thread::yield ();
		}
		// CPPUNIT_ASSERT (clientSock.isConnected ());
		
		// The client is connected.

		// Create commodity stream:
		boost::iostreams::stream<TcpClientSocket> cliSocketStream;
		cliSocketStream.open (clientSock);

		// Wait for a message...
		for (int i=0; i<256; ++i) std::cerr << cliSocketStream.get () << " ";
		std::cerr << std::endl;
		
		// std::string message;
		// cliSocketStream >> message;

		std::string expectedMessage ("MessageToClient");
		// CPPUNIT_ASSERT_EQUAL (expectedMessage, message);

		// Send a reply
		cliSocketStream << _replyToSend << std::endl;
		
		cliSocketStream.close ();
	    }
    };
    


    
    TcpClientThread clientThread ("MessageToServerFromClient0");
    boost::thread client (clientThread);
    
    TcpService* service = TcpService::openService (8899, true, false);
    
    try 
    {
	// Wait for client connection...
	  TcpServerSocket& socket = *service->acceptConnection ();

	  // CPPUNIT_ASSERT_EQUAL (1, service->getConnectionCount ());
	  
	  // Create commodity stream:
	  boost::iostreams::stream<TcpServerSocket> srvSocketStream (socket);

	  char buf[256];
	  for (int i=1; i<=255; ++i) buf[i] = (char) i;
	  std::string s (buf);

	  // Send a message to client:
	  for (int i=0; i<=255; ++i) srvSocketStream.write (s.c_str (), 256);
	  

	  // Wait for a reply:
	  std::string reply;
	  srvSocketStream >> reply;

	  std::string expectedReply ("MessageToServerFromClient0");
	  // CPPUNIT_ASSERT_EQUAL (expectedReply, reply);

	  
	  srvSocketStream.close ();

	  service->closeConnection (&socket);
	  // CPPUNIT_ASSERT_EQUAL (0, service->getConnectionCount ());
      } 
      catch (SocketException& se)
      {
	  // std::cerr << se.what () << std::endl;
	  // CPPUNIT_ASSERT (false);
      }
      
      client.join ();


      // TcpService::closeService (8899);
      */
  } 


