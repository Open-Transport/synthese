
/** ArrivalDepartureTableLog class header.
	@file ArrivalDepartureTableLog.h

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

#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG

#include "DBLogTemplate.h"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreen;
		class DisplayType;
		class DisplayScreenCPU;

		/** Journal relatif à la gestion des paramètres du téléaffichage.

			Colonnes :
				- Action effectuée

			@ingroup m54Log refLog
		*/
		class ArrivalDepartureTableLog :
			public dblog::DBLogTemplate<ArrivalDepartureTableLog>
		{
		public:
			//! @name Display virtual methods
			//@{
				std::string getName() const;
				DBLog::ColumnsVector getColumnNames() const;

				std::string getObjectName(
					util::RegistryKeyType id,
					const server::Request& searchRequest
				) const;
			//@}


			//! @name Static entry adders
			//@{
				static void	addUpdateEntry(
					const DisplayScreen& screen
					, const std::string& text
					, const security::User& user
				);
				static void	addUpdateEntry(
					const DisplayScreenCPU& cpu
					, const std::string& text
					, const security::User& user
				);
				static void	addCreateEntry(
					const DisplayScreenCPU& cpu
					, const security::User& user
				);
				static void	addCreateEntry(
					const DisplayScreen& screen
					, const security::User& user
				);
				static void addCreateEntry(
					const DisplayType& type
					, const security::User& user
				);
				static void addRemoveEntry(
					const DisplayScreen* screen
					, const security::User* user
				);

				static void addRemoveEntry(
					const DisplayScreenCPU& cpu,
					const security::User& user
				);

				static void addUpdateTypeEntry(
					const DisplayType* type
					, const security::User* user
					, const std::string& text
				);
				static void addDeleteTypeEntry(
					const DisplayType* type
					, const security::User* user
				);
			//@}
		};
	}
}

#endif

