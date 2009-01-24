
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

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include "04_time/Date.h"

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
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::TABLE_NAME("t055_calendar_template_elements");
		template<> const int SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::TABLE_ID(55);
		template<> const bool SQLiteTableSyncTemplate<CalendarTemplateElementTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::load(
			CalendarTemplateElement* object
			, const db::SQLiteResultSPtr& rows
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



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::save(
			CalendarTemplateElement* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << object->getRank()
				<< "," << object->getMinDate().toSQLString()
				<< "," << object->getMaxDate().toSQLString()
				<< "," << Conversion::ToString(object->getPositive())
				<< "," << Conversion::ToString(object->getIncludeId())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::_link(
			CalendarTemplateElement* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>::_unlink(
			CalendarTemplateElement* obj
		){
		}
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




		CalendarTemplateElementTableSync::CalendarTemplateElementTableSync()
			: SQLiteNoSyncTableSyncTemplate<CalendarTemplateElementTableSync,CalendarTemplateElement>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_CALENDAR_ID, "INTEGER");
			addTableColumn(COL_RANK, "INTEGER");
			addTableColumn(COL_MIN_DATE, "DATE");
			addTableColumn(COL_MAX_DATE, "DATE");
			addTableColumn(COL_INTERVAL, "INTEGER");
			addTableColumn(COL_POSITIVE, "INTEGER");
			addTableColumn(COL_INCLUDE_ID, "INTEGER");
		}



		vector<shared_ptr<CalendarTemplateElement> > CalendarTemplateElementTableSync::Search(
			uid calendarId
			, int first /*= 0*/
			, int number /*= 0*/
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (calendarId != UNKNOWN_VALUE)
			 	query << " AND " << COL_CALENDAR_ID << "=" << calendarId
				;
			query << " ORDER BY " << COL_RANK << " ASC";
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<CalendarTemplateElement> > objects;
				while (rows->next ())
				{
					shared_ptr<CalendarTemplateElement> object(new CalendarTemplateElement);
					load(object.get(), rows);
					link(object.get(), rows, GET_AUTO);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}



		void CalendarTemplateElementTableSync::Shift( uid calendarId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE_NAME
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
				<< " FROM " << TABLE_NAME
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
