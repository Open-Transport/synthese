/** 92_rs485_client module wide definitions
	@file 92_rs485_client/module.h
*/

#ifndef SYNTHESE_RS485_CLIENT_MODULE_H
#define SYNTHESE_RS485_CLIENT_MODULE_H

namespace synthese
{

	/** @defgroup m92 92 RS485 Client
		@ingroup m9

		Le client RS485 assure en service continu les op�rations suivantes :
			- demande du contenu au serveur, en sp�cifiant la cl� de l'afficheur, ce qui d�termine � la fois le protocole et le contenu
			- stockage du contenu re�u
			- transmission du contenu au port RS485
			- attente minute suivante

		Lorsque l'architecture mat�rielle le n�cessite, le client RS485 doit g�rer l'alimentation de plusieurs dispositifs branch�s en parall�le sur le m�me port. La boucle de fonctionnement est alors la suivante :
			- demande des contenus au serveur, en sp�cifiant la cl� de chaque afficheur
			- stockage des contenus re�us
			- transmission des contenus au port RS485, afficheur par afficheur
			- attente minute suivante

		@image html 1195572.2.000.png
		@image latex 1195572_1_000.eps "Sch�ma d'architecture cible d'une installation RS485" height=10cm

		@{
	*/

	/** 92 rs485_client namespace */
	namespace client
	{
        // mettre ici tout l'aspect client synthese tcp
        namespace rs485
        {
            // mettre ici le code rs485
        }
	}

	/** @} */

}

#endif


