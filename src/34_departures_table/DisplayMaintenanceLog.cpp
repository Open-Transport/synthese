
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

#include "DisplayMaintenanceLog.h"
#include "34_departures_table/DisplayScreen.h"

#include "01_util/Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace departurestable;
	using namespace util;

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
			v.push_back("Etat");
			v.push_back("Description");
			return v;
		}

		void DisplayMaintenanceLog::addAdminEntry(
			const DisplayScreen* screen
			, const DBLogEntry::Level& level
			, const security::User* user
			, const std::string& field
			, const std::string& oldValue
			, const std::string& newValue
		){
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_ADMIN));
			c.push_back(field + " : " + oldValue + " => " + newValue);
			DBLog::_addEntry(FACTORY_KEY, DBLogEntry::DB_LOG_INFO, c, user, screen->getKey());
		}

		void DisplayMaintenanceLog::addStatusEntry(
			const DisplayScreen* screen
			, bool status
		){
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString((int) DISPLAY_MAINTENANCE_STATUS));
			c.push_back(Conversion::ToString(status));
			DBLog::_addEntry(
				FACTORY_KEY
				, status ? DBLogEntry::DB_LOG_INFO : DBLogEntry::DB_LOG_ERROR
				, c
				, NULL
				, screen->getKey()
			);
		}

		DBLog::ColumnsVector DisplayMaintenanceLog::parse( const DBLogEntry::Content& cols ) const
		{
			ColumnsVector v;

			// Type
			switch ((EntryType) Conversion::ToInt(cols.front()))
			{
			case DISPLAY_MAINTENANCE_DISPLAY_CONTROL: v.push_back("Contrôle du matériel"); break;
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
				shared_ptr<const DisplayScreen> screen = DisplayScreen::Get(id);
				return screen->getFullName();
			}
			catch (...)
			{
				return DBLog::getObjectName(id);
			}

		}

		std::string DisplayMaintenanceLog::getName() const
		{
			return "SAI : Supervision et maintenance";
		}

		void DisplayMaintenanceLog::addControlEntry(
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
		){
			stringstream s;
			s	<< "Message : " << (messageOK ? "OK" : "KO")
				<< " - CPU : " << (cpuOK ? "OK" : "KO (code" + cpuCode +")")
				<< " - Périphériques : " << (peripheralsOK ? "OK" : "KO (code" + peripheralsCode +")")
				<< " - Driver affichage : " << (driverOK ? "OK" : "KO (code" + driverCode +")")
				<< " - Lumière : " << (lightOK ? "OK" : "KO (code" + lightCode +")")
			;

			bool status(messageOK && cpuOK && peripheralsOK && driverOK && lightOK);

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MAINTENANCE_DISPLAY_CONTROL)));
			c.push_back(Conversion::ToString(s.str()));
			DBLog::_addEntry(
				FACTORY_KEY
				, status ? DBLogEntry::DB_LOG_OK : DBLogEntry::DB_LOG_ERROR
				, c
				, NULL
				, screen->getKey()
				);
		}
	}
}
