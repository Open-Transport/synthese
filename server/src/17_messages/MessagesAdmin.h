
/** MessagesAdmin class header.
	@file MessagesAdmin.h

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

#ifndef SYNTHESE_MessagesAdmin_H__
#define SYNTHESE_MessagesAdmin_H__

#include "ActionResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "MessagesTypes.h"
#include "ScenarioTableSync.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class ScenarioTemplate;

		/** Ecran de recherche et de liste de message.
			@ingroup m17Admin refAdmin

			@image html cap_admin_messages.png
			@image latex cap_admin_messages.png "Maquette de l'écran de liste de messages" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Messages

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Texte <tt>Messages</tt>

			<i>Zone de contenu</i> :
				-# <b>Formulaire de recherche</b>
					-# <tt>Date début</tt> : champ texte permettant de chercher un message dont la fin de diffusion est postérieure à la date entrée.
					-# <tt>Date fin</tt> : champ texte permettant de chercher un message dont le début de diffusion est antérieur à la date entrée.
					-# <tt>Arrêt logique</tt> : liste déroulante consituée de tous les arrêts logiques comportant au moins un afficheur, permettant de chercher un message diffusé sur au moins un afficheur de l'arrêt sélectionné
					-# <tt>Ligne</tt> : Liste déroulante constituée de toutes les lignes desservant au moins un arrêt muni d'afficheur, permettant de chercher un message diffusé sur l'un des afficheurs desservis par la ligne
					-# <tt>Statut</tt> : Liste déroulante proposant les statuts possibles pouvant faire l'objet d'un filtrage des résultats de recherche :
						- Tous les états
						- Messages dont la diffusion est terminée
						- Messages en cours de diffusion
						- Messages en cours de diffusion avec date de fin
						- Messages en cours de diffusion sans date de fin
						- Messages à diffuser ultérieurement
					-# <tt>Conflit</tt> : Liste déroulante proposant les types de conflit possibles pouvant faire l'objet d'un filtrage des résultats de recherche :
						- Toutes les situations
						- Messages sans conflit
						- Messages prioritaires sur complémentaires
						- Conflits de messages de même niveaux
					-# <tt>Type</tt> : Liste déroulante proposant les types de messages possibles pouvant faire l'objet d'un filtrage des résultats de recherche :
						- Tous les types de messages
						- Messages complémentaires
						- Messages prioritaires
						- Scénarios
					-# Bouton <tt>Rechercher</tt> lance la recherche
				-# <b>Tableau de messages</b> : Les messages sélectionnés sont décrits par les colonnes suivantes :
					-# <tt>Sel</tt> : permet la sélection du message en vue d'une copie
					-# <tt>Dates</tt> : affiche les dates de diffusion du message, définissant son statut, et en suivant la couleur de fond de la ligne :
						- Message dont la diffusion est terminée : fond blanc
						- Message en cours de diffusion avec date de fin : fond vert clair
						- Message en cours de diffusion sans date de fin : fond rose
						- Message à diffuser ultérieurement : fond bleu clair
					-# <tt>Message</tt> : extrait des messages diffusés. Un clic sur l'extrait conduit à la page synthese::interfaces::MessageAdmin sur le message sélectionné
					-# <tt>Type</tt> : type de message diffusé
						- <tt>Complémentaire</tt> : Message complémentaire
						- <tt>Prioritaire</tt> : Message prioritaire
						- <tt>Scénario</tt> : L'entrée du tableau correspond à l'envoi d'une série de messages selon un scénario préparé à l'avance
					-# <tt>Etat</tt> : état matériel courant des afficheurs de la liste de diffusion. Une infobulle précise l'état représenté, apparaissant au contact avec le pointeur de souris
						- Pastille verte : tous les afficheurs sont en fonctionnement normal
						- Pastille orange : au moins un afficheur est en état WARNING, aucun n'est en état ERROR
						- Pastille rouge : au moins un afficheur est en état ERROR
					-# <tt>Conflit</tt> : indique si le message est en conflit avec un autre sur au moins un des afficheurs. Une pastille colorée donne l'information et affiche une infobulle lorsque le pointeur de souris est positionné au dessus de celle-ci :
						- Pastille verte : le message est en conflit sur aucun afficheur
						- Pastille orange : le message prioritaire "écrase" un message complémentaire
						- Pastille rouge : le message entre en conflit avec un message de même niveau
					-# Bouton <tt>Editer</tt> : conduit à la page synthese::interfaces::MessageAdmin sur le message correspondant à la ligne où est présent le bouton
					-# Bouton <tt>Arrêter</tt> : stoppe la diffusion du message sélectionné (disponible pour les messages dont la date de fin de diffusion n'est pas atteinte, ou pour les messages sans date de fin de diffusion)
					-# Un clic sur les titres de colonnes effectue un tri croissant puis décroissant sur la colonne sélectionnée
				-# Bouton <b>Nouvelle diffusion de message</b> : ouvre la page synthese::interfaces::MessageAdmin :
					- sur un message vierge si aucun message n'est sélectionné dans la colonne <tt>Sel</tt>
					- sur un message recopié sur un autre, sélectionné dans la colonne <tt>Sel</tt>
				-# Bouton <b>Nouvelle diffusion de scénario</b> : ouvre la page synthese::interfaces::MessagesScenarioSendAdmin sur le scénario sélectionné dans la liste déroulante. Le contenu du scénario est recopié pour être diffusé après possible modification
				-# Un maximum de 50 messages est affiché à l'écran. En cas de dépassement de ce nombre d'après les critères de recherche, un lien <tt>Messages suivants</tt> apparait et permet de visualiser les entrées suivantes. A partir de la seconde page, un lien <tt>Messages précédents</tt> apparait également.


			<i>Sécurité</i>
				- Une habilitation privée ou publique MessagesRight de niveau READ est nécessaire pour accéder à la page. Le résultat d'une recherche dépend du périmètre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation privée MessagesRight de niveau WRITE est nécessaire pour créer un nouveau message ou une nouvelle diffusion de scénario. Le contenu de la liste des scénarios dépend du périmètre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation privée MessagesRight de niveau WRITE est nécessaire pour éditer un message créé par l'utilisateur courant.
				- Une habilitation publique MessagesRight de niveau WRITE est nécessaire pour éditer un message n'appartenant pas à l'utilisateur courant.

			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.
		*/
		class MessagesAdmin :
			public admin::AdminInterfaceElementTemplate<MessagesAdmin>
		{
		public:
			static const std::string CSS_ALARM_DISABLED;
			static const std::string CSS_ALARM_DISPLAYED_WITHOUT_END_DATE;
			static const std::string CSS_ALARM_WILL_BE_DISPLAYED;
			static const std::string CSS_ALARM_DISPLAYED_WITH_END_DATE;

		private:
			bool _searchArchive;
			boost::optional<std::string>						_searchName;
			boost::optional<const ScenarioTemplate*>			_searchScenario;
			html::ActionResultHTMLTable::RequestParameters		_requestParameters;

			util::ParametersMap	_parametersMap;

		public:
			static const std::string PARAMETER_SEARCH_ARCHIVE;
			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_SEARCH_TEMPLATE;
			static const std::string SORT_ON_DATE;

			MessagesAdmin();

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

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is Home
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
			*/
			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

		};
	}
}

#endif // SYNTHESE_MessagesAdmin_H__
