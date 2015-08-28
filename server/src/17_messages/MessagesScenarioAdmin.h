
/** MessagesScenarioAdmin class header.
	@file MessagesScenarioAdmin.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "AdminInterfaceElementTemplate.h"
#include "DBLogHTMLView.h"

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class ScenarioTemplate;
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
					-# Bouton <tt>Ouvrir</tt> : Se rend vers l'écran d'édition du message sélectionné
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
		class MessagesScenarioAdmin : public admin::AdminInterfaceElementTemplate<MessagesScenarioAdmin>
		{
		public:
			static const std::string	TAB_MESSAGES;
			static const std::string	TAB_PARAMETERS;
			static const std::string	TAB_LOG;
			static const std::string	TAB_DATASOURCES;

		private:
			boost::shared_ptr<const Scenario>			_scenario;
			dblog::DBLogHTMLView						_generalLogView;

		public:
			MessagesScenarioAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			virtual void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			bool isAuthorized(
				const security::User& user
			) const;


			/** Sub pages getter.
				@return PageLinks Ordered vector of sub pages links
				@author Hugues Romain
				@date 2008

				The default implementation handles the auto registration of administrative components by getSuperiorVirtual() method.
				This method can be overloaded to create customized sub tree.
			*/
			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
			virtual std::string getTitle() const;

			virtual void _buildTabs(
				const security::Profile& profile
			) const;

			boost::shared_ptr<const Scenario> getScenario() const;
			void setScenario(boost::shared_ptr<Scenario> value);

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

		};
	}
}

#endif // SYNTHESE_MessagesScenarioAdmin_H__
