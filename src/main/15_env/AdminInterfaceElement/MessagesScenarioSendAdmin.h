
#ifndef SYNTHESE_ENV_MESSAGE_ADMIN_H
#define SYNTHESE_ENV_MESSAGE_ADMIN_H

#include "11_interfaces/AdminInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{

		/** Ecran d'envoi de messages selon un sc�nario pr��tabli.
			@ingroup m15

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
				-# <b>Contenu</b> : reprend les messages pr�par�s par le sc�nario (copies) avec possibilit� d'�dition pour personnaliser l'envoi
					-# <tt>Mod�le</tt> : Liste de choix permettant de s�lectionner un mod�le � recopier. Le choix du mod�le n'est pas � proprement parler enregistr� dans le message : seul le contenu du mod�le sera �ventuellement copi� sans souvenance du mod�le utilis�
					-# bouton <tt>Copier contenu</tt> : effectue la copie du texte du mod�le dans les champs de messages.
					-# <tt>Message court</tt> : Zone de texte dimensionn�e identiquement � l'afficheur cible, permettant de saisir le message � diffuser sur les afficheurs de petite dimension. Champ obligatoire.
					-# <tt>Message long</tt> : Zone de texte dimensionn�e identiquement � l'afficheur cible, permettant de saisir le message � diffuser sur les afficheurs de grande dimension. Champ obligatoire.
				-# Le <b>bouton Envoyer</b> effectue l'envoi apr�s effectuer les contr�les de validit� des param�tres. Notamment :
					- au moins un afficheur doit �tre s�lectionn�.
					- si au moins un afficheur est en �tat de panne, un message de confirmation apparait
					- les controles de dates doivent �tre positifs
					- les messages doivent �tre non vides

			
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
		class MessagesScenarioSendAdmin : public AdminInterfaceElement
		{

		};
	}
}

#endif