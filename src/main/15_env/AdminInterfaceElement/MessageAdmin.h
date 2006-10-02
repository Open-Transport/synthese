
#ifndef SYNTHESE_ENV_MESSAGE_ADMIN_H
#define SYNTHESE_ENV_MESSAGE_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'édition de message.
			@ingroup m15

			@image html cap_admin_message.png
			@image latex cap_admin_message.png "Maquette de l'écran d'édition de message" width=14cm
			
			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Message - [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Texte [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'édition</b> :
				-# <b>Paramètres</b>
					-# <tt>Type</tt> : Choix message complémentaire ou prioritaire. Le changement de choix lance un rafraichissement de la page pour redimensionner les zones de texte du contenu et pour charger la liste des modèles correspondants.
					-# <tt>Date début</tt> : Champ texte obligatoire représentant la date d'envoi à la minute près. Un champ date début vide ou mal formé entraine un message d'erreur empêchant la diffusion :
						@code Le message ne peut être diffusé car la date de début est incorrecte @endcode
					-# <tt>Date fin</tt> : Champ texte facultatif représentant la date de fin d'envoi à la minute près.
						- si le champ est saisi et correctement renseigné, la diffusion est possible
						- si le champ est saisi et mal renseigné, un message d'erreur empeche la diffusion :
							@code Le message ne peut être diffusé car la date de fin est incorrecte @endcode
						- si le champ est renseigné par une date antérieure à la date de début, un message d'erreur empêche la diffusion :
							@code Le message ne peut être diffusé car la date de fin est antérieure à la date de début. @endcode
						- si le champ n'est pas renseigné, un message de confirmation est proposé lors de la diffusion, prévenant l'utilisateur que sans action de sa part ce message restera en vigueur indéfiniment
				-# <b>Contenu</b>
					-# <tt>Modèle</tt> : Liste de choix permettant de sélectionner un modèle à recopier. Le choix du modèle n'est pas à proprement parler enregistré dans le message : seul le contenu du modèle sera éventuellement copié sans souvenance du modèle utilisé
					-# bouton <tt>Copier contenu</tt> : effectue la copie du texte du modèle dans les champs de messages.
					-# <tt>Message court</tt> : Zone de texte dimensionnée identiquement à l'afficheur cible, permettant de saisir le message à diffuser sur les afficheurs de petite dimension. Champ obligatoire.
					-# <tt>Message long</tt> : Zone de texte dimensionnée identiquement à l'afficheur cible, permettant de saisir le message à diffuser sur les afficheurs de grande dimension. Champ obligatoire.
				-# <b>Diffusion</b>
					-# <tt>Liste des afficheurs</tt> : Tableau rappelant les afficheurs qui recevront le message, désignés par leur emplacement précis. Chaque afficheur est complété par une pastille montrant son état matériel, afin d'éclairer l'utilisateur sur la réelle diffusion du messade qu'il peut attendre. Une infobulle précise la signification de la pastille, et apparait au contact du pointeur de souris.
					-# Bouton <tt>Supprimer</tt> : permet de retirer un afficheur de la liste de diffusion
					-# <tt>Arrêt logique</tt> : liste de choix permettant de sélectionner un arrêt logique comme source d'afficheurs destinataires. Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute à la liste de diffusion tous les afficheurs présents dans l'arrêt logique sélectionné. Le simple changement d'arrêt dans la liste entraine le rafraichissement de la page pour mettre à jour la liste des emplacements physiques.
					-# <tt>Emplacement</tt> : liste de choix permettant de sélectionner un emplacement comme source d'afficheurs destinataires. Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute à la liste de diffusion tous les afficheurs présents au niveau de l'emplacement sélectionné. Le simple changement d'emplacement dans la liste entraine le rafraichissement de la page pour mettre à jour la liste des compléments de précision d'emplacement.
					-# <tt>Complément</tt> : liste de choix permettant de sélectionner un afficheur unitairement comme source d'afficheurs destinataires. Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute à la liste de diffusion l'afficheur sélectionné.
					-# <tt>Ligne</tt> : liste de choix permettant de sélectionner une ligne (toutes les lignes desservant au moins un arrêt contenant au moins un afficheur sont présentes dans la liste). Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute à la liste de diffusion tous les afficheurs desservis par la ligne
				-# Le <b>bouton Envoyer</b> effectue l'envoi après effectuer les contrôles de validité des paramètres. Notamment :
					- au moins un afficheur doit être sélectionné.
					- si au moins un afficheur est en état de panne, un message de confirmation apparait
					- les controles de dates doivent être positifs
					- les messages doivent être non vides

			
			<i>Sécurité</i>
				- Une habilitation de niveau écriture sur le module environnement et sur l'opération messages est nécessaire pour éditer un message devant partir ou déjà parti.
				- Une habilitation de niveau lecture sur le module environnement et sur l'opération messages est nécessaire pour visualiser un message.

			<i>Journaux</i> : Les événements suivants entrainent la création d'une entrée dans le journal des messages de l'environnement :
				- Diffusion de message
				- Modification de message en cours de diffusion

		*/
		class MessageAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif