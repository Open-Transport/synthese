
#ifndef SYNTHESE_ENV_MESSAGES_LIBRARY_ADMIN_H
#define SYNTHESE_ENV_MESSAGES_LIBRARY_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'édition des modèles de texte destinés aux messages.
			@ingroup m15

			@image html cap_admin_messages_library.png
			@image latex cap_admin_messages_library.png "Maquette de l'écran d'édition des modèles de message" width=14cm
			
			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Messages - Bibliothèque

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Texte <tt>Bibliothèque</tt>

			<i>Zone de contenu</i> :
				-# <b>Tableau des textes destinés aux messages complémentaires</b>
					-# <tt>Nom</tt> : Nom désignant le texte dans les menus. Ce nom n'est pas affiché en dehors du module d'administration. Champ obligatoire et unique.
					-# <tt>Message court</tt> : Champ texte de 2x20 caractères. Champ obligatoire.
					-# <tt>Message long</tt> : Champ texte de taille illimitée (pouvant par exemple recevoir du HTML). Champ obligatoire.
					-# bouton <tt>Modifier</tt> : Enregistre les modifications effectuées dans les champs
					-# bouton <tt>Supprimer</tt> : Supprime le modèle de texte
					-# bouton <tt>Ajouter</tt> : Même comportement que <tt>Modifier</tt> sur un texte nouvellement créé
					
				-# <b>Tableau des textes destinés aux messages prioritaires</b>
					-# <tt>Nom</tt> : Nom désignant le texte dans les menus. Ce nom n'est pas affiché en dehors du module d'administration. Champ obligatoire et unique.
					-# <tt>Message court</tt> : Champ texte de 4x20 caractères. Champ obligatoire.
					-# <tt>Message long</tt> : Champ texte de 5x256 caractères. Champ obligatoire.
					-# bouton <tt>Modifier</tt> : Enregistre les modifications effectuées dans les champs
					-# bouton <tt>Supprimer</tt> : Supprime le modèle de message
					-# bouton <tt>Ajouter</tt> : Même comportement que <tt>Modifier</tt> sur un message nouvellement créé

			<i>Sécurité</i>
				- Une habilitation publique MessagesLibraryRight de niveau WRITE est nécessaire pour accéder à la page et y effectuer toutes les opérations disponibles à l'exception de la suppression de modèles.
				- Une habilitation publique MessagesLibraryRight de niveau DELETE est nécessaire pour supprimer un modèle de texte.

			<i>Journaux</i> : Les actions suivantes génèrent des entrées dans le journal de la bibliothèque de textes MessagesLibraryLog :
				- INFO : Création de modèle de texte
				- INFO : Modification de modèle de texte
				- INFO : Suppression de modèle de texte

		*/
		class MessagesLibraryAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif
