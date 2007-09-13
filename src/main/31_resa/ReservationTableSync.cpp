
/** ReservationTableSync class implementation.
	@file ReservationTableSync.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "04_time/DateTime.h"

#include "31_resa/Reservation.h"
#include "31_resa/ReservationTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace time;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Reservation>::TABLE_NAME = "t044_reservations";
		template<> const int SQLiteTableSyncTemplate<Reservation>::TABLE_ID = 44;
		template<> const bool SQLiteTableSyncTemplate<Reservation>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Reservation>::load(
			Reservation* object
			, const db::SQLiteResultSPtr& rows
		){
			object->setKey(rows->getLongLong (TABLE_COL_ID));
			object->setLineId(rows->getLongLong ( ReservationTableSync::COL_LINE_ID));
			object->setLineCode(rows->getText ( ReservationTableSync::COL_LINE_CODE));
			object->setServiceCode(rows->getText ( ReservationTableSync::COL_SERVICE_CODE));
			object->setDeparturePlaceId(rows->getLongLong ( ReservationTableSync::COL_DEPARTURE_PLACE_ID));
			object->setDeparturePlaceName(rows->getText ( ReservationTableSync::COL_DEPARTURE_PLACE_NAME));
			object->setDepartureTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_DEPARTURE_TIME)));
			object->setArrivalPlaceId(rows->getLongLong ( ReservationTableSync::COL_ARRIVAL_PLACE_ID));
			object->setArrivalPlaceName(rows->getText ( ReservationTableSync::COL_ARRIVAL_PLACE_NAME));
			object->setArrivalTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_ARRIVAL_TIME)));
			object->setReservationRuleId(rows->getLongLong ( ReservationTableSync::COL_RESERVATION_RULE_ID));
			object->setOriginDateTime(DateTime::FromSQLTimestamp(rows->getText ( ReservationTableSync::COL_ORIGIN_DATE_TIME)));
		}

		template<> void SQLiteTableSyncTemplate<Reservation>::save(Reservation* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << Conversion::ToString(object->getLineId())
				<< "," << Conversion::ToSQLiteString(object->getLineCode())
				<< "," << Conversion::ToSQLiteString(object->getServiceCode())
				<< "," << Conversion::ToString(object->getDeparturePlaceId())
				<< "," << Conversion::ToSQLiteString(object->getDeparturePlaceName())
				<< "," << object->getDepartureTime().toSQLString()
				<< "," << Conversion::ToString(object->getArrivalPlaceId())
				<< "," << Conversion::ToSQLiteString(object->getArrivalPlaceName())
				<< "," << object->getArrivalTime().toSQLString()
				<< "," << Conversion::ToString(object->getReservationRuleId())
				<< "," << object->getOriginDateTime().toSQLString()
				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace resa
	{
		const string ReservationTableSync::COL_LINE_ID = "line_id";
		const string ReservationTableSync::COL_LINE_CODE = "line_code";
		const string ReservationTableSync::COL_SERVICE_CODE = "service_code";
		const string ReservationTableSync::COL_DEPARTURE_PLACE_ID = "departure_place_id";
		const string ReservationTableSync::COL_DEPARTURE_PLACE_NAME = "departure_place_name";
		const string ReservationTableSync::COL_DEPARTURE_TIME = "departure_time";
		const string ReservationTableSync::COL_ARRIVAL_PLACE_ID = "arrival_place_id";
		const string ReservationTableSync::COL_ARRIVAL_PLACE_NAME = "arrival_place_name";
		const string ReservationTableSync::COL_ARRIVAL_TIME = "arrival_time";
		const string ReservationTableSync::COL_RESERVATION_RULE_ID = "reservation_rule_id";
		const string ReservationTableSync::COL_ORIGIN_DATE_TIME = "origin_date_time";

		ReservationTableSync::ReservationTableSync()
			: SQLiteTableSyncTemplate<Reservation>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_LINE_ID, "INTEGER");
			addTableColumn(COL_LINE_CODE, "TEXT");
			addTableColumn(COL_SERVICE_CODE, "TEXT");
			addTableColumn(COL_DEPARTURE_PLACE_ID, "INTEGER");
			addTableColumn(COL_DEPARTURE_PLACE_NAME, "TEXT");
			addTableColumn(COL_DEPARTURE_TIME, "TIMESTAMP");
			addTableColumn(COL_ARRIVAL_PLACE_ID, "INTEGER");
			addTableColumn(COL_ARRIVAL_PLACE_NAME, "TEXT");
			addTableColumn(COL_ARRIVAL_TIME, "TIMESTAMP");
			addTableColumn(COL_RESERVATION_RULE_ID, "INTEGER");
			addTableColumn(COL_ORIGIN_DATE_TIME, "TIMESTAMP");
			addTableIndex(COL_LINE_ID);
			addTableIndex(COL_DEPARTURE_PLACE_ID);
			addTableIndex(COL_ARRIVAL_PLACE_ID);
		}

		void ReservationTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
		}
		
		void ReservationTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
		}

		void ReservationTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
		}

		std::vector<shared_ptr<Reservation> > ReservationTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Reservation> > objects;
				while (rows->next ())
				{
					shared_ptr<Reservation> object(new Reservation());
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		ReservationTableSync::~ReservationTableSync()
		{

		}
	}
}
