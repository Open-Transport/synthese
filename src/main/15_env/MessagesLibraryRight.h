
/** MessagesLibraryRight class header.
	@file MessagesLibraryRight.h

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

#ifndef SYNTHESE_MESSAGES_LIBRARY_RIGHT_H
#define SYNTHESE_MESSAGES_LIBRARY_RIGHT_H

#include "05_security/Right.h"

namespace synthese
{
	namespace env
	{
		/** Habilitation portant sur la biblioth�que de messages et de sc�narios de diffusion.
			@ingroup m15

			Les niveaux de droit public utilis�s sont les suivants :
				- WRITE : Permet de cr�er, modifier des mod�les de messages et des sc�narios de diffusion
				- DELETE : Permet de supprimer des mod�les de messages et des sc�narios de diffusion
			
			Aucun niveau de droit priv� n'est utilis� pour cette habilitation.

			Cette habilitation ne peut �tre param�tr�e

		*/
		class MessagesLibraryRight : public security::Right
		{

		};
	}
}

#endif
