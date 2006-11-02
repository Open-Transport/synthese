/** 34_departures_table module wide definitions
	@file 34_departures_table/module.h
*/

#ifndef SYNTHESE_DEPARTURES_TABLE_MODULE_H
#define SYNTHESE_DEPARTURES_TABLE_MODULE_H

namespace synthese
{

	/** @defgroup m34 34 Departures/Arrivals tables

		@{
	*/

	/**

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

	*/

	/** 34_departures_table namespace */
	namespace departurestable
	{


	}

	/** @} */

}

#endif
