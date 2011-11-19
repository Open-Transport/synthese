
/** DisplayMaintenanceRight class header.
	@file DisplayMaintenanceRight.h

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

#ifndef SYNTHESE_DISPLAY_MAINTENANCE_RIGHT_H
#define SYNTHESE_DISPLAY_MAINTENANCE_RIGHT_H

#include "NetworkLineBasedRight.h"

namespace synthese
{
	namespace departure_boards
	{
		/** Habilitation portant sur la maintenance des afficheurs.
			@ingroup m54Rights refRights

			Les niveaux de droit utilisés sont les suivants :
				- READ : Permet de consulter les états de maintenance des afficheurs
				- WRITE : Permet de modifier les états de maintenance des afficheurs

			Définition du périmètre :
				- TransportNetwork : Autorisation portant sur les afficheurs situés sur un point de diffusion desservi par une ligne du réseau spécifié
				- JourneyPattern : Autorisation portant sur les afficheurs situés sur un arrêt desservi par la ligne spécifiée
				- Place : Autorisation portant sur les afficheurs situés sur un arrêt appartenant au lieu logique spécifié
				- BroadcastPoint : Autorisation portant sur les afficheurs situés sur le point de diffusion spécifié
				- NB : la notion de situation d'un afficheur est définie de deux manières :
					- si le point de diffusion est un arrêt physique, alors il doit être desservi par la ligne
					- si le point de diffusion n'est pas un arrêt physique, alors au moins un arrêt physique du lieu auquel appartient le point de diffusion doit être desservi par la ligne
		*/
		class DisplayMaintenanceRight : public pt::NetworkLineBasedRight<DisplayMaintenanceRight>
		{
		public:
		};
	}
}

#endif
