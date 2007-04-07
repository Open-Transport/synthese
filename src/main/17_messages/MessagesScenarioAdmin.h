
/** MessagesScenarioAdmin class header.
	@file MessagesScenarioAdmin.h

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

#ifndef SYNTHESE_MessagesScenarioAdmin_H__
#define SYNTHESE_MessagesScenarioAdmin_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace messages
	{
		class Scenario;

		/** Ecran d'édition de scénario de diffusion de message.
			@ingroup m17

			@image html cap_admin_scenario.png
			@image latex cap_admin_scenario.png "Maquette de l'écran d'édition de scénario" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Messages - Bibliothèque - Scénarios - Interruption métro

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Lien vers synthese::interfaces::MessagesLibraryAdmin
				- Texte <tt>Scénario</tt>
				- Texte [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'édition</b> :
				-# <b>Edition du nom</b>
					-# <tt>Nom</tt> : Champ texte permettant la saisie du nom du scénario
					-# Bouton <tt>Modifier</tt> : Enregistre le changement du nom du scénario après avoir contrôlé que le nom proposé n'est pas déjà pris par un autre scénario. En ce cas, une boîte de dialogue apparaît indiquant "Le nom entré est déjà utilisé. Veuillez utiliser un autre nom".
				-# <b>Liste de messages du scénario</b>
					-# <tt>Sel</tt> : Permet la sélection du message en vue d'une duplication
					-# <tt>Message</tt> : Texte rappelant le contenu du message. Un clic sur le texte se rend sur l'écran d'édition du message.
					-# <tt>Emplacement</tt> : Texte rappelant l'emplacement de diffusion au niveau logique. Un clic sur le texte se rend sur l'écran d'édition de l'emplacement.
					-# Bouton <tt>Modifier</tt> : Se rend vers l'écran d'édition du message sélectionné
					-# Bouton <tt>Supprimer</tt> : Supprime le message du scénario après une demande de confirmation
				-# Le <b>bouton Ajouter</b> permet l'ajout d'un nouveau message au scénario :
					- si aucun message n'est sélectionné alors un message vide est ajouté
					- si un message existant est sélectionné alors son contenu est copié dans le nouveau

			<i>Sécurité</i>
				- Une habilitation publique MessagesLibraryRight de niveau WRITE est nécessaire pour accéder à la page et y effectuer toutes les opérations disponibles.

			<i>Journaux</i> : Les événements suivants entrainent la création d'une entrée dans le journal des messages MessagesLibraryLog :
				- INFO : Ajout de message au scénario
				- INFO : Suppression de message du scénario

		*/
		class MessagesScenarioAdmin : public admin::AdminInterfaceElement
		{
		private:
			Scenario* _scenario;

		public:
			MessagesScenarioAdmin();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;

			/** Title of the admin compound.
				@return The title of the admin compound, for display purposes.
			*/
			std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_MessagesScenarioAdmin_H__
