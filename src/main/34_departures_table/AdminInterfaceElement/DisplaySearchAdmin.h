
#ifndef SYNTHESE_DISPLAY_SEARCH_ADMIN_H
#define SYNTHESE_DISPLAY_SEARCH_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de gestion du parc mat�riel d'afficheurs.
			@ingroup m34

			L'�cran de gestion du parc mat�riel d'afficheurs pr�sente sous forme tabulaire l'ensemble des afficheurs. Les colonnes suivantes sont affich�es :
				- UID
				- Emplacement : lieu logique. Un clic sur le lieu va vers la page de param�trage du lieu logique
				- Emplacement : lieu physique. Un clic sur le lieu va vers la page de param�trage du lieu logique auquel appartient le lieu physique.
				- Emplacement : compl�ment de pr�cision de lieu. Un clic sur le lieu va vers la page de param�trage du lieu logique auquel appartient le lieu pr�cis�.
				- Type. Un clic sur le type va vers la page @ref csst_ihm_display_DisplayTypesAdmin
				- D�signation. Un clic sur la d�signation va vers la page de param�trage de l'afficheur
				- R�sum� �tat de maintenance (point de couleur verte si OK, point jaune si Warning, point rouge si Erreur). Un clic sur le point va vers la page de supervision de l'afficheur.
				- Lien explicite vers la page de param�trage de l'afficheur
				- Lien explicite vers la page de supervision de l'afficheur
			
			La derni�re ligne du tableau comprend un lien "Cr�er un nouvel afficheur" et donne sur une page de param�trage d'afficheur vide.

			Au chargement, la page affiche l'ensemble des afficheurs pouvant �tre vus d'apr�s les droits de l'utilisateur.
			Des listes de choix en t�te des colonnes du tableau permettent de filtrer l'affichage par tous les champs sauf l'UID qui ne pr�sente pas d'int�r�t pour l'utilisateur.
			Un bouton pr�sent � droite de chaque liste de choix permet d'activer le tri des afficheurs selon la colonne choisie, dans l'ordre croissant apr�s le premier clic, dans l'ordre d�croissant apr�s le second clic.

			S�curit� :
				- Au moins une habilitation lecture sur le module "tableaux de d�parts" est n�cessaire pour entrer dans la page
				- Les afficheurs affich�s sont ceux qui sont compris dans le p�rim�tre d�fini par le champ d'application des habilitations sur le module "tableaux de d�parts"
				- Au moins une habilitation �criture sur le module "tableaux de d�parts" est n�cessaire pour obtenir le lien de cr�ation d'afficheur
		*/
		class DisplaySearchAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif