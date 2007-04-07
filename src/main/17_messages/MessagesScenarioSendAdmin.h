
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

		/** Ecran d'envoi de messages selon un scénario préétabli.
			@ingroup m17

			@image html cap_admin_scenario_send.png
			@image latex cap_admin_scenario_send.png "Maquette de l'écran d'envoi de messages selon un scénario" width=14cm
			
			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Messages - [Nom] (scénario)

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Texte [Nom] + <tt>(scénario)</tt>

			<i>Zone de contenu</i> : <b>Formulaire d'édition</b> :
				-# <b>Paramètres</b>
					-# <tt>Début diffusion : Date</tt> : Champ texte obligatoire représentant le jour d'envoi du message. Un champ <tt>Début de diffusion : Date</tt> vide ou mal rempli entraîne un message d'erreur empêchant la diffusion :
						@code Le message ne peut être diffusé car la date de début est incorrecte @endcode
						Le champ <tt>Début diffusion : Date</tt> est par défaut rempli par la date courante.
					-# <tt>Début diffusion : Heure</tt> : Champ texte facultatif représentant l'heure exacte d'envoi du message. En cas de non remplissage du champ, le message est envoyé dès minuit. Un champ <tt>Début de diffusion : Heure</tt> mal rempli entraîne un message d'erreur empêchant la diffusion :
						@code Le message ne peut être diffusé car l'heure de début est incorrecte @endcode
						Le champ <tt>Début diffusion : Heure</tt> est par défaut rempli par l'heure courante.
					-# <tt>Fin diffusion</tt> : Champs textes facultatifs représentant la date et l'heure de fin de diffusion du message.
						- si les champs sont saisis et correctement renseignés, la diffusion est possible
						- si un des champs est saisi et mal renseigné, un message d'erreur empeche la diffusion :
							@code Le message ne peut être diffusé car la date de fin est incorrecte @endcode
							@code Le message ne peut être diffusé car l'heure de fin est incorrecte @endcode
						- si le champ est renseigné par une date antérieure à la date de début, un message d'erreur empêche la diffusion :
							@code Le message ne peut être diffusé car la date de fin est antérieure à la date de début. @endcode
						- si le champ n'est pas renseigné, un message de confirmation est proposé lors de la diffusion, prévenant l'utilisateur que sans action de sa part ce message restera en vigueur indéfiniment
						- si le seul champ <tt>Fin diffusion : Heure</tt> est seul rempli, un message d'erreur empêche la diffusion :
							@code Le message ne peut être diffusé car la date de fin est incorrecte @endcode
					-# Bouton <tt>Appliquer</tt> : Applique les dates de début et de fin à tous les messages du scénario (qui restent modifiables unitairement par la suite). Les contrôles de cohérence de dates sont effectués lors de l'activation du bouton Appliquer.
					-# Bouton <tt>Envoyer</tt> : Diffuse l'ensemble des messages figurant dans la liste après un message de confirmation. Ce bouton n'apparaît que lorsque l'on a appliqué une fois des dates de diffusion. Les contrôles suivants sont effectués :
						- au moins un afficheur doit être sélectionné.
						- si au moins un afficheur est en état de panne, un message de confirmation apparait
						- les controles de dates doivent être positifs
						- les messages doivent être non vides

				-# <b>Contenu</b> : reprend les messages préparés par le scénario (il s'agit de copies) avec possibilité d'édition pour personnaliser l'envoi.	
					-# <tt>Sel</tt> : case de sélection permettant d'effectuer une copie du message
					-# <tt>Message</tt> : extrait des messages diffusés. Un clic sur l'extrait conduit à la page synthese::interfaces::MessageAdmin sur le message sélectionné
					-# <tt>Emplacements</tt> : résumé des emplacements de diffusion du message sous forme de liste d'arrêts logiques
					-# <tt>Etat</tt> : état matériel courant des afficheurs de la liste de diffusion du message. Une infobulle précise l'état représenté, apparaissant au contact avec le pointeur de souris
						- Pastille verte : tous les afficheurs sont en fonctionnement normal
						- Pastille orange : au moins un afficheur est en état WARNING, aucun n'est en état ERROR
						- Pastille rouge : au moins un afficheur est en état ERROR
					-# <tt>Conflit</tt> : indique si le message est en conflit avec un autre sur au moins un des afficheurs. Une pastille colorée donne l'information et affiche une infobulle lorsque le pointeur de souris est positionné au dessus de celle-ci :
						- Pastille verte : le message est en conflit sur aucun afficheur
						- Pastille orange : le message prioritaire "écrase" un message complémentaire
						- Pastille rouge : le message entre en conflit avec un message de même niveau
					-# <tt>Modifier</tt> : conduit à la page @ref synthese::interfaces::MessageAdmin "d'édition du message" correspondant à la ligne où est présent le bouton. NB : les modifications effectuées sur ce message n'affecteront que la copie utilisée pour cet envoi et en aucun cas le sécnario sauvegardé en bibliothèque.
					-# <tt>Supprimer</tt> : permet de retirer le message de l'envoi. NB : cela ne supprime en aucun cas le message du scénario de la bibliothèque
					-# <tt>Ajouter</tt> : permet d'ajouter un nouveau message à l'envoi. Si un message existant est sélectionné le nouveau message correspond à une copie.
				-# Le <b>bouton Arrêter</b> interrompt la diffusion de tous les messages du scénario en remplaçant la date de fin des messages par la date courante, après un message de confirmation.
				
			
			<i>Sécurité</i>
				- Une habilitation privée MessagesRight de niveau READ est nécessaire pour accéder à la page en consultation pour visualiser un envoi effectué par l'utilisateur courant
				- Une habilitation publique MessagesRight de niveau READ est nécessaire pour accéder à la page en consultation pour visualiser un envoi non effectué par l'utilisateur courant
				- Une habilitation privée MessagesRight de niveau WRITE est nécessaire pour accéder à la page pour créer une envoi et pour éditer un envoi effectué par l'utilisateur courant
				- Une habilitation publique MessagesRight de niveau WRITE est nécessaire pour accéder à la page pour éditer un envoi non effectué par l'utilisateur courant

			<i>Journaux</i> : Les événements suivants entrainent la création d'une entrée dans le journal des messages de l'environnement :
				- INFO : Diffusion selon scénario
				- INFO : Modification de scénario en cours de diffusion
				- WARNING : Diffusion selon scénario sur au moins un afficheur déclaré hors service

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