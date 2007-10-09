
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
#include "13_dblog/DBLogEntry.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace departurestable
	{
		class DisplayScreen;
		
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
				- Contrôle positif :
					- 1 : Contrôle positif
					- 0 : Contrôle négatif
					- -1 : Pas de contrôle
				- Description de l'entrée
				
		*/
		class DisplayMaintenanceLog : public util::FactorableTemplate<dblog::DBLog, DisplayMaintenanceLog>
		{
		public:
			typedef enum
			{
				DISPLAY_MAINTENANCE_DISPLAY_CONTROL = 10
				, DISPLAY_MAINTENANCE_DATA_CONTROL = 15
				, DISPLAY_MAINTENANCE_ADMIN = 20
				, DISPLAY_MAINTENANCE_STATUS = 30
			} EntryType;
			
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;
			DBLog::ColumnsVector parse(const dblog::DBLogEntry::Content& cols ) const;
			std::string getObjectName(uid id) const;
			
			static void	addAdminEntry(
				const DisplayScreen* screen
				, const dblog::DBLogEntry::Level& level
				, const security::User* user
				, const std::string& field
				, const std::string& oldValue
				, const std::string& newValue
			);
			static void	addStatusEntry(
				const DisplayScreen* screen
				, bool status
			);
			static void addControlEntry(
				const DisplayScreen* screen
				, bool messageOK
				, bool cpuOK
				, std::string cpuCode
				, bool peripheralsOK
				, std::string peripheralsCode
				, bool driverOK
				, std::string driverCode
				, bool lightOK
				, std::string lightCode
			);
		};
	}
}

#endif

