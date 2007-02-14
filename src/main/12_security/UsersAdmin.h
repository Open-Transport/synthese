
/** UsersAdmin class header.
	@file UsersAdmin.h

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

#ifndef SYNTHESE_SECURITY_USERS_ADMIN_H
#define SYNTHESE_SECURITY_USERS_ADMIN_H

#include <vector>

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace security
	{
		class User;

		/** Ecran de recherche et liste d'utilisateurs.
			@ingroup m12
		
			@image html cap_admin_users.png
			@image latex cap_admin_users.png "Maquette de l'écran de recherche d'utilisateur" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::HomeAdmin
				- Texte <tt>Utilisateurs</tt>

			<i>Zone de contenus</i> :
				-# <b>Formulaire de recherche</b> :
					-# <tt>Login</tt> : champ texte : recherche des utilisateurs dont le login contient le texte saisi. Absence de texte : filtre désactivé
					-# <tt>Nom</tt> : champ texte : recherche des utilisateurs dont le nom ou le prénom contient le texte saisi. Absence de texte : filtre désactivé
					-# <tt>Profil</tt> : liste déroulante contenant tous les profils correspondant à au moins un utilisateur existant :
						- si un profil est sélectionné, alors seuls s'affichent les utilisateurs ayant le profil sélectionné
						- si aucun profil n'est sélectionné, alors le filtre est désactivé
					-# Bouton <tt>Rechercher</tt> : lancement de la recherche
				-# <b>Tableau des utilisateurs</b> : Les utilisateurs recherchés sont décrits par les colonnes suivantes :
					-# <tt>Sel</tt> : case à cocher permettant une sélection d'un ou plusieurs utilisateurs en vue d'une suppression ou d'une copie
					-# <tt>Login</tt> : login de l'utilisateur
					-# <tt>Nom</tt> : prénom et nom de l'utilisateur
					-# <tt>Profil</tt> : profil de l'utilisateur
					-# Bouton <tt>Editer</tt> : conduit vers la page synthese::interfaces::UserAdmin pour l'utilisateur affiché sur la ligne
				-# <b>Ligne d'ajout d'utilisateur</b> :
					-# Champ <tt>Login</tt> : champ texte. Entrer ici le login de l'utilisateur. Celui-ci doit être unique. En cas de tentative de création d'utilisateur avec un login déjà pris, la création est abandonnée et un message d'erreur apparait :
						@code L'utilisateur ne peut être créé car le login entré est déjà pris. Veuillez choisir un autre login @endcode
						Ce champ est en outre obligatoire. S'il n'est pas rempli la création est abandonnée et un message d'erreur apparait :
						@code L'utilisateur ne peut être créé car le login n'est pas renseigné. Veuillez renseigner le champ login. @endcode
					-# Champ <tt>Nom</tt> : champ texte. Entrer ici le nom de famille de l'utilisateur. Ce champ est obligatoire. S'il n'est pas rempli la création est abandonnée et un message d'erreur apparait :
						@code L'utilisateur ne peut être créé car le nom n'est pas renseigné. Veuillez renseigner le champ nom. @endcode
                    -# Champ <tt>Profil</tt> : liste de choix. Sélectionner ici un profil d'habilitations. Champ obligatoire.
					-# Bouton <tt>Ajouter</tt> : crée l'utilisateur en fonction des valeur saisies. La saisie des informations personnelles de l'utilisateur se poursuit sur l'écran synthese::interfaces::UserAdmin.
				-# Un maximum de 50 entrées est affiché à l'écran. En cas de dépassement de ce nombre d'après les critères de recherche, un lien <tt>Entrées suivantes</tt> apparait et permet de visualiser les entrées suivantes. A partir de la seconde page, un lien <tt>Entrées précédentes</tt> apparait également.
				-# Un <b>Bouton de suppression</b> permet de supprimer les utilisateurs sélectionnés grâce aux cases à cocher. Après confirmation par une boite de dialogue, la suppression est effectuée pour chaque utilisateur :
					- l'utilisateur est réellement supprimé si il n'est à l'origine d'aucune entrée de journal
					- l'utilisateur est désactivé si il est à l'origine d'au moins une entrée de journal, afin de permettre d'accéder à ses informations dans le cadre de la consultation ultérieure du journal. En ce cas, son login est tout de même remis à disposition.
			
			<i>Sécurité</i>
				- Une habilitation publique SecurityRight de niveau READ est nécessaire pour accéder à la page en consultation. NB : Les résultats de la recherche d'utilisateur <b>ne dépendent pas</b> du périmètre de l'habilitation de l'utilisateur courant (la page accédée en lecture seule est considérée comme un annuaire)
				- Une habilitation publique SecurityRight de niveau WRITE est nécessaire pour disposer du bouton de création d'utilisateur. La liste des profils pouvant être utilisés est la liste des profils inférieurs ou égaux à celui de l'habilitation.
				- Une habilitation publique SecurityRight de niveau WRITE est nécessaire pour disposer du bouton d'édition d'un utilisateur différent de l'utilisateur courant : seuls les utilisateurs d'un profil inférieur ou égal à celui de l'habilitation sont éditables.
				- Une habilitation privée SecurityRight de niveau WRITE est nécessaire pour disposer du bouton d'édition sur l'utilisateur courant.
				- Une habilitation publique SecurityRight de niveau DELETE est nécessaire pour disposer du bouton de suppression d'utilisateur.

			<i>Journaux</i> : Les opérations suivantes sont consignées dans le journal de sécurité :
				- INFO : Création d'utilisateur
				- INFO : Suppression d'utilisateur : le choix "suppression ou désactivation" est notifié dans l'entrée
		*/
		class UsersAdmin: public admin::AdminInterfaceElement
		{
		private:
			static const std::string PARAM_SEARCH_PROFILE_ID;
			static const std::string PARAM_SEARCH_NAME;
			static const std::string PARAM_SEARCH_LOGIN;
			static const std::string PARAM_SEARCH_FIRST;
			static const std::string PARAM_SEARCH_NUMBER;

			std::vector<User*> _users;
			bool _nextButton;
			std::string _searchLogin;
			std::string _searchName;
			uid _searchProfileId;
			int _first;
			int _number;

		public:
			UsersAdmin();
			~UsersAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const admin::AdminRequest::ParametersMap& map);
			std::string getTitle() const;
			void display(std::ostream& stream, const admin::AdminRequest* request = NULL) const;

		};
	}
}

#endif

