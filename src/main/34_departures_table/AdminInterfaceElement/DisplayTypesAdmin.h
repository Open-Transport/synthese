
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
			@image latex cap_displa_types_admin.png "Ecran d'édition des types d'affichage" width=14cm
			
			L'écran de gestion des types d'affichage présente sous forme tabulaire l'ensemble des types d'affichage.
			
			L'écran de gestion propose les fonctionnalités suivantes :
			
				- @b "Edition directe" des afficheurs existants :
					- la désignation du type (texte non vide)
					- l'interface utilisée (liste de choix présentant les interfaces installées - exemple : Lumiplan, HTML charte 1, HTML charte 2, etc.)
					- le nombre de départs affichés (nombre supérieur à 1)
					- le bouton Modifier permet d'enregistrer les modifications
				- @b Ajout de type d'afficheur : le bouton Ajouter permet d'ajouter un type d'afficheur vide.
				- @b Suppression de type d'afficheur : le bouton Supprimer permet de retirer un type d'afficheur. 
					- Ce bouton n'apparaît que sur les types non utilisés. 
					- Un contrôle d'intégrité est réalisé à nouveau au moment de la suppression proprement dite. En cas d'existence d'afficheur au moment de la suppression, un message d'erreur apparaît :
						@code 
Ce type d'afficheur ne peut être supprimé car il est utilisé par au moins un afficheur.
@endcode
						
			@a Sécurité
				- L'habilitation Lecture sur * sur le module "tableaux de départs" est nécessaire pour visualiser la page en consultation
				- L'habilitation Ecriture sur * sur le module "tableaux de départs" est nécessaire pour éditer les types d'afficheurs, en créer, et en supprimer
		*/
		class DisplayTypesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif