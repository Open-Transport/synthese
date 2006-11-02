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

		@image latex 1195515.1.000.eps "Architecture cible d'un système de téléaffichage SYNTHESE" height=10cm
		@image html 1195515.2.000.png

		<h3>Structure des pages d'administration liées au module tableaux de départs et d'arrivée</h3>

		<tt>C/E/S</tt> = Création / Edition / Suppression

		@dot
		digraph {
			Login -> Accueil;
				Accueil -> Messages;
					Messages -> Bibliothèque;
				Accueil -> Utilisateurs;
					Utilisateurs -> Profils;
				Accueil -> Emplacements;
				Accueil -> Afficheurs;
					Afficheurs -> "Types d'afficheurs";
				Accueil -> Journal;

			Messages -> "C/E/S Diffusion message";
			Messages -> "C/E/S Diffusion scénario de message";

			Bibliothèque -> "C/E/S Modèle de message complémentaire";
			Bibliothèque -> "C/E/S Modèle de message prioritaire";
			Bibliothèque -> "C/E/S Scénario";

			Utilisateurs -> "Création utilisateur";
			Utilisateurs -> "Edition utilisateur";
			Utilisateurs -> "Suppression utilisateur";
			
			Profils -> "C/E/S profil";
			
			Emplacements -> "Edition emplacement";

			Afficheurs -> "C/E/S afficheur";
			Afficheurs -> "Simulation afficheur";
			Afficheurs -> "Supervision afficheur";

			Journal -> "Sécurité";
			Journal -> "Administration tableaux départs";
			Journal -> "Supervision tableaux départs";
			Journal -> "Administration bibliothèque messages";
			Journal -> "Diffusion de messages";
			Journal -> "Administration données réseau";


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
