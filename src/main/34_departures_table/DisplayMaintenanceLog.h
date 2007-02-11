
/** DisplayMaintenanceLog class header.
	@file DisplayMaintenanceLog.h

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

#ifndef SYNTHESE_DISPLAY_MAINTENANCE_LOG
#define SYNTHESE_DISPLAY_MAINTENANCE_LOG

#include "13_dblog/DBLog.h"

namespace synthese
{
	namespace departurestable
	{
		/** Journal relatif à la maintenance des afficheurs.

			Ce journal contient deux types d'entrée :
				- Les entrées relatives aux manipulations effectuées sur la console d'administration
				- Les entrées issues de remontées d'alarmes par la fonction SendLogEntry issues des clients de supervision des afficheurs.

			Le brassage des deux informations permet d'exploiter directement le journal pour connaître l'état d'un afficheur en observant la dernière entrée de type contrôle.

			Colonnes :
				- UID de l'afficheur concerné
				- Type d'entrée : 
					- contrôle : l'entrée est issue d'une requête de contrôle de bon fonctionnement (une requete sur la dernière entrée connue de ce type donne l'état de l'afficheur, et l'absence d'entrée de ce type depuis un certain délai peut également être interprétée comme une absence de fonctionnement)
					- admin : l'entrée trace une manipulation faite dans la console d'administration
					- state : l'entrée trace un basculement manuel en service / hors service (une requete sur la dernière entrée connue de ce type donne l'état de l'afficheur)
				- Description de l'entrée
				
		*/
		class DisplayMaintenanceLog : public dblog::DBLog
		{
		public:
			DisplayMaintenanceLog();
			DBLog::ColumnsNameVector getColumnNames() const;
		};
	}
}

#endif

