////////////////////////////////////////////////////////////////////////////////
/// MessageAdmin class header.
///	@file MessageAdmin.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_MessageAdmin_H__
#define SYNTHESE_MessageAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;

		/** Ecran d'édition de message.
			@ingroup m17Admin refAdmin

			@image html cap_admin_message.png
			@image latex cap_admin_message.png "Maquette de l'écran d'édition de message" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Message - [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::admin::HomeAdmin
				- Lien vers synthese::messages::MessagesAdmin
				- Texte [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'édition</b> :
				-# <b>Paramètres</b>
					-# <tt>Type</tt> : Choix message complémentaire ou prioritaire. Le changement de choix lance un rafraichissement de la page pour redimensionner les zones de texte du contenu et pour charger la liste des modèles correspondants.
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
				-# <b>Contenu</b>
					-# <tt>Modèle</tt> : Liste de choix permettant de sélectionner un modèle à recopier. Le choix du modèle n'est pas à proprement parler enregistré dans le message : seul le contenu du modèle sera éventuellement copié sans souvenance du modèle utilisé
					-# bouton <tt>Copier contenu</tt> : effectue la copie du texte du modèle dans les champs de messages.
					-# <tt>Message court</tt> : Zone de texte dimensionnée identiquement à l'afficheur cible, permettant de saisir le message à diffuser sur les afficheurs de petite dimension. Champ obligatoire.
					-# <tt>Message long</tt> : Zone de texte dimensionnée identiquement à l'afficheur cible, permettant de saisir le message à diffuser sur les afficheurs de grande dimension. Champ obligatoire.
				-# <b>Diffusion</b>
					-# <tt>Liste des afficheurs</tt> : Tableau rappelant les afficheurs qui recevront le message, désignés par leur emplacement précis. Chaque afficheur est complété par une pastille montrant son état matériel, afin d'éclairer l'utilisateur sur la réelle diffusion du messade qu'il peut attendre. Une infobulle précise la signification de la pastille, et apparait au contact du pointeur de souris.
					-# Bouton <tt>Supprimer</tt> : permet de retirer un afficheur de la liste de diffusion
					-# <tt>Ajout d'afficheur</tt> : le formulaire de recherche permet de sélectionner des afficheurs pouvant être ajoutés à la liste de diffusion à l'aide du bouton <tt>Ajouter les afficheurs sélectionnés</tt>. Cette zone est identique à l'écran de gestion des afficheurs.
				-# Le <b>bouton OK</b> effectue l'envoi après effectuer les contrôles de validité des paramètres. Notamment :
					- au moins un afficheur doit être sélectionné.
					- si au moins un afficheur est en état de panne, un message de confirmation apparait
					- les controles de dates doivent être positifs
					- les messages doivent être non vides


			<i>Sécurité</i>
				- Une habilitation MessagesRight de niveau READ est nécessaire pour visualiser les paramètres d'un message.
				- Une habilitation MessagesRight de niveau WRITE est nécessaire pour pour éditer un message devant partir ou déjà partis.
				- Les listes définissant les points de diffusion du message sont paramétrées par le périmètre des habilitations privées et publiques de niveau WRITE et supérieurs de l'utilisateur :
					- Les arrêts logiques proposés sont les arrêts explicitement autorisés, les arrêts contenant au moins un point de diffusion explicitement autorisé, et les arrêts desservis par au moins une ligne autorisée (ou bien une ligne appartenant à un réseau autorisé)
					- Les emplacements proposés sont les arrêts physiques explicitements autorisés, les arrêts physiques desservis par une ligne autorisée, et les points de diffusion de l'arrêt logique n'étant pas des arrêts physiques
					- Les lignes proposées sont les lignes explicitement autorisées, ainsi que l'ensemble des lignes des réseaux explicitement autorisés.

			<i>Journaux</i> : Les événements suivants entrainent la création d'une entrée dans le journal des messages MessagesLog :
				- INFO : Diffusion de message
				- INFO : Modification de message en cours de diffusion
				- WARNING : Diffusion de message sur un afficheur signalé hors service

		*/
		class MessageAdmin : public admin::AdminInterfaceElementTemplate<MessageAdmin>
		{
		public:
			static const std::string TAB_PARAMS;
			static const std::string TAB_DATASOURCES;

		private:
			boost::shared_ptr<const Alarm>	_alarm;
			util::ParametersMap			_parameters;

		public:
			MessageAdmin();

			boost::shared_ptr<const Alarm>	getAlarm() const;
			void setMessage(boost::shared_ptr<const Alarm> value);

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;




			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;

			virtual void _buildTabs(
				const security::Profile& profile
			) const;

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



			virtual std::string getTitle() const;

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

#endif // SYNTHESE_MessageAdmin_H__
