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

	<h3>Structure des pages d'administration li�es au module tableaux de d�parts et d'arriv�e</h3>

	<tt>C/E/S</tt> = Cr�ation / Edition / Suppression

	@dot
	digraph {
		Login -> Accueil;
			Accueil -> Messages;
				Messages -> Biblioth�que;
			Accueil -> Utilisateurs;
				Utilisateurs -> Profils;
			Accueil -> Emplacements;
			Accueil -> Afficheurs;
				Afficheurs -> "Types d'afficheurs";
			Accueil -> Journal;

		Messages -> "C/E/S Diffusion message";
		Messages -> "C/E/S Diffusion sc�nario de message";

		Biblioth�que -> "C/E/S Mod�le de message compl�mentaire";
		Biblioth�que -> "C/E/S Mod�le de message prioritaire";
		Biblioth�que -> "C/E/S Sc�nario";

		Utilisateurs -> "Cr�ation utilisateur";
		Utilisateurs -> "Edition utilisateur";
		Utilisateurs -> "Suppression utilisateur";
		
		Profils -> "C/E/S profil";
		
		Emplacements -> "Edition emplacement";

		Afficheurs -> "C/E/S afficheur";
		Afficheurs -> "Simulation afficheur";
		Afficheurs -> "Supervision afficheur";

		Journal -> "S�curit�";
		Journal -> "Administration tableaux d�parts";
		Journal -> "Supervision tableaux d�parts";
		Journal -> "Administration biblioth�que messages";
		Journal -> "Diffusion de messages";
		Journal -> "Administration donn�es r�seau";


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
