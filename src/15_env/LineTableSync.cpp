
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

#include "CommercialLineTableSync.h"
#include "LineTableSync.h"
#include "FareTableSync.h"
#include "RollingStockTableSync.h"
#include "DataSourceTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace impex;
	

	template<> const string util::FactorableTemplate<SQLiteTableSync,LineTableSync>::FACTORY_KEY(
		"15.30.01 Lines"
	);

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
		const string LineTableSync::COL_DATASOURCE_ID("data_source");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<LineTableSync>::TABLE(
			"t009_lines"
			);
		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<LineTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(LineTableSync::COL_COMMERCIAL_LINE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_AXISID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(LineTableSync::COL_TIMETABLENAME, SQL_TEXT),
			SQLiteTableSync::Field(LineTableSync::COL_DIRECTION, SQL_TEXT),
			SQLiteTableSync::Field(LineTableSync::COL_ISWALKINGLINE, SQL_BOOLEAN),
			SQLiteTableSync::Field(LineTableSync::COL_USEINDEPARTUREBOARDS, SQL_BOOLEAN),
			SQLiteTableSync::Field(LineTableSync::COL_USEINTIMETABLES, SQL_BOOLEAN),
			SQLiteTableSync::Field(LineTableSync::COL_USEINROUTEPLANNING, SQL_BOOLEAN),
			SQLiteTableSync::Field(LineTableSync::COL_ROLLINGSTOCKID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_FAREID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_ALARMID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_BIKECOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_HANDICAPPEDCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_PEDESTRIANCOMPLIANCEID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_RESERVATIONRULEID, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_WAYBACK, SQL_INTEGER),
			SQLiteTableSync::Field(LineTableSync::COL_DATASOURCE_ID, SQL_INTEGER),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<LineTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(LineTableSync::COL_COMMERCIAL_LINE_ID.c_str(), ""),
			SQLiteTableSync::Index(LineTableSync::COL_DATASOURCE_ID.c_str(), ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::Load(
			Line* line,
			const db::SQLiteResultSPtr& rows,
			Env& env,
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
					line->setCommercialLine(CommercialLineTableSync::Get(commercialLineId, env, linkLevel).get());
				}
				catch(ObjectNotFoundException<CommercialLine>)
				{
					Log::GetInstance().warn("Bad value " + Conversion::ToString(commercialLineId) + " for fare in line " + Conversion::ToString(line->getKey()));
				}

				RegistryKeyType dataSourceId(rows->getLongLong(LineTableSync::COL_DATASOURCE_ID));
				if(dataSourceId > 0)
				{
					try
					{
						line->setDataSource(
							DataSourceTableSync::Get(dataSourceId, env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<DataSource>)
					{
						Log::GetInstance().warn("Bad value " + Conversion::ToString(dataSourceId) + " for data source in line " + Conversion::ToString(line->getKey()));
					}
				}


				line->setRollingStock(RollingStockTableSync::Get(rollingStockId, env, linkLevel, AUTO_CREATE).get());
// 				line->setFare (FareTableSync::Get (fareId, env, linkLevel, AUTO_CREATE));
// 				line->setBikeCompliance (BikeComplianceTableSync::Get (bikeComplianceId,env, linkLevel, AUTO_CREATE));
// 				line->setHandicappedCompliance (HandicappedComplianceTableSync::Get (handicappedComplianceId,env, linkLevel, AUTO_CREATE));
// 				line->setPedestrianCompliance (PedestrianComplianceTableSync::Get (pedestrianComplianceId, env, linkLevel, AUTO_CREATE));
// 				line->setReservationRule (ReservationRuleTableSync::Get (reservationRuleId,env, linkLevel, AUTO_CREATE));
				
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::Save(Line* object)
		{
			if(!object->getCommercialLine()) throw Exception("Line save error. Missing commercial line");
			stringstream query;
			if (object->getKey() <= 0) object->setKey(getId());
			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey()) <<
				"," << object->getCommercialLine()->getKey() <<
				",0" <<
				"," << Conversion::ToSQLiteString(object->getName()) <<
				"," << Conversion::ToSQLiteString(object->getTimetableName()) <<
				"," << Conversion::ToSQLiteString(object->getDirection()) <<
				"," << Conversion::ToString(object->getWalkingLine()) <<
				"," << Conversion::ToString(object->getUseInDepartureBoards()) <<
				"," << Conversion::ToString(object->getUseInTimetables()) <<
				"," << Conversion::ToString(object->getUseInRoutePlanning()) <<
				"," << (
					object->getRollingStock() ? Conversion::ToString(object->getRollingStock()->getKey()) :
					"0") <<
				",0" <<
				",0,0,0,0,0" <<
				"," << Conversion::ToString(object->getWayBack()) <<
				"," << (object->getDataSource() ? Conversion::ToString(object->getDataSource()->getKey()) : "0") <<
			")";
			DBModule::GetSQLite()->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<LineTableSync,Line>::Unlink(Line* obj)
		{
			obj->setRollingStock(NULL);
			obj->setCommercialLine(NULL);
			obj->setDataSource(NULL);
		}

	}

	namespace env
	{
		LineTableSync::LineTableSync()
			: SQLiteRegistryTableSyncTemplate<LineTableSync,Line>()
		{
		}


		void LineTableSync::Search(
			Env& env,
			uid commercialLineId,
			uid dataSourceId
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 ";
			if (commercialLineId != UNKNOWN_VALUE)
				query << " AND " << COL_COMMERCIAL_LINE_ID << "=" << commercialLineId;
			if (dataSourceId != UNKNOWN_VALUE)
				query << " AND " << COL_DATASOURCE_ID << "=" << dataSourceId;
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

