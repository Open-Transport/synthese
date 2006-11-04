/** 72_rs485_client module wide definitions
	@file 72_rs485_client/module.h
*/

#ifndef SYNTHESE_RS485_CLIENT_MODULE_H
#define SYNTHESE_RS485_CLIENT_MODULE_H

namespace synthese
{

	/** @defgroup m72 72 RS485 Client

		Le t�l�affichage consiste en la diffusion de la liste des prochains d�parts aux arr�ts, de mani�re dynamique.

		Plusieurs dispositifs peuvent �tre utilis�s pour le t�l�affichage :
			- ecran de type VGA connect� sur unit� centrale ou terminal RDP
			- �cran de type analogique, connect� sur un g�n�rateur de caract�res (Velec...)
			- afficheurs de type LCD, pilot�s par port RS485 (protocole Lumiplan)
			- afficheurs m�caniques, pilot�s par port RS485 (protocole Velec)
			- etc.

		Hormis le premier cas assimil� � une utilisation web de SYNTHESE, il est n�cessaire d'implanter un client TCP sur l'ordinateur reli� aux dispositifs de t�l�affichage, charg� de transmettre les pages r�sultat au travers du port RS485.

		Un "Client RS485" est donc n�cessaire pour g�rer l'ensemble des cas. Les diff�rents protocoles sont impl�ment�s sous forme d'interfaces, et la d�finition des contenus est faite dans des descriptions de tableaux, comme pour les interfaces HTML.

		Il assure en service continu les op�rations suivantes :
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

