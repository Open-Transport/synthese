
#ifndef SYNTHESE_ENV_MESSAGES_LIBRARY_ADMIN_H
#define SYNTHESE_ENV_MESSAGES_LIBRARY_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'�dition des mod�les de messages.
			@ingroup m15

			@image html cap_admin_messages_library.png
			@image latex cap_admin_messages_library.png "Maquette de l'�cran d'�dition des mod�les de message" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Messages - Biblioth�que

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Texte <tt>Biblioth�que</tt>

			<i>Zone de contenu</i> :
				-# <b>Tableau des messages compl�mentaires</b>
					-# <tt>Nom</tt> : Nom d�signant le message dans les menus. Ce nom n'est pas affich� en dehors du module d'administration. Champ obligatoire et unique.
					-# <tt>Message court</tt> : Champ texte de 2x20 caract�res. Champ obligatoire.
					-# <tt>Message long</tt> : Champ texte de 256 caract�res. Champ obligatoire.
					-# bouton <tt>Modifier</tt> : Enregistre les modifications effectu�es dans les champs
					-# bouton <tt>Supprimer</tt> : Supprime le mod�le de message
					-# bouton <tt>Ajouter</tt> : M�me comportement que <tt>Modifier</tt> sur un message nouvellement cr��
					
				-# <b>Tableau des messages prioritaires</b>
					-# <tt>Nom</tt> : Nom d�signant le message dans les menus. Ce nom n'est pas affich� en dehors du module d'administration. Champ obligatoire et unique.
					-# <tt>Message court</tt> : Champ texte de 4x20 caract�res. Champ obligatoire.
					-# <tt>Message long</tt> : Champ texte de 5x256 caract�res. Champ obligatoire.
					-# bouton <tt>Modifier</tt> : Enregistre les modifications effectu�es dans les champs
					-# bouton <tt>Supprimer</tt> : Supprime le mod�le de message
					-# bouton <tt>Ajouter</tt> : M�me comportement que <tt>Modifier</tt> sur un message nouvellement cr��

			<i>S�curit�</i>
				- Une habilitation publique MessagesLibraryRight de niveau WRITE est n�cessaire pour acc�der � la page et y effectuer toutes les op�rations disponibles � l'exception de la suppression de mod�les.
				- Une habilitation publique MessagesLibraryRight de niveau DELETE est n�cessaire pour supprimer un mod�le de message.

			<i>Journaux</i> : Les actions suivantes g�n�rent des entr�es dans le journal de la biblioth�que de messages MessagesLibraryLog :
				- INFO : Cr�ation de mod�le de message
				- INFO : Modification de mod�le de message
				- INFO : Suppression de mod�le de message

		*/
		class MessagesLibraryAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif