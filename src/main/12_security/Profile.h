
/** Profile class header.
	@file Profile.h

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

#ifndef SYNTHESE_SECURITY_PROFILE_H
#define SYNTHESE_SECURITY_PROFILE_H

#include <vector>
#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "12_security/Right.h"

namespace synthese
{
	namespace security
	{
		/** Profil utilisateur.
			@ingroup m12
		
			Un profil utilisateur est un "mod�le" de droits utilisateurs consistant en une liste d'@ref synthese::security::Right "habilitations" pr�d�finies.

			Un profil utilisateur peut h�riter d'un autre : l'ensemble des habilitations d�finies par le profil parent est inclus, � l'exception de celles qui sont red�finies. Un profil peut ne pas avoir de parent.

			Exemples de profils utilisateurs :
				- Utilisateur non identifi� : peut utiliser les services grand public
				- Utilisateur classique : m�mes droits qu'Anonymous, ainsi que les fonctionnalit�s de param�trage utilisateur
				- Utilisateur identifi� comme client du syst�me, d'un r�seau de transport : m�mes droits qu'User, et peut de plus utiliser la fonction de r�servation
				- Conducteur de ligne de transport : m�me droits qu'User, et peut en outre consulter les listes de r�servations
				- Op�rateur syst�me : peut g�rer les utilisateurs, prendre les r�servations, consulter les listes de r�servations
				- Responsable de ligne : peut prendre les r�servations, consulter les listes de r�servations, �diter des messages sur la ligne
				- Administrateur : tous les droits

		*/
		class Profile : public util::Registrable<uid,Profile>
		{
		public:
			typedef std::map<std::pair<std::string, std::string>, Right*> RightsVector;

		private:
			std::string		_name;
			RightsVector	_rights;
			uid				_parentId;
			Right::Level	_privateGeneralLevel;
			Right::Level	_publicGeneralLevel;

		public:
			/** Comparison operator between profiles.
				@param profile Profile to compare with
				@return true if the compared profile permits at least one thing that the current profile can not do.
			*/
			int operator<=(const Profile& profile) const;

			Profile(uid id=0);
			~Profile();

			//!	\name Getters
			//@{
				const std::string&	getName()														const;
				const uid			getParentId()													const;
				const RightsVector&	getRights()														const;
				Right*				getRight(const std::string& key, const std::string& parameter)	const;
				const Right::Level&	getPrivateRight()												const;
				const Right::Level&	getPublicRight()												const;
			//@}


			//!	\name Setters
			//@{
				void setName(const std::string& name);
				void setRights(const RightsVector& rightsvector);
				void cleanRights();
				void removeRight(const std::string& key, const std::string& parameter);
				void addRight(Right* right);
				void setParent(uid id);
				void setPrivateRight(const Right::Level& level);
				void setPublicRight(const Right::Level& level);
			//@}
		};
	}
}

#endif
