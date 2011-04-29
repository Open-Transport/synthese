
/** PTOperationStatisticsTableSync class implementation.
	@file PTOperationStatisticsTableSync.cpp

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

#include "PTOperationStatisticsTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "VehicleTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace pt;

	namespace pt_operation
	{
		PTOperationStatisticsTableSync::CountSearchResult PTOperationStatisticsTableSync::Count(
			boost::gregorian::date_period period,
			Step rowStep,
			Step colStep,
			What what
		){
			CountSearchResult r;
			stringstream s;
			bool hasRowStep(rowStep != NO_STEP);
			bool hasColStep(colStep != NO_STEP);

			s << "SELECT " <<
				"SUM(" << GetSQLWhat(what) << ") AS result";
				if(hasRowStep) s << "," << GetSQLColumn(rowStep) << " AS row";
				if(hasColStep) s << "," << GetSQLColumn(colStep) << " AS col";
			s << " FROM " << VehiclePositionTableSync::TABLE.NAME << " AS r " <<
				" WHERE " <<
				VehiclePositionTableSync::COL_TIME << ">='" << gregorian::to_iso_extended_string(period.begin())  << " 00:00:00' AND " <<
				VehiclePositionTableSync::COL_TIME << "<'" << gregorian::to_iso_extended_string(period.end()) << " 00:00:00'";
			if(hasRowStep || hasColStep)
			{
				s << " GROUP BY ";
				if(hasRowStep) s << GetSQLGroupBy(rowStep);
				if(hasRowStep && hasColStep) s << ",";
				if(hasColStep) s << GetSQLGroupBy(colStep);
			}
			if(hasRowStep || hasColStep)
			{
				s << " ORDER BY ";
				if(hasRowStep) s << GetSQLColumn(rowStep);
				if(hasRowStep && hasColStep) s << ",";
				if(hasColStep) s << GetSQLColumn(colStep);
			}


			DBResultSPtr rows = DBModule::GetDB()->execQuery(s.str());
			while(rows->next ())
			{
				const string row(hasRowStep ? rows->getText("row") : "row");
				const string col(hasColStep ? rows->getText("col") : "col");

				CountSearchResult::iterator itrow(r.find(row));
				if(itrow == r.end())
				{
					itrow = r.insert(make_pair(row, CountSearchResult::mapped_type())).first;
				}
				itrow->second.insert(make_pair(col, rows->getInt("result")));
			}
			return r;
		}



		std::string PTOperationStatisticsTableSync::GetSQLColumn( Step step )
		{
			//TODO take into account of DBMS (this runs only with SQLite)
			if(step == LINE_STEP) return "(SELECT c."+ CommercialLineTableSync::COL_SHORT_NAME +" FROM "+ CommercialLineTableSync::TABLE.NAME +" AS c INNER JOIN "+ JourneyPatternTableSync::TABLE.NAME +" AS jp ON jp."+ JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID +"=c."+ TABLE_COL_ID +" INNER JOIN "+ ScheduledServiceTableSync::TABLE.NAME +" AS s ON s."+ ScheduledServiceTableSync::COL_PATHID +"=jp."+ TABLE_COL_ID +" WHERE s."+ TABLE_COL_ID +"=r."+ VehiclePositionTableSync::COL_SERVICE_ID +")";
			if(step == SERVICE_STEP) return VehiclePositionTableSync::COL_SERVICE_ID;
			if(step == DATE_STEP) return "strftime('%Y-%m-%d'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == HOUR_STEP) return "strftime('%H'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == WEEK_DAY_STEP) return "strftime('%w'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == WEEK_STEP) return "strftime('%W'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == MONTH_STEP) return "strftime('%Y-%m'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == YEAR_STEP) return "strftime('%Y'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == VEHICLE_STEP) return "(SELECT v."+ VehicleTableSync::COL_NAME +" FROM "+ VehicleTableSync::TABLE.NAME +" AS v WHERE v."+ TABLE_COL_ID +"=r."+ VehiclePositionTableSync::COL_VEHICLE_ID +")";
			return string();
		}



		std::string PTOperationStatisticsTableSync::GetSQLGroupBy( Step step )
		{
			if(step == SERVICE_STEP) return VehiclePositionTableSync::COL_SERVICE_ID;
			if(step == LINE_STEP) return "(SELECT jp."+ JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID +" FROM "+ JourneyPatternTableSync::TABLE.NAME +" AS jp INNER JOIN "+ ScheduledServiceTableSync::TABLE.NAME +" AS s ON s."+ ScheduledServiceTableSync::COL_PATHID +"=jp."+ TABLE_COL_ID +" WHERE s."+ TABLE_COL_ID +"=r."+ VehiclePositionTableSync::COL_SERVICE_ID +")";
			if(step == DATE_STEP) return "strftime('%Y-%m-%d'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == HOUR_STEP) return "strftime('%H'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == WEEK_DAY_STEP) return "strftime('%w'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == WEEK_STEP) return "strftime('%W'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == MONTH_STEP) return "strftime('%Y-%m'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == YEAR_STEP) return "strftime('%Y'," + VehiclePositionTableSync::COL_TIME +")";
			if(step == VEHICLE_STEP) return "r."+ VehiclePositionTableSync::COL_VEHICLE_ID;
		}



		std::string PTOperationStatisticsTableSync::GetSQLWhat( What what )
		{
			stringstream s;
			s << "(SELECT p2." << VehiclePositionTableSync::COL_METER_OFFSET << " FROM " << VehiclePositionTableSync::TABLE.NAME << " AS p2 WHERE p2." <<
				VehiclePositionTableSync::COL_TIME << ">r." << VehiclePositionTableSync::COL_TIME <<
				" AND p2." << VehiclePositionTableSync::COL_METER_OFFSET << ">0" <<
				" AND p2." << VehiclePositionTableSync::COL_VEHICLE_ID << "=r." << VehiclePositionTableSync::COL_VEHICLE_ID <<
				" ORDER BY p2." << VehiclePositionTableSync::COL_TIME << " LIMIT 1) - " << VehiclePositionTableSync::COL_METER_OFFSET;
			return s.str();
		}
	}
}
