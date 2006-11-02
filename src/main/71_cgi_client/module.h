/** 71_cgi_client module wide definitions
	@file 71_cgi_client/module.h
*/

#ifndef SYNTHESE_CGI_CLIENT_MODULE_H
#define SYNTHESE_CGI_CLIENT_MODULE_H

namespace synthese
{

	/** @defgroup m71 71 CGI Client

		Le client CGI est destiné à l'usage de SYNTHESE dans le cadre d'un site web. Il permet au serveur web de commander à SYNTHESE des pages web contenant les résultats de requêtes. L'interface définit le langage de sortie des pages, qui est ainsi indéfini. En règle général, le client CGI est utilisé pour obtenir des pages HTML ou XML.

		L'architecture cible de l'intégration des services SYNTHESE à un site web est résumée par le schéma suivant :

		@image latex 1195561.1.000.eps "Architecture cible de l'intégration de SYNTHESE à un site web" height=10cm
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

