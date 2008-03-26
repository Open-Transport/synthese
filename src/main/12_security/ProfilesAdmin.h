
/** ProfilesAdmin class header.
	@file ProfilesAdmin.h

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

#ifndef SYNTHESE_PROFILES_ADMIN_H
#define SYNTHESE_PROFILES_ADMIN_H

#include "05_html/ResultHTMLTable.h"

#include "32_admin/AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Ecran de gestion des profils utilisateurs.
			@ingroup m12Admin refAdmin

			@image html cap_admin_users_profiles.png
			@image latex cap_admin_users_profiles.png "Maquette de l'�cran d'administration des profils utilisateurs" width=14cm

			La <i>Barre de navigation</i> propose deux liens :
				- vers la page d'accueil du module d'administration
				- vers la page de gestion des utilisateurs

			<i>Zone de contenu</i> : L'�cran de gestion des profils utilisateurs est constitu� de trois parties :
				-# <b>formulaire de recherche</b> comportant deux crit�res :
					-# Recherche sur le nom
					-# Recherche sur une habilitation : seuls les profils comportant une habilitation de la classe sp�cifi�e ou h�ritant d'un tel profil sont affich�es.
				-# <b>Tableau des r�sultats</b> dont chaque ligne repr�sente un profil utilisateurs, constitu� des colonnes suivantes :
					-# <tt>Sel</tt> : Colonne de s�lection : permet � l'utilisateur de s�lectionner un ou plusieurs profils pour les supprimer ou pour les copier
					-# <tt>Nom</tt> : Nom du profil
					-# <tt>R�sum�</tt> : R�sum� des habilitations associ�es au profil. Ce r�sum� est une liste � puces :
						-# le profil parent
						-# chaque habilitation, d�crite par :
							-# le niveau de droit
							-# le statut public/priv�
							-# la d�signation de l'habilitation
							-# le p�rim�tre de l'habilitation (selon sp�cificit�s de l'habilitation)
					-# bouton <tt>Modifier</tt> dans les lignes de profil existant, si le profil est modifiable
					-# bouton <tt>Ajouter</tt> : permet de cr�er un nouveau profil. Un nom non vide doit �tre entr�.
				-# <b>Bouton <tt>Supprimer</tt></b> : permet de supprimer les profils s�lectionn�s, uniquement si ceux-ci ne sont pas utilis�s

			Les profils affich�s sont s�lectionn�s comme suit :
				- Lors du premier affichage de la page, tous les profils sont affich�s
				- En cas de saisie d'un nom dans le formulaire de recherche, seuls les profils dont le nom inclut le texte entr� sont affich�s.
				- En cas de s�lection d'un module dans le formulaire de recherche, seuls les profils comportant au moins une habilitation concernant le module s�lectionn� sont affich�s.

			Un tableau ne peut en aucun cas d�passer 50 profils. Si plus de 50 profils correspondent aux crit�res courants de recherche, alors il est propos� de passer � l'�cran suivant par un lien.

			<i>S�curit�</i> :
				- Une habilitation SecurityRights de niveau public WRITE est n�cessaire pour acc�der � la page. Le r�sultat de la recherche d�pend du profil de l'habilitation : seuls les profils inf�rieurs ou �gaux au profil de l'habilitation sont affich�s.
				- Une habilitation SecurityRights de niveau public DELETE est n�cessaire pour pouvoir supprimer un profil. Seuls les profils inutilis�s inf�rieurs ou �gaux au profil de l'habilitation peuvent �tre supprim�s.

			<i>Journaux</i> : Les �v�nements suivants entrainent la cr�ation d'une entr�e dans le journal de s�curit� :
				- INFO : Cr�ation de profil
				- INFO : Suppression de profil
				- WARNING : Tentative de suppression de profil utilis�
		*/
		class ProfilesAdmin : public admin::AdminInterfaceElementTemplate<ProfilesAdmin>
		{
			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_SEARCH_RIGHT;

			std::string _searchName;
			std::string _searchRightName;

			html::ResultHTMLTable::RequestParameters	_requestParameters;

		public:
			ProfilesAdmin();
			void setFromParametersMap(const server::ParametersMap& map);
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request = NULL) const;
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
			
			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@param request User request
				@return PageLinks A link to the page if the parent is Users
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
				) const;
		};
	}
}

#endif
