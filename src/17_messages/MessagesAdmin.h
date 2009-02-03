
/** MessagesAdmin class header.
	@file MessagesAdmin.h

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

#ifndef SYNTHESE_MessagesAdmin_H__
#define SYNTHESE_MessagesAdmin_H__

#include "DateTime.h"
#include "ActionResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "17_messages/Types.h"
#include "MessagesAdvancedSelectTableSync.h"

namespace synthese
{
	namespace messages
	{
		class SingleSentAlarm;
		class SentScenario;

		/** Ecran de recherche et de liste de message.
			@ingroup m17Admin refAdmin

			@image html cap_admin_messages.png
			@image latex cap_admin_messages.png "Maquette de l'�cran de liste de messages" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Messages

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Texte <tt>Messages</tt>

			<i>Zone de contenu</i> :
				-# <b>Formulaire de recherche</b>
					-# <tt>Date d�but</tt> : champ texte permettant de chercher un message dont la fin de diffusion est post�rieure � la date entr�e.
					-# <tt>Date fin</tt> : champ texte permettant de chercher un message dont le d�but de diffusion est ant�rieur � la date entr�e.
					-# <tt>Arr�t logique</tt> : liste d�roulante consitu�e de tous les arr�ts logiques comportant au moins un afficheur, permettant de chercher un message diffus� sur au moins un afficheur de l'arr�t s�lectionn�
					-# <tt>Ligne</tt> : Liste d�roulante constitu�e de toutes les lignes desservant au moins un arr�t muni d'afficheur, permettant de chercher un message diffus� sur l'un des afficheurs desservis par la ligne
					-# <tt>Statut</tt> : Liste d�roulante proposant les statuts possibles pouvant faire l'objet d'un filtrage des r�sultats de recherche :
						- Tous les �tats
						- Messages dont la diffusion est termin�e
						- Messages en cours de diffusion
						- Messages en cours de diffusion avec date de fin
						- Messages en cours de diffusion sans date de fin
						- Messages � diffuser ult�rieurement
					-# <tt>Conflit</tt> : Liste d�roulante proposant les types de conflit possibles pouvant faire l'objet d'un filtrage des r�sultats de recherche :
						- Toutes les situations
						- Messages sans conflit
						- Messages prioritaires sur compl�mentaires
						- Conflits de messages de m�me niveaux
					-# <tt>Type</tt> : Liste d�roulante proposant les types de messages possibles pouvant faire l'objet d'un filtrage des r�sultats de recherche :
						- Tous les types de messages
						- Messages compl�mentaires
						- Messages prioritaires
						- Sc�narios
					-# Bouton <tt>Rechercher</tt> lance la recherche
				-# <b>Tableau de messages</b> : Les messages s�lectionn�s sont d�crits par les colonnes suivantes :
					-# <tt>Sel</tt> : permet la s�lection du message en vue d'une copie
					-# <tt>Dates</tt> : affiche les dates de diffusion du message, d�finissant son statut, et en suivant la couleur de fond de la ligne :
						- Message dont la diffusion est termin�e : fond blanc
						- Message en cours de diffusion avec date de fin : fond vert clair
						- Message en cours de diffusion sans date de fin : fond rose
						- Message � diffuser ult�rieurement : fond bleu clair
					-# <tt>Message</tt> : extrait des messages diffus�s. Un clic sur l'extrait conduit � la page synthese::interfaces::MessageAdmin sur le message s�lectionn�
					-# <tt>Type</tt> : type de message diffus�
						- <tt>Compl�mentaire</tt> : Message compl�mentaire
						- <tt>Prioritaire</tt> : Message prioritaire
						- <tt>Sc�nario</tt> : L'entr�e du tableau correspond � l'envoi d'une s�rie de messages selon un sc�nario pr�par� � l'avance
					-# <tt>Etat</tt> : �tat mat�riel courant des afficheurs de la liste de diffusion. Une infobulle pr�cise l'�tat repr�sent�, apparaissant au contact avec le pointeur de souris
						- Pastille verte : tous les afficheurs sont en fonctionnement normal
						- Pastille orange : au moins un afficheur est en �tat WARNING, aucun n'est en �tat ERROR
						- Pastille rouge : au moins un afficheur est en �tat ERROR
					-# <tt>Conflit</tt> : indique si le message est en conflit avec un autre sur au moins un des afficheurs. Une pastille color�e donne l'information et affiche une infobulle lorsque le pointeur de souris est positionn� au dessus de celle-ci :
						- Pastille verte : le message est en conflit sur aucun afficheur
						- Pastille orange : le message prioritaire "�crase" un message compl�mentaire
						- Pastille rouge : le message entre en conflit avec un message de m�me niveau
					-# Bouton <tt>Editer</tt> : conduit � la page synthese::interfaces::MessageAdmin sur le message correspondant � la ligne o� est pr�sent le bouton
					-# Bouton <tt>Arr�ter</tt> : stoppe la diffusion du message s�lectionn� (disponible pour les messages dont la date de fin de diffusion n'est pas atteinte, ou pour les messages sans date de fin de diffusion)
					-# Un clic sur les titres de colonnes effectue un tri croissant puis d�croissant sur la colonne s�lectionn�e
				-# Bouton <b>Nouvelle diffusion de message</b> : ouvre la page synthese::interfaces::MessageAdmin :
					- sur un message vierge si aucun message n'est s�lectionn� dans la colonne <tt>Sel</tt>
					- sur un message recopi� sur un autre, s�lectionn� dans la colonne <tt>Sel</tt>
				-# Bouton <b>Nouvelle diffusion de sc�nario</b> : ouvre la page synthese::interfaces::MessagesScenarioSendAdmin sur le sc�nario s�lectionn� dans la liste d�roulante. Le contenu du sc�nario est recopi� pour �tre diffus� apr�s possible modification
				-# Un maximum de 50 messages est affich� � l'�cran. En cas de d�passement de ce nombre d'apr�s les crit�res de recherche, un lien <tt>Messages suivants</tt> apparait et permet de visualiser les entr�es suivantes. A partir de la seconde page, un lien <tt>Messages pr�c�dents</tt> apparait �galement.
			
			
			<i>S�curit�</i>
				- Une habilitation priv�e ou publique MessagesRight de niveau READ est n�cessaire pour acc�der � la page. Le r�sultat d'une recherche d�pend du p�rim�tre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation priv�e MessagesRight de niveau WRITE est n�cessaire pour cr�er un nouveau message ou une nouvelle diffusion de sc�nario. Le contenu de la liste des sc�narios d�pend du p�rim�tre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation priv�e MessagesRight de niveau WRITE est n�cessaire pour �diter un message cr�� par l'utilisateur courant.
				- Une habilitation publique MessagesRight de niveau WRITE est n�cessaire pour �diter un message n'appartenant pas � l'utilisateur courant.
				
			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne g�n�re d'entr�e dans un journal.
		*/
		class MessagesAdmin : public admin::AdminInterfaceElementTemplate<MessagesAdmin>
		{
		public:
			static const std::string CSS_ALARM_DISABLED;
			static const std::string CSS_ALARM_DISPLAYED_WITHOUT_END_DATE;
			static const std::string CSS_ALARM_WILL_BE_DISPLAYED;
			static const std::string CSS_ALARM_DISPLAYED_WITH_END_DATE;

		private:
			typedef enum { ALL_STATUS, BROADCAST_OVER, BROADCAST_RUNNING, BROADCAST_RUNNING_WITH_END, BROADCAST_RUNNING_WITHOUT_END, FUTURE_BROADCAST } StatusSearch;

			time::DateTime										_startDate;
			time::DateTime										_endDate;
			StatusSearch										_searchStatus;
			AlarmLevel											_searchLevel;
			AlarmConflict										_searchConflict;
			html::ActionResultHTMLTable::RequestParameters		_requestParameters;
			html::ActionResultHTMLTable::ResultParameters 		_resultParameters;

			SentMessages					_messages;
			
			server::ParametersMap	_parametersMap;
			
		public:
			static const std::string PARAMETER_SEARCH_START;
			static const std::string PARAMETER_SEARCH_END;
			static const std::string PARAMETER_SEARCH_LEVEL;
			static const std::string PARAMETER_SEARCH_STATUS;
			static const std::string PARAMETER_SEARCH_CONFLICT;

			MessagesAdmin();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			virtual void setFromParametersMap(
				const server::ParametersMap& map,
				bool doDisplayPreparationActions = true
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
			void display(std::ostream& stream, interfaces::VariablesMap& variables) const;

			bool isAuthorized() const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is Home
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
			) const;

			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008					
			*/
			virtual bool isPageVisibleInTree(const AdminInterfaceElement& currentPage) const;

		};
	}
}

#endif // SYNTHESE_MessagesAdmin_H__
