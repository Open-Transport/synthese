
/** CalendarTemplateElementTableSync class implementation.
	@file CalendarTemplateElementTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include <sstream>

#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateElement.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"

#include "Date.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace time;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,CalendarTemplateElementTableSync>::FACTORY_KEY("55.11 Calendar Template Elements");
	}
	
	namespace timetables
	{
		const std::string CalendarTemplateElementTableSync::COL_CALENDAR_ID("calendar_id");
		const std::string CalendarTemplateElementTableSync::COL_RANK("rank");
		const std::string CalendarTemplateElementTableSync::COL_MIN_DATE("min_date");
		const std::string CalendarTemplateElementTableSync::COL_MAX_DATE("max_date");
		const std::string CalendarTemplateElementTableSync::COL_INTERVAL("interval");
		const std::string CalendarTemplateElementTableSync::COL_POSITIVE("positive");
		const std::string CalendarTemplateElementTableSync::COL_INCLUDE_ID("include_id");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::TABLE(
			"t055_calendar_template_elements"
		);
		
		
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_RANK, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_MIN_DATE, SQL_DATE),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_MAX_DATE, SQL_DATE),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_INTERVAL, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_POSITIVE, SQL_INTEGER),
			SQLiteTableSync::Field(CalendarTemplateElementTableSync::COL_INCLUDE_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::Load(
			CalendarTemplateElement* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setRank(rows->getInt(CalendarTemplateElementTableSync::COL_RANK));
			object->setIncludeId(rows->getLongLong(CalendarTemplateElementTableSync::COL_INCLUDE_ID));
			object->setMinDate(Date::FromSQLDate(rows->getText(CalendarTemplateElementTableSync::COL_MIN_DATE)));
			object->setMaxDate(Date::FromSQLDate(rows->getText(CalendarTemplateElementTableSync::COL_MAX_DATE)));
			object->setInterval(rows->getInt(CalendarTemplateElementTableSync::COL_INTERVAL));
			object->setPositive(rows->getBool(CalendarTemplateElementTableSync::COL_POSITIVE));
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::Save(
			CalendarTemplateElement* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << object->getRank()
				<< "," << object->getMinDate().toSQLString()
				<< "," << object->getMaxDate().toSQLString()
				<< "," << Conversion::ToString(object->getPositive())
				<< "," << Conversion::ToString(object->getIncludeId())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::Unlink(
			CalendarTemplateElement* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		CalendarTemplateElementTableSync::SearchResult CalendarTemplateElementTableSync::Search(
			Env& env,
			uid calendarId
			, int first /*= 0*/
			, boost::optional<std::size_t> number  /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (calendarId != UNKNOWN_VALUE)
				query << " AND " << COL_CALENDAR_ID << "=" << calendarId
			;
			query << " ORDER BY " << COL_RANK << " ASC";
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void CalendarTemplateElementTableSync::Shift( uid calendarId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE.NAME
				<< " SET " << COL_RANK << "=" << COL_RANK << ((delta > 0) ? "+" : "") << delta
				<< " WHERE " << COL_CALENDAR_ID << "=" << calendarId
				<< " AND " << COL_RANK << ((delta > 0) ? ">=" : "<=") << rank
				;

			sqlite->execUpdate(query.str());
		}



		int CalendarTemplateElementTableSync::GetMaxRank( uid calendarId )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << COL_RANK << ") AS mr "
				<< " FROM " << TABLE.NAME
				<< " WHERE " << COL_CALENDAR_ID << "=" << calendarId
				;

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				while (rows->next ())
				{
					return rows->getText("mr").empty() ? UNKNOWN_VALUE : rows->getInt("mr");
				}
				return UNKNOWN_VALUE;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
