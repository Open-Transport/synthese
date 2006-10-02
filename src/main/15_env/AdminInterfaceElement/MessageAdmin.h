
#ifndef SYNTHESE_ENV_MESSAGE_ADMIN_H
#define SYNTHESE_ENV_MESSAGE_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'�dition de message.
			@ingroup m15

			@image html cap_admin_message.png
			@image latex cap_admin_message.png "Maquette de l'�cran d'�dition de message" width=14cm
			
			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Message - [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::AdminHome
				- Lien vers synthese::interfaces::MessagesAdmin
				- Texte [Nom]

			<i>Zone de contenu</i> : <b>Formulaire d'�dition</b> :
				-# <b>Param�tres</b>
					-# <tt>Type</tt> : Choix message compl�mentaire ou prioritaire. Le changement de choix lance un rafraichissement de la page pour redimensionner les zones de texte du contenu et pour charger la liste des mod�les correspondants.
					-# <tt>Date d�but</tt> : Champ texte obligatoire repr�sentant la date d'envoi � la minute pr�s. Un champ date d�but vide ou mal form� entraine un message d'erreur emp�chant la diffusion :
						@code Le message ne peut �tre diffus� car la date de d�but est incorrecte @endcode
					-# <tt>Date fin</tt> : Champ texte facultatif repr�sentant la date de fin d'envoi � la minute pr�s.
						- si le champ est saisi et correctement renseign�, la diffusion est possible
						- si le champ est saisi et mal renseign�, un message d'erreur empeche la diffusion :
							@code Le message ne peut �tre diffus� car la date de fin est incorrecte @endcode
						- si le champ est renseign� par une date ant�rieure � la date de d�but, un message d'erreur emp�che la diffusion :
							@code Le message ne peut �tre diffus� car la date de fin est ant�rieure � la date de d�but. @endcode
						- si le champ n'est pas renseign�, un message de confirmation est propos� lors de la diffusion, pr�venant l'utilisateur que sans action de sa part ce message restera en vigueur ind�finiment
				-# <b>Contenu</b>
					-# <tt>Mod�le</tt> : Liste de choix permettant de s�lectionner un mod�le � recopier. Le choix du mod�le n'est pas � proprement parler enregistr� dans le message : seul le contenu du mod�le sera �ventuellement copi� sans souvenance du mod�le utilis�
					-# bouton <tt>Copier contenu</tt> : effectue la copie du texte du mod�le dans les champs de messages.
					-# <tt>Message court</tt> : Zone de texte dimensionn�e identiquement � l'afficheur cible, permettant de saisir le message � diffuser sur les afficheurs de petite dimension. Champ obligatoire.
					-# <tt>Message long</tt> : Zone de texte dimensionn�e identiquement � l'afficheur cible, permettant de saisir le message � diffuser sur les afficheurs de grande dimension. Champ obligatoire.
				-# <b>Diffusion</b>
					-# <tt>Liste des afficheurs</tt> : Tableau rappelant les afficheurs qui recevront le message, d�sign�s par leur emplacement pr�cis. Chaque afficheur est compl�t� par une pastille montrant son �tat mat�riel, afin d'�clairer l'utilisateur sur la r�elle diffusion du messade qu'il peut attendre. Une infobulle pr�cise la signification de la pastille, et apparait au contact du pointeur de souris.
					-# Bouton <tt>Supprimer</tt> : permet de retirer un afficheur de la liste de diffusion
					-# <tt>Arr�t logique</tt> : liste de choix permettant de s�lectionner un arr�t logique comme source d'afficheurs destinataires. Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute � la liste de diffusion tous les afficheurs pr�sents dans l'arr�t logique s�lectionn�. Le simple changement d'arr�t dans la liste entraine le rafraichissement de la page pour mettre � jour la liste des emplacements physiques.
					-# <tt>Emplacement</tt> : liste de choix permettant de s�lectionner un emplacement comme source d'afficheurs destinataires. Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute � la liste de diffusion tous les afficheurs pr�sents au niveau de l'emplacement s�lectionn�. Le simple changement d'emplacement dans la liste entraine le rafraichissement de la page pour mettre � jour la liste des compl�ments de pr�cision d'emplacement.
					-# <tt>Compl�ment</tt> : liste de choix permettant de s�lectionner un afficheur unitairement comme source d'afficheurs destinataires. Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute � la liste de diffusion l'afficheur s�lectionn�.
					-# <tt>Ligne</tt> : liste de choix permettant de s�lectionner une ligne (toutes les lignes desservant au moins un arr�t contenant au moins un afficheur sont pr�sentes dans la liste). Un clic sur le bouton <tt>Ajouter</tt> correspondant ajoute � la liste de diffusion tous les afficheurs desservis par la ligne
				-# Le <b>bouton Envoyer</b> effectue l'envoi apr�s effectuer les contr�les de validit� des param�tres. Notamment :
					- au moins un afficheur doit �tre s�lectionn�.
					- si au moins un afficheur est en �tat de panne, un message de confirmation apparait
					- les controles de dates doivent �tre positifs
					- les messages doivent �tre non vides

			
			<i>S�curit�</i>
				- Une habilitation de niveau �criture sur le module environnement et sur l'op�ration messages est n�cessaire pour �diter un message devant partir ou d�j� parti.
				- Une habilitation de niveau lecture sur le module environnement et sur l'op�ration messages est n�cessaire pour visualiser un message.

			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal des messages de l'environnement :
				- Diffusion de message
				- Modification de message en cours de diffusion

		*/
		class MessageAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif