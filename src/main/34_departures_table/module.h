/** 34_departures_table module wide definitions
	@file 34_departures_table/module.h
*/

#ifndef SYNTHESE_DEPARTURES_TABLE_MODULE_H
#define SYNTHESE_DEPARTURES_TABLE_MODULE_H

namespace synthese
{

	/** @defgroup m34 34 Departures/Arrivals tables

		@subsection m34_target_structure Architecture fonctionnelle cible

		Un syst�me de t�l�affichage bas� sur SYNTHESE repose sur les principes suivants :
			- la base de donn�es de SYNTHESE constitue le point de convergence des diverses sources de donn�es. Dans le cadre du t�l�affichage, les sources majeures sont : 
				- les sources dites <i>th�oriques</i> : elles proviennent de fichiers mis � jour par saisie manuelle ou par import informatique
				- les sources dites <i>temps r�el</i> : saisies manuelles dans la console d'administration (ex : saisie de messages...) ou r�ceptions de donn�es transmises automatiquement (ex : contr�les de supervision issues du client de supervision, import modifications horaires SAE...)
			- le t�l�affichage est un service du syst�me SYNTHESE comme un autre : il exploite la base de donn�es � chaud et fabrique des objets r�sultats (une liste de d�parts ou d'arriv�e) qui sont transmis au module d'interface, charg� de les diffuser selon un langage d�fini
			- le module d'interface permet de d�finir le format des sorties sous forme de donn�es d'entr�e, dans une grande libert� de choix. Les formats de sortie peuvent �tre d�finis en HTML pour int�gration � une charte graphique, en XML pour communiquer avec d'autres applications, ou encore selon un langage propri�taire pour piloter des dispositifs de t�l�affichage mat�riels

		Le sch�ma suivant repr�sente l'architecture cible d'un syst�me de t�l�affichage bas� sur la solution SYNTHESE :

		@image latex 1195515_1_000.eps "Architecture cible d'un syst�me de t�l�affichage SYNTHESE" height=10cm
		@image html 1195515.2.000.png


		@{
	*/

	/** 34_departures_table namespace */
	namespace departurestable
	{


	}

	/** @} */

}

#endif
