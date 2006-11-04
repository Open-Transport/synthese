
#ifndef SYNTHESE_DISPLAY_SEARCH_ADMIN_H
#define SYNTHESE_DISPLAY_SEARCH_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de gestion du parc mat�riel d'afficheurs.
			@ingroup m34

			@image html cap_admin_displays.png
			@image latex cap_admin_displays.png "Maquette de l'�cran de gestion des afficheurs" width=14cm

			<i>Titre de la fen�tre</i> : SYNTHESE Admin - Afficheurs
				
			<i>Barre de navigation</i> :
				- Lien vers l'accueil de la console d'administration

			<i>Zone de contenus</i> :
				-# <b>Zone de recherche</b> : Diff�rents champs permettent d'effectuer une recherche d'afficheurs :
					- @c UID : l'UID de l'afficheur doit contenir le texte entr� dans le champ (num�rique). Laisser le champ vide n'effectue aucun filtrage sur l'UID
					- @c Emplacement : liste de choix form�e de l'ensemble des arr�ts logiques du r�seau qui poss�dent au moins un afficheur. Un choix @c (tous) permet d'annuler le filtre sur l'emplacement.
					- @c Ligne : Liste de choix form�e de l'ensemble des lignes qui desservent au moins un arr�t logique poss�dant au moins un afficheur, repr�sent�es par leur code SYNTHESE (Ex : MP/TLS/41). Un choix @c (tous) permet d'annuler le filtre sur la ligne.
					- @c Type : Liste de choix form�e par l'ensemble des types d'afficheurs d�finis sur l'�cran @ref synthese::interface::DisplayTypesAdmin. Un choix @c (tous� permet d'annuler le filtre sur le type d'afficheur.
					- @c Etat : Liste de choix form�e par des combinaisons d'�tat de maintenance :
						- @c (tous) : le filtre est d�sactiv�
						- @c OK : seuls les afficheurs en bon �tat de fonctionnement sont s�lectionn�s
						- @c Warning : seuls les afficheurs en �tat Warning (mat�riel ou coh�rence donn�es) sont s�lectionn�s
						- @c Warning+Error : seuls les afficheurs en �tat Warning ou Error (mat�riel ou coh�rence donn�es) sont s�lectionn�s
						- @c Error : seuls les afficheurs en �tat Error (mat�riel ou coh�rence donn�es) sont s�lectionn�s
						- <tt>Hors service</tt> : seuls les afficheurs hors service sont s�lectionn�s
					Au chargement, la page affiche l'ensemble des afficheurs pouvant �tre vus d'apr�s les droits de l'utilisateur.
				-# <b>Tableau r�sultat de recherche</b> : Les colonnes suivantes sont pr�sentes :
					- @c UID : Code SYNTHESE de l'afficheur. Un clic sur l'UID va vers la page @ref synthese::interface::DisplayAdmin.
					- @c Emplacement : R�sum� de l'emplacement de l'afficheur. Un clic sur l'emplacement va vers la page @ref synthese::interface::DisplayLocations sur le lieu logique de l'afficheur. Le contenu est constitu� par les trois champs suivants, s�par�s par un /.  :
						- Lieu logique
						- Lieu physique (arr�t physique ou autre emplacement interne au lieu logique)
						- Compl�ment de pr�cision
					- @c Type : Type d'afficheur. Un clic sur le type va vers la page @ref synthese::interface::DisplayTypesAdmin.
					- @c Etat : R�sum� de l'�tat de maintenance. Un clic sur le contenu de la colonne va vers la page @ref synthese::interface::DisplayMaintenanceAdmin :
						- Si l'afficheur est d�clar� en service, deux points de couleur (vert = OK, orange = Warning, rouge = Error), faisant appara�tre une infobulle pr�cisant leur signification au contact du pointeur de souris - voir d�tail sur page DisplayMaintenanceAdmin :
							- le premier point correspond au contr�le de coh�rence de donn�es
							- le second point correspond � l'�tat du mat�riel
						- Si l'afficheur est d�clar� hors service, la mention HS suivie de la date de la mise hors service est pr�sente, en couleur rouge
					- @c Actions : Trois boutons permettent d'acc�der aux fonctions suivantes :
						- @c Modifier : Dirige vers la page @ref synthese::interface::DisplayAdmin
						- @c Simuler : Ouvre une fen�tre pop-up effectuant un affichage similaire � ce qui est diffus� sur l'afficheur, pour les �crans de type HTML. Pour les �cran au protocole Lumiplan, le code Lumiplan est affich�.
						- @c Supervision : Dirige vers la page @ref synthese::interface::DisplayMaintenanceAdmin
					- La derni�re ligne du tableau comprend un lien "Cr�er un nouvel afficheur" et donne sur une page de param�trage d'afficheur vide.
					- Un clic sur le titre des colonnes du tableau effectue un tri selon la colonne (ascendant puis descendant).
				-# <b>Lien afficheurs suivants</b> : Au maximum 50 afficheurs sont repr�sent�s sur la page. En cas de d�passement de ce nombre, ce lien appara�t et permet de se rendre aux afficheurs suivants. A partir de la seconde page, un lien <tt>Afficheurs pr�c�dents</tt> est �galement propos�.

			<i>S�curit�</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ est n�cessaire pour acc�der � la page. Le r�sultat de la recherche d�pend du p�rim�tre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE est n�cessaire pour obtenir les boutons <tt>Modifier</tt> et <tt>Cr�er un nouvel afficheur</tt>
				- Une habilitation publique ArrivalDepartureTableRight de niveau USE est n�cessaire pour obtenir les boutons <tt>Simuler</tt>, sur les afficheurs contenus dans le p�rim�tre de l'habilitation.
				- Une habilitation publique DisplayMaintenanceRight de niveau READ est n�cessaire pour obtenir les boutons <tt>Supervision</tt>, sur les afficheurs contenus dans le p�rim�tre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau DELETE est n�cessaire pour obtenir le bouton <tt>Supprimer</tt>

			<i>Journal</i> : Les actions suivants entrainent une �criture dans le journal du t�l�affichage ArrivalDepartureTableLog :
				- INFO : cr�ation d'un afficheur
				- INFO : suppression d'un afficheur

			<i>Journal</i> : Les actions suivants entra�nent une �criture dans le journal de maintenance des afficheurs DisplayMaintenanceLog :
				- WARNING : premi�re constatation d'une absence d'entr�e de type contr�le sur un afficheur dans un d�lai compris entre 150% et 500% de la dur�e pr�suppos�e entre deux contr�les. 
				- ERROR : premi�re constatation d'une absence d'entr�e de type contr�le sur un afficheur dans un d�lai sup�rieur � 500% de la dur�e pr�suppos�e entre deux contr�les. 
				- NB : Ces deux entr�es apparaissent � la premi�re visualisation d'un probl�me de ce type dans une console d'administration, afin de suppl�er � l'absence de d�mon de surveillance. Un passage en contr�le continu avec alerte pourrait �tre impl�ment�.
		*/
		class DisplaySearchAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif