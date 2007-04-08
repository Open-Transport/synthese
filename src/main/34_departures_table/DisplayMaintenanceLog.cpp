
/** DisplayMaintenanceLog class implementation.
	@file DisplayMaintenanceLog.cpp

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

#include "12_security/User.h"

#include "34_departures_table/DisplayMaintenanceLog.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace dblog;
    
	namespace departurestable
	{


		DisplayMaintenanceLog::DisplayMaintenanceLog()
			: DBLog("Supervision et maintenance des afficheurs (tableaux de départs)")
		{
		
		}

		DBLog::ColumnsVector DisplayMaintenanceLog::getColumnNames() const
		{
			ColumnsVector v;
			v.push_back("Afficheur");
			v.push_back("Type");
			v.push_back("Description");
			return v;
		}

		void DisplayMaintenanceLog::addControlEntry( const DisplayScreen* screen, const DBLogEntry::Level& level, const std::string& text )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(screen->getKey()));
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_DATA_CONTROL));
			c.push_back(text);
			DBLog::_addEntry(level, c, NULL);
		}

		void DisplayMaintenanceLog::addAdminEntry( const DisplayScreen* screen, const DBLogEntry::Level& level, const security::User* user, const std::string& field, const std::string& oldValue, const std::string& newValue )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(screen->getKey()));
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_ADMIN));
			c.push_back(field + " : " + oldValue + " => " + newValue);
			DBLog::_addEntry(DBLogEntry::DB_LOG_INFO, c, user);
		}

		void DisplayMaintenanceLog::addStatusEntry( const DisplayScreen* screen, bool status )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(screen->getKey()));
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_STATUS));
			c.push_back(Conversion::ToString(status));
			DBLog::_addEntry(status ? DBLogEntry::DB_LOG_INFO : DBLogEntry::DB_LOG_ERROR, c, NULL);
		}

		void DisplayMaintenanceLog::addDataControlEntry( const DisplayScreen* screen, bool ok, const std::string& text )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(screen->getKey()));
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_DATA_CONTROL));
			c.push_back(text);
			DBLog::_addEntry(ok ? DBLogEntry::DB_LOG_INFO : DBLogEntry::DB_LOG_WARNING, c, NULL);
		}

		DBLog::ColumnsVector DisplayMaintenanceLog::parse( const DBLogEntry::Content& cols ) const
		{
			ColumnsVector v;

			// Screen
			try
			{
				DisplayScreen* screen = DeparturesTableModule::getDisplayScreens().get(Conversion::ToLongLong(cols.front()));
				v.push_back(screen->getFullName());
			}
			catch (...)
			{
				v.push_back(cols.front());
			}

			// Type
			switch ((EntryType) Conversion::ToInt(cols.front()))
			{
			case DISPLAY_MAINTENANCE_DATA_CONTROL: v.push_back("Contrôle des données"); break;
			case DISPLAY_MAINTENANCE_DISPLAY_CONTROL: v.push_back("Contrôle du matériel"); break;
			case DISPLAY_MAINTENANCE_ADMIN: v.push_back("Administration"); break;
			case DISPLAY_MAINTENANCE_STATUS: v.push_back("Etat en service"); break;
			default: v.push_back("");
			}

			// Text
			v.push_back(cols.front());

			return v;
		}
	}
}
