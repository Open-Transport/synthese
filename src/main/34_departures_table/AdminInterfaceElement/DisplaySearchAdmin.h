
#ifndef SYNTHESE_DISPLAY_SEARCH_ADMIN_H
#define SYNTHESE_DISPLAY_SEARCH_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de gestion du parc matériel d'afficheurs.
			@ingroup m34

			L'écran de gestion du parc matériel d'afficheurs présente sous forme tabulaire l'ensemble des afficheurs. Les colonnes suivantes sont affichées :
				- UID
				- Emplacement : lieu logique. Un clic sur le lieu va vers la page de paramétrage du lieu logique
				- Emplacement : lieu physique. Un clic sur le lieu va vers la page de paramétrage du lieu logique auquel appartient le lieu physique.
				- Emplacement : complément de précision de lieu. Un clic sur le lieu va vers la page de paramétrage du lieu logique auquel appartient le lieu précisé.
				- Type. Un clic sur le type va vers la page @ref csst_ihm_display_DisplayTypesAdmin
				- Désignation. Un clic sur la désignation va vers la page de paramétrage de l'afficheur
				- Résumé état de maintenance (point de couleur verte si OK, point jaune si Warning, point rouge si Erreur). Un clic sur le point va vers la page de supervision de l'afficheur.
				- Lien explicite vers la page de paramétrage de l'afficheur
				- Lien explicite vers la page de supervision de l'afficheur
			
			La dernière ligne du tableau comprend un lien "Créer un nouvel afficheur" et donne sur une page de paramétrage d'afficheur vide.

			Au chargement, la page affiche l'ensemble des afficheurs pouvant être vus d'après les droits de l'utilisateur.
			Des listes de choix en tête des colonnes du tableau permettent de filtrer l'affichage par tous les champs sauf l'UID qui ne présente pas d'intérêt pour l'utilisateur.
			Un bouton présent à droite de chaque liste de choix permet d'activer le tri des afficheurs selon la colonne choisie, dans l'ordre croissant après le premier clic, dans l'ordre décroissant après le second clic.

			Sécurité :
				- Au moins une habilitation lecture sur le module "tableaux de départs" est nécessaire pour entrer dans la page
				- Les afficheurs affichés sont ceux qui sont compris dans le périmètre défini par le champ d'application des habilitations sur le module "tableaux de départs"
				- Au moins une habilitation écriture sur le module "tableaux de départs" est nécessaire pour obtenir le lien de création d'afficheur
		*/
		class DisplaySearchAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif