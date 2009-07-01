////////////////////////////////////////////////////////////////////////////////
/// MessageAdmin class header.
///	@file MessageAdmin.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

		/** Ecran d'�dition de message.
			@ingroup m17Admin refAdmin

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
				-# <b>Contenu</b>
					-# <tt>Mod�le</tt> : Liste de choix permettant de s�lectionner un mod�le � recopier. Le choix du mod�le n'est pas � proprement parler enregistr� dans le message : seul le contenu du mod�le sera �ventuellement copi� sans souvenance du mod�le utilis�
					-# bouton <tt>Copier contenu</tt> : effectue la copie du texte du mod�le dans les champs de messages.
					-# <tt>Message court</tt> : Zone de texte dimensionn�e identiquement � l'afficheur cible, permettant de saisir le message � diffuser sur les afficheurs de petite dimension. Champ obligatoire.
					-# <tt>Message long</tt> : Zone de texte dimensionn�e identiquement � l'afficheur cible, permettant de saisir le message � diffuser sur les afficheurs de grande dimension. Champ obligatoire.
				-# <b>Diffusion</b>
					-# <tt>Liste des afficheurs</tt> : Tableau rappelant les afficheurs qui recevront le message, d�sign�s par leur emplacement pr�cis. Chaque afficheur est compl�t� par une pastille montrant son �tat mat�riel, afin d'�clairer l'utilisateur sur la r�elle diffusion du messade qu'il peut attendre. Une infobulle pr�cise la signification de la pastille, et apparait au contact du pointeur de souris.
					-# Bouton <tt>Supprimer</tt> : permet de retirer un afficheur de la liste de diffusion
					-# <tt>Ajout d'afficheur</tt> : le formulaire de recherche permet de s�lectionner des afficheurs pouvant �tre ajout�s � la liste de diffusion � l'aide du bouton <tt>Ajouter les afficheurs s�lectionn�s</tt>. Cette zone est identique � l'�cran de gestion des afficheurs.
				-# Le <b>bouton OK</b> effectue l'envoi apr�s effectuer les contr�les de validit� des param�tres. Notamment :
					- au moins un afficheur doit �tre s�lectionn�.
					- si au moins un afficheur est en �tat de panne, un message de confirmation apparait
					- les controles de dates doivent �tre positifs
					- les messages doivent �tre non vides

			
			<i>S�curit�</i>
				- Une habilitation MessagesRight de niveau READ est n�cessaire pour visualiser les param�tres d'un message.
				- Une habilitation MessagesRight de niveau WRITE est n�cessaire pour pour �diter un message devant partir ou d�j� partis.
				- Les listes d�finissant les points de diffusion du message sont param�tr�es par le p�rim�tre des habilitations priv�es et publiques de niveau WRITE et sup�rieurs de l'utilisateur :
					- Les arr�ts logiques propos�s sont les arr�ts explicitement autoris�s, les arr�ts contenant au moins un point de diffusion explicitement autoris�, et les arr�ts desservis par au moins une ligne autoris�e (ou bien une ligne appartenant � un r�seau autoris�)
					- Les emplacements propos�s sont les arr�ts physiques explicitements autoris�s, les arr�ts physiques desservis par une ligne autoris�e, et les points de diffusion de l'arr�t logique n'�tant pas des arr�ts physiques
					- Les lignes propos�es sont les lignes explicitement autoris�es, ainsi que l'ensemble des lignes des r�seaux explicitement autoris�s.
				
			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal des messages MessagesLog :
				- INFO : Diffusion de message
				- INFO : Modification de message en cours de diffusion
				- WARNING : Diffusion de message sur un afficheur signal� hors service

		*/
		class MessageAdmin : public admin::AdminInterfaceElementTemplate<MessageAdmin>
		{
		public:
			static const std::string TAB_PARAMS;

		private:
			boost::shared_ptr<const Alarm>	_alarm;
			server::ParametersMap			_parameters;

		public:
			MessageAdmin();

			boost::shared_ptr<const Alarm>	getAlarm() const;
			void setMessage(boost::shared_ptr<const Alarm> value);

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doDisplayPreparationActions = true
			);
			
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
			virtual server::ParametersMap getParametersMap() const;

			virtual void _buildTabs() const;

			/** Display of the content of the admin element.
				@param stream Stream to write on.
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables) const;

			bool isAuthorized() const;



			virtual std::string getTitle() const;
			
			/** Gets the opening position of the node in the tree view.
				@return Always visible
				@author Hugues Romain
				@date 2008					
			*/
			virtual bool isPageVisibleInTree(const AdminInterfaceElement& currentPage) const;
		};
	}
}

#endif // SYNTHESE_MessageAdmin_H__
