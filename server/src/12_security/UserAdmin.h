
/** UserAdmin class header.
	@file UserAdmin.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;

		/** Ecran d'édition d'utilisateur.
			@ingroup m12Admin refAdmin

			@image html cap_admin_user.png
			@image latex cap_admin_user.png "Maquette de l'écran d'édition d'utilisateur" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Utilisateurs - [Prénom] [Nom]

			<i>Barre de navigation</i> :
				- Lien vers synthese::admin::HomeAdmin
				- Lien vers synthese::security::UsersAdmin
				- Prénom et Nom de l'utilisateur

			<i>Zone de contenus</i> : Il s'agit d'un <b>Formulaire d'édition</b>.
				-# <b>Connexion</b>
					-# <tt>Login</tt> : Champ texte contenant le nom entré pour se connecter sur SYNTHESE. Champ obligatoire. Le login doit être unique. En cas de tentative de création d'utilisateur avec un login déjà pris, la modification est abandonnée et un message d'erreur apparait :
						@code L'utilisateur ne peut modifié car le login entré est déjà pris. Veuillez choisir un autre login @endcode
						Ce champ est en outre obligatoire. S'il n'est pas rempli la modification est abandonnée et un message d'erreur apparait :
						@code L'utilisateur doit posséder un login. Veuillez renseigner le champ login. @endcode
					-# <tt>Mot de passe</tt> et <tt>Mot de passe (vérification)</tt> : Champs texte cachés contenant le mot de passe utilisé pour se connecter sur SYNTHESE. Lors de l'affichage de la page ces champs ne sont pas remplis. Lors de la modification, trois scénarios peuvent se produire :
						- Les champs sont laissés vides, auquel cas le mot de passe n'est pas modifié
						- Les champs sont remplis à l'identique par un nouveau mot de passe, auquel cas le mot de passe est modifié comme spécifié
						- Les champs sont remplis différemment, auquel cas la modification est interrompue, et un message d'erreur apparait :
							@code Les mots de passe saisis ne sont pas identiques. Veuillez vérifier les mots de passe entrés. @endcode
				-# <b>Coordonnées</b>
					-# <tt>Prénom</tt> : Champ texte facultatif
					-# <tt>Nom</tt> : Champ texte obligatoire. S'il n'est pas rempli la modification est interrompue et un message d'erreur apparait :
						@code L'utilisateur doit avoir un nom. @endcode
					-# <tt>Adresse</tt> : Champ texte long, facultatif
					-# <tt>Code postal</tt> : Champ texte long, facultatif
					-# <tt>Ville</tt> : Champ texte, facultatif
					-# <tt>E-mail</tt> : Champ texte, facultatif
				-# <b>Droits</b> : cette section n'est disponible que pour les utilisateurs disposant d'une habilitation sur le module sécurité en entier.
					-# <tt>Connexion autorisée</tt> : Champ OUI/NON. Si le choix est sur NON, l'utilisateur est temporairement désactivé. Cela permet à l'administrateur de suspendre un droit d'accès sans toutefois supprimer l'utilisateur.
					-# <tt>Profil</tt> : Liste de choix permettant de sélectionner le profil d'habilitations de l'utilisateur.

			<i>Sécurité</i>
				- Une habilitation privée SecurityRight de niveau READ est nécessaire pour accéder à la page en consultation sur l'utilisateur courant. Dans ce cas la partie "droits" n'est pas affichée.
				- Une habilitation privée SecurityRight de niveau WRITE est nécessaire pour accéder à la page en modification sur l'utilisateur courant. Dans ce cas la partie "droits" n'est pas affichée.
				- Une habilitation publique SecurityRight de niveau READ est nécessaire pour accéder à la page en consultation sur un utilisateur quelconque. Dans ce cas la partie "droits" n'est pas affichée.
				- Une habilitation publique SecurityRight de niveau WRITE est nécessaire pour accéder à la page en modification sur un utilisateur quelconque, à condition que le profil de l'utilisateur édité soit inférieur ou égal au profil de l'habilitation. Dans ce cas la partie "droits" est affichée et modifiable".


			<i>Journaux</i> : Les opérations suivantes sont consignées dans le journal de sécurité SecurityLog :
				- INFO : Chaque modification effectuée avec succès

		*/
		class UserAdmin: public admin::AdminInterfaceElementTemplate<UserAdmin>
		{
			boost::shared_ptr<const User> _user;
			bool _userError;

		public:
			UserAdmin();

			void setUser(boost::shared_ptr<User> value);
			void setUserC(boost::shared_ptr<const User> value);
			boost::shared_ptr<const User> getUser() const;

			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
				*/
			virtual util::ParametersMap getParametersMap() const;



			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;

			bool isAuthorized(
				const security::User& user
			) const;

			virtual std::string getTitle() const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

		};
	}
}

#endif

