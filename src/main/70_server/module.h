/** 70_server module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_SERVER_MODULE_H
#define SYNTHESE_SERVER_MODULE_H

namespace synthese
{

	/** @defgroup m70 70 SYNTHESE server
		
		Pour pouvoir �tre appel� par plusieurs applications, le serveur SYNTHESE est un serveur TCP autonome, tout comme MySQL par exemple. La couche TCP est bas�e sur le module @ref m00 .

		Pour l'interroger, des modules clients TCP sont d�velopp�s dans les applications clientes :
			- Les clients autonomes : @ref m71, @ref m71, @ref m73
			- Un client TCP int�gr� au r�f�rentiel r�seau (voir projet R�f�rentiel r�seau), permettant l'affichage cartographiques de donn�es inconnues de SYNTHESE

		@section m00carto Dispositions particuli�res fonctionnalit�s cartographiques

		L'utilisation des fonctionnalit�s cartographiques permettent de fournir au sein de la requ�te des donn�es compl�mentaires � afficher. A titre indicatif, l'utilisation pr�vue des clients pour le trac� de carte est la suivante :

		<table class="tableau">
		<tr><th>Client</th><th>Fournisseur de donn�es</th><th>Utilisateur de donn�es globales</th><th>Type de s�lection</th></tr>
		<tr><th>R�f�rentiel r�seau</th><td>Objets</td><td>Fonds	X, Y, �chelle</td></tr>
		<tr><th>Client CGI</th><td>NON</td><td>Fonds + Objets</td><td>X,Y, �chelle</td></tr>
		</table>

		@subsection archicarte Architecture interne serveur cartographique modules 3 Trac� de carte

		Chaque carte est obtenue par le passage d'une requ�te XML, compos�e de trois groupes d'�l�ments :
		- Des donn�es dont la dur�e de vie est propre � la requ�te
		- Des instructions de s�lection des donn�es � afficher
		- Des param�tres d'affichage de la carte dont le format de sortie

		L'interpr�tation d'une requ�te donne lieu, en fonction du format de sortie, � la fabrication d'une carte vectorielle et/ou � la fabrication d'une table de liens, en fonction des crit�res de s�lection de donn�es sp�cifi�es dans la requ�te, et � partir des donn�es globales charg�es dans le serveur, et des donn�es sp�cifiques fournies dans la requ�te.

		La carte vectorielle est initialement cod�e selon le standard PostScript, puis peut �tre export�e sous divers formats gr�ce � la biblioth�que GhostScript (PDF, JPEG, etc�), ou bien peut �tre directement enregistr�e au format PS pour impression.

		La table de liens est stock�e en m�moire sur des structure de donn�es simples (tableaux dynamiques) et est export�e selon divers formats (PDF, HTML, etc.)

		Le sch�ma d'architecture interne concernant les fonctionnalit�s cartographiques est le suivant :

		@image html m3.png


		@{
	*/

	/** 70_server namespace */
	namespace server
	{


	}

	/** @} */

}

#endif
