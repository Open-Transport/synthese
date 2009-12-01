
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
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TimetableTableSync>::FACTORY_KEY("55.01 Timetables");
	}
	
	
	namespace timetables
	{
		const std::string TimetableTableSync::COL_BOOK_ID("book_id");
		const std::string TimetableTableSync::COL_RANK("rank");
		const std::string TimetableTableSync::COL_TITLE("title");
		const std::string TimetableTableSync::COL_MUST_BEGIN_A_PAGE("must_begin_a_page");
		const std::string TimetableTableSync::COL_CALENDAR_ID("calendar_id");
		const std::string TimetableTableSync::COL_IS_BOOK("is_book");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TimetableTableSync>::TABLE(
			"t052_timetables"
		);



		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TimetableTableSync>::_FIELDS[]=
		{

			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TimetableTableSync::COL_BOOK_ID, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_RANK, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_TITLE, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_MUST_BEGIN_A_PAGE, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_IS_BOOK, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TimetableTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				TimetableTableSync::COL_BOOK_ID.c_str(),
				TimetableTableSync::COL_RANK.c_str(),
				""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::Load(
			Timetable* object
			, const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setBookId(rows->getLongLong(TimetableTableSync::COL_BOOK_ID));
			object->setRank(rows->getInt(TimetableTableSync::COL_RANK));
			object->setMustBeginAPage(rows->getBool(TimetableTableSync::COL_MUST_BEGIN_A_PAGE));
			object->setTitle(rows->getText(TimetableTableSync::COL_TITLE));
			object->setIsBook(rows->getBool(TimetableTableSync::COL_IS_BOOK));
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				{
					TimetableRowTableSync::SearchResult rows(
						TimetableRowTableSync::Search(env, object->getKey())
						);
					BOOST_FOREACH(shared_ptr<TimetableRow> row, rows)
					{
						object->addRow(*row);
					}
				}

				try
				{
					object->setBaseCalendar(
						CalendarTemplateTableSync::Get(
							rows->getLongLong(TimetableTableSync::COL_CALENDAR_ID),
							env,
							UP_LINKS_LOAD_LEVEL
						).get()
					);
				}
				catch(ObjectNotFoundException<CalendarTemplate>)
				{
					Log::GetInstance().warn("Error in timetable definition : no such calendar template");
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::Save(
			Timetable* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< object->getKey()
				<< "," << object->getBookId()
				<< "," << object->getRank()
				<< "," << Conversion::ToSQLiteString(object->getTitle())
				<< "," << object->getMustBeginAPage()
				<< "," << (object->getBaseCalendar() ? object->getBaseCalendar()->getKey() : RegistryKeyType(0))
				<< "," << object->getIsBook()
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::Unlink(
			Timetable* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		TimetableTableSync::TimetableTableSync()
			: SQLiteNoSyncTableSyncTemplate<TimetableTableSync, Timetable>()
		{
		}



		TimetableTableSync::SearchResult TimetableTableSync::Search(
			Env& env,
			uid bookId
			, bool orderByParent
			, bool orderByTitle
			, bool raisingOrder
			, int first
			, int number,
			LinkLevel linkLevel
		){
			stringstream query;

			// Content
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
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

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void TimetableTableSync::Shift( uid bookId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE.NAME
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
				<< " FROM " << TABLE.NAME
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
