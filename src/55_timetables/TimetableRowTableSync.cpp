
/** TimetableRowTableSync class implementation.
	@file TimetableRowTableSync.cpp
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

#include "TimetableRowTableSync.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "StopAreaTableSync.hpp"
#include "DBModule.h"
#include "SQLite.h"
#include "SQLiteResult.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,TimetableRowTableSync>::FACTORY_KEY("55.02 Timetable rows");
	}
	
	namespace timetables
	{
		const string TimetableRowTableSync::COL_TIMETABLE_ID("timetable_id");
		const string TimetableRowTableSync::COL_RANK("rank");
		const string TimetableRowTableSync::COL_PLACE_ID("place_id");
		const string TimetableRowTableSync::COL_IS_DEPARTURE("is_departure");
		const string TimetableRowTableSync::COL_IS_ARRIVAL("is_arrival");
		const string TimetableRowTableSync::COL_IS_COMPULSORY("is_compulsory");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<TimetableRowTableSync>::TABLE(
			"t053_timetable_rows"
		);
		
		
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<TimetableRowTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(TimetableRowTableSync::COL_TIMETABLE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableRowTableSync::COL_RANK, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableRowTableSync::COL_PLACE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableRowTableSync::COL_IS_DEPARTURE, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableRowTableSync::COL_IS_ARRIVAL, SQL_INTEGER),
			SQLiteTableSync::Field(TimetableRowTableSync::COL_IS_COMPULSORY, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<TimetableRowTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				TimetableRowTableSync::COL_TIMETABLE_ID.c_str()
				, TimetableRowTableSync::COL_RANK.c_str()
			, ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::Load(
			TimetableRow* object
			, const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Properties
			object->setKey(rows->getLongLong(TABLE_COL_ID));
			object->setCompulsory(static_cast<TimetableRow::tTypeGareIndicateur>(rows->getInt(TimetableRowTableSync::COL_IS_COMPULSORY)));
			object->setIsArrival(rows->getBool(TimetableRowTableSync::COL_IS_ARRIVAL));
			object->setIsDeparture(rows->getBool(TimetableRowTableSync::COL_IS_DEPARTURE));
			
			try
			{
				object->setPlace(
					StopAreaTableSync::GetEditable(
						rows->getLongLong(TimetableRowTableSync::COL_PLACE_ID),
						env, linkLevel
					).get()
				);
			}
			catch (...)
			{
				Log::GetInstance().warn("Error in timetable definition : no such place");
			}

			object->setRank(rows->getInt(TimetableRowTableSync::COL_RANK));

			object->setTimetableId(rows->getLongLong(TimetableRowTableSync::COL_TIMETABLE_ID));
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::Save(
			TimetableRow* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<TimetableRowTableSync> query(*object);
			query.addField(object->getTimetableId());
			query.addField(object->getRank());
			query.addField(object->getPlace() ? object->getPlace()->getKey() : RegistryKeyType(0));
			query.addField(object->getIsDeparture());
			query.addField(object->getIsArrival());
			query.addField(static_cast<int>(object->getCompulsory()));
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::Unlink(
			TimetableRow* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		TimetableRowTableSync::SearchResult TimetableRowTableSync::Search(
			Env& env,
			optional<RegistryKeyType> timetableId
			, bool orderByTimetable
			, bool raisingOrder
			, int first
			, boost::optional<std::size_t> number,
			LinkLevel linkLevel
		){
			SelectQuery<TimetableRowTableSync> query;

			// Selection
			if (timetableId)
			{
				query.addWhereField(COL_TIMETABLE_ID, *timetableId);
			}
			
			// Ordering
			if (orderByTimetable)
			{
				query.addOrderField(COL_TIMETABLE_ID, raisingOrder);
				query.addOrderField(COL_RANK, raisingOrder);
			}

			// Size
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
		}



		void TimetableRowTableSync::Shift( util::RegistryKeyType timetableId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE.NAME
				<< " SET " << COL_RANK << "=" << COL_RANK << ((delta > 0) ? "+" : "") << delta
				<< " WHERE " << COL_TIMETABLE_ID << "=" << timetableId
				<< " AND " << COL_RANK << ">=" << rank
				;

			sqlite->execUpdate(query.str());
	
		}



		int TimetableRowTableSync::GetMaxRank( util::RegistryKeyType timetableId )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << COL_RANK << ") AS mr "
				<< " FROM " << TABLE.NAME
				<< " WHERE " << COL_TIMETABLE_ID << "=" << timetableId
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
