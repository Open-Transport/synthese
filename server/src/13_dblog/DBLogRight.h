
/** DBLogRight class header.
	@file DBLogRight.h

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

#ifndef SYNTHESE_DB_LOG_RIGHT_H
#define SYNTHESE_DB_LOG_RIGHT_H

#include <string>

#include "RightTemplate.h"

namespace synthese
{
	namespace dblog
	{
		/** Habilitation portant sur la gestion des journaux.
			@ingroup m13Rights refRights

			Les niveaux de droit utilisés sont les suivants :
				- READ : Permet de consulter un journal
				- DELETE : Permet de purger un journal

			Définition du périmètre :
				- NB : Le périmètre n'est pas défini par l'habilitation DBLogRight qui porte sur le principe même d'accéder aux journaux. Chaque journal fait à son tour l'objet d'un contrôle de droits le plus souvent basé sur les habilitations du module auquel ils appartiennent. Voir la documentation de chaque journal.
		*/
		class DBLogRight : public security::RightTemplate<DBLogRight>
		{
		public:
			std::string displayParameter(
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
