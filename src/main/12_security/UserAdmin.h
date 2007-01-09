
/** UserAdmin class header.
	@file UserAdmin.h

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

#ifndef SYNTHESE_SECURITY_USER_ADMIN_H
#define SYNTHESE_SECURITY_USER_ADMIN_H

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace security
	{
		class User;

		/** Ecran d'�dition d'utilisateur.
			@ingroup m12
		
			@image html cap_admin_user.png
			@image latex cap_admin_user.png "Maquette de l'�cran d'�dition d'utilisateur" width=14cm

			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Utilisateurs - [Pr�nom] [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::interfaces::HomeAdmin
				- Lien vers synthese::interfaces::UsersAdmin
				- Pr�nom et Nom de l'utilisateur

			<i>Zone de contenus</i> : Il s'agit d'un <b>Formulaire d'�dition</b>.
				-# <b>Connexion</b>
					-# <tt>Login</tt> : Champ texte contenant le nom entr� pour se connecter sur SYNTHESE. Champ obligatoire. Le login doit �tre unique. En cas de tentative de cr�ation d'utilisateur avec un login d�j� pris, la modification est abandonn�e et un message d'erreur apparait :
						@code L'utilisateur ne peut modifi� car le login entr� est d�j� pris. Veuillez choisir un autre login @endcode
						Ce champ est en outre obligatoire. S'il n'est pas rempli la modification est abandonn�e et un message d'erreur apparait :
						@code L'utilisateur doit poss�der un login. Veuillez renseigner le champ login. @endcode
					-# <tt>Mot de passe</tt> et <tt>Mot de passe (v�rification)</tt> : Champs texte cach�s contenant le mot de passe utilis� pour se connecter sur SYNTHESE. Lors de l'affichage de la page ces champs ne sont pas remplis. Lors de la modification, trois sc�narios peuvent se produire :
						- Les champs sont laiss�s vides, auquel cas le mot de passe n'est pas modifi�
						- Les champs sont remplis � l'identique par un nouveau mot de passe, auquel cas le mot de passe est modifi� comme sp�cifi�
						- Les champs sont remplis diff�remment, auquel cas la modification est interrompue, et un message d'erreur apparait :
							@code Les mots de passe saisis ne sont pas identiques. Veuillez v�rifier les mots de passe entr�s. @endcode
				-# <b>Coordonn�es</b>
					-# <tt>Pr�nom</tt> : Champ texte facultatif
					-# <tt>Nom</tt> : Champ texte obligatoire. S'il n'est pas rempli la modification est interrompue et un message d'erreur apparait :
						@code L'utilisateur doit avoir un nom. @endcode
					-# <tt>Adresse</tt> : Champ texte long, facultatif
					-# <tt>Code postal</tt> : Champ texte long, facultatif
					-# <tt>Ville</tt> : Champ texte, facultatif
					-# <tt>E-mail</tt> : Champ texte, facultatif
				-# <b>Droits</b> : cette section n'est disponible que pour les utilisateurs disposant d'une habilitation sur le module s�curit� en entier.
					-# <tt>Connexion autoris�e</tt> : Champ OUI/NON. Si le choix est sur NON, l'utilisateur est temporairement d�sactiv�. Cela permet � l'administrateur de suspendre un droit d'acc�s sans toutefois supprimer l'utilisateur.
					-# <tt>Profil</tt> : Liste de choix permettant de s�lectionner le profil d'habilitations de l'utilisateur.
								
			<i>S�curit�</i>
				- Une habilitation priv�e SecurityRight de niveau READ est n�cessaire pour acc�der � la page en consultation sur l'utilisateur courant. Dans ce cas la partie "droits" n'est pas affich�e.
				- Une habilitation priv�e SecurityRight de niveau WRITE est n�cessaire pour acc�der � la page en modification sur l'utilisateur courant. Dans ce cas la partie "droits" n'est pas affich�e.
				- Une habilitation publique SecurityRight de niveau READ est n�cessaire pour acc�der � la page en consultation sur un utilisateur quelconque. Dans ce cas la partie "droits" n'est pas affich�e.
				- Une habilitation publique SecurityRight de niveau WRITE est n�cessaire pour acc�der � la page en modification sur un utilisateur quelconque, � condition que le profil de l'utilisateur �dit� soit inf�rieur ou �gal au profil de l'habilitation. Dans ce cas la partie "droits" est affich�e et modifiable".


			<i>Journaux</i> : Les op�rations suivantes sont consign�es dans le journal de s�curit� SecurityLog :
				- INFO : Chaque modification effectu�e avec succ�s

		*/
		class UserAdmin: public admin::AdminInterfaceElement
		{
			static const std::string PARAM_USER_ID;

			const User* _user;
			bool _userError;

		public:
			UserAdmin();
			~UserAdmin();

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);

			std::string getTitle() const;
			void display(std::ostream& stream, const server::Request* request) const;
		};
	}
}

#endif

