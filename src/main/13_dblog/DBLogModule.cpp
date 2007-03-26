
/** DBLogModule class implementation.
	@file DBLogModule.cpp

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

#include "01_util/Constants.h"

#include "13_dblog/DBLogModule.h"

using namespace std;

namespace synthese
{
	namespace dblog
	{


		map<int, std::string> DBLogModule::getEntryLevelLabels( bool withAll/*=false*/ )
		{
			map<int, std::string> m;
			if (withAll)
				m.insert(make_pair(UNKNOWN_VALUE, "(tous)"));
			m.insert(make_pair((int) DBLogEntry::DB_LOG_INFO, getEntryLevelLabel(DBLogEntry::DB_LOG_INFO)));
			m.insert(make_pair((int) DBLogEntry::DB_LOG_WARNING, getEntryLevelLabel(DBLogEntry::DB_LOG_WARNING)));
			m.insert(make_pair((int) DBLogEntry::DB_LOG_ERROR, getEntryLevelLabel(DBLogEntry::DB_LOG_ERROR)));
			return m;
		}

		std::string DBLogModule::getEntryLevelLabel( const DBLogEntry::Level& level )
		{
			switch (level)
			{
			case DBLogEntry::DB_LOG_INFO : return "Information";
			case DBLogEntry::DB_LOG_WARNING : return "Alerte";
			case DBLogEntry::DB_LOG_ERROR : return "Erreur";
			}
		}
	}
}
