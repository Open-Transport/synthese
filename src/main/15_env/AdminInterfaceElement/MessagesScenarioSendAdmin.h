
#ifndef SYNTHESE_ENV_MESSAGE_ADMIN_H
#define SYNTHESE_ENV_MESSAGE_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'envoi de messages selon un scénario préétabli.
			@ingroup m15

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
				-# <b>Contenu</b> : reprend les messages préparés par le scénario (copies) avec possibilité d'édition pour personnaliser l'envoi
					-# <tt>Modèle</tt> : Liste de choix permettant de sélectionner un modèle à recopier. Le choix du modèle n'est pas à proprement parler enregistré dans le message : seul le contenu du modèle sera éventuellement copié sans souvenance du modèle utilisé
					-# bouton <tt>Copier contenu</tt> : effectue la copie du texte du modèle dans les champs de messages.
					-# <tt>Message court</tt> : Zone de texte dimensionnée identiquement à l'afficheur cible, permettant de saisir le message à diffuser sur les afficheurs de petite dimension. Champ obligatoire.
					-# <tt>Message long</tt> : Zone de texte dimensionnée identiquement à l'afficheur cible, permettant de saisir le message à diffuser sur les afficheurs de grande dimension. Champ obligatoire.
				-# Le <b>bouton Envoyer</b> effectue l'envoi après effectuer les contrôles de validité des paramètres. Notamment :
					- au moins un afficheur doit être sélectionné.
					- si au moins un afficheur est en état de panne, un message de confirmation apparait
					- les controles de dates doivent être positifs
					- les messages doivent être non vides

			
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
		class MessagesScenarioSendAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif