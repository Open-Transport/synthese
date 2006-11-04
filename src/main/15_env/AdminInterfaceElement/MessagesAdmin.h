
#ifndef SYNTHESE_ENV_MESSAGES_ADMIN_H
#define SYNTHESE_ENV_MESSAGES_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran de recherche et de liste de message.
			@ingroup m15

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
						- Pastille orange : au moins un afficheur est en état Warning, aucun n'est en état Error
						- Pastille rouge : au moins un afficheur est en état Warning
					-# Bouton <tt>Editer</tt> : conduit à la page synthese::interfaces::MessageAdmin sur le message correspondant à la ligne où est présent le bouton
					-# Un clic sur les titres de colonnes effectue un tri croissant puis décroissant sur la colonne sélectionnée
				-# Bouton <b>Ajouter</b> : ouvre la page synthese::interfaces::MessageAdmin :
					- sur un message vierge si aucun message n'est sélectionné dans la colonne <tt>Sel</tt>
					- sur un message recopié sur un autre, sélectionné dans la colonne <tt>Sel</tt>
				-# Un maximum de 50 messages est affiché à l'écran. En cas de dépassement de ce nombre d'après les critères de recherche, un lien <tt>Messages suivants</tt> apparait et permet de visualiser les entrées suivantes. A partir de la seconde page, un lien <tt>Messages précédents</tt> apparait également.
			
			
			<i>Sécurité</i>
				- Une habilitation MessagesRight de niveau READ est nécessaire pour accéder à la page.
				- Le résultat d'une recherche dépend du périmètre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation MessagesRight de niveau WRITE_BELONG est nécessaire pour créer un nouveau message ou une nouvelle diffusion de scénario.
				- Le contenu de la liste des scénarios dépend du périmètre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation MessagesRight de niveau WRITE_BELONG est nécessaire pour éditer un message créé par l'utilisateur.
				- Une habilitation MessagesRight de niveau WRITE est nécessaire pour éditer un message n'appartenant pas à l'utilisateur.
				
			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne génère d'entrée dans un journal.

		*/
		class MessagesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif