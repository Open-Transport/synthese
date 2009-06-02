////////////////////////////////////////////////////////////////////////////////
/// DisplayMaintenanceLog class implementation.
///	@file DisplayMaintenanceLog.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "User.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMaintenanceRight.h"
#include "Request.h"
#include "DisplayScreen.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayMonitoringStatus.h"
#include "Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace dblog;
	using namespace security;
	using namespace departurestable;
	using namespace util;
	using namespace time;
	using namespace server;

	namespace util
	{
		template<> const string FactorableTemplate<DBLog, DisplayMaintenanceLog>::FACTORY_KEY("displaymaintenance");
	}
    
	namespace departurestable
	{
		const int DisplayMaintenanceLog::_COL_TYPE(0);
		const int DisplayMaintenanceLog::_COL_FIELD(1);
		const int DisplayMaintenanceLog::_COL_TEXT(2);



		DBLog::ColumnsVector DisplayMaintenanceLog::getColumnNames() const
		{
			ColumnsVector v;
			v.push_back("Type");
			v.push_back("R�sum�");
			v.push_back("D�tail");
			return v;
		}


		bool DisplayMaintenanceLog::isAuthorized(
			const Request& request,
			const security::RightLevel& level
		) const {
			return request.isAuthorized<DisplayMaintenanceRight>(level);
		}



		void DisplayMaintenanceLog::AddAdminEntry(
			const DisplayScreen& screen
			, const security::User& user
			, const std::string& field
			, const std::string& oldValue
			, const std::string& newValue
			, const DBLogEntry::Level level
		){
			if (oldValue == newValue)
				return;

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MAINTENANCE_ADMIN)));
			c.push_back(field);
			c.push_back(oldValue + " => " + newValue);
			DBLog::_addEntry(FACTORY_KEY, level, c, &user, screen.getKey());
		}

		void DisplayMaintenanceLog::AddAdminEntry( const DisplayScreenCPU& cpu , const security::User& user , const std::string& field , const std::string& oldValue , const std::string& newValue , const dblog::DBLogEntry::Level level /*= dblog::DBLogEntry::DB_LOG_INFO */ )
		{
			if (oldValue == newValue)
				return;

			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MAINTENANCE_ADMIN)));
			c.push_back(field);
			c.push_back(oldValue + " => " + newValue);
			DBLog::_addEntry(FACTORY_KEY, level, c, &user, cpu.getKey());
		}



		DBLog::ColumnsVector DisplayMaintenanceLog::parse(
			const DBLogEntry& cols,
			const server::Request& searchRequest
		) const	{
			ColumnsVector v;
			const DBLogEntry::Content& c(cols.getContent());

			// Type
			switch (static_cast<EntryType>(Conversion::ToInt(c[_COL_TYPE])))
			{
			case DISPLAY_MONITORING_STATUS_CHANGE: v.push_back("Changement d'�tat"); break;
			case DISPLAY_MAINTENANCE_ADMIN: v.push_back("Administration"); break;
			case DISPLAY_MONITORING_DOWN: v.push_back("Perte du contact"); break;
			case DISPLAY_MONITORING_UP: v.push_back("Reprise du contact"); break;
			default: v.push_back("");
			}

			// Text
			v.push_back(c[_COL_FIELD]);
			v.push_back(c[_COL_TEXT]);

			return v;
		}



		std::string DisplayMaintenanceLog::getObjectName( uid id ) const
		{
			try
			{
				if(decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
				{
					shared_ptr<const DisplayScreen> screen = DisplayScreenTableSync::Get(id, Env::GetOfficialEnv(), UP_LINKS_LOAD_LEVEL);
					return screen->getFullName();
				}
				else if(decodeTableId(id) == DisplayScreenCPUTableSync::TABLE.ID)
				{
					shared_ptr<const DisplayScreenCPU> cpu = DisplayScreenCPUTableSync::Get(id, Env::GetOfficialEnv(), UP_LINKS_LOAD_LEVEL);
					return cpu->getFullName();
				}
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
			const DisplayScreen& screen,
			const DisplayMonitoringStatus& oldValue,
			const DisplayMonitoringStatus& newValue
		) {
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
				screen.getKey()
			);
		}



		void DisplayMaintenanceLog::AddMonitoringUpEntry(
			const DisplayScreen& screen,
			const ptime& downTime
		){
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_UP)));
			c.push_back(string());
			c.push_back("Etait perdu depuis " + to_simple_string(downTime));

			DBLog::_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_INFO,
				c,
				NULL,
				screen.getKey()
			);
		}

		void DisplayMaintenanceLog::AddMonitoringUpEntry(
			const DisplayScreenCPU& cpu,
			const ptime& downTime )
		{
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_UP)));
			c.push_back(string());
			c.push_back("Etait perdu depuis " + to_simple_string(downTime));

			DBLog::_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_INFO,
				c,
				NULL,
				cpu.getKey()
			);
		}



		void DisplayMaintenanceLog::AddMonitoringDownEntry(
			const DisplayScreen& screen
		){
			// Control last entry : if already a down entry, do not reyrite any identical entry
			shared_ptr<const DBLogEntry> lastEntry(DBLog::_getLastEntry(FACTORY_KEY, screen.getKey()));
			if(	lastEntry.get() == NULL ||
				lastEntry->getContent()[0] == Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_DOWN))
			){
				return;
			}

			// Write the entry
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_DOWN)));
			c.push_back(string());
			c.push_back(string());

			DBLog::_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_ERROR,
				c,
				NULL,
				screen.getKey()
			);
		}



		void DisplayMaintenanceLog::AddMonitoringFirstEntry(
			const DisplayScreen& screen,
			const DisplayMonitoringStatus& value
		) {
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
					screen.getKey()
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
					screen.getKey()
				);
			}
		}

		void DisplayMaintenanceLog::AddMonitoringFirstEntry( const DisplayScreenCPU& cpu, const DisplayMonitoringStatus& value )
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
				cpu.getKey()
			);
		}

		void DisplayMaintenanceLog::AddMonitoringDownEntry( const DisplayScreenCPU& cpu )
		{
			// Control last entry : if already a down entry, do not rewrite any identical entry
			shared_ptr<const DBLogEntry> lastEntry(DBLog::_getLastEntry(FACTORY_KEY, cpu.getKey()));
			if(	lastEntry.get() == NULL ||
				lastEntry->getContent()[0] == Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_DOWN))
			){
				return;
			}

			// Write the entry
			DBLogEntry::Content c;
			c.push_back(Conversion::ToString(static_cast<int>(DISPLAY_MONITORING_DOWN)));
			c.push_back(string());
			c.push_back(string());

			DBLog::_addEntry(
				FACTORY_KEY,
				DBLogEntry::DB_LOG_ERROR,
				c,
				NULL,
				cpu.getKey()
			);
		}
	}
}
