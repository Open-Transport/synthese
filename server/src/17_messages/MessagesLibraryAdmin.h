
/** MessagesLibraryAdmin class header.
	@file MessagesLibraryAdmin.h

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

#ifndef SYNTHESE_MessagesLibraryAdmin_H__
#define SYNTHESE_MessagesLibraryAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "ResultHTMLTable.h"

namespace synthese
{
	namespace messages
	{
		class ScenarioFolder;

		/** Ecran d'édition des modèles de texte destinés aux messages.
			@ingroup m17

			@image html cap_admin_messages_library.png
			@image latex cap_admin_messages_library.png "Maquette de l'écran d'édition des modèles de message" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Messages - Bibliothèque

			<i>Barre de navigation</i> :
				- Lien vers synthese::admin::HomeAdmin
				- Lien vers synthese::messages::MessagesAdmin
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
		class MessagesLibraryAdmin : public admin::AdminInterfaceElementTemplate<MessagesLibraryAdmin>
		{
		public:
			static const std::string PARAMETER_NAME;

		private:
			html::ResultHTMLTable::RequestParameters	_requestParameters;
			boost::shared_ptr<const ScenarioFolder>		_folder;
			util::Env									_subFoldersEnv;


		public:
			MessagesLibraryAdmin();

			boost::shared_ptr<const ScenarioFolder> getFolder() const;
			void setFolder(boost::shared_ptr<ScenarioFolder> folder);
			void setFolder(boost::shared_ptr<const ScenarioFolder> folder);

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
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
				const security::User& profile
			) const;




			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is Messages
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
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
			virtual std::string getIcon() const;

			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
		};
}	}

#endif // SYNTHESE_MessagesLibraryAdmin_H__
