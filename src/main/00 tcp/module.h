/** 00_tcp module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_TCP_MODULE_H
#define SYNTHESE_TCP_MODULE_H

namespace synthese
{

	/** @defgroup m00 00 TCP socket client server implementation
		@{
	*/ /**
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

	/** 00_tcp namespace */
	namespace tcp
	{


	}

	/** @} */

}

#endif
