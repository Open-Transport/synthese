
/** Resa StatisticsTableSync class implementation.
	@file ResaStatisticsTableSync.cpp

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

#include "ResaStatisticsTableSync.h"
#include "ReservationTableSync.h"
#include "ReservationTransactionTableSync.h"
#include "StopAreaTableSync.hpp"
#include "CityTableSync.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace geography;

	namespace resa
	{
		ResaStatisticsTableSync::ResaCountSearchResult ResaStatisticsTableSync::CountCalls(
			boost::gregorian::date_period period,
			Step rowStep,
			Step colStep,
			boost::optional<util::RegistryKeyType> lineFilter,
			boost::logic::tribool cancelledFilter
		){
			ResaCountSearchResult r;
			stringstream s;
			bool hasRowStep(rowStep != NO_STEP);
			bool hasColStep(colStep != NO_STEP);
			DB* db = DBModule::GetDB();

			s << "SELECT " <<
				"SUM(t." << ReservationTransactionTableSync::COL_SEATS << ") AS number";
				if(hasRowStep) s << "," << GetSQLColumnOrGroupBy(db, rowStep, true) << " AS row";
				if(hasColStep) s << "," << GetSQLColumnOrGroupBy(db, colStep, true) << " AS col";
			s << " FROM " << ReservationTableSync::TABLE.NAME << " AS r " <<
				" INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " AS t ON t." << TABLE_COL_ID << "=r." << ReservationTableSync::COL_TRANSACTION_ID <<
				" LEFT JOIN " << StopAreaTableSync::TABLE.NAME << " AS sd ON sd." << TABLE_COL_ID << "=r." << ReservationTableSync::COL_DEPARTURE_PLACE_ID <<
				" LEFT JOIN " << CityTableSync::TABLE.NAME << " AS cd ON cd." << TABLE_COL_ID << "=sd." << StopAreaTableSync::TABLE_COL_CITYID <<
				" LEFT JOIN " << StopAreaTableSync::TABLE.NAME << " AS sa ON sa." << TABLE_COL_ID << "=r." << ReservationTableSync::COL_ARRIVAL_PLACE_ID <<
				" LEFT JOIN " << CityTableSync::TABLE.NAME << " AS ca ON ca." << TABLE_COL_ID << "=sa." << StopAreaTableSync::TABLE_COL_CITYID <<
				" WHERE " <<
				ReservationTableSync::COL_ORIGIN_DATE_TIME << ">='" << gregorian::to_iso_extended_string(period.begin())  << " 00:00:00' AND " <<
				ReservationTableSync::COL_ORIGIN_DATE_TIME << "<'" << gregorian::to_iso_extended_string(period.end()) << " 00:00:00'";
			if(lineFilter)
			{
				s << " AND " << ReservationTableSync::COL_LINE_ID << "=" << *lineFilter;
			}
			if(!indeterminate(cancelledFilter))
			{
				s << " AND " << ReservationTransactionTableSync::COL_CANCELLATION_TIME << " IS " << (cancelledFilter ? "NOT " : "") << " NULL";
			}
			if(hasRowStep || hasColStep)
			{
				s << " GROUP BY ";
				if(hasRowStep) s << GetSQLColumnOrGroupBy(db, rowStep, false);
				if(hasRowStep && hasColStep) s << ",";
				if(hasColStep) s << GetSQLColumnOrGroupBy(db, colStep, false);
			}
			if(hasRowStep || hasColStep)
			{
				s << " ORDER BY ";
				if(hasRowStep) s << GetSQLColumnOrGroupBy(db, rowStep, true);
				if(hasRowStep && hasColStep) s << ",";
				if(hasColStep) s << GetSQLColumnOrGroupBy(db, colStep, true);
			}


			DBResultSPtr rows = DBModule::GetDB()->execQuery(s.str());
			while (rows->next ())
			{
				const string row(hasRowStep ? rows->getText("row") : "row");
				const string col(hasColStep ? rows->getText("col") : "col");

				ResaCountSearchResult::iterator itrow(r.find(row));
				if(itrow == r.end())
				{
					itrow = r.insert(make_pair(row, ResaCountSearchResult::mapped_type())).first;
				}
				itrow->second.insert(make_pair(col, rows->getInt("number")));
			}
			return r;
		}



		string ResaStatisticsTableSync::GetSQLColumnOrGroupBy(DB* db, Step step, bool column)
		{
			if(step == SERVICE_STEP) return ReservationTableSync::COL_SERVICE_CODE;
			if(step == DATE_STEP) return db->getSQLDateFormat("%Y-%m-%d", ReservationTableSync::COL_ORIGIN_DATE_TIME);
			if(step == HOUR_STEP) return db->getSQLDateFormat("%H", ReservationTableSync::COL_ORIGIN_DATE_TIME);
			if(step == WEEK_DAY_STEP) return db->getSQLDateFormat("%w", ReservationTableSync::COL_ORIGIN_DATE_TIME);
			if(step == WEEK_STEP) return db->getSQLDateFormat("%W", ReservationTableSync::COL_ORIGIN_DATE_TIME);
			if(step == MONTH_STEP) return db->getSQLDateFormat("%Y-%m", ReservationTableSync::COL_ORIGIN_DATE_TIME);
			if(step == YEAR_STEP) return db->getSQLDateFormat("%Y", ReservationTableSync::COL_ORIGIN_DATE_TIME);
			if(column)
			{
				if(step == DEPARTURE_STOP_STEP) return ReservationTableSync::COL_DEPARTURE_PLACE_NAME;
				if(step == DEPARTURE_CITY_STEP) return "cd." + CityTableSync::TABLE_COL_NAME;
				if(step == ARRIVAL_STOP_STEP) return ReservationTableSync::COL_ARRIVAL_PLACE_NAME;
				if(step == ARRIVAL_CITY_STEP) return "ca." + CityTableSync::TABLE_COL_NAME;
			}
			else
			{
				if(step == DEPARTURE_STOP_STEP) return ReservationTableSync::COL_DEPARTURE_PLACE_ID;
				if(step == DEPARTURE_CITY_STEP) return "sd." + StopAreaTableSync::TABLE_COL_CITYID;
				if(step == ARRIVAL_STOP_STEP) return ReservationTableSync::COL_ARRIVAL_PLACE_ID;
				if(step == ARRIVAL_CITY_STEP) return "sa." + StopAreaTableSync::TABLE_COL_CITYID;
			}
			if(db->isBackend(DB::SQLITE_BACKEND))
			{
				if(step == RESERVATION_DELAY_10_MIN_STEP)
					return "10*((strftime('%s',r." + ReservationTableSync::COL_DEPARTURE_TIME +
						") - strftime('%s',t."+ ReservationTransactionTableSync::COL_BOOKING_TIME + ")) / 600)";
				if(step == RESERVATION_DELAY_30_MIN_STEP)
					return "30*((strftime('%s',r."+ ReservationTableSync::COL_DEPARTURE_TIME +
						") - strftime('%s',t."+ ReservationTransactionTableSync::COL_BOOKING_TIME +")) / 1800)";
				if(step == RESERVATION_DELAY_60_MIN_STEP)
					return "(strftime('%s',r."+ ReservationTableSync::COL_DEPARTURE_TIME +
						") - strftime('%s',t."+ ReservationTransactionTableSync::COL_BOOKING_TIME +")) / 3600";
			}
			else if(db->isBackend(DB::MYSQL_BACKEND))
			{
				if(step == RESERVATION_DELAY_10_MIN_STEP)
					return "10*(TIMESTAMPDIFF(SECOND, t."+ ReservationTransactionTableSync::COL_BOOKING_TIME +",r."+
						ReservationTableSync::COL_DEPARTURE_TIME +") DIV 600)";
				if(step == RESERVATION_DELAY_30_MIN_STEP)
					return "30*(TIMESTAMPDIFF(SECOND, t."+ ReservationTransactionTableSync::COL_BOOKING_TIME +",r."+
						ReservationTableSync::COL_DEPARTURE_TIME +") DIV 1800)";
				if(step == RESERVATION_DELAY_60_MIN_STEP)
					return "TIMESTAMPDIFF(SECOND, t."+ ReservationTransactionTableSync::COL_BOOKING_TIME +",r."+
						ReservationTableSync::COL_DEPARTURE_TIME +") DIV 3600";
			}
			else
			{
				assert(false);
			}
			return string();
		}
	}
}
