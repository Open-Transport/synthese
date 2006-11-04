/** 34_departures_table module wide definitions
	@file 34_departures_table/module.h
*/

#ifndef SYNTHESE_DEPARTURES_TABLE_MODULE_H
#define SYNTHESE_DEPARTURES_TABLE_MODULE_H

namespace synthese
{

	/** @defgroup m34 34 Departures/Arrivals tables

		<h3>Architecture cible</h3>

		Un système de téléaffichage basé sur SYNTHESE repose sur les principes suivants :
		- la base de données de SYNTHESE constitue le point de convergence des diverses sources de données. Dans le cadre du téléaffichage, les sources majeures sont : des sources en saisie manuelle, des sources théoriques en import, et des sources de modification à chaud, manuelles (saisie de messages) ou automatique (import événements SAE)
		- le téléaffichage est un service du système SYNTHESE comme un autre : il exploite la base de données à chaud et fabrique des objets résultats (une liste de départs ou d'arrivée) qui sont transmis au module d'interface, chargé de les diffuser selon un langage défini
		- le module d'interface permet de définir le format des sorties sous forme de données d'entrée, dans une grande liberté de choix. Les formats de sortie peuvent être définis en HTML pour intégration à une charte graphique, en XML pour communiquer avec d'autres applications, ou encore selon un langage propriétaire pour piloter des dispositifs de téléaffichage matériels

		Le schéma suivant représente l'architecture cible d'un système de téléaffichage basé sur la solution SYNTHESE :

		@image latex 1195515_1_000.eps "Architecture cible d'un système de téléaffichage SYNTHESE" height=10cm
		@image html 1195515.2.000.png

		<h3>Structure des pages d'administration liées au module tableaux de départs et d'arrivée</h3>

		<tt>C/E/S</tt> = Création / Edition / Suppression

		@dot
		digraph {
			size="6,12"
			rankdir=LR
			node [shape=rectangle style=filled color="red" fillcolor="gold"]
	
			Login
			Accueil [URL="\ref synthese::interfaces::HomeAdmin"]
			Messages

			Login -> Accueil
				Accueil -> Messages
					Messages -> Bibliotheque
				Accueil -> Utilisateurs
					Utilisateurs -> Profils
				Accueil -> Emplacements
				Accueil -> Afficheurs
					Afficheurs -> "Types d'afficheurs";
				Accueil -> Journal;

			Messages -> "C/E/S Diffusion message";
			Messages -> "C/E/S Diffusion scenario de message";

			Bibliotheque -> "C/E/S Modele de message complementaire";
			Bibliotheque -> "C/E/S Modele de message prioritaire";
			Bibliotheque -> "C/E/S Scenario";

			Utilisateurs -> "C/E/S utilisateur";
			
			Profils -> "C/E/S profil";
			
			Emplacements -> "Edition emplacement";

			Afficheurs -> "C/E/S afficheur";
			Afficheurs -> "Simulation afficheur";
			Afficheurs -> "Supervision afficheur";

			Journal -> "Securite";
			Journal -> "Administration tableaux departs";
			Journal -> "Supervision tableaux departs";
			Journal -> "Administration bibliotheque messages";
			Journal -> "Diffusion de messages";
			Journal -> "Administration donnees reseau";


		}
		@enddot

		@{
	*/

	/** 34_departures_table namespace */
	namespace departurestable
	{


	}

	/** @} */

}

#endif
