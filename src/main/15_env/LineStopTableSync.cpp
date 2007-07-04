

/** LineStopTableSync class implementation.
	@file LineStopTableSync.cpp

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

#include "LineStop.h"
#include "LineStopTableSync.h"
#include "15_env/EnvModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<LineStop>::TABLE_NAME = "t010_line_stops";
		template<> const int SQLiteTableSyncTemplate<LineStop>::TABLE_ID = 10;
		template<> const bool SQLiteTableSyncTemplate<LineStop>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<LineStop>::load(LineStop* ls, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{

			uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

			uid fromPhysicalStopId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, LineStopTableSync::COL_PHYSICALSTOPID)));

			uid lineId (
				Conversion::ToLongLong (rows.getColumn (rowIndex, LineStopTableSync::COL_LINEID)));

			int rankInPath (
				Conversion::ToInt (rows.getColumn (rowIndex, LineStopTableSync::COL_RANKINPATH)));

			bool isDeparture (Conversion::ToBool (
				rows.getColumn (rowIndex, LineStopTableSync::COL_ISDEPARTURE)));
			bool isArrival (Conversion::ToBool (
				rows.getColumn (rowIndex, LineStopTableSync::COL_ISARRIVAL)));

			double metricOffset (
				Conversion::ToDouble (rows.getColumn (rowIndex, LineStopTableSync::COL_METRICOFFSET)));

			std::string viaPointsStr (
				rows.getColumn (rowIndex, LineStopTableSync::COL_VIAPOINTS));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			ls->clearViaPoints ();

			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 (":");
			tokenizer viaPointsTokens (viaPointsStr, sep1);
			for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
				viaPointIter != viaPointsTokens.end (); ++viaPointIter)
			{
				tokenizer valueTokens (*viaPointIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// X:Y
				ls->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					Conversion::ToDouble (*(++valueIter))));
			}

			ls->setKey(id);
			ls->setMetricOffset(metricOffset);
			ls->setIsArrival(isArrival);
			ls->setIsDeparture(isDeparture);
			ls->setPhysicalStop(EnvModule::getPhysicalStops ().getUpdateable(fromPhysicalStopId).get());
			ls->setLine(EnvModule::getLines ().getUpdateable (lineId).get());
			ls->setRankInPath(rankInPath);

			EnvModule::getLines ().getUpdateable (lineId)->addEdge(ls);
			
		}

		template<> void SQLiteTableSyncTemplate<LineStop>::save(LineStop* object)
		{
/*			const SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());	/// @todo Use grid ID
               
			 query
				<< " REPLACE INTO " << TABLE_NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				/// @todo fill other fields separated by ,
				<< ")";
			sqlite->execUpdate(query.str());
*/		}

	}

	namespace env
	{
		const std::string LineStopTableSync::COL_PHYSICALSTOPID ("physical_stop_id");
		const std::string LineStopTableSync::COL_LINEID ("line_id");
		const std::string LineStopTableSync::COL_RANKINPATH ("rank_in_path");
		const std::string LineStopTableSync::COL_ISDEPARTURE ("is_departure");
		const std::string LineStopTableSync::COL_ISARRIVAL ("is_arrival");
		const std::string LineStopTableSync::COL_METRICOFFSET ("metric_offset");
		const std::string LineStopTableSync::COL_SCHEDULEINPUT ("schedule_input");
		const std::string LineStopTableSync::COL_VIAPOINTS ("via_points");

		LineStopTableSync::LineStopTableSync()
			: SQLiteTableSyncTemplate<LineStop>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_PHYSICALSTOPID, "INTEGER", false);
			addTableColumn (COL_LINEID, "INTEGER", false);
			addTableColumn (COL_RANKINPATH, "INTEGER", false);
			addTableColumn (COL_ISDEPARTURE, "BOOLEAN", false);
			addTableColumn (COL_ISARRIVAL, "BOOLEAN", false);
			addTableColumn (COL_METRICOFFSET, "DOUBLE", true);
			addTableColumn (COL_VIAPOINTS, "TEXT", true);

			addTableIndex(COL_LINEID);
			addTableIndex(COL_PHYSICALSTOPID);
		}

		void LineStopTableSync::rowsAdded(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows, bool isFirstSync)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				if (EnvModule::getLineStops().contains(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))))
				{
					load(EnvModule::getLineStops().getUpdateable(Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID))).get(), rows, i);
				}
				else
				{
					shared_ptr<LineStop> object(new LineStop);
					load(object.get(), rows, i);
					EnvModule::getLineStops().add(object);
				}
			}
		}
		
		void LineStopTableSync::rowsUpdated(const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows)
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getLineStops().contains(id))
				{
					shared_ptr<LineStop> object = EnvModule::getLineStops().getUpdateable(id);
					load(object.get(), rows, i);
				}
			}
		}

		void LineStopTableSync::rowsRemoved( const db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResult& rows )
		{
			for (int i=0; i<rows.getNbRows(); ++i)
			{
				uid id = Conversion::ToLongLong(rows.getColumn(i, TABLE_COL_ID));
				if (EnvModule::getLineStops().contains(id))
				{
					EnvModule::getLineStops().remove(id);
				}
			}
		}

		std::vector<shared_ptr<LineStop> > LineStopTableSync::search(int first /*= 0*/, int number /*= 0*/ )
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
				vector<shared_ptr<LineStop> > objects;
				for (int i = 0; i < result.getNbRows(); ++i)
				{
					shared_ptr<LineStop> object(new LineStop());
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
