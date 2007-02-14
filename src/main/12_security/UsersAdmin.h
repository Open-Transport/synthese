
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
			@image latex cap_admin_users.png "Maquette de l'�cran de recherche d'utilisateur" width=14cm

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::HomeAdmin
				- Texte <tt>Utilisateurs</tt>

			<i>Zone de contenus</i> :
				-# <b>Formulaire de recherche</b> :
					-# <tt>Login</tt> : champ texte : recherche des utilisateurs dont le login contient le texte saisi. Absence de texte : filtre d�sactiv�
					-# <tt>Nom</tt> : champ texte : recherche des utilisateurs dont le nom ou le pr�nom contient le texte saisi. Absence de texte : filtre d�sactiv�
					-# <tt>Profil</tt> : liste d�roulante contenant tous les profils correspondant � au moins un utilisateur existant :
						- si un profil est s�lectionn�, alors seuls s'affichent les utilisateurs ayant le profil s�lectionn�
						- si aucun profil n'est s�lectionn�, alors le filtre est d�sactiv�
					-# Bouton <tt>Rechercher</tt> : lancement de la recherche
				-# <b>Tableau des utilisateurs</b> : Les utilisateurs recherch�s sont d�crits par les colonnes suivantes :
					-# <tt>Sel</tt> : case � cocher permettant une s�lection d'un ou plusieurs utilisateurs en vue d'une suppression ou d'une copie
					-# <tt>Login</tt> : login de l'utilisateur
					-# <tt>Nom</tt> : pr�nom et nom de l'utilisateur
					-# <tt>Profil</tt> : profil de l'utilisateur
					-# Bouton <tt>Editer</tt> : conduit vers la page synthese::interfaces::UserAdmin pour l'utilisateur affich� sur la ligne
				-# <b>Ligne d'ajout d'utilisateur</b> :
					-# Champ <tt>Login</tt> : champ texte. Entrer ici le login de l'utilisateur. Celui-ci doit �tre unique. En cas de tentative de cr�ation d'utilisateur avec un login d�j� pris, la cr�ation est abandonn�e et un message d'erreur apparait :
						@code L'utilisateur ne peut �tre cr�� car le login entr� est d�j� pris. Veuillez choisir un autre login @endcode
						Ce champ est en outre obligatoire. S'il n'est pas rempli la cr�ation est abandonn�e et un message d'erreur apparait :
						@code L'utilisateur ne peut �tre cr�� car le login n'est pas renseign�. Veuillez renseigner le champ login. @endcode
					-# Champ <tt>Nom</tt> : champ texte. Entrer ici le nom de famille de l'utilisateur. Ce champ est obligatoire. S'il n'est pas rempli la cr�ation est abandonn�e et un message d'erreur apparait :
						@code L'utilisateur ne peut �tre cr�� car le nom n'est pas renseign�. Veuillez renseigner le champ nom. @endcode
                    -# Champ <tt>Profil</tt> : liste de choix. S�lectionner ici un profil d'habilitations. Champ obligatoire.
					-# Bouton <tt>Ajouter</tt> : cr�e l'utilisateur en fonction des valeur saisies. La saisie des informations personnelles de l'utilisateur se poursuit sur l'�cran synthese::interfaces::UserAdmin.
				-# Un maximum de 50 entr�es est affich� � l'�cran. En cas de d�passement de ce nombre d'apr�s les crit�res de recherche, un lien <tt>Entr�es suivantes</tt> apparait et permet de visualiser les entr�es suivantes. A partir de la seconde page, un lien <tt>Entr�es pr�c�dentes</tt> apparait �galement.
				-# Un <b>Bouton de suppression</b> permet de supprimer les utilisateurs s�lectionn�s gr�ce aux cases � cocher. Apr�s confirmation par une boite de dialogue, la suppression est effectu�e pour chaque utilisateur :
					- l'utilisateur est r�ellement supprim� si il n'est � l'origine d'aucune entr�e de journal
					- l'utilisateur est d�sactiv� si il est � l'origine d'au moins une entr�e de journal, afin de permettre d'acc�der � ses informations dans le cadre de la consultation ult�rieure du journal. En ce cas, son login est tout de m�me remis � disposition.
			
			<i>S�curit�</i>
				- Une habilitation publique SecurityRight de niveau READ est n�cessaire pour acc�der � la page en consultation. NB : Les r�sultats de la recherche d'utilisateur <b>ne d�pendent pas</b> du p�rim�tre de l'habilitation de l'utilisateur courant (la page acc�d�e en lecture seule est consid�r�e comme un annuaire)
				- Une habilitation publique SecurityRight de niveau WRITE est n�cessaire pour disposer du bouton de cr�ation d'utilisateur. La liste des profils pouvant �tre utilis�s est la liste des profils inf�rieurs ou �gaux � celui de l'habilitation.
				- Une habilitation publique SecurityRight de niveau WRITE est n�cessaire pour disposer du bouton d'�dition d'un utilisateur diff�rent de l'utilisateur courant : seuls les utilisateurs d'un profil inf�rieur ou �gal � celui de l'habilitation sont �ditables.
				- Une habilitation priv�e SecurityRight de niveau WRITE est n�cessaire pour disposer du bouton d'�dition sur l'utilisateur courant.
				- Une habilitation publique SecurityRight de niveau DELETE est n�cessaire pour disposer du bouton de suppression d'utilisateur.

			<i>Journaux</i> : Les op�rations suivantes sont consign�es dans le journal de s�curit� :
				- INFO : Cr�ation d'utilisateur
				- INFO : Suppression d'utilisateur : le choix "suppression ou d�sactivation" est notifi� dans l'entr�e
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

