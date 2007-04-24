
/** DisplayDataControlLog class header.
	@file DisplayDataControlLog.h

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

#ifndef SYNTHESE_DISPLAY_DATACONTROL_LOG
#define SYNTHESE_DISPLAY_DATACONTROL_LOG

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogEntry.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace departurestable
	{
		class DisplayScreen;
		
		/** Journal relatif au contrôle des données de définition des afficheurs.

			@ingroup m34log reflog				
		*/
		class DisplayDataControlLog : public dblog::DBLog
		{
		public:
			DisplayDataControlLog();
			DBLog::ColumnsVector getColumnNames() const;
			DBLog::ColumnsVector parse(const dblog::DBLogEntry::Content& cols ) const;
			void	addDataControlEntry(
				boost::shared_ptr<const DisplayScreen> screen
				, const dblog::DBLogEntry::Level& level
				, const std::string& text);
			std::string getObjectName(uid id) const;
		};
	}
}

#endif

