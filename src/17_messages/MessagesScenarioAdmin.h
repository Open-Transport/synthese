
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

#include "AdminInterfaceElementTemplate.h"
#include "DBLogHTMLView.h"

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class ScenarioTemplate;
		class Scenario;

		/** Ecran d'�dition de sc�nario de diffusion de message.
			@ingroup m17

			@image html cap_admin_scenario.png
			@image latex cap_admin_scenario.png "Maquette de l'�cran d'�dition de sc�nario" width=14cm

			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Messages - Biblioth�que - Sc�narios - Interruption m�tro

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Lien vers synthese::interfaces::MessagesLibraryAdmin
				- Texte <tt>Sc�nario</tt>
				- Texte [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'�dition</b> :
				-# <b>Edition du nom</b>
					-# <tt>Nom</tt> : Champ texte permettant la saisie du nom du sc�nario
					-# Bouton <tt>Modifier</tt> : Enregistre le changement du nom du sc�nario apr�s avoir contr�l� que le nom propos� n'est pas d�j� pris par un autre sc�nario. En ce cas, une bo�te de dialogue appara�t indiquant "Le nom entr� est d�j� utilis�. Veuillez utiliser un autre nom".
				-# <b>Liste de messages du sc�nario</b>
					-# <tt>Sel</tt> : Permet la s�lection du message en vue d'une duplication
					-# <tt>Message</tt> : Texte rappelant le contenu du message. Un clic sur le texte se rend sur l'�cran d'�dition du message.
					-# <tt>Emplacement</tt> : Texte rappelant l'emplacement de diffusion au niveau logique. Un clic sur le texte se rend sur l'�cran d'�dition de l'emplacement.
					-# Bouton <tt>Modifier</tt> : Se rend vers l'�cran d'�dition du message s�lectionn�
					-# Bouton <tt>Supprimer</tt> : Supprime le message du sc�nario apr�s une demande de confirmation
				-# Le <b>bouton Ajouter</b> permet l'ajout d'un nouveau message au sc�nario :
					- si aucun message n'est s�lectionn� alors un message vide est ajout�
					- si un message existant est s�lectionn� alors son contenu est copi� dans le nouveau

			<i>S�curit�</i>
				- Une habilitation publique MessagesLibraryRight de niveau WRITE est n�cessaire pour acc�der � la page et y effectuer toutes les op�rations disponibles.

			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal des messages MessagesLibraryLog :
				- INFO : Ajout de message au sc�nario
				- INFO : Suppression de message du sc�nario

		*/
		class MessagesScenarioAdmin : public admin::AdminInterfaceElementTemplate<MessagesScenarioAdmin>
		{
		public:
			static const std::string	TAB_MESSAGES;
			static const std::string	TAB_PARAMETERS;
			static const std::string	TAB_VARIABLES;
			static const std::string	TAB_LOG;

		private:
			boost::shared_ptr<const SentScenario>		_sentScenario;
			boost::shared_ptr<const ScenarioTemplate>	_templateScenario;
			boost::shared_ptr<const Scenario>			_scenario;
			dblog::DBLogHTMLView						_generalLogView;

		public:
			MessagesScenarioAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			virtual void setFromParametersMap(
				const server::ParametersMap& map,
				bool doDisplayPreparationActions,
				bool objectWillBeCreatedLater
			);

			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;
			
			
			
			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const;

			bool isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const;


			/** Sub pages getter.
				@return PageLinks Ordered vector of sub pages links
				@author Hugues Romain
				@date 2008
				
				The default implementation handles the auto registration of administrative components by getSuperiorVirtual() method.
				This method can be overloaded to create customized sub tree.
			*/
			virtual PageLinks getSubPages(
				boost::shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
			) const;

			virtual std::string getTitle() const;

			virtual void _buildTabs(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const;

			boost::shared_ptr<const Scenario> getScenario() const;
			void setScenario(boost::shared_ptr<Scenario> value);
		};
	}
}

#endif // SYNTHESE_MessagesScenarioAdmin_H__
