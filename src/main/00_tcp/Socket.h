
/** Socket class header.
	@file Socket.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_TCP_SOCKET_H
#define SYNTHESE_TCP_SOCKET_H

#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

// #include <WinIoCtl.h>

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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
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

/** @} */


namespace synthese
{
	namespace tcp
	{

		/** Implémetation de la couche réseau TCP/IP
			@author Christophe Romain
			@date 2005

			Cette classe permet la mise en place d'un serveur tcp ou udp
			Elle assure aussi la fonction de client.

			La connection est surveillée par timer ce qui permet de remonter 
			des exceptions lors d'innactivité réseaux.
			Les exceptions sont également utilisées pour remonter les erreurs
			de connections ou de dialogue.

			Utilisation en mode serveur:
			@code
Socket socket;
socket.open(hostmask,port,proto);
socket.server();
while(1) {
	SOCKET client = socket.acceptConnection();
	socket.read(client, buffer, size, timeout);
	socket.write(client, buffer, size, timeout);
	socket.close(client);
}
			@endcode

			Utilisation en mode client:
			@code
Socket socket;
socket.open(hostname,port,proto);
socket.connectToServer();
socket.read(Client,Buffer,Size,Timeout);
socket.write(Client,Buffer,Size,Timeout);
			@endcode

			@ingroup m00
		*/
		class Socket
		{
		private:
			bool _nonBlocking;
			sockaddr_in _sockAddr;
			SOCKET _socket;

		protected:
			void name(const char* hostName, int portNumber);
			void initializeSocket(const char* protoName);
		    
		public:
			Socket (bool nonBlocking = true);
			~Socket ();
			void closeSocket();
			void closeSocket (SOCKET socket);
			void open(const char* hostName, 
				  int portNumber, 
				  const char* protoName);

			void server();
			SOCKET acceptConnection();
			SOCKET connectToServer();
			int write(const char* buffer, int size, int timeout);
			int write(SOCKET socket, const char* buffer, int size, int timeout);

			/** If timeout (in milliseconds) is not strictly positive this is a blocking read operation.
			 */
			int read(char* buffer, int size, int timeout_ms);

			int read(SOCKET socket, char* buffer, int size, int timeout_ms);
		};

	}
}

#endif
