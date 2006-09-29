/** CGI Client main implementation.
	@file 71_cgi_client/main.cpp
	@ingroup m71
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "00_tcp/Socket.h"
#include "module.h"

using std::cout;
using std::endl;


namespace synthese
{
    namespace cgi_client
    {
    }
}

#define MAX_QUERY_SIZE 4096

#ifdef UNIX
	int main(int argc, char **argv)
#endif
#ifdef WIN32
	#include <io.h>
    int main(int argc, TCHAR* argv[], TCHAR* envp[])
#endif
{
    char *path, *soft, *method, *script, *ip, *qptr, *query = NULL;
    struct in_addr addr;
    static char buffer[MAX_QUERY_SIZE+16]; /* query + ip */
    static char welcome[32];
    int bufpos;
    synthese::tcp::Socket socket;

    // Récupèration de l'environnement CGI
    path = getenv("PATH");
    soft = getenv("SERVER_SOFTWARE");
    script = getenv("SCRIPT_NAME");
    method = getenv("REQUEST_METHOD");
    if(method) {
	if(*method == 'p' || *method == 'P')
	    read(0, query, atoi(getenv("CONTENT_LENGTH")));
	else
	    query = getenv("QUERY_STRING");
    } else {
	query = getenv("QUERY_STRING");
    }
    ip = getenv("REMOTE_ADDR");
    if(ip) addr.s_addr = inet_addr(getenv("REMOTE_ADDR"));

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
		    
    // Initialise la connection au serveur
    try
    {
	socket.open(DEF_SERV, DEF_PORT, DEF_PROTO);
	socket.connectToServer();
    }
    catch (const char *err)
    {
	socket.closeSocket();
	cout << "HTTP/1.0 503 Service Unavailable: " << err << "\n\n";
	exit(2);
    }
    // Vérification de l'identité du serveur
    try
    {
	socket.read(welcome, strlen(WELCOME_MSG), 10);
	if(strncmp(welcome, WELCOME_MSG, strlen(WELCOME_MSG)))
	    throw "no server";
    }
    catch (const char *err)
    {
	socket.closeSocket();
	cout << "HTTP/1.0 500 Internal Server Error\n\n";
	exit(3);
    }

    // Envoi de la requête au serveur
    socket.write(buffer, strlen(buffer), 0);

    // Retour de la réponse
    cout << "Cache-Control: no-cache\n";
    cout << "Content-type: text/html; charset=ISO-8859-1\n\n";
    while(socket.read(buffer, MAX_QUERY_SIZE, 10))
	cout << buffer;
    /** ATTENTION
	ici danger de fermer avant la fin
	si read=0 alors que calcul en cours ?
	-> ajouter une séquence de terminaison !
    */

    // Fin nominale
    socket.closeSocket();
    exit(0);
}
