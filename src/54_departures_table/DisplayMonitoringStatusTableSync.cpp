////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatusTableSync class implementation.
///	@file DisplayMonitoringStatusTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-19 10:50
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

#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayScreen.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "Conversion.h"
#include "DisplayMaintenanceLog.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departurestable;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,DisplayMonitoringStatusTableSync>::FACTORY_KEY("34.10 Display monitoring status");
	}

	namespace departurestable
	{
		const string DisplayMonitoringStatusTableSync::COL_SCREEN_ID("screen_id");
		const string DisplayMonitoringStatusTableSync::COL_TIME("time");
		const string DisplayMonitoringStatusTableSync::COL_GENERAL_STATUS("general_status");
		const string DisplayMonitoringStatusTableSync::COL_MEMORY_STATUS("memory_status");
		const string DisplayMonitoringStatusTableSync::COL_CLOCK_STATUS("clock_status");
		const string DisplayMonitoringStatusTableSync::COL_EEPROM_STATUS("eeprom_status");
		const string DisplayMonitoringStatusTableSync::COL_TEMP_SENSOR_STATUS("temp_sensor_status");
		const string DisplayMonitoringStatusTableSync::COL_LIGHT_STATUS("light_status");
		const string DisplayMonitoringStatusTableSync::COL_LIGHT_DETAIL("light_detail");
		const string DisplayMonitoringStatusTableSync::COL_DISPLAY_STATUS("display_status");
		const string DisplayMonitoringStatusTableSync::COL_DISPLAY_DETAIL("display_detail");
		const string DisplayMonitoringStatusTableSync::COL_SOUND_STATUS("sound_status");
		const string DisplayMonitoringStatusTableSync::COL_SOUND_DETAIL("sound_detail");
		const string DisplayMonitoringStatusTableSync::COL_TEMPERATURE_STATUS("temperature_status");
		const string DisplayMonitoringStatusTableSync::COL_TEMPERATURE_VALUE("temperature_value");
		const string DisplayMonitoringStatusTableSync::COL_COMMUNICATION_STATUS("communication_status");
		const string DisplayMonitoringStatusTableSync::COL_LOCALIZATION_STATUS("localization_status");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<DisplayMonitoringStatusTableSync>::TABLE(
			"t057_display_monitoring_status",
			true
		);


		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<DisplayMonitoringStatusTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_SCREEN_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_TIME, SQL_TIMESTAMP),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_GENERAL_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_MEMORY_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_CLOCK_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_EEPROM_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_TEMP_SENSOR_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_LIGHT_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_LIGHT_DETAIL, SQL_TEXT),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_DISPLAY_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_DISPLAY_DETAIL, SQL_TEXT),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_SOUND_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_SOUND_DETAIL, SQL_TEXT),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_TEMPERATURE_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_TEMPERATURE_VALUE, SQL_DOUBLE),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_COMMUNICATION_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayMonitoringStatusTableSync::COL_LOCALIZATION_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<DisplayMonitoringStatusTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(DisplayMonitoringStatusTableSync::COL_SCREEN_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<DisplayMonitoringStatusTableSync,DisplayMonitoringStatus>::Load(
			DisplayMonitoringStatus* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setTime(rows->getTimestamp(DisplayMonitoringStatusTableSync::COL_TIME));
			object->setGeneralStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_GENERAL_STATUS)));
			object->setMemoryStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_MEMORY_STATUS)));
			object->setClockStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_CLOCK_STATUS)));
			object->setEepromStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_EEPROM_STATUS)));
			object->setTempSensorStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_TEMP_SENSOR_STATUS)));
			object->setLightStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_LIGHT_STATUS)));
			object->setLightDetail(rows->getText(DisplayMonitoringStatusTableSync::COL_LIGHT_DETAIL));
			object->setDisplayStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_DISPLAY_STATUS)));
			object->setDisplayDetail(rows->getText(DisplayMonitoringStatusTableSync::COL_DISPLAY_DETAIL));
			object->setSoundStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_SOUND_STATUS)));
			object->setSoundDetail(rows->getText(DisplayMonitoringStatusTableSync::COL_SOUND_DETAIL));
			object->setTemperatureStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_TEMPERATURE_STATUS)));
			object->setTemperatureValue(rows->getDouble(DisplayMonitoringStatusTableSync::COL_TEMPERATURE_VALUE));
			object->setCommunicationStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_COMMUNICATION_STATUS)));
			object->setLocalizationStatus(static_cast<DisplayMonitoringStatus::Status>(rows->getInt(DisplayMonitoringStatusTableSync::COL_LOCALIZATION_STATUS)));

			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType id(rows->getLongLong(DisplayMonitoringStatusTableSync::COL_SCREEN_ID));
				try
				{
					object->setScreen(NULL);
					object->setCPU(NULL);
					if(decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
					{
						object->setScreen(DisplayScreenTableSync::Get(id, env, linkLevel).get());
					}
					else if(decodeTableId(id) == DisplayScreenCPUTableSync::TABLE.ID)
					{
						object->setCPU(DisplayScreenCPUTableSync::Get(id, env, linkLevel).get());
					}
					else
					{
						Log::GetInstance().warn("Data corrupted in "+ TABLE.NAME + " on display screen : "+ Conversion::ToString(rows->getLongLong(DisplayMonitoringStatusTableSync::COL_SCREEN_ID)) + " not found");
					}
				}
				catch (ObjectNotFoundException<DisplayScreen>&)
				{
					Log::GetInstance().warn("Data corrupted in "+ TABLE.NAME + " on display screen : "+ Conversion::ToString(rows->getLongLong(DisplayMonitoringStatusTableSync::COL_SCREEN_ID)) + " not found");
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<DisplayMonitoringStatusTableSync,DisplayMonitoringStatus>::Save(
			DisplayMonitoringStatus* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey()) << "," <<
				(	(object->getScreen() == NULL) ? 
					(object->getCPU() == NULL ? "0" : lexical_cast<string>(object->getCPU()->getKey())) : 
					lexical_cast<string>(object->getScreen()->getKey())
				) << "," <<
				"'" << to_iso_string(object->getTime()) << "',"
				<< static_cast<int>(object->getGeneralStatus()) << ","
				<< static_cast<int>(object->getMemoryStatus()) << ","
				<< static_cast<int>(object->getClockStatus()) << ","
				<< static_cast<int>(object->getEepromStatus()) << ","
				<< static_cast<int>(object->getTempSensorStatus()) << ","
				<< static_cast<int>(object->getLightStatus()) << ","
				<< Conversion::ToSQLiteString(object->getLightDetail()) << ","
				<< static_cast<int>(object->getDisplayStatus()) << ","
				<< Conversion::ToSQLiteString(object->getDisplayDetail()) << ","
				<< static_cast<int>(object->getSoundStatus()) << ","
				<< Conversion::ToSQLiteString(object->getSoundDetail()) << ","
				<< static_cast<int>(object->getTemperatureStatus()) << ","
				<< Conversion::ToString(object->getTemperatureValue()) << ","
				<< static_cast<int>(object->getCommunicationStatus()) << ","
				<< static_cast<int>(object->getLocalizationStatus())
				<< ")";
			sqlite->execUpdate(query.str());
		}


		
		template<> void SQLiteDirectTableSyncTemplate<DisplayMonitoringStatusTableSync,DisplayMonitoringStatus>::Unlink(
			DisplayMonitoringStatus* obj
		){
		}
	}
	
	
	
	namespace departurestable
	{
		DisplayMonitoringStatusTableSync::SearchResult DisplayMonitoringStatusTableSync::Search(
			Env& env,
			RegistryKeyType screenId,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			bool orderByScreenId,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (screenId != UNKNOWN_VALUE)
			{
				query << " AND " << COL_SCREEN_ID << "=" << Conversion::ToString(screenId);
			}
			if (orderByScreenId)
				query << " ORDER BY " << COL_SCREEN_ID << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << Conversion::ToString(*number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		boost::shared_ptr<DisplayMonitoringStatus> DisplayMonitoringStatusTableSync::GetStatus(
			const DisplayScreen& screen
		){
			Env env;
			SearchResult entries(
				Search(env, screen.getKey(), 0, 1, true, true, FIELDS_ONLY_LOAD_LEVEL)
			);

			if(entries.empty())
			{
				return shared_ptr<DisplayMonitoringStatus>();
			}

			shared_ptr<DisplayMonitoringStatus> status(entries.front());
			if(screen.isDown(*status))
			{
				DisplayMaintenanceLog::AddMonitoringDownEntry(screen);
			}

			return status;
		}



		boost::posix_time::ptime DisplayMonitoringStatusTableSync::GetLastContact(
			const DisplayScreenCPU& cpu
		){
			Env env;
			SearchResult entries(
				Search(env, cpu.getKey(), 0, 1, true, true, FIELDS_ONLY_LOAD_LEVEL)
			);

			if(entries.empty())
			{
				return not_a_date_time;
			}

			shared_ptr<DisplayMonitoringStatus> status(entries.front());
			if(cpu.isDown(status->getTime()))
			{
				DisplayMaintenanceLog::AddMonitoringDownEntry(cpu);
			}

			return status->getTime();
		}
	}
}
