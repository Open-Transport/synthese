
/** ArrivalDepartureTableLog class header.
	@file ArrivalDepartureTableLog.h

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

#ifndef SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG
#define SYNTHESE_ARRIVAL_DEPARTURE_TABLE_LOG

#include "13_dblog/DBLog.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		/** Journal relatif � la gestion des param�tres du t�l�affichage.

			Colonnes :
				- Action effectu�e				

			@ingroup m34Log refLog
		*/
		class ArrivalDepartureTableLog : public util::FactorableTemplate<dblog::DBLog, ArrivalDepartureTableLog>
		{
		public:
			std::string getName() const;
			DBLog::ColumnsVector getColumnNames() const;
			static void	addUpdateEntry(
				boost::shared_ptr<const DisplayScreen> screen
				, const std::string& text
				, boost::shared_ptr<const security::User> user
				);
			std::string getObjectName(uid id) const;
		};
	}
}

#endif

