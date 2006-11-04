/** 72_rs485_client module wide definitions
	@file 72_rs485_client/module.h
*/

#ifndef SYNTHESE_RS485_CLIENT_MODULE_H
#define SYNTHESE_RS485_CLIENT_MODULE_H

namespace synthese
{

	/** @defgroup m72 72 RS485 Client

		Le téléaffichage consiste en la diffusion de la liste des prochains départs aux arrêts, de manière dynamique.

		Plusieurs dispositifs peuvent être utilisés pour le téléaffichage :
			- ecran de type VGA connecté sur unité centrale ou terminal RDP
			- écran de type analogique, connecté sur un générateur de caractères (Velec...)
			- afficheurs de type LCD, pilotés par port RS485 (protocole Lumiplan)
			- afficheurs mécaniques, pilotés par port RS485 (protocole Velec)
			- etc.

		Hormis le premier cas assimilé à une utilisation web de SYNTHESE, il est nécessaire d'implanter un client TCP sur l'ordinateur relié aux dispositifs de téléaffichage, chargé de transmettre les pages résultat au travers du port RS485.

		Un "Client RS485" est donc nécessaire pour gérer l'ensemble des cas. Les différents protocoles sont implémentés sous forme d'interfaces, et la définition des contenus est faite dans des descriptions de tableaux, comme pour les interfaces HTML.

		Il assure en service continu les opérations suivantes :
			- demande du contenu au serveur, en spécifiant la clé de l'afficheur, ce qui détermine à la fois le protocole et le contenu
			- stockage du contenu reçu
			- transmission du contenu au port RS485
			- attente minute suivante

		Lorsque l'architecture matérielle le nécessite, le client RS485 doit gérer l'alimentation de plusieurs dispositifs branchés en parallèle sur le même port. La boucle de fonctionnement est alors la suivante :
			- demande des contenus au serveur, en spécifiant la clé de chaque afficheur
			- stockage des contenus reçus
			- transmission des contenus au port RS485, afficheur par afficheur
			- attente minute suivante

		@image html 1195572.2.000.png
		@image latex 1195572_1_000.eps "Schéma d'architecture cible d'une installation RS485" height=10cm

		@{
	*/

	/** 72 rs485_client namespace */
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

