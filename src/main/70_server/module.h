/** 70_server module wide definitions
	@file module.h
*/

#ifndef SYNTHESE_SERVER_MODULE_H
#define SYNTHESE_SERVER_MODULE_H

namespace synthese
{

	/** @defgroup m70 70 SYNTHESE server
		
		Pour pouvoir ï¿½tre appelï¿½ par plusieurs applications, le serveur SYNTHESE est un serveur TCP autonome, tout comme MySQL par exemple. La couche TCP est basée sur le module @ref m00 .

		Pour l'interroger, des modules clients TCP sont dï¿½veloppï¿½s dans les applications clientes :
			- Les clients autonomes : @ref m71, @ref m71, @ref m73
			- Un client TCP intï¿½grï¿½ au rï¿½fï¿½rentiel rï¿½seau (voir projet Rï¿½fï¿½rentiel rï¿½seau), permettant l'affichage cartographiques de donnï¿½es inconnues de SYNTHESE

		@section m00carto Dispositions particuliï¿½res fonctionnalitï¿½s cartographiques

		L'utilisation des fonctionnalitï¿½s cartographiques permettent de fournir au sein de la requï¿½te des donnï¿½es complï¿½mentaires ï¿½ afficher. A titre indicatif, l'utilisation prï¿½vue des clients pour le tracï¿½ de carte est la suivante :

		<table class="tableau">
		<tr><th>Client</th><th>Fournisseur de donnï¿½es</th><th>Utilisateur de donnï¿½es globales</th><th>Type de sï¿½lection</th></tr>
		<tr><th>Rï¿½fï¿½rentiel rï¿½seau</th><td>Objets</td><td>Fonds	X, Y, ï¿½chelle</td></tr>
		<tr><th>Client CGI</th><td>NON</td><td>Fonds + Objets</td><td>X,Y, ï¿½chelle</td></tr>
		</table>

		@subsection archicarte Architecture interne serveur cartographique modules 3 Tracï¿½ de carte

		Chaque carte est obtenue par le passage d'une requï¿½te XML, composï¿½e de trois groupes d'ï¿½lï¿½ments :
		- Des donnï¿½es dont la durï¿½e de vie est propre ï¿½ la requï¿½te
		- Des instructions de sï¿½lection des donnï¿½es ï¿½ afficher
		- Des paramï¿½tres d'affichage de la carte dont le format de sortie

		L'interprï¿½tation d'une requï¿½te donne lieu, en fonction du format de sortie, ï¿½ la fabrication d'une carte vectorielle et/ou ï¿½ la fabrication d'une table de liens, en fonction des critï¿½res de sï¿½lection de donnï¿½es spï¿½cifiï¿½es dans la requï¿½te, et ï¿½ partir des donnï¿½es globales chargï¿½es dans le serveur, et des donnï¿½es spï¿½cifiques fournies dans la requï¿½te.

		La carte vectorielle est initialement codï¿½e selon le standard PostScript, puis peut ï¿½tre exportï¿½e sous divers formats grï¿½ce ï¿½ la bibliothï¿½que GhostScript (PDF, JPEG, etcï¿½), ou bien peut ï¿½tre directement enregistrï¿½e au format PS pour impression.

		La table de liens est stockï¿½e en mï¿½moire sur des structure de donnï¿½es simples (tableaux dynamiques) et est exportï¿½e selon divers formats (PDF, HTML, etc.)

		Le schï¿½ma d'architecture interne concernant les fonctionnalitï¿½s cartographiques est le suivant :

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
