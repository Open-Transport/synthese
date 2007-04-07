
/** MessagesScenarioSendAdmin class header.
	@file MessagesScenarioSendAdmin.h

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

#ifndef SYNTHESE_MessagesScenarioSendAdmin_H__
#define SYNTHESE_MessagesScenarioSendAdmin_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace messages
	{
		class Scenario;

		/** Ecran d'envoi de messages selon un sc�nario pr��tabli.
			@ingroup m17

			@image html cap_admin_scenario_send.png
			@image latex cap_admin_scenario_send.png "Maquette de l'�cran d'envoi de messages selon un sc�nario" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Messages - [Nom] (sc�nario)

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Texte [Nom] + <tt>(sc�nario)</tt>

			<i>Zone de contenu</i> : <b>Formulaire d'�dition</b> :
				-# <b>Param�tres</b>
					-# <tt>D�but diffusion : Date</tt> : Champ texte obligatoire repr�sentant le jour d'envoi du message. Un champ <tt>D�but de diffusion : Date</tt> vide ou mal rempli entra�ne un message d'erreur emp�chant la diffusion :
						@code Le message ne peut �tre diffus� car la date de d�but est incorrecte @endcode
						Le champ <tt>D�but diffusion : Date</tt> est par d�faut rempli par la date courante.
					-# <tt>D�but diffusion : Heure</tt> : Champ texte facultatif repr�sentant l'heure exacte d'envoi du message. En cas de non remplissage du champ, le message est envoy� d�s minuit. Un champ <tt>D�but de diffusion : Heure</tt> mal rempli entra�ne un message d'erreur emp�chant la diffusion :
						@code Le message ne peut �tre diffus� car l'heure de d�but est incorrecte @endcode
						Le champ <tt>D�but diffusion : Heure</tt> est par d�faut rempli par l'heure courante.
					-# <tt>Fin diffusion</tt> : Champs textes facultatifs repr�sentant la date et l'heure de fin de diffusion du message.
						- si les champs sont saisis et correctement renseign�s, la diffusion est possible
						- si un des champs est saisi et mal renseign�, un message d'erreur empeche la diffusion :
							@code Le message ne peut �tre diffus� car la date de fin est incorrecte @endcode
							@code Le message ne peut �tre diffus� car l'heure de fin est incorrecte @endcode
						- si le champ est renseign� par une date ant�rieure � la date de d�but, un message d'erreur emp�che la diffusion :
							@code Le message ne peut �tre diffus� car la date de fin est ant�rieure � la date de d�but. @endcode
						- si le champ n'est pas renseign�, un message de confirmation est propos� lors de la diffusion, pr�venant l'utilisateur que sans action de sa part ce message restera en vigueur ind�finiment
						- si le seul champ <tt>Fin diffusion : Heure</tt> est seul rempli, un message d'erreur emp�che la diffusion :
							@code Le message ne peut �tre diffus� car la date de fin est incorrecte @endcode
					-# Bouton <tt>Appliquer</tt> : Applique les dates de d�but et de fin � tous les messages du sc�nario (qui restent modifiables unitairement par la suite). Les contr�les de coh�rence de dates sont effectu�s lors de l'activation du bouton Appliquer.
					-# Bouton <tt>Envoyer</tt> : Diffuse l'ensemble des messages figurant dans la liste apr�s un message de confirmation. Ce bouton n'appara�t que lorsque l'on a appliqu� une fois des dates de diffusion. Les contr�les suivants sont effectu�s :
						- au moins un afficheur doit �tre s�lectionn�.
						- si au moins un afficheur est en �tat de panne, un message de confirmation apparait
						- les controles de dates doivent �tre positifs
						- les messages doivent �tre non vides

				-# <b>Contenu</b> : reprend les messages pr�par�s par le sc�nario (il s'agit de copies) avec possibilit� d'�dition pour personnaliser l'envoi.	
					-# <tt>Sel</tt> : case de s�lection permettant d'effectuer une copie du message
					-# <tt>Message</tt> : extrait des messages diffus�s. Un clic sur l'extrait conduit � la page synthese::interfaces::MessageAdmin sur le message s�lectionn�
					-# <tt>Emplacements</tt> : r�sum� des emplacements de diffusion du message sous forme de liste d'arr�ts logiques
					-# <tt>Etat</tt> : �tat mat�riel courant des afficheurs de la liste de diffusion du message. Une infobulle pr�cise l'�tat repr�sent�, apparaissant au contact avec le pointeur de souris
						- Pastille verte : tous les afficheurs sont en fonctionnement normal
						- Pastille orange : au moins un afficheur est en �tat WARNING, aucun n'est en �tat ERROR
						- Pastille rouge : au moins un afficheur est en �tat ERROR
					-# <tt>Conflit</tt> : indique si le message est en conflit avec un autre sur au moins un des afficheurs. Une pastille color�e donne l'information et affiche une infobulle lorsque le pointeur de souris est positionn� au dessus de celle-ci :
						- Pastille verte : le message est en conflit sur aucun afficheur
						- Pastille orange : le message prioritaire "�crase" un message compl�mentaire
						- Pastille rouge : le message entre en conflit avec un message de m�me niveau
					-# <tt>Modifier</tt> : conduit � la page @ref synthese::interfaces::MessageAdmin "d'�dition du message" correspondant � la ligne o� est pr�sent le bouton. NB : les modifications effectu�es sur ce message n'affecteront que la copie utilis�e pour cet envoi et en aucun cas le s�cnario sauvegard� en biblioth�que.
					-# <tt>Supprimer</tt> : permet de retirer le message de l'envoi. NB : cela ne supprime en aucun cas le message du sc�nario de la biblioth�que
					-# <tt>Ajouter</tt> : permet d'ajouter un nouveau message � l'envoi. Si un message existant est s�lectionn� le nouveau message correspond � une copie.
				-# Le <b>bouton Arr�ter</b> interrompt la diffusion de tous les messages du sc�nario en rempla�ant la date de fin des messages par la date courante, apr�s un message de confirmation.
				
			
			<i>S�curit�</i>
				- Une habilitation priv�e MessagesRight de niveau READ est n�cessaire pour acc�der � la page en consultation pour visualiser un envoi effectu� par l'utilisateur courant
				- Une habilitation publique MessagesRight de niveau READ est n�cessaire pour acc�der � la page en consultation pour visualiser un envoi non effectu� par l'utilisateur courant
				- Une habilitation priv�e MessagesRight de niveau WRITE est n�cessaire pour acc�der � la page pour cr�er une envoi et pour �diter un envoi effectu� par l'utilisateur courant
				- Une habilitation publique MessagesRight de niveau WRITE est n�cessaire pour acc�der � la page pour �diter un envoi non effectu� par l'utilisateur courant

			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal des messages de l'environnement :
				- INFO : Diffusion selon sc�nario
				- INFO : Modification de sc�nario en cours de diffusion
				- WARNING : Diffusion selon sc�nario sur au moins un afficheur d�clar� hors service

		*/
		class MessagesScenarioSendAdmin : public admin::AdminInterfaceElement
		{
		private:
			Scenario*	_scenario;

		public:
			MessagesScenarioSendAdmin();
			
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

#endif // SYNTHESE_MessagesScenarioSendAdmin_H__