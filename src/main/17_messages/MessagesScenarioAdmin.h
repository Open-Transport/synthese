
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
