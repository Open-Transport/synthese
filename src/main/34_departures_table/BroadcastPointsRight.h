
/** PlacesRight class header.
	@file PlacesRight.h

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

#ifndef SYNTHESE_PLACES_RIGHT_H
#define SYNTHESE_PLACES_RIGHT_H

#include "12_security/Right.h"

namespace synthese
{
	namespace departurestable
	{
		/** Habilitation portant sur la gestion de lieux.
			@ingroup m34

			Les niveaux de droit public utilisés sont les suivants :
				- READ : Permet de consulter la liste des lieux et d'en visualiser les détails
				- WRITE : Permet de créer ou d'éditer un lieu
				- DELETE : Permet de supprimer un lieu

			Aucun droit privé n'est utilisé pour cette habilitation.

			Définition du périmètre :
				- TransportNetwork : Autorisation portant sur les lieux desservis par au moins une ligne du réseau spécifié
				- Line : Autorisation portant sur les lieux desservis par la ligne spécifiée
				- City : Autorisation portant sur les lieux appartenant à la commune spécifiée
				- Place : Autorisation portant sur le lieu spécifié
		*/
		class BroadcastPointsRight : public security::Right
		{
		public:
			BroadcastPointsRight();

		};
	}
}

#endif
