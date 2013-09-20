
/** TimetableTableSync class implementation.
	@file TimetableTableSync.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "TimetableTableSync.h"

#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLine.h"
#include "Conversion.h"
#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "Profile.h"
#include "RankUpdateQuery.hpp"
#include "ReplaceQuery.h"
#include "Session.h"
#include "StopPoint.hpp"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace calendar;
	using namespace pt;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TimetableTableSync>::FACTORY_KEY("55.01 Timetables");
	}

	namespace timetables
	{
		const std::string TimetableTableSync::COL_BOOK_ID("book_id");
		const std::string TimetableTableSync::COL_RANK("rank");
		const std::string TimetableTableSync::COL_TITLE("title");
		const std::string TimetableTableSync::COL_CALENDAR_ID("calendar_id");
		const std::string TimetableTableSync::COL_FORMAT("format");
		const std::string TimetableTableSync::COL_AUTHORIZED_LINES("authorized_lines");
		const std::string TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS("authorized_physical_stops");
		const std::string TimetableTableSync::COL_TRANSFER_TIMETABLE_BEFORE("transfer_timetable_before");
		const std::string TimetableTableSync::COL_TRANSFER_TIMETABLE_AFTER("transfer_timetable_after");
		const std::string TimetableTableSync::COL_IGNORE_EMPTY_ROWS = "ignore_empty_rows";
		const std::string TimetableTableSync::COL_COMPRESSION = "compression";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TimetableTableSync>::TABLE(
			"t052_timetables"
		);



		template<> const Field DBTableSyncTemplate<TimetableTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(TimetableTableSync::COL_BOOK_ID, SQL_INTEGER),
			Field(TimetableTableSync::COL_RANK, SQL_INTEGER),
			Field(TimetableTableSync::COL_TITLE, SQL_TEXT),
			Field(TimetableTableSync::COL_CALENDAR_ID, SQL_INTEGER),
			Field(TimetableTableSync::COL_FORMAT, SQL_INTEGER),
			Field(TimetableTableSync::COL_AUTHORIZED_LINES, SQL_TEXT),
			Field(TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS, SQL_TEXT),
			Field(TimetableTableSync::COL_TRANSFER_TIMETABLE_BEFORE, SQL_INTEGER),
			Field(TimetableTableSync::COL_TRANSFER_TIMETABLE_AFTER, SQL_INTEGER),
			Field(TimetableTableSync::COL_IGNORE_EMPTY_ROWS, SQL_BOOLEAN),
			Field(TimetableTableSync::COL_COMPRESSION, SQL_BOOLEAN),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TimetableTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					TimetableTableSync::COL_BOOK_ID.c_str(),
					TimetableTableSync::COL_RANK.c_str(),
			"")	);
			return r;
		}



		template<> void OldLoadSavePolicy<TimetableTableSync,Timetable>::Load(
			Timetable* object
			, const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Properties
			object->setKey(rows->getLongLong(TABLE_COL_ID));
			object->setBookId(rows->getLongLong(TimetableTableSync::COL_BOOK_ID));
			object->setRank(rows->getInt(TimetableTableSync::COL_RANK));
			object->setTitle(rows->getText(TimetableTableSync::COL_TITLE));
			object->setContentType(static_cast<Timetable::ContentType>(rows->getInt(TimetableTableSync::COL_FORMAT)));
			object->setIgnoreEmptyRows(rows->getBool(TimetableTableSync::COL_IGNORE_EMPTY_ROWS));
			object->setCompression(rows->getBool(TimetableTableSync::COL_COMPRESSION));

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
					catch (ObjectNotFoundException<CommercialLine>&)
					{
						Log::GetInstance().warn("Data corrupted in " + TimetableTableSync::TABLE.NAME + "/" + TimetableTableSync::COL_AUTHORIZED_LINES);
					}
				}


				vector<string> pstops = Conversion::ToStringVector(rows->getText (TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS));
				object->clearAuthorizedPhysicalStops();
				BOOST_FOREACH(const string& pstop, pstops)
				{
					try
					{
						RegistryKeyType id(lexical_cast<RegistryKeyType>(pstop));
						object->addAuthorizedPhysicalStop(Env::GetOfficialEnv().get<StopPoint>(id).get());
					}
					catch (ObjectNotFoundException<StopPoint>&)
					{
						Log::GetInstance().warn("Data corrupted in " + TimetableTableSync::TABLE.NAME + "/" + TimetableTableSync::COL_AUTHORIZED_PHYSICAL_STOPS);
					}
				}

				object->clearRows();
				{
					TimetableRowTableSync::SearchResult rows(
						TimetableRowTableSync::Search(env, object->getKey())
					);
					BOOST_FOREACH(const boost::shared_ptr<TimetableRow>& row, rows)
					{
						object->addRow(*row);
					}
				}

				if(rows->getLongLong(TimetableTableSync::COL_CALENDAR_ID) > 0)
				{
					try
					{
						RegistryKeyType baseCalendarId(rows->getLongLong(TimetableTableSync::COL_CALENDAR_ID));
						object->setBaseCalendar(
							CalendarTemplateTableSync::Get(baseCalendarId, env).get()
						);
						CalendarTemplateElementTableSync::Search(env, baseCalendarId);
					}
					catch(ObjectNotFoundException<CalendarTemplate>)
					{
						Log::GetInstance().warn("Error in timetable definition : no such calendar template");
					}
				}

				// Transfer timetable before
				if(rows->getLongLong(TimetableTableSync::COL_TRANSFER_TIMETABLE_BEFORE) > 0)
				{
					try
					{
						object->setTransferTimetableBefore(
							TimetableTableSync::GetEditable(
								rows->getLongLong(TimetableTableSync::COL_TRANSFER_TIMETABLE_BEFORE),
								env,
								linkLevel
							).get()
						);
					}
					catch(ObjectNotFoundException<Timetable>)
					{
						Log::GetInstance().warn("Error in timetable definition : no such before transfer timetable");
					}
				}

				// Transfer timetable after
				if(rows->getLongLong(TimetableTableSync::COL_TRANSFER_TIMETABLE_AFTER) > 0)
				{
					try
					{
						object->setTransferTimetableAfter(
							TimetableTableSync::GetEditable(
								rows->getLongLong(TimetableTableSync::COL_TRANSFER_TIMETABLE_AFTER),
								env,
								linkLevel
							).get()
						);
					}
					catch(ObjectNotFoundException<Timetable>)
					{
						Log::GetInstance().warn("Error in timetable definition : no such after transfer timetable");
					}
				}
			}
		}



		template<> void OldLoadSavePolicy<TimetableTableSync,Timetable>::Save(
			Timetable* object,
			optional<DBTransaction&> transaction
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
				BOOST_FOREACH(const StopPoint* pstop, object->getAuthorizedPhysicalStops())
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
			query.addField(authorizedLines.str());
			query.addField(authorizedPhysicalStops.str());
			query.addField(object->getTransferTimetableBefore(1) ? object->getTransferTimetableBefore(1)->getKey() : RegistryKeyType(0));
			query.addField(object->getTransferTimetableAfter(1) ? object->getTransferTimetableAfter(1)->getKey() : RegistryKeyType(0));
			query.addField(object->getIgnoreEmptyRows());
			query.addField(object->getCompression());
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<TimetableTableSync,Timetable>::Unlink(
			Timetable* obj
		){
		}


		template<> bool DBTableSyncTemplate<TimetableTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<TimetableTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			TimetableRowTableSync::SearchResult rows(TimetableRowTableSync::Search(env, id));
			BOOST_FOREACH(const TimetableRowTableSync::SearchResult::value_type& row, rows)
			{
				TimetableRowTableSync::Remove(NULL, row->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<TimetableTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TimetableTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace timetables
	{
		TimetableTableSync::SearchResult TimetableTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> bookId
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
			if (bookId)
			{
				query << " AND " << COL_BOOK_ID << "=" << *bookId;
			}

			// Ordering
			if (orderByParent)
				query << " ORDER BY " << COL_BOOK_ID << (raisingOrder ? " ASC" : " DESC")
					<< "," << COL_RANK << (raisingOrder ? " ASC" : " DESC");
			else if (orderByTitle)
				query << " ORDER BY " << COL_TITLE << (raisingOrder ? " ASC" : " DESC");

			// Size
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void TimetableTableSync::Shift( util::RegistryKeyType bookId , int rank , int delta )
		{
			RankUpdateQuery<TimetableTableSync> query(COL_RANK, delta, rank);
			query.addWhereField(COL_BOOK_ID, bookId);
			query.execute();
		}



		optional<size_t> TimetableTableSync::GetMaxRank( util::RegistryKeyType bookId )
		{
			DB* db = DBModule::GetDB();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << COL_RANK << ") AS mr "
				<< " FROM " << TABLE.NAME
				<< " WHERE " << COL_BOOK_ID << "=" << bookId
			;

			try
			{
				DBResultSPtr rows = db->execQuery(query.str());
				while (rows->next ())
				{
					return rows->getOptionalUnsignedInt("mr");
				}
				return optional<size_t>();
			}
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}
}	}
