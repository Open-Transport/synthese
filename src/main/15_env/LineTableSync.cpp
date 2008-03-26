
/** LineTableSync class implementation.
	@file LineTableSync.cpp

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
#include "02_db/SQLite.h"
#include "02_db/SQLiteException.h"

#include "15_env/AxisTableSync.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/ReservationRuleTableSync.h"
#include "15_env/BikeComplianceTableSync.h"
#include "15_env/HandicappedComplianceTableSync.h"
#include "15_env/PedestrianComplianceTableSync.h"
#include "15_env/LineTableSync.h"
#include "15_env/FareTableSync.h"
#include "15_env/RollingStockTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync,LineTableSync>::FACTORY_KEY("15.30.01 Lines");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<LineTableSync>::TABLE_NAME = "t009_lines";
		template<> const int SQLiteTableSyncTemplate<LineTableSync>::TABLE_ID = 9;
		template<> const bool SQLiteTableSyncTemplate<LineTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::load(Line* line, const db::SQLiteResultSPtr& rows )
		{
			line->setKey(rows->getLongLong (TABLE_COL_ID));

			std::string name (
			    rows->getText (LineTableSync::COL_NAME));
			std::string timetableName (
			    rows->getText (LineTableSync::COL_TIMETABLENAME));
			std::string direction (
			    rows->getText (LineTableSync::COL_DIRECTION));

			bool isWalkingLine (rows->getBool (LineTableSync::COL_ISWALKINGLINE));
			bool useInDepartureBoards (rows->getBool (LineTableSync::COL_USEINDEPARTUREBOARDS));
			bool useInTimetables (rows->getBool (LineTableSync::COL_USEINTIMETABLES));
			bool useInRoutePlanning (rows->getBool (LineTableSync::COL_USEINROUTEPLANNING));
			
			line->setName(name);
			line->setTimetableName (timetableName);
			line->setDirection (direction);
			line->setWalkingLine (isWalkingLine);
			line->setUseInDepartureBoards (useInDepartureBoards);
			line->setUseInTimetables (useInTimetables);
			line->setUseInRoutePlanning (useInRoutePlanning);
		}

		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::save(Line* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::_link(Line* line, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			uid axisId (rows->getLongLong (LineTableSync::COL_AXISID));
			uid rollingStockId (rows->getLongLong (LineTableSync::COL_ROLLINGSTOCKID));
			uid fareId (rows->getLongLong (LineTableSync::COL_FAREID));
			uid alarmId (rows->getLongLong (LineTableSync::COL_ALARMID));
			uid bikeComplianceId (rows->getLongLong (LineTableSync::COL_BIKECOMPLIANCEID));
			uid pedestrianComplianceId (rows->getLongLong (LineTableSync::COL_PEDESTRIANCOMPLIANCEID));
			uid handicappedComplianceId (rows->getLongLong (LineTableSync::COL_HANDICAPPEDCOMPLIANCEID));
			uid reservationRuleId (rows->getLongLong (LineTableSync::COL_RESERVATIONRULEID));

			line->setAxis(AxisTableSync::Get(axisId,line,true,temporary));

			try
			{
				if (rollingStockId > 0)
					line->setRollingStock(RollingStockTableSync::Get(rollingStockId,line,false,temporary));
			}
			catch(...)
			{
				Log::GetInstance().warn("Bad value " + Conversion::ToString(rollingStockId) + " for rolling stock in line " + Conversion::ToString(line->getKey()));
			}
			// Fare
			try
			{
				if (fareId > 0)
					line->setFare (FareTableSync::Get (fareId,line,true,temporary));
			}
			catch(...)
			{
				Log::GetInstance().warn("Bad value " + Conversion::ToString(fareId) + " for fare in line " + Conversion::ToString(line->getKey()));
			}

			// GET_AUTO is used to benefit of the Neutral Element if id=0
			line->setBikeCompliance (BikeComplianceTableSync::Get (bikeComplianceId,line,true,GET_AUTO));
			line->setHandicappedCompliance (HandicappedComplianceTableSync::Get (handicappedComplianceId,line,true,GET_AUTO));
			line->setPedestrianCompliance (PedestrianComplianceTableSync::Get (pedestrianComplianceId,line,true,GET_AUTO));
			line->setCommercialLine(CommercialLineTableSync::Get(rows->getLongLong (LineTableSync::COL_COMMERCIAL_LINE_ID), line,true,GET_AUTO));
			line->setReservationRule (ReservationRuleTableSync::Get (reservationRuleId,line,true,GET_AUTO));

		}

		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::_unlink(Line* obj)
		{
			obj->setAxis(NULL);
			obj->setFare(NULL);
			obj->setBikeCompliance(NULL);
			obj->setRollingStock(NULL);
			obj->setReservationRule(NULL);
			obj->setBikeCompliance(NULL);
			obj->setHandicappedCompliance(NULL);
			obj->setPedestrianCompliance(NULL);
		}

	}

	namespace env
	{
		const std::string LineTableSync::COL_AXISID ("axis_id");
		const std::string LineTableSync::COL_COMMERCIAL_LINE_ID = "commercial_line_id";
		const std::string LineTableSync::COL_NAME ("name");
		const std::string LineTableSync::COL_TIMETABLENAME ("timetable_name");
		const std::string LineTableSync::COL_DIRECTION ("direction");
		const std::string LineTableSync::COL_ISWALKINGLINE ("is_walking_line");
		const std::string LineTableSync::COL_USEINDEPARTUREBOARDS ("use_in_departure_boards");
		const std::string LineTableSync::COL_USEINTIMETABLES ("use_in_timetables");
		const std::string LineTableSync::COL_USEINROUTEPLANNING ("use_in_routeplanning");
		const std::string LineTableSync::COL_ROLLINGSTOCKID ("rolling_stock_id");
		const std::string LineTableSync::COL_FAREID ("fare_id");
		const std::string LineTableSync::COL_ALARMID ("alarm_id");
		const std::string LineTableSync::COL_BIKECOMPLIANCEID ("bike_compliance_id");
		const std::string LineTableSync::COL_HANDICAPPEDCOMPLIANCEID ("handicapped_compliance_id");
		const std::string LineTableSync::COL_PEDESTRIANCOMPLIANCEID ("pedestrian_compliance_id");
		const std::string LineTableSync::COL_RESERVATIONRULEID ("reservation_rule_id");

		LineTableSync::LineTableSync()
			: SQLiteRegistryTableSyncTemplate<LineTableSync,Line>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_COMMERCIAL_LINE_ID, "INTEGER", false);
			addTableColumn (COL_AXISID, "INTEGER", false);
			
			addTableColumn (COL_NAME, "TEXT", true);
			addTableColumn (COL_TIMETABLENAME, "TEXT", true);
			addTableColumn (COL_DIRECTION, "TEXT", true);
			addTableColumn (COL_ISWALKINGLINE, "BOOLEAN", true);
			addTableColumn (COL_USEINDEPARTUREBOARDS, "BOOLEAN", true);
			addTableColumn (COL_USEINTIMETABLES, "BOOLEAN", true);
			addTableColumn (COL_USEINROUTEPLANNING, "BOOLEAN", true);

			addTableColumn (COL_ROLLINGSTOCKID, "INTEGER", true);
			addTableColumn (COL_FAREID, "INTEGER", true);
			addTableColumn (COL_ALARMID, "INTEGER", true);
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER", true);
			addTableColumn (COL_RESERVATIONRULEID, "INTEGER", true);

			addTableIndex(COL_COMMERCIAL_LINE_ID);
		}


		std::vector<shared_ptr<Line> > LineTableSync::search(
			uid commercialLineId
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (commercialLineId != UNKNOWN_VALUE)
				query << " AND " << COL_COMMERCIAL_LINE_ID << "=" << commercialLineId;
			if (orderByName)
				query << " ORDER BY " << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<Line> > objects;
				while (rows->next ())
				{
					shared_ptr<Line> object(new Line());
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
	}
}

