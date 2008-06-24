
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

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include "15_env/PublicTransportStopZoneConnectionPlace.h"

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
	
	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<TimetableRowTableSync>::TABLE_NAME("t053_timetable_rows");
		template<> const int SQLiteTableSyncTemplate<TimetableRowTableSync>::TABLE_ID(53);
		template<> const bool SQLiteTableSyncTemplate<TimetableRowTableSync>::HAS_AUTO_INCREMENT(true);



		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::load(
			TimetableRow* object
			, const db::SQLiteResultSPtr& rows
		){
			// Columns reading
			uid id(rows->getLongLong(TABLE_COL_ID));

			// Properties
			object->setKey(id);
			object->setCompulsory(static_cast<tTypeGareIndicateur>(rows->getInt(TimetableRowTableSync::COL_IS_COMPULSORY)));
			object->setIsArrival(rows->getBool(TimetableRowTableSync::COL_IS_ARRIVAL));
			object->setIsDeparture(rows->getBool(TimetableRowTableSync::COL_IS_DEPARTURE));
			
			try
			{
				object->setPlace(PublicTransportStopZoneConnectionPlace::Get(rows->getLongLong(TimetableRowTableSync::COL_PLACE_ID)).get());
			}
			catch (...)
			{
				Log::GetInstance().warn("Error in timetable definition : no such place");
			}

			object->setRank(rows->getInt(TimetableRowTableSync::COL_RANK));

			object->setTimetableId(rows->getLongLong(TimetableRowTableSync::COL_TIMETABLE_ID));
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::save(
			TimetableRow* object
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << object->getTimetableId()
				<< "," << object->getRank()
				<< "," << (object->getPlace() ? object->getPlace()->getId() : uid(0))
				<< "," << Conversion::ToString(object->getIsDeparture())
				<< "," << Conversion::ToString(object->getIsArrival())
				<< "," << static_cast<int>(object->getCompulsory())
				<< ")";
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::_link(
			TimetableRow* object
			, const SQLiteResultSPtr& rows
			, GetSource temporary
		){
		}


		template<> void SQLiteDirectTableSyncTemplate<TimetableRowTableSync,TimetableRow>::_unlink(
			TimetableRow* obj
		){
		}
	}
	
	
	
	namespace timetables
	{
		const std::string TimetableRowTableSync::COL_TIMETABLE_ID("timetable_id");
		const std::string TimetableRowTableSync::COL_RANK("rank");
		const std::string TimetableRowTableSync::COL_PLACE_ID("place_id");
		const std::string TimetableRowTableSync::COL_IS_DEPARTURE("is_departure");
		const std::string TimetableRowTableSync::COL_IS_ARRIVAL("is_arrival");
		const std::string TimetableRowTableSync::COL_IS_COMPULSORY("is_compulsory");



		TimetableRowTableSync::TimetableRowTableSync()
			: SQLiteNoSyncTableSyncTemplate<TimetableRowTableSync,TimetableRow>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_TIMETABLE_ID, "INTEGER");
			addTableColumn(COL_RANK, "INTEGER");
			addTableColumn(COL_PLACE_ID, "INTEGER");
			addTableColumn(COL_IS_DEPARTURE, "INTEGER");
			addTableColumn(COL_IS_ARRIVAL, "INTEGER");
			addTableColumn(COL_IS_COMPULSORY, "INTEGER");

			vector<string> c;
			c.push_back(COL_TIMETABLE_ID);
			c.push_back(COL_RANK);
			addTableIndex(c);
		}



		vector<shared_ptr<TimetableRow> > TimetableRowTableSync::Search(
			uid timetableId
			, bool orderByTimetable
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
			if (timetableId != UNKNOWN_VALUE)
			 	query << " AND " << COL_TIMETABLE_ID << "=" << timetableId;
			
			// Ordering
			if (orderByTimetable)
				query << " ORDER BY " << COL_TIMETABLE_ID << (raisingOrder ? " ASC" : " DESC")
					<< "," << COL_RANK << (raisingOrder ? " ASC" : " DESC");

			// Size
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<TimetableRow> > objects;
				while (rows->next ())
				{
					shared_ptr<TimetableRow> object(new TimetableRow);
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



		void TimetableRowTableSync::Shift( uid timetableId , int rank , int delta )
		{
			SQLite* sqlite = DBModule::GetSQLite();

			stringstream query;

			// Content
			query
				<< "UPDATE " << TABLE_NAME
				<< " SET " << COL_RANK << "=" << COL_RANK << ((delta > 0) ? "+" : "") << delta
				<< " WHERE " << COL_TIMETABLE_ID << "=" << timetableId
				<< " AND " << COL_RANK << ((delta > 0) ? ">=" : "<=") << rank
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
				<< " FROM " << TABLE_NAME
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
