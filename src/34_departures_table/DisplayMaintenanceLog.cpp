
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

#include "User.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayMonitoringStatus.h"
#include "Conversion.h"

#include <sstream>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace departurestable;
	using namespace util;
	using namespace time;

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
			v.push_back("Résumé");
			v.push_back("Détail");
			return v;
		}

		void DisplayMaintenanceLog::AddAdminEntry(
			const DisplayScreen* screen
			, const security::User* user
			, const std::string& field
			, const std::string& oldValue
			, const std::string& newValue
			, const DBLogEntry::Level level
		){
			assert(screen != NULL);
			
			if (oldValue == newValue)
				return;

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MAINTENANCE_ADMIN)));
			c.push_back(field);
			c.push_back(oldValue + " => " + newValue);
			DBLog::_addEntry(FACTORY_KEY, level, c, user, screen->getKey());
		}

		DBLog::ColumnsVector DisplayMaintenanceLog::parse( const DBLogEntry::Content& cols ) const
		{
			ColumnsVector v;

			// Type
			switch ((EntryType) Conversion::ToInt(cols.front()))
			{
			case DISPLAY_MONITORING_STATUS_CHANGE: v.push_back("Changement d'état"); break;
			case DISPLAY_MAINTENANCE_ADMIN: v.push_back("Administration"); break;
			case DISPLAY_MONITORING_DOWN: v.push_back("Perte du contact"); break;
			case DISPLAY_MONITORING_UP: v.push_back("Reprise du contact"); break;
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
				shared_ptr<const DisplayScreen> screen = DisplayScreenTableSync::Get(id, Env::GetOfficialEnv(), UP_LINKS_LOAD_LEVEL);
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



		void DisplayMaintenanceLog::AddStatusChangeEntry(
			const DisplayScreen* screen,
			const DisplayMonitoringStatus& oldValue,
			const DisplayMonitoringStatus& newValue
		) {
			assert(screen != NULL);

			DisplayMonitoringStatus::Status newStatus(newValue.getGlobalStatus());

			stringstream s;
			s	<< DisplayMonitoringStatus::GetStatusString(oldValue.getGlobalStatus()) << " => "
				<< DisplayMonitoringStatus::GetStatusString(newStatus);

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_STATUS_CHANGE)));
			c.push_back(s.str());
			c.push_back(newValue.getDetail());

			DBLogEntry::Level level(DBLogEntry::DB_LOG_ERROR);
			if (newStatus == DisplayMonitoringStatus::DISPLAY_MONITORING_OK)
			{
				level = DBLogEntry::DB_LOG_INFO;
			}
			else if (newStatus == DisplayMonitoringStatus::DISPLAY_MONITORING_WARNING || newStatus == DisplayMonitoringStatus::DISPLAY_MONITORING_UNKNOWN)
			{
				level = DBLogEntry::DB_LOG_WARNING;
			}

			DBLog::_addEntry(
				FACTORY_KEY,
				level,
				c,
				NULL,
				screen->getKey()
			);
		}



		void DisplayMaintenanceLog::AddMonitoringUpEntry(
			const DisplayScreen* screen,
			const DateTime& downTime
		){
			assert(screen != NULL);

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_UP)));
			c.push_back(string());
			c.push_back("Etait perdu depuis " + downTime.toString());

			DBLog::_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_INFO,
				c,
				NULL,
				screen->getKey()
			);
		}



		void DisplayMaintenanceLog::AddMonitorDownEntry(
			const DisplayScreen* screen
		) {
			assert(screen != NULL);

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_DOWN)));
			c.push_back(string());
			c.push_back(string());

			DBLog::_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_INFO,
				c,
				NULL,
				screen->getKey()
			);
		}



		void DisplayMaintenanceLog::AddMonitoringFirstEntry(
			const DisplayScreen* screen,
			const DisplayMonitoringStatus& value
		) {
			assert(screen != NULL);

			DisplayMonitoringStatus::Status newStatus(value.getGlobalStatus());

			{
				DBLogEntry::Content c;
				c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_UP)));
				c.push_back(string());
				c.push_back("Premier contact");

				DBLog::_addEntry(
					FACTORY_KEY,
					DBLogEntry::DB_LOG_INFO,
					c,
					NULL,
					screen->getKey()
					);
			}

			{
				stringstream s;
				s	<< " => " << DisplayMonitoringStatus::GetStatusString(newStatus);

				DBLogEntry::Content c;
				c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_STATUS_CHANGE)));
				c.push_back(s.str());
				c.push_back(value.getDetail());

				DBLogEntry::Level level(DBLogEntry::DB_LOG_ERROR);
				if (newStatus == DisplayMonitoringStatus::DISPLAY_MONITORING_OK)
				{
					level = DBLogEntry::DB_LOG_INFO;
				}
				else if (newStatus == DisplayMonitoringStatus::DISPLAY_MONITORING_WARNING || newStatus == DisplayMonitoringStatus::DISPLAY_MONITORING_UNKNOWN)
				{
					level = DBLogEntry::DB_LOG_WARNING;
				}

				DBLog::_addEntry(
					FACTORY_KEY,
					level,
					c,
					NULL,
					screen->getKey()
					);
			}
		}
	}
}
