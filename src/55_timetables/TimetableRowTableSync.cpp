
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

#include <sstream>

#include "TimetableRowTableSync.h"
#include "TimetableRow.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "Conversion.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace env;

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
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setCompulsory(static_cast<TimetableRow::tTypeGareIndicateur>(rows->getInt(TimetableRowTableSync::COL_IS_COMPULSORY)));
			object->setIsArrival(rows->getBool(TimetableRowTableSync::COL_IS_ARRIVAL));
			object->setIsDeparture(rows->getBool(TimetableRowTableSync::COL_IS_DEPARTURE));
			
			try
			{
				object->setPlace(
					ConnectionPlaceTableSync::GetEditable(
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
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << object->getTimetableId()
				<< "," << object->getRank()
				<< "," << (object->getPlace() ? object->getPlace()->getKey() : uid(0))
				<< "," << Conversion::ToString(object->getIsDeparture())
				<< "," << Conversion::ToString(object->getIsArrival())
				<< "," << static_cast<int>(object->getCompulsory())
				<< ")";
			sqlite->execUpdate(query.str(), transaction);
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
			uid timetableId
			, bool orderByTimetable
			, bool raisingOrder
			, int first
			, boost::optional<std::size_t> number,
			LinkLevel linkLevel
		){
			stringstream query;
			
			// Content
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			
			// Selection
			if (timetableId != UNKNOWN_VALUE)
			 	query << " AND " << COL_TIMETABLE_ID << "=" << timetableId;
			
			// Ordering
			if (orderByTimetable)
				query << " ORDER BY " << COL_TIMETABLE_ID << (raisingOrder ? " ASC" : " DESC")
					<< "," << COL_RANK << (raisingOrder ? " ASC" : " DESC");

			// Size
			if (number)
				query << " LIMIT " << Conversion::ToString(*number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		void TimetableRowTableSync::Shift( uid timetableId , int rank , int delta )
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



		int TimetableRowTableSync::GetMaxRank( uid timetableId )
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
