
/** SecurityRight class header.
	@file SecurityRight.h

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

#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

#include "12_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation portant sur la gestion de la s�curit�.
			@ingroup m12

			Les niveaux de droit utilis�s sont les suivants :

			Habilitations priv�es :
				- USE : Autorise � l'utilisateur � se connecter
				- READ : Permet � l'utilisateur de consulter les propri�t�s de son compte, sauf celles de la cat�gorie "Droits"
				- WRITE : Permet � l'utilisateur de consulter les propri�t�s de son compte, sauf celles de la cat�gorie "Droits"

			Habilitations publiques :
				- READ : Permet de consulter toutes les propri�t�s de tous les comptes
				- WRITE : Permet de modifier toutes les propri�t�s de tous les comptes, d'administrer les profils utilisateurs
				- DELETE : Permet de supprimer un compte, un profil utilisateur

			D�finition du p�rim�tre :
				- Profile : Autorisation portant sur les utilisateurs du profil sp�cifi�
				- NB : Une habilitation sans p�rim�tre reste restreinte � l'ensemble des profils inf�rieurs ou �gaux, au sens de l'op�rateur <= sur les profils.
		*/
		class SecurityRight : public Right
		{
		public:
			SecurityRight();
			std::string	displayParameter()	const;

		};
	}
}

#endif
