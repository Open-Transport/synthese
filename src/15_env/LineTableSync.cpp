
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

#include "Conversion.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "AxisTableSync.h"
#include "CommercialLineTableSync.h"
#include "ReservationRuleTableSync.h"
#include "BikeComplianceTableSync.h"
#include "HandicappedComplianceTableSync.h"
#include "PedestrianComplianceTableSync.h"
#include "LineTableSync.h"
#include "FareTableSync.h"
#include "RollingStockTableSync.h"

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

		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::Load(
			Line* line,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
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
			logic::tribool wayBack(rows->getTribool(LineTableSync::COL_WAYBACK));
			
			line->setName(name);
			line->setTimetableName (timetableName);
			line->setDirection (direction);
			line->setWalkingLine (isWalkingLine);
			line->setUseInDepartureBoards (useInDepartureBoards);
			line->setUseInTimetables (useInTimetables);
			line->setUseInRoutePlanning (useInRoutePlanning);
			line->setWayBack(wayBack);

			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				uid axisId (rows->getLongLong (LineTableSync::COL_AXISID));
				uid rollingStockId (rows->getLongLong (LineTableSync::COL_ROLLINGSTOCKID));
				uid fareId (rows->getLongLong (LineTableSync::COL_FAREID));
				uid alarmId (rows->getLongLong (LineTableSync::COL_ALARMID));
				uid bikeComplianceId (rows->getLongLong (LineTableSync::COL_BIKECOMPLIANCEID));
				uid pedestrianComplianceId (rows->getLongLong (LineTableSync::COL_PEDESTRIANCOMPLIANCEID));
				uid handicappedComplianceId (rows->getLongLong (LineTableSync::COL_HANDICAPPEDCOMPLIANCEID));
				uid reservationRuleId (rows->getLongLong (LineTableSync::COL_RESERVATIONRULEID));
				RegistryKeyType commercialLineId(rows->getLongLong (LineTableSync::COL_COMMERCIAL_LINE_ID));

				try
				{
					line->setAxis(AxisTableSync::Get(axisId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<Axis>)
				{
					Log::GetInstance().warn("Bad value " + Conversion::ToString(axisId) + " for axis in line " + Conversion::ToString(line->getKey()));
				}

				try
				{
					line->setCommercialLine(CommercialLineTableSync::Get(commercialLineId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<CommercialLine>)
				{
					Log::GetInstance().warn("Bad value " + Conversion::ToString(commercialLineId) + " for fare in line " + Conversion::ToString(line->getKey()));
				}


				line->setRollingStock(RollingStockTableSync::Get(rollingStockId, env, linkLevel, AUTO_CREATE).get());
				line->setFare (FareTableSync::Get (fareId, env, linkLevel, AUTO_CREATE));
				line->setBikeCompliance (BikeComplianceTableSync::Get (bikeComplianceId,env, linkLevel, AUTO_CREATE));
				line->setHandicappedCompliance (HandicappedComplianceTableSync::Get (handicappedComplianceId,env, linkLevel, AUTO_CREATE));
				line->setPedestrianCompliance (PedestrianComplianceTableSync::Get (pedestrianComplianceId, env, linkLevel, AUTO_CREATE));
				line->setReservationRule (ReservationRuleTableSync::Get (reservationRuleId,env, linkLevel, AUTO_CREATE));
				
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::Save(Line* object)
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



		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::Unlink(Line* obj, Env* env)
		{
			obj->setAxis(NULL);
			obj->setRollingStock(NULL);			
			obj->setCommercialLine(NULL);
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
		const std::string LineTableSync::COL_WAYBACK("wayback");

		LineTableSync::LineTableSync()
			: SQLiteRegistryTableSyncTemplate<LineTableSync,Line>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_COMMERCIAL_LINE_ID, "INTEGER", false);
			addTableColumn (COL_AXISID, "INTEGER", false);
			
			addTableColumn (COL_NAME, "TEXT");
			addTableColumn (COL_TIMETABLENAME, "TEXT");
			addTableColumn (COL_DIRECTION, "TEXT");
			addTableColumn (COL_ISWALKINGLINE, "BOOLEAN");
			addTableColumn (COL_USEINDEPARTUREBOARDS, "BOOLEAN");
			addTableColumn (COL_USEINTIMETABLES, "BOOLEAN");
			addTableColumn (COL_USEINROUTEPLANNING, "BOOLEAN");

			addTableColumn (COL_ROLLINGSTOCKID, "INTEGER");
			addTableColumn (COL_FAREID, "INTEGER");
			addTableColumn (COL_ALARMID, "INTEGER");
			addTableColumn (COL_BIKECOMPLIANCEID, "INTEGER");
			addTableColumn (COL_HANDICAPPEDCOMPLIANCEID, "INTEGER");
			addTableColumn (COL_PEDESTRIANCOMPLIANCEID, "INTEGER");
			addTableColumn (COL_RESERVATIONRULEID, "INTEGER");

			addTableColumn(COL_WAYBACK, "INTEGER");

			addTableIndex(COL_COMMERCIAL_LINE_ID);
		}


		void LineTableSync::Search(
			Env& env,
			uid commercialLineId
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
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

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}

