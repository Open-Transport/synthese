
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

#include "12_security/Constants.h"

namespace synthese
{
	namespace security
	{
		class Right;

		/** Profil utilisateur.
			@ingroup m12
		
			Un profil utilisateur est un "modèle" de droits utilisateurs consistant en une liste d'@ref synthese::security::Right "habilitations" prédéfinies.

			Un profil utilisateur peut hériter d'un autre : l'ensemble des habilitations définies par le profil parent est inclus, à l'exception de celles qui sont redéfinies. Un profil peut ne pas avoir de parent.

			Exemples de profils utilisateurs :
				- Utilisateur non identifié : peut utiliser les services grand public
				- Utilisateur classique : mêmes droits qu'Anonymous, ainsi que les fonctionnalités de paramétrage utilisateur
				- Utilisateur identifié comme client du système, d'un réseau de transport : mêmes droits qu'User, et peut de plus utiliser la fonction de réservation
				- Conducteur de ligne de transport : même droits qu'User, et peut en outre consulter les listes de réservations
				- Opérateur système : peut gérer les utilisateurs, prendre les réservations, consulter les listes de réservations
				- Responsable de ligne : peut prendre les réservations, consulter les listes de réservations, éditer des messages sur la ligne
				- Administrateur : tous les droits

		*/
		class Profile : public util::Registrable<uid,Profile>
		{
		public:
			typedef std::map<std::pair<std::string, std::string>, Right*> RightsVector;
			typedef std::map<std::string, Right*> RightsOfSameClassMap;

		private:
			std::string		_name;
			RightsVector	_rights;
			uid				_parentId;

			/** Extractor of the rights corresponding to a class key.
				@param key The class key to search
				@return RightsOfSameClassMap Rights of the specified class
				@author Hugues Romain
				@date 2007				
			*/
			RightsOfSameClassMap	_getRights(const std::string& key) const;

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
			//@}


			//!	\name Setters
			//@{
				void setName(const std::string& name);
				void setRights(const RightsVector& rightsvector);
				void cleanRights();
				void removeRight(const std::string& key, const std::string& parameter);
				void addRight(Right* right);
				void setParent(uid id);
			//@}

			//! \name Calculators
			//@{
				/** Control of authorization.
					@param right Right to compare with
					@return bool True if the needed right is respected :
						- If the profile contains a global right with a sufficient level in public and private domains
						- Or If the profile contains a right of the same class with a global perimeter, and with a sufficient level in public and private domains
						- Or if the profile contains a right of the same class with a sufficient perimeter, and with a sufficient level in public and private domains
					@author Hugues Romain
					@date 2007
					
				*/
				bool isAuthorized(const Right* right) const;

				/** Search of a contained right in the right vector, from the class key and the perimeter string.
					@param key Class key to search (default : the global right)
					@param parameter Perimeter string to search (default : global perimeter)
					@return Right* The specified right if exists, NULL else
					@author Hugues Romain
					@date 2007					
				*/
				Right* getRight(const std::string key = GLOBAL_PERIMETER, const std::string parameter = GLOBAL_PERIMETER)	const;

			//@}
		};
	}
}

#endif
