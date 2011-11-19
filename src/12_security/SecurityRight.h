
/** SecurityRight class header.
	@file SecurityRight.h

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

#ifndef SYNTHESE_SECURITY_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_SECURITY_RIGHT_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "RightTemplate.h"

namespace synthese
{
	namespace security
	{
		class Profile;

		/** Habilitation portant sur la gestion de la sécurité.
			@ingroup m12Rights refRights

			Les niveaux de droit utilisés sont les suivants :

			Habilitations privées :
				- USE : Autorise à l'utilisateur à se connecter
				- READ : Permet à l'utilisateur de consulter les propriétés de son compte, sauf celles de la catégorie "Droits"
				- WRITE : Permet à l'utilisateur de consulter les propriétés de son compte, sauf celles de la catégorie "Droits"

			Habilitations publiques :
				- READ : Permet de consulter toutes les propriétés de tous les comptes
				- WRITE : Permet de modifier toutes les propriétés de tous les comptes, d'administrer les profils utilisateurs
				- DELETE : Permet de supprimer un compte, un profil utilisateur

			Définition du périmètre :
				- Profile : Autorisation portant sur les utilisateurs du profil spécifié
				- NB : Une habilitation sans périmètre reste restreinte à l'ensemble des profils inférieurs ou égaux, au sens de l'opérateur <= sur les profils.
		*/
		class SecurityRight : public RightTemplate<SecurityRight>
		{
		public:
			static void addSubProfilesLabel(
				ParameterLabelsVector& map,
				boost::shared_ptr<Profile> parent,
				std::string label
			);

			std::string	displayParameter(
				util::Env& env = util::Env::GetOfficialEnv()
			) const;
			bool perimeterIncludes(
				const std::string& perimeter,
				util::Env& env = util::Env::GetOfficialEnv()
			) const;
		};
	}
}

#endif
