
/** MessagesLibraryRight class header.
	@file MessagesLibraryRight.h

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

#ifndef SYNTHESE_MESSAGES_LIBRARY_RIGHT_H
#define SYNTHESE_MESSAGES_LIBRARY_RIGHT_H

#include "12_security/RightTemplate.h"

namespace synthese
{
	namespace messages
	{
		/** Habilitation portant sur la bibliothèque de messages et de scénarios de diffusion.
			@ingroup m17

			Les niveaux de droit public utilisés sont les suivants :
				- WRITE : Permet de créer, modifier des modèles de messages et des scénarios de diffusion
				- DELETE : Permet de supprimer des modèles de messages et des scénarios de diffusion

			Aucun niveau de droit privé n'est utilisé pour cette habilitation.

			Cette habilitation ne peut être paramétrée

		*/
		class MessagesLibraryRight : public security::RightTemplate<MessagesLibraryRight>
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
