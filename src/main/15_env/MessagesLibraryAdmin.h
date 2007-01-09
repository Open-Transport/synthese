
/** MessagesLibraryAdmin class header.
	@file MessagesLibraryAdmin.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ENV_MESSAGES_LIBRARY_ADMIN_H
#define SYNTHESE_ENV_MESSAGES_LIBRARY_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace env
	{

		/** Ecran d'�dition des mod�les de texte destin�s aux messages.
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
				-# <b>Tableau des textes destin�s aux messages compl�mentaires</b>
					-# <tt>Nom</tt> : Nom d�signant le texte dans les menus. Ce nom n'est pas affich� en dehors du module d'administration. Champ obligatoire et unique.
					-# <tt>Message court</tt> : Champ texte de 2x20 caract�res. Champ obligatoire.
					-# <tt>Message long</tt> : Champ texte de taille illimit�e (pouvant par exemple recevoir du HTML). Champ obligatoire.
					-# bouton <tt>Modifier</tt> : Enregistre les modifications effectu�es dans les champs
					-# bouton <tt>Supprimer</tt> : Supprime le mod�le de texte
					-# bouton <tt>Ajouter</tt> : M�me comportement que <tt>Modifier</tt> sur un texte nouvellement cr��
					
				-# <b>Tableau des textes destin�s aux messages prioritaires</b>
					-# <tt>Nom</tt> : Nom d�signant le texte dans les menus. Ce nom n'est pas affich� en dehors du module d'administration. Champ obligatoire et unique.
					-# <tt>Message court</tt> : Champ texte de 4x20 caract�res. Champ obligatoire.
					-# <tt>Message long</tt> : Champ texte de 5x256 caract�res. Champ obligatoire.
					-# bouton <tt>Modifier</tt> : Enregistre les modifications effectu�es dans les champs
					-# bouton <tt>Supprimer</tt> : Supprime le mod�le de message
					-# bouton <tt>Ajouter</tt> : M�me comportement que <tt>Modifier</tt> sur un message nouvellement cr��

			<i>S�curit�</i>
				- Une habilitation publique MessagesLibraryRight de niveau WRITE est n�cessaire pour acc�der � la page et y effectuer toutes les op�rations disponibles � l'exception de la suppression de mod�les.
				- Une habilitation publique MessagesLibraryRight de niveau DELETE est n�cessaire pour supprimer un mod�le de texte.

			<i>Journaux</i> : Les actions suivantes g�n�rent des entr�es dans le journal de la biblioth�que de textes MessagesLibraryLog :
				- INFO : Cr�ation de mod�le de texte
				- INFO : Modification de mod�le de texte
				- INFO : Suppression de mod�le de texte

		*/
		class MessagesLibraryAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif
