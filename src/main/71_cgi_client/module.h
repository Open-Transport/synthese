/** 71_cgi_client module wide definitions
	@file 71_cgi_client/module.h
*/

#ifndef SYNTHESE_CGI_CLIENT_MODULE_H
#define SYNTHESE_CGI_CLIENT_MODULE_H

namespace synthese
{

	/** @defgroup m71 71 CGI Client

		Le client CGI est destin� � l'usage de SYNTHESE dans le cadre d'un site web. Il permet au serveur web de commander � SYNTHESE des pages web contenant les r�sultats de requ�tes. L'interface d�finit le langage de sortie des pages, qui est ainsi ind�fini. En r�gle g�n�ral, le client CGI est utilis� pour obtenir des pages HTML ou XML.

		L'architecture cible de l'int�gration des services SYNTHESE � un site web est r�sum�e par le sch�ma suivant :

		@image latex 1195561.1.000.eps "Architecture cible de l'int�gration de SYNTHESE � un site web" height=10cm
		@image html 1195561.2.000.png

		@{
	*/

	#define DEF_PORT 3591
	#define DEF_PROTO "tcp"
	#define WELCOME_MSG "Welcome to SYNTHESE"


	/** 71 cgi_client namespace */
	namespace cgi_client
	{


	}

	/** @} */

}

#endif

