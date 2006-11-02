/** 34_departures_table module wide definitions
	@file 34_departures_table/module.h
*/

#ifndef SYNTHESE_DEPARTURES_TABLE_MODULE_H
#define SYNTHESE_DEPARTURES_TABLE_MODULE_H

namespace synthese
{

	/** @defgroup m34 34 Departures/Arrivals tables

		<h3>Architecture cible</h3>

		Un syst�me de t�l�affichage bas� sur SYNTHESE repose sur les principes suivants :
		- la base de donn�es de SYNTHESE constitue le point de convergence des diverses sources de donn�es. Dans le cadre du t�l�affichage, les sources majeures sont : des sources en saisie manuelle, des sources th�oriques en import, et des sources de modification � chaud, manuelles (saisie de messages) ou automatique (import �v�nements SAE)
		- le t�l�affichage est un service du syst�me SYNTHESE comme un autre : il exploite la base de donn�es � chaud et fabrique des objets r�sultats (une liste de d�parts ou d'arriv�e) qui sont transmis au module d'interface, charg� de les diffuser selon un langage d�fini
		- le module d'interface permet de d�finir le format des sorties sous forme de donn�es d'entr�e, dans une grande libert� de choix. Les formats de sortie peuvent �tre d�finis en HTML pour int�gration � une charte graphique, en XML pour communiquer avec d'autres applications, ou encore selon un langage propri�taire pour piloter des dispositifs de t�l�affichage mat�riels

		Le sch�ma suivant repr�sente l'architecture cible d'un syst�me de t�l�affichage bas� sur la solution SYNTHESE :

		@image latex 1195515.1.000.eps "Architecture cible d'un syst�me de t�l�affichage SYNTHESE" height=10cm
		@image html 1195515.2.000.png

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

		@{
	*/

	/** 34_departures_table namespace */
	namespace departurestable
	{


	}

	/** @} */

}

#endif
