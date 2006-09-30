
#ifndef SYNTHESE_DISPLAY_TYPES_ADMIN_H
#define SYNTHESE_DISPLAY_TYPES_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'administration des types d'affichage.
			@ingroup m34

			@image html cap_displa_types_admin.png
			@image latex cap_displa_types_admin.png "Ecran d'�dition des types d'affichage" width=14cm
			
			L'�cran de gestion des types d'affichage pr�sente sous forme tabulaire l'ensemble des types d'affichage.
			
			L'�cran de gestion propose les fonctionnalit�s suivantes :
			
				- @b "Edition directe" des afficheurs existants :
					- la d�signation du type (texte non vide)
					- l'interface utilis�e (liste de choix pr�sentant les interfaces install�es - exemple : Lumiplan, HTML charte 1, HTML charte 2, etc.)
					- le nombre de d�parts affich�s (nombre sup�rieur � 1)
					- le bouton Modifier permet d'enregistrer les modifications
				- @b Ajout de type d'afficheur : le bouton Ajouter permet d'ajouter un type d'afficheur vide.
				- @b Suppression de type d'afficheur : le bouton Supprimer permet de retirer un type d'afficheur. 
					- Ce bouton n'appara�t que sur les types non utilis�s. 
					- Un contr�le d'int�grit� est r�alis� � nouveau au moment de la suppression proprement dite. En cas d'existence d'afficheur au moment de la suppression, un message d'erreur appara�t :
						@code 
Ce type d'afficheur ne peut �tre supprim� car il est utilis� par au moins un afficheur.
@endcode
						
			@a S�curit�
				- L'habilitation Lecture sur * sur le module "tableaux de d�parts" est n�cessaire pour visualiser la page en consultation
				- L'habilitation Ecriture sur * sur le module "tableaux de d�parts" est n�cessaire pour �diter les types d'afficheurs, en cr�er, et en supprimer
		*/
		class DisplayTypesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif