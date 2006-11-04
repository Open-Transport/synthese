
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
						- Pastille orange : au moins un afficheur est en �tat Warning, aucun n'est en �tat Error
						- Pastille rouge : au moins un afficheur est en �tat Warning
					-# Bouton <tt>Editer</tt> : conduit � la page synthese::interfaces::MessageAdmin sur le message correspondant � la ligne o� est pr�sent le bouton
					-# Un clic sur les titres de colonnes effectue un tri croissant puis d�croissant sur la colonne s�lectionn�e
				-# Bouton <b>Ajouter</b> : ouvre la page synthese::interfaces::MessageAdmin :
					- sur un message vierge si aucun message n'est s�lectionn� dans la colonne <tt>Sel</tt>
					- sur un message recopi� sur un autre, s�lectionn� dans la colonne <tt>Sel</tt>
				-# Un maximum de 50 messages est affich� � l'�cran. En cas de d�passement de ce nombre d'apr�s les crit�res de recherche, un lien <tt>Messages suivants</tt> apparait et permet de visualiser les entr�es suivantes. A partir de la seconde page, un lien <tt>Messages pr�c�dents</tt> apparait �galement.
			
			
			<i>S�curit�</i>
				- Une habilitation MessagesRight de niveau READ est n�cessaire pour acc�der � la page.
				- Le r�sultat d'une recherche d�pend du p�rim�tre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation MessagesRight de niveau WRITE_BELONG est n�cessaire pour cr�er un nouveau message ou une nouvelle diffusion de sc�nario.
				- Le contenu de la liste des sc�narios d�pend du p�rim�tre des habilitations MessagesRight de l'utilisateur.
				- Une habilitation MessagesRight de niveau WRITE_BELONG est n�cessaire pour �diter un message cr�� par l'utilisateur.
				- Une habilitation MessagesRight de niveau WRITE est n�cessaire pour �diter un message n'appartenant pas � l'utilisateur.
				
			<i>Journaux</i>
				- Aucune action issue de ce composant d'administration ne g�n�re d'entr�e dans un journal.

		*/
		class MessagesAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif