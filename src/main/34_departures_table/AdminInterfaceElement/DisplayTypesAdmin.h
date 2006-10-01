
#ifndef SYNTHESE_DISPLAY_TYPES_ADMIN_H
#define SYNTHESE_DISPLAY_TYPES_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'administration des types d'affichage.
			@ingroup m34

			@image html cap_admin_display_types.png
			@image latex cap_admin_display_types.png "Ecran d'édition des types d'affichage" width=14cm
			
			L'écran de gestion des types d'affichage présente sous forme tabulaire l'ensemble des types d'affichage.
			
			L'écran de gestion propose les fonctionnalités suivantes :
			
				- @b "Edition directe" des afficheurs existants :
					- la désignation du type (texte non vide)
					- l'interface utilisée (liste de choix présentant les interfaces installées - exemple : Lumiplan, HTML charte 1, HTML charte 2, etc.)
					- le nombre de départs affichés (liste de coix : de 1 à 50)
					- le bouton Modifier permet d'enregistrer les modifications
				- @b Ajout de type d'afficheur : le bouton Ajouter permet d'ajouter un type d'afficheur directement. Un contrôle des donneés entrées est effectué : en cas de données manquante, l'afficheur n'est pas créé et un message d'erreur apparaît :
					@code
L'afficheur ne peut être créé car un ou plusieurs champs n'a pas été correctement renseigné.
@endcode
				- @b Suppression de type d'afficheur : le bouton Supprimer permet de retirer un type d'afficheur s'il n'est utilisé par aucun afficheur.
					- Ce bouton n'apparaît que sur les types non utilisés. 
					- Un contrôle d'intégrité est réalisé à nouveau au moment de la suppression proprement dite. En cas d'existence d'afficheur au moment de la suppression, un message d'erreur apparaît :
						@code 
Ce type d'afficheur ne peut être supprimé car il est utilisé par au moins un afficheur.
@endcode
						
			@a Sécurité
				- L'habilitation Lecture sur * sur le module Tableaux de départs est nécessaire pour visualiser la page en consultation
				- L'habilitation Ecriture sur * sur le module Tableaux de départs est nécessaire pour éditer les types d'afficheurs, en créer, et en supprimer
		*/
		class DisplayTypesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif