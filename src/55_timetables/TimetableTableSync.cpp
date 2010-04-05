
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
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "Interface.h"
#include "CommercialLine.h"
#include "PhysicalStop.h"
#include "ReplaceQuery.h"

#include "01_util/Conversion.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace calendar;
	using namespace interfaces;
	using namespace pt;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TimetableTableSync>::FACTORY_KEY("55.01 Timetables");
	}
	
	
	namespace timetables
	{
		const std::string TimetableTableSync::COL_BOOK_ID("book_id");
		const std::string TimetableTableSync::COL_RANK("rank");
		const std::string TimetableTableSync::COL_TITLE("title");
		const std::string TimetableTableSync::COL_CALENDAR_ID("calendar_id");
		const std::string TimetableTableSync::COL_FORMAT("format");
		const std::string TimetableTableSync::COL_INTERFACE_ID("interface_id");
		const std::string TimetableTableSync::COL_AUTHORIZED_LINES("authorized_lines");
		const std::string TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS("authorized_physical_stops");
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
			SQLiteTableSync::Field(TimetableTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_FORMAT, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_INTERFACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableTableSync::COL_AUTHORIZED_LINES, SQL_TEXT),
			SQLiteTableSync::Field(TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS, SQL_TEXT),
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
			object->setTitle(rows->getText(TimetableTableSync::COL_TITLE));
			object->setContentType(static_cast<Timetable::ContentType>(rows->getInt(TimetableTableSync::COL_FORMAT)));
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				vector<string> lines = Conversion::ToStringVector(rows->getText (TimetableTableSync::COL_AUTHORIZED_LINES));
				object->clearAuthorizedLines();
				BOOST_FOREACH(const string& line, lines)
				{
					try
					{
						RegistryKeyType id(lexical_cast<RegistryKeyType>(line));
						object->addAuthorizedLine(Env::GetOfficialEnv().get<CommercialLine>(id).get());
					}
					catch (ObjectNotFoundException<CommercialLine>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + TimetableTableSync::COL_AUTHORIZED_LINES);
					}
				}


				vector<string> pstops = Conversion::ToStringVector(rows->getText (TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS));
				object->clearAuthorizedPhysicalStops();
				BOOST_FOREACH(const string& pstop, pstops)
				{
					try
					{
						RegistryKeyType id(lexical_cast<RegistryKeyType>(pstop));
						object->addAuthorizedPhysicalStop(Env::GetOfficialEnv().get<PhysicalStop>(id).get());
					}
					catch (ObjectNotFoundException<PhysicalStop>& e)
					{
						Log::GetInstance().warn("Data corrupted in " + TABLE.NAME + "/" + TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS);
					}
				}


				{
					TimetableRowTableSync::SearchResult rows(
						TimetableRowTableSync::Search(env, object->getKey())
					);
					BOOST_FOREACH(shared_ptr<TimetableRow> row, rows)
					{
						object->addRow(*row);
					}
				}

				if(rows->getLongLong(TimetableTableSync::COL_CALENDAR_ID) > 0)
				{
					try
					{
						object->setBaseCalendar(
							Env::GetOfficialEnv().get<CalendarTemplate>(rows->getLongLong(TimetableTableSync::COL_CALENDAR_ID)).get()
						);
					}
					catch(ObjectNotFoundException<CalendarTemplate>)
					{
						Log::GetInstance().warn("Error in timetable definition : no such calendar template");
					}
				}

				if(rows->getLongLong(TimetableTableSync::COL_INTERFACE_ID) > 0)
				{
					try
					{
						object->setInterface(
							Env::GetOfficialEnv().get<Interface>(rows->getLongLong(TimetableTableSync::COL_INTERFACE_ID)).get()
						);
					}
					catch (ObjectNotFoundException<Interface> e)
					{
						Log::GetInstance().warn("Error in timetable definition : no such interface");
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableTableSync,Timetable>::Save(
			Timetable* object,
			optional<SQLiteTransaction&> transaction
		){
			// Preparation
			stringstream authorizedLines;
			{
				bool first(true);
				BOOST_FOREACH(const CommercialLine* line, object->getAuthorizedLines())
				{
					if(!first) authorizedLines << ",";
					authorizedLines << line->getKey();
					first = false;
				}
			}
			stringstream authorizedPhysicalStops;
			{
				bool first(true);
				BOOST_FOREACH(const PhysicalStop* pstop, object->getAuthorizedPhysicalStops())
				{
					if(!first) authorizedPhysicalStops << ",";
					authorizedPhysicalStops << pstop->getKey();
					first = false;
				}
			}

			// Writing
			ReplaceQuery<TimetableTableSync> query(*object);
			query.addField(object->getBookId());
			query.addField(object->getRank());
			query.addField(object->getTitle());
			query.addField(object->getBaseCalendar() ? object->getBaseCalendar()->getKey() : RegistryKeyType(0));
			query.addField(static_cast<int>(object->getContentType()));
			query.addField(object->getInterface() ? object->getInterface()->getKey() : RegistryKeyType(0));
			query.addField(authorizedLines.str());
			query.addField(authorizedPhysicalStops.str());
			query.execute(transaction);
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
				<< " AND " << COL_RANK << ">=" << rank
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
				throw util::Exception(e.getMessage());
			}
		}
	}
}
