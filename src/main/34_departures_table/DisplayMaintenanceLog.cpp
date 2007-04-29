
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
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<DBLog, DisplayMaintenanceLog>::FACTORY_KEY("displaymaintenance");
	}
    
	namespace departurestable
	{

		DBLog::ColumnsVector DisplayMaintenanceLog::getColumnNames() const
		{
			ColumnsVector v;
			v.push_back("Type");
			v.push_back("Description");
			return v;
		}

		void DisplayMaintenanceLog::addAdminEntry(boost::shared_ptr<const DisplayScreen> screen, const DBLogEntry::Level& level, boost::shared_ptr<const security::User> user, const std::string& field, const std::string& oldValue, const std::string& newValue )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_ADMIN));
			c.push_back(field + " : " + oldValue + " => " + newValue);
			DBLog::_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, c, user, screen->getKey());
		}

		void DisplayMaintenanceLog::addStatusEntry( boost::shared_ptr<const DisplayScreen> screen, bool status )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_STATUS));
			c.push_back(Conversion::ToString(status));
			DBLog::_addEntry(FACTORY_KEY, status ? DBLogEntry::DB_LOG_INFO : DBLogEntry::DB_LOG_ERROR, c, boost::shared_ptr<const User>(), screen->getKey());
		}

		DBLog::ColumnsVector DisplayMaintenanceLog::parse( const DBLogEntry::Content& cols ) const
		{
			ColumnsVector v;

			// Type
			switch ((EntryType) Conversion::ToInt(cols.front()))
			{
			case DISPLAY_MAINTENANCE_DISPLAY_CONTROL: v.push_back("Contr�le du mat�riel"); break;
			case DISPLAY_MAINTENANCE_ADMIN: v.push_back("Administration"); break;
			case DISPLAY_MAINTENANCE_STATUS: v.push_back("Etat en service"); break;
			default: v.push_back("");
			}

			// Text
			v.push_back(cols.front());

			return v;
		}

		std::string DisplayMaintenanceLog::getObjectName( uid id ) const
		{
			// Screen
			try
			{
				shared_ptr<const DisplayScreen> screen = DeparturesTableModule::getDisplayScreens().get(id);
				return screen->getFullName();
			}
			catch (...)
			{
				return DBLog::getObjectName(id);
			}

		}

		std::string DisplayMaintenanceLog::getName() const
		{
			return "Supervision et maintenance des afficheurs (tableaux de d�parts)";
		}
	}
}
