
#ifndef SYNTHESE_DISPLAY_SEARCH_ADMIN_H
#define SYNTHESE_DISPLAY_SEARCH_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de gestion du parc matériel d'afficheurs.
			@ingroup m34

			@image html cap_admin_displays.png
			@image latex cap_admin_displays.png "Maquette de l'écran de gestion des afficheurs" width=14cm

			<i>Titre de la fenêtre</i> : SYNTHESE Admin - Afficheurs
				
			<i>Barre de navigation</i> :
				- Lien vers l'accueil de la console d'administration

			<i>Zone de contenus</i> :
				-# <b>Zone de recherche</b> : Différents champs permettent d'effectuer une recherche d'afficheurs :
					- @c UID : l'UID de l'afficheur doit contenir le texte entré dans le champ (numérique). Laisser le champ vide n'effectue aucun filtrage sur l'UID
					- @c Emplacement : liste de choix formée de l'ensemble des arrêts logiques du réseau qui possèdent au moins un afficheur. Un choix @c (tous) permet d'annuler le filtre sur l'emplacement.
					- @c Ligne : Liste de choix formée de l'ensemble des lignes qui desservent au moins un arrêt logique possédant au moins un afficheur, représentées par leur code SYNTHESE (Ex : MP/TLS/41). Un choix @c (tous) permet d'annuler le filtre sur la ligne.
					- @c Type : Liste de choix formée par l'ensemble des types d'afficheurs définis sur l'écran @ref synthese::interface::DisplayTypesAdmin. Un choix @c (tousà permet d'annuler le filtre sur le type d'afficheur.
					- @c Etat : Liste de choix formée par des combinaisons d'état de maintenance :
						- @c (tous) : le filtre est désactivé
						- @c OK : seuls les afficheurs en bon état de fonctionnement sont sélectionnés
						- @c Warning : seuls les afficheurs en état Warning (matériel ou cohérence données) sont sélectionnés
						- @c Warning+Error : seuls les afficheurs en état Warning ou Error (matériel ou cohérence données) sont sélectionnés
						- @c Error : seuls les afficheurs en état Error (matériel ou cohérence données) sont sélectionnés
						- <tt>Hors service</tt> : seuls les afficheurs hors service sont sélectionnés
					Au chargement, la page affiche l'ensemble des afficheurs pouvant être vus d'après les droits de l'utilisateur.
				-# <b>Tableau résultat de recherche</b> : Les colonnes suivantes sont présentes :
					- @c UID : Code SYNTHESE de l'afficheur. Un clic sur l'UID va vers la page @ref synthese::interface::DisplayAdmin.
					- @c Emplacement : Résumé de l'emplacement de l'afficheur. Un clic sur l'emplacement va vers la page @ref synthese::interface::DisplayLocations sur le lieu logique de l'afficheur. Le contenu est constitué par les trois champs suivants, séparés par un /.  :
						- Lieu logique
						- Lieu physique (arrêt physique ou autre emplacement interne au lieu logique)
						- Complément de précision
					- @c Type : Type d'afficheur. Un clic sur le type va vers la page @ref synthese::interface::DisplayTypesAdmin.
					- @c Etat : Résumé de l'état de maintenance. Un clic sur le contenu de la colonne va vers la page @ref synthese::interface::DisplayMaintenanceAdmin :
						- Si l'afficheur est déclaré en service, deux points de couleur (vert = OK, orange = Warning, rouge = Error), faisant apparaître une infobulle précisant leur signification au contact du pointeur de souris - voir détail sur page DisplayMaintenanceAdmin :
							- le premier point correspond au contrôle de cohérence de données
							- le second point correspond à l'état du matériel
						- Si l'afficheur est déclaré hors service, la mention HS suivie de la date de la mise hors service est présente, en couleur rouge
					- @c Actions : Trois boutons permettent d'accéder aux fonctions suivantes :
						- @c Modifier : Dirige vers la page @ref synthese::interface::DisplayAdmin
						- @c Simuler : Ouvre une fenêtre pop-up effectuant un affichage similaire à ce qui est diffusé sur l'afficheur, pour les écrans de type HTML. Pour les écran au protocole Lumiplan, le code Lumiplan est affiché.
						- @c Supervision : Dirige vers la page @ref synthese::interface::DisplayMaintenanceAdmin
					- La dernière ligne du tableau comprend un lien "Créer un nouvel afficheur" et donne sur une page de paramétrage d'afficheur vide.
					- Un clic sur le titre des colonnes du tableau effectue un tri selon la colonne (ascendant puis descendant).
				-# <b>Lien afficheurs suivants</b> : Au maximum 50 afficheurs sont représentés sur la page. En cas de dépassement de ce nombre, ce lien apparaît et permet de se rendre aux afficheurs suivants. A partir de la seconde page, un lien <tt>Afficheurs précédents</tt> est également proposé.

			<i>Sécurité</i> :
				- Une habilitation publique ArrivalDepartureTableRight de niveau READ est nécessaire pour accéder à la page. Le résultat de la recherche dépend du périmètre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau WRITE est nécessaire pour obtenir les boutons <tt>Modifier</tt> et <tt>Créer un nouvel afficheur</tt>
				- Une habilitation publique ArrivalDepartureTableRight de niveau USE est nécessaire pour obtenir les boutons <tt>Simuler</tt>, sur les afficheurs contenus dans le périmètre de l'habilitation.
				- Une habilitation publique DisplayMaintenanceRight de niveau READ est nécessaire pour obtenir les boutons <tt>Supervision</tt>, sur les afficheurs contenus dans le périmètre de l'habilitation.
				- Une habilitation publique ArrivalDepartureTableRight de niveau DELETE est nécessaire pour obtenir le bouton <tt>Supprimer</tt>

			<i>Journal</i> : Les actions suivants entrainent une écriture dans le journal du téléaffichage ArrivalDepartureTableLog :
				- INFO : création d'un afficheur
				- INFO : suppression d'un afficheur

			<i>Journal</i> : Les actions suivants entraînent une écriture dans le journal de maintenance des afficheurs DisplayMaintenanceLog :
				- WARNING : première constatation d'une absence d'entrée de type contrôle sur un afficheur dans un délai compris entre 150% et 500% de la durée présupposée entre deux contrôles. 
				- ERROR : première constatation d'une absence d'entrée de type contrôle sur un afficheur dans un délai supérieur à 500% de la durée présupposée entre deux contrôles. 
				- NB : Ces deux entrées apparaissent à la première visualisation d'un problème de ce type dans une console d'administration, afin de suppléer à l'absence de démon de surveillance. Un passage en contrôle continu avec alerte pourrait être implémenté.
		*/
		class DisplaySearchAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif