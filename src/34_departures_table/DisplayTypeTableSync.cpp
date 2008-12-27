////////////////////////////////////////////////////////////////////////////////
/// DisplayTypeTableSync class implementation.
///	@file DisplayTypeTableSync.cpp
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

#include "DisplayTypeTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "InterfaceTableSync.h"
#include "Conversion.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace departurestable;
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,DisplayTypeTableSync>::FACTORY_KEY("34.00 Display Types");
	}

	namespace departurestable
	{
		const string DisplayTypeTableSync::COL_NAME = "name";
		const string DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID = "interface_id";
		const string DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID = "audio_interface_id";
		const string DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID = "monitoring_interface_id";
		const string DisplayTypeTableSync::COL_ROWS_NUMBER = "rows_number";
		const string DisplayTypeTableSync::COL_MAX_STOPS_NUMBER("max_stops_number");
		const string DisplayTypeTableSync::COL_TIME_BETWEEN_CHECKS("time_between_checks");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<DisplayTypeTableSync>::TABLE(
			"t036_display_types"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<DisplayTypeTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_ROWS_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_MAX_STOPS_NUMBER, SQL_INTEGER),
			SQLiteTableSync::Field(DisplayTypeTableSync::COL_TIME_BETWEEN_CHECKS, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<DisplayTypeTableSync>::_INDEXES[] =
		{
			SQLiteTableSyncTemplate::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::Load(
			DisplayType* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( DisplayTypeTableSync::COL_NAME));
			object->setRowNumber(rows->getInt ( DisplayTypeTableSync::COL_ROWS_NUMBER));
			object->setMaxStopsNumber(rows->getInt ( DisplayTypeTableSync::COL_MAX_STOPS_NUMBER));
			object->setTimeBetweenChecks(rows->getInt(DisplayTypeTableSync::COL_TIME_BETWEEN_CHECKS));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				try
				{
					uid id(rows->getLongLong(DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID));
					if (id > 0)
					{
						object->setDisplayInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID, e);
				}
				try
				{
					uid id(rows->getLongLong(DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID));
					if (id > 0)
					{
						object->setAudioInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayTypeTableSync::COL_AUDIO_INTERFACE_ID, e);
				}
				try
				{
					uid id(rows->getLongLong(DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID));
					if (id > 0)
					{
						object->setMonitoringInterface(InterfaceTableSync::Get(id, env, linkLevel).get());
					}
				}
				catch (ObjectNotFoundException<Interface>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + DisplayTypeTableSync::COL_MONITORING_INTERFACE_ID, e);
				}
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::Unlink(
			DisplayType* obj
		){
			obj->setDisplayInterface(NULL);
			obj->setAudioInterface(NULL);
			obj->setMonitoringInterface(NULL);
		}

    

		template<> void SQLiteDirectTableSyncTemplate<DisplayTypeTableSync,DisplayType>::Save(
			DisplayType* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			if (object->getKey() <= 0)
				object->setKey(getId());
			stringstream query;
			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << ((object->getDisplayInterface() != NULL) ? Conversion::ToString(object->getDisplayInterface()->getKey()) : "0")
				<< "," << ((object->getAudioInterface() != NULL) ? Conversion::ToString(object->getAudioInterface()->getKey()) : "0")
				<< "," << ((object->getMonitoringInterface() != NULL) ? Conversion::ToString(object->getMonitoringInterface()->getKey()) : "0")
				<< "," << Conversion::ToString(object->getRowNumber())
				<< "," << Conversion::ToString(object->getMaxStopsNumber())
				<< "," << Conversion::ToString(object->getTimeBetweenChecks())
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace departurestable
	{
		DisplayTypeTableSync::DisplayTypeTableSync()
			: SQLiteRegistryTableSyncTemplate<DisplayTypeTableSync,DisplayType>()
		{
		}

		void DisplayTypeTableSync::Search(
			Env& env,
			string exactName,
			RegistryKeyType interfaceId,
			int first, /*= 0*/
			int number, /*= 0*/
			bool orderByName,
			bool orderByInterfaceName,
			bool orderByRows,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT t.*"
				<< " FROM " << TABLE.NAME << " AS t";
			if (orderByInterfaceName)
			{
				query << " INNER JOIN " << InterfaceTableSync::TABLE.NAME << " AS i ON i." << TABLE_COL_ID  << "=t." << DisplayTypeTableSync::COL_DISPLAY_INTERFACE_ID;
			}

			query << " WHERE 1";
			if (!exactName.empty())
			{
				query << " AND t." << COL_NAME << " LIKE " << Conversion::ToSQLiteString(exactName);
			}
			if (interfaceId != UNKNOWN_VALUE)
			{
				query << " AND t." << COL_DISPLAY_INTERFACE_ID << "=" << Conversion::ToString(interfaceId);
			}

			if (orderByName)
			{
				query << " ORDER BY t." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			else if(orderByInterfaceName)
			{
				query << " ORDER BY i." << InterfaceTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			else if(orderByRows)
			{
				query << " ORDER BY t." << COL_ROWS_NUMBER << (raisingOrder ? " ASC" : " DESC");
			}

			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
