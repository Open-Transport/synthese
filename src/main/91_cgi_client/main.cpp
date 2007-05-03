/** CGI Client main implementation.
	@file 91_cgi_client/main.cpp
	@ingroup m91
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "module.h"

#include "30_server/Request.h"
#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/SocketException.h"

#include <boost/iostreams/stream.hpp>


using std::cout;
using std::endl;

using namespace synthese::server;
using namespace synthese::tcp;


#define MAX_QUERY_SIZE 4096

int main(int argc, char **argv)
{
    // Response header 
    cout << "Cache-Control: no-cache\n";
    cout << "Content-type: text/html; charset=ISO-8859-1\n\n";

    // first command line argument = server name or ip address
    const char* server = "localhost";
    if (argc > 1) 
    {
	server = argv[1];
    }
    int port = DEF_PORT;
    if (argc > 2) 
    {
	port = atoi (argv[2]);
    }
  
    char *path, *soft, *method, *script, *ip, *qptr, *query = NULL;
    struct in_addr addr;
    static char buffer[MAX_QUERY_SIZE+16]; /* query + ip */
    static char welcome[32];
    int bufpos;

    // Récupèration de l'environnement CGI
    path = getenv("PATH");
    soft = getenv("SERVER_SOFTWARE");
    script = getenv("SCRIPT_NAME");
    method = getenv("REQUEST_METHOD");
    if(method) {
	if(*method == 'p' || *method == 'P')
	{
	    query = (char*) malloc((atoi(getenv("CONTENT_LENGTH"))+2) * sizeof(char));
	    fgets(query, atoi(getenv("CONTENT_LENGTH"))+1, stdin);
	}
	else
	    query = getenv("QUERY_STRING");
    } else {
	query = getenv("QUERY_STRING");
    }
    ip = getenv("REMOTE_ADDR");
    if (ip)
	addr.s_addr = inet_addr(getenv("REMOTE_ADDR"));

    // Erreur si pas de requête
    if( query == NULL )
    {
	cout << "HTTP/1.0 400 Bad Request\n\n";
	exit(1);
    }

    // Formattage de la requête
    memset(buffer, 0, MAX_QUERY_SIZE);
    bufpos = 0;
    qptr = query;
    while( *qptr && (qptr-query<MAX_QUERY_SIZE-1))
    {
	// Conversion des caractères de la forme %30
	if(*qptr == '%') 
	{
	    char hex[3];
	    memcpy(hex,qptr+1,2);
	    hex[2] = 0;
	    buffer[bufpos] = (unsigned char)strtol(hex, NULL, 16);
	    if(buffer[bufpos] == '~') buffer[bufpos] = '\''; 
	    bufpos++; qptr += 3;
	}
	// Conversion des caractères de la forme \\30
	else if(*qptr == '\\')
	{
	    char hex[3];
	    memcpy(hex,qptr+2,2);
	    hex[2] = 0;
	    buffer[bufpos] = (unsigned char)strtol(hex, NULL, 16);
	    bufpos++; qptr += 4;
	}
	// Conversion du '+' en ' ' 
	else if(*qptr == '+') 
	{
	    buffer[bufpos++] = ' ';
	    qptr++;
	}
	else if(*qptr)
	{
	    buffer[bufpos++] = *qptr;
	    qptr++;
	}
    }

    // Adding of the client IP address to the request
    strcat(buffer, Request::PARAMETER_SEPARATOR.c_str());
    strcat(buffer, Request::PARAMETER_IP.c_str());
    strcat(buffer, Request::PARAMETER_ASSIGNMENT.c_str());
    strcat(buffer, ip);
    // Adding of the client url prefix to the request
    strcat(buffer, Request::PARAMETER_SEPARATOR.c_str());
    strcat(buffer, Request::PARAMETER_CLIENT_URL.c_str());
    strcat(buffer, Request::PARAMETER_ASSIGNMENT.c_str());
    strcat(buffer, script);
    // Adding end of line to close the request
    strcat(buffer, "\r\n");
		
    // Init server connection
    int timeout = 1800;
    TcpClientSocket clientSock (server, port, timeout);
    boost::iostreams::stream<TcpClientSocket> cliSocketStream;

    try 
    {
	clientSock.tryToConnect ();
	
	if (clientSock.isConnected () == false)
	{
	    cout << "HTTP/1.0 503 Service Unavailable\n\n";
	    exit(2);
	}
	
	cliSocketStream.open (clientSock);
	
	// Send request to server
	cliSocketStream << buffer << std::flush;
	
	while (cliSocketStream.get (buffer, MAX_QUERY_SIZE))
	    cout << buffer;
	
	cliSocketStream.close ();
	exit(0);
    }
    catch (SocketException& e)
    {
	cliSocketStream.close ();
	cout << "HTTP/1.0 503 Service Unavailable: " << e.getMessage () << "\n\n";
	exit(2);
    }

}

