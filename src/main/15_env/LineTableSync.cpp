
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
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "15_env/EnvModule.h"
#include "Line.h"
#include "LineTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<Line>::TABLE_NAME = "t009_lines";
		template<> const int SQLiteTableSyncTemplate<Line>::TABLE_ID = 9;
		template<> const bool SQLiteTableSyncTemplate<Line>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<Line>::load(Line* line, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			line->setKey(Conversion::ToLongLong(rows.getColumn(rowIndex, TABLE_COL_ID)));
			uid axisId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_AXISID)));

			std::string name (
				rows.getColumn (rowIndex, LineTableSync::COL_NAME));
			std::string timetableName (
				rows.getColumn (rowIndex, LineTableSync::COL_TIMETABLENAME));
			std::string direction (
				rows.getColumn (rowIndex, LineTableSync::COL_DIRECTION));

			bool isWalkingLine (Conversion::ToBool (rows.getColumn (rowIndex, LineTableSync::COL_ISWALKINGLINE)));
			bool useInDepartureBoards (Conversion::ToBool (rows.getColumn (rowIndex, LineTableSync::COL_USEINDEPARTUREBOARDS)));
			bool useInTimetables (Conversion::ToBool (rows.getColumn (rowIndex, LineTableSync::COL_USEINTIMETABLES)));
			bool useInRoutePlanning (Conversion::ToBool (rows.getColumn (rowIndex, LineTableSync::COL_USEINROUTEPLANNING)));

			uid rollingStockId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_ROLLINGSTOCKID)));
			uid fareId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_FAREID)));
			uid alarmId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_ALARMID)));
			uid bikeComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_BIKECOMPLIANCEID)));
			uid pedestrianComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_PEDESTRIANCOMPLIANCEID)));
			uid handicappedComplianceId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_HANDICAPPEDCOMPLIANCEID)));
			uid reservationRuleId (Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_RESERVATIONRULEID)));

			line->setName(name);
			line->setAxis(EnvModule::getAxes().get(axisId).get());
			line->setTimetableName (timetableName);
			line->setDirection (direction);
			line->setWalkingLine (isWalkingLine);
			line->setUseInDepartureBoards (useInDepartureBoards);
			line->setUseInTimetables (useInTimetables);
			line->setUseInRoutePlanning (useInRoutePlanning);
			line->setRollingStockId (rollingStockId);
//			line->setFare (EnvModule::getFares ().get (fareId));
			line->setBikeCompliance (EnvModule::getBikeCompliances ().get (bikeComplianceId).get());
			line->setHandicappedCompliance (EnvModule::getHandicappedCompliances ().get (handicappedComplianceId).get());
			line->setPedestrianCompliance (EnvModule::getPedestrianCompliances ().get (pedestrianComplianceId).get());
			line->setCommercialLine(EnvModule::getCommercialLines().get(Conversion::ToLongLong (rows.getColumn (rowIndex, LineTableSync::COL_COMMERCIAL_LINE_ID))).get());
//			line->setReservationRule (EnvModule::getReservationRules ().get (reservationRuleId));
		}

		template<> void SQLiteTableSyncTemplate<Line>::save(Line* object)
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
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
			: SQLiteTableSyncTemplate<Line>(true, true, TRIGGERS_ENABLED_CLAUSE)
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

		void LineTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				shared_ptr<Line> object(new Line);
				load(object.get(), rows, i);
				EnvModule::getLines().add(object);
			}
		}

		void LineTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid lineId = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getLines().contains(lineId))
				{
					shared_ptr<Line> object = EnvModule::getLines().getUpdateable(lineId);
					load(object.get(), rows, i);
				}
			}
		}

		void LineTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid lineId = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getLines().contains(lineId))
				{
					EnvModule::getLines().remove(lineId);
				}
			}
		}

		std::vector<shared_ptr<Line> > LineTableSync::search(int first /*= 0*/, int number /*= 0*/ )
		{
			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
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
				SQLiteResult result = sqlite->execQuery(query.str());
				vector<shared_ptr<Line> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<Line> object(new Line());
					load(object.get(), result, i);
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

