
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
				"SUM(t." << Seats::FIELD.name << ") AS number";
				if(hasRowStep) s << "," << GetSQLColumnOrGroupBy(db, rowStep, true) << " AS row";
				if(hasColStep) s << "," << GetSQLColumnOrGroupBy(db, colStep, true) << " AS col";
			s << " FROM " << ReservationTableSync::TABLE.NAME << " AS r " <<
				" INNER JOIN " << ReservationTransactionTableSync::TABLE.NAME << " AS t ON t." << TABLE_COL_ID << "=r." << Transaction::FIELD.name <<
				" LEFT JOIN " << StopAreaTableSync::TABLE.NAME << " AS sd ON sd." << TABLE_COL_ID << "=r." << DeparturePlaceId::FIELD.name <<
				" LEFT JOIN " << CityTableSync::TABLE.NAME << " AS cd ON cd." << TABLE_COL_ID << "=sd." << pt::CityId::FIELD.name <<
				" LEFT JOIN " << StopAreaTableSync::TABLE.NAME << " AS sa ON sa." << TABLE_COL_ID << "=r." << ArrivalPlaceId::FIELD.name <<
				" LEFT JOIN " << CityTableSync::TABLE.NAME << " AS ca ON ca." << TABLE_COL_ID << "=sa." << pt::CityId::FIELD.name <<
				" WHERE " <<
				OriginDateTime::FIELD.name << ">='" << gregorian::to_iso_extended_string(period.begin())  << " 00:00:00' AND " <<
				OriginDateTime::FIELD.name << "<'" << gregorian::to_iso_extended_string(period.end()) << " 00:00:00'";
			if(lineFilter)
			{
				s << " AND " << LineId::FIELD.name << "=" << *lineFilter;
			}
			if(!indeterminate(cancelledFilter))
			{
				s << " AND " << CancellationTime::FIELD.name << " IS " << (cancelledFilter ? "NOT " : "") << " NULL";
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
			if(step == SERVICE_STEP) return ServiceCode::FIELD.name;
			if(step == DATE_STEP) return db->getSQLDateFormat("%Y-%m-%d", OriginDateTime::FIELD.name);
			if(step == HOUR_STEP) return db->getSQLDateFormat("%H", OriginDateTime::FIELD.name);
			if(step == WEEK_DAY_STEP) return db->getSQLDateFormat("%w", OriginDateTime::FIELD.name);
			if(step == WEEK_STEP) return db->getSQLDateFormat("%W", OriginDateTime::FIELD.name);
			if(step == MONTH_STEP) return db->getSQLDateFormat("%Y-%m", OriginDateTime::FIELD.name);
			if(step == YEAR_STEP) return db->getSQLDateFormat("%Y", OriginDateTime::FIELD.name);
			if(column)
			{
				if(step == DEPARTURE_STOP_STEP) return DeparturePlaceName::FIELD.name;
				if(step == DEPARTURE_CITY_STEP) return "cd." + CityTableSync::TABLE_COL_NAME;
				if(step == ARRIVAL_STOP_STEP) return ArrivalPlaceName::FIELD.name;
				if(step == ARRIVAL_CITY_STEP) return "ca." + CityTableSync::TABLE_COL_NAME;
			}
			else
			{
				if(step == DEPARTURE_STOP_STEP) return DeparturePlaceId::FIELD.name;
				if(step == DEPARTURE_CITY_STEP) return "sd." + pt::CityId::FIELD.name;
				if(step == ARRIVAL_STOP_STEP) return ArrivalPlaceId::FIELD.name;
				if(step == ARRIVAL_CITY_STEP) return "sa." + pt::CityId::FIELD.name;
			}
			if(db->isBackend(DB::SQLITE_BACKEND))
			{
				if(step == RESERVATION_DELAY_10_MIN_STEP)
					return "10*((strftime('%s',r." + DepartureTime::FIELD.name +
						") - strftime('%s',t."+ BookingTime::FIELD.name + ")) / 600)";
				if(step == RESERVATION_DELAY_30_MIN_STEP)
					return "30*((strftime('%s',r."+ DepartureTime::FIELD.name +
						") - strftime('%s',t."+ BookingTime::FIELD.name +")) / 1800)";
				if(step == RESERVATION_DELAY_60_MIN_STEP)
					return "(strftime('%s',r."+ DepartureTime::FIELD.name +
						") - strftime('%s',t."+ BookingTime::FIELD.name +")) / 3600";
			}
			else if(db->isBackend(DB::MYSQL_BACKEND))
			{
				if(step == RESERVATION_DELAY_10_MIN_STEP)
					return "10*(TIMESTAMPDIFF(SECOND, t."+ BookingTime::FIELD.name +",r."+
						DepartureTime::FIELD.name +") DIV 600)";
				if(step == RESERVATION_DELAY_30_MIN_STEP)
					return "30*(TIMESTAMPDIFF(SECOND, t."+ BookingTime::FIELD.name +",r."+
						DepartureTime::FIELD.name +") DIV 1800)";
				if(step == RESERVATION_DELAY_60_MIN_STEP)
					return "TIMESTAMPDIFF(SECOND, t."+ BookingTime::FIELD.name +",r."+
						DepartureTime::FIELD.name +") DIV 3600";
			}
			else
			{
				assert(false);
			}
			return string();
		}
	}
}
