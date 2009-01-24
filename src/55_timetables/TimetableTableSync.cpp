
/** TimetableTableSync class implementation.
	@file TimetableTableSync.cpp
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

#include "Timetable.h"
#include "TimetableTableSync.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TimetableTableSync>::FACTORY_KEY("55.01 Timetables");
	}
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<TimetableTableSync>::TABLE_NAME("t052_timetables");
		template<> const int SQLiteTableSyncTemplate<TimetableTableSync>::TABLE_ID(52);
		template<> const bool SQLiteTableSyncTemplate<TimetableTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::load(
			Timetable* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setBookId(rows->getLongLong(TimetableTableSync::COL_BOOK_ID));
			object->setRank(rows->getInt(TimetableTableSync::COL_RANK));
			object->setTemplateCalendarId(rows->getLongLong(TimetableTableSync::COL_CALENDAR_ID));
			object->setMustBeginAPage(rows->getBool(TimetableTableSync::COL_MUST_BEGIN_A_PAGE));
			object->setTitle(rows->getText(TimetableTableSync::COL_TITLE));
			object->setIsBook(rows->getBool(TimetableTableSync::COL_IS_BOOK));
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::save(
			Timetable* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << object->getBookId()
				<< "," << object->getRank()
				<< "," << Conversion::ToSQLiteString(object->getTitle())
				<< "," << Conversion::ToString(object->getMustBeginAPage())
				<< "," << Conversion::ToString(object->getTemplateCalendarId())
				<< "," << Conversion::ToString(object->getIsBook())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::_link(
			Timetable* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
			vector<shared_ptr<TimetableRow> > grows(TimetableRowTableSync::Search(object->getKey()));
			for (vector<shared_ptr<TimetableRow> >::const_iterator it(grows.begin()); it != grows.end(); ++it)
				object->addRow(**it);
		}


		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::_unlink(
			Timetable* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		const std::string TimetableTableSync::COL_BOOK_ID("book_id");
		const std::string TimetableTableSync::COL_RANK("rank");
		const std::string TimetableTableSync::COL_TITLE("title");
		const std::string TimetableTableSync::COL_MUST_BEGIN_A_PAGE("must_begin_a_page");
		const std::string TimetableTableSync::COL_CALENDAR_ID("calendar_id");
		const std::string TimetableTableSync::COL_IS_BOOK("is_book");



		TimetableTableSync::TimetableTableSync()
			: SQLiteNoSyncTableSyncTemplate<TimetableTableSync, Timetable>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_BOOK_ID, "INTEGER");
			addTableColumn(COL_RANK, "INTEGER");
			addTableColumn(COL_TITLE, "TEXT");
			addTableColumn(COL_MUST_BEGIN_A_PAGE, "INTEGER");
			addTableColumn(COL_CALENDAR_ID, "INTEGER");
			addTableColumn(COL_IS_BOOK, "INTEGER");

			vector<string> c;
			c.push_back(COL_BOOK_ID);
			c.push_back(COL_RANK);
			addTableIndex(c);
		}




		vector<shared_ptr<Timetable> > TimetableTableSync::Search(
			uid bookId
			, bool orderByParent
			, bool orderByTitle
			, bool raisingOrder
			, int first
			, int number
		){
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			
			// Selection
			if (bookId != UNKNOWN_VALUE)
			 	query << " AND " << COL_BOOK_ID << "=" << bookId;

			// Ordering
			if (orderByParent)
				query << " ORDER BY " << COL_BOOK_ID << (raisingOrder ? " ASC" : " DESC")
					<< "," << COL_RANK << (raisingOrder ? " ASC" : " DESC");
			else if (orderByTitle)
				query << " ORDER BY " << COL_TITLE << (raisingOrder ? " ASC" : " DESC");

			// Size
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Timetable> > objects;
				while (rows->next ())
				{
					shared_ptr<Timetable> object(new Timetable);
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



		void TimetableTableSync::Shift( uid bookId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE_NAME
				<< " SET " << COL_RANK << "=" << COL_RANK << ((delta > 0) ? "+" : "") << delta
				<< " WHERE " << COL_BOOK_ID << "=" << bookId
				<< " AND " << COL_RANK << ((delta > 0) ? ">=" : "<=") << rank
				;

			sqlite->execUpdate(query.str());
		}



		int TimetableTableSync::GetMaxRank( uid bookId )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << COL_RANK << ") AS mr "
				<< " FROM " << TABLE_NAME
				<< " WHERE " << COL_BOOK_ID << "=" << bookId
			;

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				while (rows->next ())
				{
					return rows->getInt("mr");
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
