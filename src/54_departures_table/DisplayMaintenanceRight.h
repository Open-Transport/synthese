
/** DisplayMaintenanceRight class header.
	@file DisplayMaintenanceRight.h

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

#ifndef SYNTHESE_DISPLAY_MAINTENANCE_RIGHT_H
#define SYNTHESE_DISPLAY_MAINTENANCE_RIGHT_H

#include "NetworkLineBasedRight.h"

namespace synthese
{
	namespace departurestable
	{
		/** Habilitation portant sur la maintenance des afficheurs.
			@ingroup m54Rights refRights

			Les niveaux de droit utilis�s sont les suivants :
				- READ : Permet de consulter les �tats de maintenance des afficheurs
				- WRITE : Permet de modifier les �tats de maintenance des afficheurs

			D�finition du p�rim�tre :
				- TransportNetwork : Autorisation portant sur les afficheurs situ�s sur un point de diffusion desservi par une ligne du r�seau sp�cifi�
				- Line : Autorisation portant sur les afficheurs situ�s sur un arr�t desservi par la ligne sp�cifi�e
				- Place : Autorisation portant sur les afficheurs situ�s sur un arr�t appartenant au lieu logique sp�cifi�
				- BroadcastPoint : Autorisation portant sur les afficheurs situ�s sur le point de diffusion sp�cifi�
				- NB : la notion de situation d'un afficheur est d�finie de deux mani�res :
					- si le point de diffusion est un arr�t physique, alors il doit �tre desservi par la ligne
					- si le point de diffusion n'est pas un arr�t physique, alors au moins un arr�t physique du lieu auquel appartient le point de diffusion doit �tre desservi par la ligne
		*/
		class DisplayMaintenanceRight : public pt::NetworkLineBasedRight<DisplayMaintenanceRight>
		{
		public:
		};
	}
}

#endif
